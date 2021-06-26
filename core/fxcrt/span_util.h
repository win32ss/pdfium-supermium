// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CORE_FXCRT_SPAN_UTIL_H_
#define CORE_FXCRT_SPAN_UTIL_H_

#include <vector>

#include "third_party/base/check_op.h"
#include "third_party/base/span.h"

namespace fxcrt {

// NOTE: there are separate versions fot span<T> and span<const T>
// source arguments because type deduction seems to have trouble with
// automatically converting span<T> to span<const T>.

// Bounds-checked copies from spans into spans.
template <typename T>
void spancpy(pdfium::span<T> dst, pdfium::span<const T> src) {
  CHECK_GE(dst.size(), src.size());
  memcpy(dst.data(), src.data(), src.size_bytes());
}

template <typename T>
void spancpy(pdfium::span<T> dst, pdfium::span<T> src) {
  CHECK_GE(dst.size(), src.size());
  memcpy(dst.data(), src.data(), src.size_bytes());
}

// Bounds-checked moves from spans into spans.
template <typename T>
void spanmove(pdfium::span<T> dst, pdfium::span<T> src) {
  CHECK_GE(dst.size(), src.size());
  memmove(dst.data(), src.data(), src.size_bytes());
}

template <typename T>
void spanmove(pdfium::span<T> dst, pdfium::span<const T> src) {
  CHECK_GE(dst.size(), src.size());
  memmove(dst.data(), src.data(), src.size_bytes());
}

// Bounds-checked sets into spans.
template <typename T>
void spanset(pdfium::span<T> dst, uint8_t val) {
  memset(dst.data(), val, dst.size_bytes());
}

// Bounds-checked zeroing of spans.
template <typename T>
void spanclr(pdfium::span<T> dst) {
  memset(dst.data(), 0, dst.size_bytes());
}

// Extracting subspans from arrays.
template <typename T, typename A>
pdfium::span<T> Subspan(std::vector<T, A>& vec,
                        size_t start,
                        size_t count = -1) {
  return pdfium::make_span(vec).subspan(start, count);
}

}  // namespace fxcrt

#endif  // CORE_FXCRT_SPAN_UTIL_H_
