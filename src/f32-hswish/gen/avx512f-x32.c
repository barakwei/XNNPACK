// Auto-generated file. Do not edit!
//   Template: src/f32-hswish/avx512f.c.in
//   Generator: tools/xngen
//
// Copyright 2019 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <immintrin.h>

#include <xnnpack/common.h>
#include <xnnpack/intrinsics-polyfill.h>
#include <xnnpack/vbinary.h>


void xnn_f32_hswish_ukernel__avx512f_x32(
    size_t n,
    const float* x,
    float* y,
    const union xnn_f32_hswish_params params[restrict static 1])
{
  assert(n != 0);
  assert(n % sizeof(float) == 0);

  const __m512 vsixth = _mm512_broadcast_f32x4(_mm_load_ps(params->sse.sixth));
  const __m512 vhalf = _mm512_broadcast_f32x4(_mm_load_ps(params->sse.half));
  const __m512 vone = _mm512_broadcast_f32x4(_mm_load_ps(params->sse.one));
  const __m512 vzero = _mm512_setzero_ps();

  for (; n >= 32 * sizeof(float); n -= 32 * sizeof(float)) {
    const __m512 vx456789ABCDEFGHIJ = _mm512_loadu_ps(x);
    const __m512 vxKLMNOPQRSTUVWXYZ = _mm512_loadu_ps(x + 16);
    x += 32;

    __m512 vacc456789ABCDEFGHIJ = _mm512_fmadd_ps(vx456789ABCDEFGHIJ, vsixth, vhalf);
    __m512 vaccKLMNOPQRSTUVWXYZ = _mm512_fmadd_ps(vxKLMNOPQRSTUVWXYZ, vsixth, vhalf);

    vacc456789ABCDEFGHIJ = _mm512_max_ps(vacc456789ABCDEFGHIJ, vzero);
    vaccKLMNOPQRSTUVWXYZ = _mm512_max_ps(vaccKLMNOPQRSTUVWXYZ, vzero);

    vacc456789ABCDEFGHIJ = _mm512_min_ps(vacc456789ABCDEFGHIJ, vone);
    vaccKLMNOPQRSTUVWXYZ = _mm512_min_ps(vaccKLMNOPQRSTUVWXYZ, vone);

    vacc456789ABCDEFGHIJ = _mm512_mul_ps(vacc456789ABCDEFGHIJ, vx456789ABCDEFGHIJ);
    vaccKLMNOPQRSTUVWXYZ = _mm512_mul_ps(vaccKLMNOPQRSTUVWXYZ, vxKLMNOPQRSTUVWXYZ);

    _mm512_storeu_ps(y, vacc456789ABCDEFGHIJ);
    _mm512_storeu_ps(y + 16, vaccKLMNOPQRSTUVWXYZ);
    y += 32;
  }
  for (; n >= 16 * sizeof(float); n -= 16 * sizeof(float)) {
    const __m512 vx = _mm512_loadu_ps(x);
    x += 16;
    __m512 vacc = _mm512_fmadd_ps(vx, vsixth, vhalf);
    vacc = _mm512_max_ps(vacc, vzero);
    vacc = _mm512_min_ps(vacc, vone);
    vacc = _mm512_mul_ps(vacc, vx);
    _mm512_storeu_ps(y, vacc);
    y += 16;
  }
  if XNN_UNLIKELY(n != 0) {
    assert(n >= 1 * sizeof(float));
    assert(n <= 16 * sizeof(float));
    // Prepare mask for valid 32-bit elements (depends on n).
    n >>= 2 /* log2(sizeof(float)) */;
    const __mmask16 vmask = _cvtu32_mask16((uint16_t) ((uint32_t) (UINT32_C(1) << n) - UINT32_C(1)));

    const __m512 vx = _mm512_maskz_loadu_ps(vmask, x);
    __m512 vacc = _mm512_fmadd_ps(vx, vsixth, vhalf);
    vacc = _mm512_max_ps(vacc, vzero);
    vacc = _mm512_min_ps(vacc, vone);
    vacc = _mm512_mul_ps(vacc, vx);
    _mm512_mask_storeu_ps(y, vmask, vacc);
  }
}
