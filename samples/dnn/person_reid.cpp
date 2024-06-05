/*
This sample detects the query person in the given video file.

Authors of samples and Youtu ReID baseline:
        Xing Sun <winfredsun@tencent.com>
        Feng Zheng <zhengf@sustech.edu.cn>
        Xinyang Jiang <sevjiang@tencent.com>
        Fufu Yu <fufuyu@tencent.com>
        Enwei Zhang <miyozhang@tencent.com>

Copyright (C) 2020-2021, Tencent.
Copyright (C) 2020-2021, SUSTech.

How to use:
    sample command to run:

        ./person_reid --video=/path/to/videofile --model=path/to/youtu_reid_baseline_medium.onnx --yolo=path/to/yolov8n.onnx
    The system will ask you to mark the person to be tracked

    You can download a baseline ReID model from:
        https://github.com/ReID-Team/ReID_extra_testdata

*/

#include <iostream>
#include <fstream>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>

#include "common.hpp"

using namespace cv;
using namespace cv::dnn;
using namespace std;

const string param_keys =
    "{help    h  |           | show help message}"
    "{model   m  |           | network model}"
    "{query   q  |           | Path to target image. Skip this argument to select target in the video frame.}"
    "{batch_size |    1     | batch size of each inference}"
    "{video   v  | vtest.avi | video file path}"
    "{yolo       |           | Path to yolov8n.onnx}"
    "{resize_h   |    256    | resize input to specific height}"
    "{resize_w   |    128    | resize input to specific width}";

const string backend_keys = format(
    "{ backend   | 0 | Choose one of computation backends: "
    "%d: automatically (by default), "
    "%d: Intel's Deep Learning Inference Engine (https://software.intel.com/openvino-toolkit), "
    "%d: OpenCV implementation, "
    "%d: VKCOM, "
    "%d: CUDA }",
    DNN_BACKEND_DEFAULT, DNN_BACKEND_INFERENCE_ENGINE, DNN_BACKEND_OPENCV, DNN_BACKEND_VKCOM, DNN_BACKEND_CUDA);

const string target_keys = format(
    "{ target    | 0 | Choose one of target computation devices: "
    "%d: CPU target (by default), "
    "%d: OpenCL, "
    "%d: OpenCL fp16 (half-float precision), "
    "%d: VPU, "
    "%d: Vulkan, "
    "%d: CUDA, "
    "%d: CUDA fp16 (half-float preprocess) }",
    DNN_TARGET_CPU, DNN_TARGET_OPENCL, DNN_TARGET_OPENCL_FP16, DNN_TARGET_MYRIAD, DNN_TARGET_VULKAN, DNN_TARGET_CUDA, DNN_TARGET_CUDA_FP16);

string keys = param_keys + backend_keys + target_keys;


struct MatComparator
{
    bool operator()(const Mat &a, const Mat &b) const
    {
        return a.data < b.data; // This is a simple pointer comparison, not content!
    }
};

// Global variable for drawing function to select target
map<Mat, Rect, MatComparator> imgDict;
bool drawing = false;
int ix = -1, iy = -1;
Rect rect;

static void extractFrames(const string& queryImgPath, const string& videoPath, Net* reidNet, const string& yoloPath, int resize_h, int resize_w, int batch_size);

int main(int argc, char **argv)
{
    CommandLineParser parser(argc, argv, keys);

    if (argc == 1 || parser.has("help"))
    {
        parser.printMessage();
        return 0;
    }
    parser = CommandLineParser(argc, argv, keys);
    parser.about("Use this script to run ReID networks using OpenCV.");

    const string modelPath = parser.get<String>("model");
    const string queryImagePath = parser.get<String>("query");
    const string videoPath = findFile(parser.get<String>("video"));
    const string yoloPath = parser.get<String>("yolo");
    const int batch_size = parser.get<int>("batch_size");
    const int backend = parser.get<int>("backend");
    const int target = parser.get<int>("target");
    const int resize_h = parser.get<int>("resize_h");
    const int resize_w = parser.get<int>("resize_w");

    Net net = readNet(modelPath);
    net.setPreferableBackend(backend);
    net.setPreferableTarget(target);

    extractFrames(queryImagePath, videoPath, &net, yoloPath, resize_h, resize_w, batch_size);
    return 0;
}

static Mat preprocess(const Mat &img)
{
    const double mean[3] = {0.485, 0.456, 0.406};
    const double std[3] = {0.229, 0.224, 0.225};
    Mat ret = Mat(img.rows, img.cols, CV_32FC3);
    for (int y = 0; y < ret.rows; y++)
    {
        for (int x = 0; x < ret.cols; x++)
        {
            for (int c = 0; c < 3; c++)
            {
                ret.at<Vec3f>(y,x)[c] = (float)((img.at<Vec3b>(y,x)[c] / 255.0 - mean[2 - c]) / std[2 - c]);
            }
        }
    }
    return ret;
}

static vector<float> normalization(const vector<float> &feature)
{
    vector<float> ret;
    float sum = 0.0;
    for (int i = 0; i < (int)feature.size(); i++)
    {
        sum += feature[i] * feature[i];
    }
    sum = sqrt(sum);
    for (int i = 0; i < (int)feature.size(); i++)
    {
        ret.push_back(feature[i] / sum);
    }
    return ret;
}

static void extractFeatures(vector<Mat> &imglist, Net *net, const int &resize_h, const int &resize_w, vector<vector<float>> &features, int batch_size=32)
{
    for (int st = 0; st < (int)imglist.size(); st += batch_size)
    {
        vector<Mat> batch;
        for (int delta = 0; delta < batch_size && st + delta < (int)imglist.size(); delta++)
        {
            Mat img = imglist[st + delta];
            batch.push_back(preprocess(img));
        }
        Mat blob = blobFromImages(batch, 1.0, Size(resize_w, resize_h), Scalar(0.0,0.0,0.0), true, false, CV_32F);
        net->setInput(blob);
        Mat out=net->forward();
        vector<int> s {out.size[0], out.size[1]};
        out = out.reshape(1, s);
        for (int i = 0; i < out.rows; i++)
        {
            vector<float> temp_feature;
            for (int j = 0; j < out.cols; j++)
            {
                temp_feature.push_back(out.at<float>(i, j));
            }
            features.push_back(normalization(temp_feature));
        }
    }
    return;
}

static float similarity(const vector<float> &feature1, const vector<float> &feature2)
{
    float result = 0.0;
    for (int i = 0; i < (int)feature1.size(); i++)
    {
        result += feature1[i] * feature2[i];
    }
    return result;
}

static int getTopK(const vector<vector<float>> &queryFeatures, const vector<vector<float>> &galleryFeatures)
{
    if (queryFeatures.empty() || galleryFeatures.empty())
        return -1; // No valid index if either feature list is empty

    int bestIndex = -1;
    float maxSimilarity = -1000000000.0;

    const vector<float> &query = queryFeatures[0];

    for (int j = 0; j < (int)galleryFeatures.size(); j++)
    {
        float currentSimilarity = similarity(query, galleryFeatures[j]);
        if (currentSimilarity > maxSimilarity)
        {
            maxSimilarity = currentSimilarity;
            bestIndex = j;
        }
    }
    return bestIndex;
}

static vector<Mat> yoloDetector(Mat &frame, Net &net)
{
    int height = frame.rows;
    int width = frame.cols;

    int length = max(height, width);

     Mat image = Mat::zeros(Size(length, length), frame.type());

    frame.copyTo(image(Rect(0, 0, width, height)));

    // Calculate the scale
    double scale = static_cast<double>(length) / 640.0;

    Mat blob;
    blobFromImage(image, blob, 1.0/255.0, Size(640, 640), Scalar(), true, false, CV_32F);
    net.setInput(blob);

    vector<Mat> outputs;
    net.forward(outputs);
    Mat reshapedMatrix = outputs[0].reshape(0, 84);  // Reshape to 2D (84 rows, 8400 columns)

    Mat outputTransposed;
    transpose(reshapedMatrix, outputTransposed);

    int rows = outputTransposed.rows;

    vector<Rect2d> boxes;
    vector<float> scores;
    vector<int> class_ids;

    for (int i = 0; i < rows; i++) {
        double minScore, maxScore;
        Point minClassLoc, maxClassLoc;
        minMaxLoc(outputTransposed.row(i).colRange(4, outputTransposed.cols), &minScore, &maxScore, &minClassLoc, &maxClassLoc);

        if (maxScore >= 0.25 && maxClassLoc.x == 0) {
            double centerX = outputTransposed.at<float>(i, 0);
            double centerY = outputTransposed.at<float>(i, 1);
            double w = outputTransposed.at<float>(i, 2);
            double h = outputTransposed.at<float>(i, 3);

            Rect2d box(
                centerX - 0.5 * w, // x
                centerY - 0.5 * h, // y
                w, // width
                h // height
            );
            boxes.push_back(box);
            scores.push_back(maxScore);
            class_ids.push_back(maxClassLoc.x); // x location gives the index
        }
    }

    // Apply Non-Maximum Suppression
    vector<int> indexes;
    NMSBoxes(boxes, scores, 0.25, 0.45, indexes, 0.5, 0);

    vector<Mat> images;
    for (int index : indexes) {
        int x = round(boxes[index].x * scale);
        int y = round(boxes[index].y * scale);
        int w = round(boxes[index].width * scale);
        int h = round(boxes[index].height * scale);

        // Make sure the box is within the frame
        x = max(0, x);
        y = max(0, y);
        w = min(w, frame.cols - x);
        h = min(h, frame.rows - y);

        // Crop the image
        Rect roi(x, y, w, h); // Define a region of interest
        Mat crop_img = frame(roi); // Crop the region from the frame
        images.push_back(crop_img);
        imgDict[crop_img] = roi;
    }
    return images;
}

static void drawRectangle(int event, int x, int y, int, void* param) {
    Mat& img = *(Mat*)param;

    switch (event) {
        case EVENT_LBUTTONDOWN:
            drawing = true;
            ix = x;
            iy = y;
            break;

        case EVENT_MOUSEMOVE:
            if (drawing) {
                Mat img_copy = img.clone();
                rectangle(img_copy, Point(ix, iy), Point(x, y), Scalar(0, 255, 0), 2);
                imshow("TRACKING", img_copy);
            }
            break;

        case EVENT_LBUTTONUP:
            drawing = false;
            rect = Rect(Point(ix, iy), Point(x, y));
            rectangle(img, rect, Scalar(0, 255, 0), 2);
            imshow("TRACKING", img);
            break;
    }
}

void extractFrames(const string& queryImgPath, const string& videoPath, Net* reidNet, const string& yoloPath, int resize_h = 384, int resize_w = 128, int batch_size = 1) {
    VideoCapture cap(videoPath);
    if (!cap.isOpened()) {
        cerr << "Error: Video could not be opened." << endl;
        return;
    }

    Net net = readNet(yoloPath);
    vector<Mat> queryImages;

    Mat queryImg;
    if (!queryImgPath.empty()) {
        queryImg = imread(queryImgPath);
        if (queryImg.empty()) {
            cerr << "Error: Query image could not be loaded." << endl;
            return;
        }
        queryImages.push_back(queryImg);
    } else {
        Mat firstFrame;
        cap.read(firstFrame);
        if (firstFrame.empty()) {
            cerr << "Error reading the video" << endl;
            return;
        }

        putText(firstFrame, "Draw Bounding Box on Target", Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 0, 0), 2);
        imshow("TRACKING", firstFrame);
        setMouseCallback("TRACKING", drawRectangle, &firstFrame);

        for(;;) {
            if (rect.width > 0 && rect.height > 0) {
                queryImg = firstFrame(rect).clone();
                queryImages.push_back(queryImg);
                break;
            }
            if (waitKey(1) == 'q' || waitKey(1) == 27) {
                return;
            }
        }
    }

    Mat frame;
    vector<vector<float>> queryFeatures;
    extractFeatures(queryImages, reidNet, resize_h, resize_w, queryFeatures, batch_size);


    for(;;) {
        if (!cap.read(frame) || frame.empty()) {
            break;
        }
        vector<Mat> detectedImages = yoloDetector(frame, net);

        vector<vector<float>> galleryFeatures;
        extractFeatures(detectedImages, reidNet, resize_h, resize_w, galleryFeatures, batch_size);

        int topk_idx = getTopK(queryFeatures, galleryFeatures);
        if (topk_idx != -1 && static_cast<int>(detectedImages.size()) > topk_idx) {
            Mat topImg = detectedImages[topk_idx];

            Rect bbox = imgDict[topImg];
            rectangle(frame, bbox, Scalar(0, 0, 255), 2);
            putText(frame, "Target", Point(bbox.x, bbox.y - 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 2);
        }

        putText(frame, "Tracking", Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 0, 0), 2);
        imshow("TRACKING", frame);
        if (waitKey(1) == 'q' || waitKey(1) == 27) {
            break;
        }
    }

    cap.release();
    destroyAllWindows();
}