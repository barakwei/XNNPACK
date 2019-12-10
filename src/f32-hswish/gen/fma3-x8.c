// Auto-generated file. Do not edit!
//   Template: src/f32-hswish/avx.c.in
//   Generator: tools/xngen
//
// Copyright 2019 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <immintrin.h>

#include <xnnpack/common.h>
#include <xnnpack/vbinary.h>


static const int32_t mask_table[14] = {-1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0};

void xnn_f32_hswish_ukernel__fma3_x8(
    size_t n,
    const float* x,
    float* y,
    const union xnn_f32_hswish_params params[restrict static 1])
{
  assert(n != 0);
  assert(n % sizeof(float) == 0);

  const __m256 vsixth = _mm256_broadcast_ps((const __m128*) params->sse.sixth);
  const __m256 vhalf = _mm256_broadcast_ps((const __m128*) params->sse.half);
  const __m256 vone = _mm256_broadcast_ps((const __m128*) params->sse.one);
  const __m256 vzero = _mm256_setzero_ps();

  for (; n >= 8 * sizeof(float); n -= 8 * sizeof(float)) {
    const __m256 vx456789AB = _mm256_loadu_ps(x);
    x += 8;

    __m256 vacc456789AB = _mm256_fmadd_ps(vx456789AB, vsixth, vhalf);

    vacc456789AB = _mm256_max_ps(vacc456789AB, vzero);

    vacc456789AB = _mm256_min_ps(vacc456789AB, vone);

    vacc456789AB = _mm256_mul_ps(vacc456789AB, vx456789AB);

    _mm256_storeu_ps(y, vacc456789AB);
    y += 8;
  }
  for (; n >= 8 * sizeof(float); n -= 8 * sizeof(float)) {
    const __m256 vx = _mm256_loadu_ps(x);
    x += 8;
    __m256 vacc = _mm256_fmadd_ps(vx, vsixth, vhalf);
    vacc = _mm256_max_ps(vacc, vzero);
    vacc = _mm256_min_ps(vacc, vone);
    vacc = _mm256_mul_ps(vacc, vx);
    _mm256_storeu_ps(y, vacc);
    y += 8;
  }
  if XNN_UNLIKELY(n != 0) {
    assert(n >= 1 * sizeof(float));
    assert(n <= 7 * sizeof(float));
    __m256i vmask = _mm256_loadu_si256((const __m256i*) ((uintptr_t) &mask_table[7] - n));

    const __m256 vx = _mm256_maskload_ps(x, vmask);
    __m256 vacc = _mm256_fmadd_ps(vx, vsixth, vhalf);
    vacc = _mm256_max_ps(vacc, vzero);
    vacc = _mm256_min_ps(vacc, vone);
    vacc = _mm256_mul_ps(vacc, vx);
    _mm256_maskstore_ps(y, vmask, vacc);
  }
}
