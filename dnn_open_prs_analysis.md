# OpenCV DNN Open Pull Requests Analysis

> Generated: 2026-02-23
> Source: https://github.com/opencv/opencv (open PRs)
> Scope: All open PRs with `category: dnn`, `category: dnn (onnx)`, or DNN-related content
> Note: PRs authored by abhishek-gola excluded.

---

## Summary

| Category | Count |
|---|---|
| Targeting 5.x branch with 5.0-release milestone | 5 |
| Targeting 5.x branch (no milestone, active) | 4 |
| Targeting 4.x branch (still relevant) | 4 |
| Stale / Low priority | 2 |

---

## TIER 1 — Highly Relevant for OpenCV 5.x (5.0-release milestone assigned)

These PRs directly target the `5.x` branch and have the `5.0-release` milestone assigned by maintainers, indicating they are actively being tracked for the upcoming release.

### #28543 — dnn(onnx): Enable QLinear layers in dynamic engine for int8 models
- **URL:** https://github.com/opencv/opencv/pull/28543
- **Author:** shahkarnav115-beep
- **Labels:** `feature`, `category: dnn (onnx)`
- **Milestone:** 5.0-release
- **Base branch:** 5.x
- **Last updated:** 2026-02-22
- **Why relevant:** Adds quantized (int8) inference support for QLinearConv, QLinearMatMul etc. in the new ONNX dynamic engine. Critical for deploying quantized models efficiently. Tied to GSoC 2026 idea for ONNX QDQ inference. Companion PR: opencv_extra#1319.

### #28524 — CPU Kernels for fp32 KV Cache
- **URL:** https://github.com/opencv/opencv/pull/28524
- **Author:** nklskyoy
- **Labels:** `feature`, `category: dnn`
- **Milestone:** 5.0-release
- **Base branch:** 5.x
- **Last updated:** 2026-02-19
- **Why relevant:** Implements CPU kernels (pagedAttnQKGemm, pagedAttnAVGemm) for KV-Cache support required by LLM inference (paged attention). Foundational for GenAI/LLM workloads in OpenCV DNN.

### #28355 — features2d: Add DISK (Deep Image Structure and Keypoints) detector via DNN module
- **URL:** https://github.com/opencv/opencv/pull/28355
- **Author:** satyam102006
- **Labels:** `feature`, `category: features2d`, `category: dnn`
- **Milestone:** 5.0-release
- **Base branch:** 5.x
- **Last updated:** 2026-01-29
- **Why relevant:** Adds the DISK learned feature detector/descriptor using DNN inference. High-quality deep feature detector with 10 comments showing active review. Companion: opencv_extra#1313.

### #28262 — DNN: Add QLinearAdd/Mul support to new ONNX engine
- **URL:** https://github.com/opencv/opencv/pull/28262
- **Author:** satyam102006
- **Labels:** `feature`, `category: dnn (onnx)`
- **Milestone:** 5.0-release
- **Base branch:** 5.x
- **Last updated:** 2026-02-18
- **Why relevant:** Implements `QLinearAdd` and `QLinearMul` operator parsing in the new ONNX importer. Required for full int8 quantized model support. Fixes #26310. Verified with PyTorch opset 13 quantized models. 7 comments showing active review.

### #27534 — [WIP] GSoC 2025: Add Tokenizer Support to DNN Module
- **URL:** https://github.com/opencv/opencv/pull/27534
- **Author:** JorgeV92
- **Labels:** `feature`, `GSoC`, `category: dnn`
- **Milestone:** 5.0-release
- **Base branch:** 5.x
- **Last updated:** 2026-01-29
- **Why relevant:** Introduces a tokenizer module under `modules/dnn/src/tokenizer` as part of GSoC 2025. BPE tokenization for LLM/GenAI preprocessing. 21 comments showing significant community interest and review. Companion: opencv_extra#1276.

---

## TIER 2 — Active and Relevant for OpenCV 5.x (no milestone yet, but actively developed)

These PRs target the `5.x` branch, have recent activity, and address important DNN functionality.

### #28484 — Enabling KV-Cache support to AttentionOnnxAi layer
- **URL:** https://github.com/opencv/opencv/pull/28484
- **Author:** shahkarnav115-beep
- **Labels:** `feature`, `category: dnn (onnx)`
- **Base branch:** 5.x
- **Last updated:** 2026-02-12
- **Why relevant:** Adds past Key/Value cache support to the Attention layer, enabling efficient autoregressive LLM inference. Critical for transformer-based models. 5 active comments.

### #28473 — [DNN] Add Tokenizer API and BPE Implementation for GenAI Support (DRAFT)
- **URL:** https://github.com/opencv/opencv/pull/28473
- **Author:** satyam102006
- **Labels:** `feature`, `category: dnn`
- **Base branch:** 5.x
- **Last updated:** 2026-02-07
- **Note:** DRAFT — overlaps with #27534 (GSoC tokenizer). May be consolidated or superseded.
- **Why relevant:** Alternative/complementary tokenizer implementation. Relevant for GenAI pipeline completeness.

### #28327 — Fix #26349: Implement forward(layerName) support for new DNN engine
- **URL:** https://github.com/opencv/opencv/pull/28327
- **Author:** satyam102006
- **Labels:** `category: dnn`
- **Base branch:** 5.x
- **Last updated:** 2026-01-28
- **Why relevant:** Bugfix — `forward(outputName)` was crashing or throwing "Not Implemented" in the new engine. Required for intermediate layer inspection and extraction workflows. 7 comments.

### #24151 — Add resizeOnnx (RFC)
- **URL:** https://github.com/opencv/opencv/pull/24151
- **Author:** Ginkgo-Biloba
- **Labels:** `RFC`
- **Milestone:** 5.0-release
- **Base branch:** 4.x (tagged for 5.0-release)
- **Last updated:** 2025-05-14
- **Why relevant:** Adds proper ONNX-compliant resize operations. RFC label but assigned 5.0-release milestone. 8 comments. Core primitive needed for many ONNX models.

---

## TIER 3 — Relevant for OpenCV 4.x (targets 4.x branch, still applicable)

These PRs target the stable `4.x` branch and address legitimate bugs or optimizations.

### #28305 — dnn(cuda): fix zero-size allocation check and add defensive asserts in ManagedPtr
- **URL:** https://github.com/opencv/opencv/pull/28305
- **Author:** shahkarnav115-beep
- **Labels:** `category: dnn`, `category: gpu/cuda (contrib)`
- **Base branch:** 4.x
- **Last updated:** 2026-01-15
- **Why relevant:** Defensive fix for CUDA DNN's `ManagedPtr` zero-size allocation — prevents silent crashes/undefined behavior in CUDA DNN path on 4.x.

### #25923 — DNN: add TensorRT backend (DRAFT)
- **URL:** https://github.com/opencv/opencv/pull/25923
- **Author:** zihaomu
- **Labels:** `optimization`, `category: dnn`, `category: gpu/cuda (contrib)`, `pr: Discussion Required`
- **Base branch:** 4.x
- **Last updated:** 2025-12-09
- **Note:** DRAFT + `pr: Discussion Required` label. TensorRT 8.6 integration.
- **Why relevant:** Adding a native TensorRT backend to DNN is high-value for NVIDIA GPU deployment. The `Discussion Required` label means it needs architectural decisions before it can progress. Still potentially relevant for 4.x users.

### #24414 — Partially ported dnn resize_layer to T-API
- **URL:** https://github.com/opencv/opencv/pull/24414
- **Author:** kallaballa
- **Labels:** `optimization`, `category: dnn`
- **Base branch:** 4.x
- **Last updated:** 2023-11-17
- **Why relevant:** T-API (Transparent API / OpenCL) port for the DNN resize layer. Optimization PR that improves performance on OpenCL-capable hardware. Stalled since late 2023 but the optimization is still valid.

### #23689 — DNN: optimize quantize convolution layer
- **URL:** https://github.com/opencv/opencv/pull/23689
- **Author:** zihaomu
- **Labels:** `optimization`, `category: dnn`, `pr: needs test`
- **Milestone:** 5.0-release
- **Base branch:** 4.x
- **Last updated:** 2024-12-09
- **Why relevant:** int8 convolution optimization for ARMv8 (tested on Apple M1 with ~13ms → ~7ms improvement on ResNet50). Has `pr: needs test` label blocking merge. Despite targeting 4.x, has 5.0-release milestone. Important for embedded/mobile deployment.

---

## TIER 4 — Stale or Low Priority

These PRs have not been updated recently or face significant barriers to merging.

| PR | Title | Base | Last Updated | Status |
|---|---|---|---|---|
| #22181 | [GSoC] [WIP] Add Audio Visual Speech Recognition pre-processing functions | 4.x | 2023-11-08 | WIP, stalled since 2022 GSoC. Sample code, not core DNN. Low priority. |
| #18066 | [GSoC] GPU-enabled OpenCV.js, webgpu-dawn support for dnn module | 4.x | 2023-09-15 | Very old (2020 GSoC). WebGPU/Dawn landscape has changed significantly. Likely needs full rewrite. |

---

## Quick Reference — All DNN PRs by Priority

| Priority | PR # | Author | Title | Base | Milestone |
|---|---|---|---|---|---|
| HIGH | #28543 | shahkarnav115-beep | Enable QLinear layers (int8) in dynamic engine | 5.x | 5.0-release |
| HIGH | #28524 | nklskyoy | CPU Kernels for fp32 KV Cache | 5.x | 5.0-release |
| HIGH | #28355 | satyam102006 | DISK feature detector via DNN | 5.x | 5.0-release |
| HIGH | #28262 | satyam102006 | QLinearAdd/Mul in new ONNX engine | 5.x | 5.0-release |
| HIGH | #27534 | JorgeV92 | Tokenizer support (GSoC 2025) | 5.x | 5.0-release |
| MEDIUM | #28484 | shahkarnav115-beep | KV-Cache in AttentionOnnxAi layer | 5.x | — |
| MEDIUM | #28473 | satyam102006 | Tokenizer API + BPE (DRAFT) | 5.x | — |
| MEDIUM | #28327 | satyam102006 | forward(layerName) bugfix for new engine | 5.x | — |
| MEDIUM | #24151 | Ginkgo-Biloba | resizeOnnx (RFC) | 4.x | 5.0-release |
| LOW-4x | #28305 | shahkarnav115-beep | CUDA ManagedPtr defensive fix | 4.x | — |
| LOW-4x | #25923 | zihaomu | TensorRT backend (DRAFT) | 4.x | — |
| LOW-4x | #24414 | kallaballa | dnn resize_layer T-API port | 4.x | — |
| LOW-4x | #23689 | zihaomu | Quantize convolution optimization (ARM) | 4.x | 5.0-release |
| STALE | #22181 | spazewalker | Audio Visual Speech Recognition sample | 4.x | — |
| STALE | #18066 | NALLEIN | GPU-enabled OpenCV.js / WebGPU DNN | 4.x | — |
