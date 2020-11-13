// Auto-generated file. Do not edit!
//   Template: src/f16-spmm/neonfp16arith.c.in
//   Generator: tools/xngen
//
// Copyright 2019 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <arm_neon.h>

#include <xnnpack/spmm.h>


void xnn_f16_spmm_minmax_ukernel_24x1__neonfp16arith_x2(
    size_t batch_size,
    size_t output_channels,
    const void*restrict input,
    const void*restrict weights,
    const int32_t*restrict widx_dmap,
    const uint32_t*restrict nidx_nnzmap,
    void*restrict output,
    const struct xnn_f16_scaleminmax_params params[restrict XNN_MIN_ELEMENTS(1)])
{
  assert(batch_size != 0);

  const __fp16*restrict i = (const __fp16*) input;
  __fp16*restrict o = (__fp16*) output;

  const float16x8_t vscale = vld1q_dup_f16((const __fp16*) &params->scale);
  const float16x8_t vmax = vld1q_dup_f16((const __fp16*) &params->max);
  const float16x8_t vmin = vld1q_dup_f16((const __fp16*) &params->min);

  size_t n = batch_size;
  while XNN_LIKELY(n >= 24) {
    const __fp16*restrict w = (const __fp16*) weights;
    const int32_t* dmap = widx_dmap;
    const uint32_t* nnzmap = nidx_nnzmap;
    size_t c = output_channels;
    do {
      uint32_t nnz = *nnzmap++;
      float16x8_t vacc01234567x0 = vld1q_dup_f16(w); w += 1;
      float16x8_t vacc01234567x1 = vmovq_n_f16(0.0f);
      float16x8_t vacc89ABCDEFx0 = vacc01234567x0;
      float16x8_t vacc89ABCDEFx1 = vmovq_n_f16(0.0f);
      float16x8_t vaccGHIJKLMNx0 = vacc01234567x0;
      float16x8_t vaccGHIJKLMNx1 = vmovq_n_f16(0.0f);
      for (; nnz >= 2; nnz -= 2) {
        const intptr_t diff0 = dmap[0];
        const intptr_t diff1 = dmap[1];
        dmap += 2;
        const float16x8_t va01234567x0 = vld1q_f16(i);
        const float16x8_t va89ABCDEFx0 = vld1q_f16(i + 8);
        const float16x8_t vaGHIJKLMNx0 = vld1q_f16(i + 16);
        i = (const __fp16*restrict) ((uintptr_t) i + (uintptr_t) diff0);
        const float16x8_t vb0 = vld1q_dup_f16(w); w += 1;
        vacc01234567x0 = vfmaq_f16(vacc01234567x0, va01234567x0, vb0);
        vacc89ABCDEFx0 = vfmaq_f16(vacc89ABCDEFx0, va89ABCDEFx0, vb0);
        vaccGHIJKLMNx0 = vfmaq_f16(vaccGHIJKLMNx0, vaGHIJKLMNx0, vb0);
        const float16x8_t va01234567x1 = vld1q_f16(i);
        const float16x8_t va89ABCDEFx1 = vld1q_f16(i + 8);
        const float16x8_t vaGHIJKLMNx1 = vld1q_f16(i + 16);
        i = (const __fp16*restrict) ((uintptr_t) i + (uintptr_t) diff1);
        const float16x8_t vb1 = vld1q_dup_f16(w); w += 1;
        vacc01234567x1 = vfmaq_f16(vacc01234567x1, va01234567x1, vb1);
        vacc89ABCDEFx1 = vfmaq_f16(vacc89ABCDEFx1, va89ABCDEFx1, vb1);
        vaccGHIJKLMNx1 = vfmaq_f16(vaccGHIJKLMNx1, vaGHIJKLMNx1, vb1);
      }
      float16x8_t vacc01234567 = vacc01234567x0;
      float16x8_t vacc89ABCDEF = vacc89ABCDEFx0;
      float16x8_t vaccGHIJKLMN = vaccGHIJKLMNx0;
      vacc01234567 = vaddq_f16(vacc01234567, vacc01234567x1);
      vacc89ABCDEF = vaddq_f16(vacc89ABCDEF, vacc89ABCDEFx1);
      vaccGHIJKLMN = vaddq_f16(vaccGHIJKLMN, vaccGHIJKLMNx1);
      if XNN_LIKELY(nnz != 0) {
        do {
          const intptr_t diff = *dmap++;
          const float16x8_t va01234567 = vld1q_f16(i);
          const float16x8_t va89ABCDEF = vld1q_f16(i + 8);
          const float16x8_t vaGHIJKLMN = vld1q_f16(i + 16);
          i = (const __fp16*restrict) ((uintptr_t) i + (uintptr_t) diff);
          const float16x8_t vb = vld1q_dup_f16(w); w += 1;
          vacc01234567 = vfmaq_f16(vacc01234567, va01234567, vb);
          vacc89ABCDEF = vfmaq_f16(vacc89ABCDEF, va89ABCDEF, vb);
          vaccGHIJKLMN = vfmaq_f16(vaccGHIJKLMN, vaGHIJKLMN, vb);
        } while (--nnz != 0);
      }
      float16x8_t vout01234567 = vmulq_f16(vacc01234567, vscale);
      float16x8_t vout89ABCDEF = vmulq_f16(vacc89ABCDEF, vscale);
      float16x8_t voutGHIJKLMN = vmulq_f16(vaccGHIJKLMN, vscale);
      vout01234567 = vminq_f16(vout01234567, vmax);
      vout89ABCDEF = vminq_f16(vout89ABCDEF, vmax);
      voutGHIJKLMN = vminq_f16(voutGHIJKLMN, vmax);
      vout01234567 = vmaxq_f16(vout01234567, vmin);
      vout89ABCDEF = vmaxq_f16(vout89ABCDEF, vmin);
      voutGHIJKLMN = vmaxq_f16(voutGHIJKLMN, vmin);
      vst1q_f16(o, vout01234567);
      vst1q_f16(o + 8, vout89ABCDEF);
      vst1q_f16(o + 16, voutGHIJKLMN);
      o += batch_size;
    } while (--c != 0);
    o -= batch_size * output_channels;
    o += 24;
    i += 24;
    n -= 24;
  }
  if XNN_UNLIKELY(n != 0) {
    if (n & 16) {
      const __fp16*restrict w = (const __fp16*) weights;
      const int32_t* dmap = widx_dmap;
      const uint32_t* nnzmap = nidx_nnzmap;
      size_t c = output_channels;
      do {
        uint32_t nnz = *nnzmap++;
        float16x8_t vacc01234567 = vld1q_dup_f16(w); w += 1;
        float16x8_t vacc89ABCDEF = vacc01234567;
        if XNN_LIKELY(nnz != 0) {
          do {
            const intptr_t diff = *dmap++;
            const float16x8_t va01234567 = vld1q_f16(i);
            const float16x8_t va89ABCDEF = vld1q_f16(i + 8);
            i = (const __fp16*restrict) ((uintptr_t) i + (uintptr_t) diff);
            const float16x8_t vb = vld1q_dup_f16(w); w += 1;
            vacc01234567 = vfmaq_f16(vacc01234567, va01234567, vb);
            vacc89ABCDEF = vfmaq_f16(vacc89ABCDEF, va89ABCDEF, vb);
          } while (--nnz != 0);
        }
        float16x8_t vout01234567 = vminq_f16(vacc01234567, vmax);
        float16x8_t vout89ABCDEF = vminq_f16(vacc89ABCDEF, vmax);
        vout01234567 = vmaxq_f16(vout01234567, vmin);
        vout89ABCDEF = vmaxq_f16(vout89ABCDEF, vmin);
        vst1q_f16(o, vout01234567);
        vst1q_f16(o + 8, vout89ABCDEF);
        o += batch_size;
      } while (--c != 0);
      o -= batch_size * output_channels;
      o += 16;
      i += 16;
    }
    if (n & 8) {
      const __fp16*restrict w = (const __fp16*) weights;
      const int32_t* dmap = widx_dmap;
      const uint32_t* nnzmap = nidx_nnzmap;
      size_t c = output_channels;
      do {
        uint32_t nnz = *nnzmap++;
        float16x8_t vacc01234567 = vld1q_dup_f16(w); w += 1;
        if XNN_LIKELY(nnz != 0) {
          do {
            const intptr_t diff = *dmap++;
            const float16x8_t va01234567 = vld1q_f16(i);
            i = (const __fp16*restrict) ((uintptr_t) i + (uintptr_t) diff);
            const float16x8_t vb = vld1q_dup_f16(w); w += 1;
            vacc01234567 = vfmaq_f16(vacc01234567, va01234567, vb);
          } while (--nnz != 0);
        }
        float16x8_t vout01234567 = vminq_f16(vacc01234567, vmax);
        vout01234567 = vmaxq_f16(vout01234567, vmin);
        vst1q_f16(o, vout01234567);
        o += batch_size;
      } while (--c != 0);
      o -= batch_size * output_channels;
      o += 8;
      i += 8;
    }
    if (n & 4) {
      const __fp16*restrict w = (const __fp16*) weights;
      const int32_t* dmap = widx_dmap;
      const uint32_t* nnzmap = nidx_nnzmap;
      size_t c = output_channels;
      do {
        uint32_t nnz = *nnzmap++;
        float16x4_t vacc0123 = vld1_dup_f16(w); w += 1;
        if XNN_LIKELY(nnz != 0) {
          do {
            const intptr_t diff = *dmap++;
            const float16x4_t va0123 = vld1_f16(i);
            i = (const __fp16*restrict) ((uintptr_t) i + (uintptr_t) diff);
            const float16x4_t vb = vld1_dup_f16(w); w += 1;
            vacc0123 = vfma_f16(vacc0123, va0123, vb);
          } while (--nnz != 0);
        }
        float16x4_t vout0123 = vmin_f16(vacc0123, vget_low_f16(vmax));
        vout0123 = vmax_f16(vout0123, vget_low_f16(vmin));
        vst1_f16(o, vout0123);
        o += batch_size;
      } while (--c != 0);
      o -= batch_size * output_channels;
      o += 4;
      i += 4;
    }
    if (n & 2) {
      const __fp16*restrict w = (const __fp16*) weights;
      const int32_t* dmap = widx_dmap;
      const uint32_t* nnzmap = nidx_nnzmap;
      size_t c = output_channels;
      do {
        uint32_t nnz = *nnzmap++;
        float16x4_t vacc01 = vld1_dup_f16(w); w += 1;
        if XNN_LIKELY(nnz != 0) {
          do {
            const intptr_t diff = *dmap++;
            const float16x4_t va01 = vreinterpret_f16_f32(vld1_dup_f32(__builtin_assume_aligned(i, 1)));
            i = (const __fp16*restrict) ((uintptr_t) i + (uintptr_t) diff);
            const float16x4_t vb = vld1_dup_f16(w); w += 1;
            vacc01 = vfma_f16(vacc01, va01, vb);
          } while (--nnz != 0);
        }
        float16x4_t vout01 = vmin_f16(vacc01, vget_low_f16(vmax));
        vout01 = vmax_f16(vout01, vget_low_f16(vmin));
        vst1_lane_f32(__builtin_assume_aligned(o, 1), vreinterpret_f32_f16(vout01), 0);
        o += batch_size;
      } while (--c != 0);
      o -= batch_size * output_channels;
      o += 2;
      i += 2;
    }
    if (n & 1) {
      const __fp16*restrict w = (const __fp16*) weights;
      const int32_t* dmap = widx_dmap;
      const uint32_t* nnzmap = nidx_nnzmap;
      size_t c = output_channels;
      do {
        uint32_t nnz = *nnzmap++;
        float16x4_t vacc0 = vld1_dup_f16(w); w += 1;
        if XNN_LIKELY(nnz != 0) {
          do {
            const intptr_t diff = *dmap++;
            const float16x4_t va0 = vld1_dup_f16(i);
            i = (const __fp16*restrict) ((uintptr_t) i + (uintptr_t) diff);
            const float16x4_t vb = vld1_dup_f16(w); w += 1;
            vacc0 = vfma_f16(vacc0, va0, vb);
          } while (--nnz != 0);
        }
        float16x4_t vout0 = vmin_f16(vacc0, vget_low_f16(vmax));
        vout0 = vmax_f16(vout0, vget_low_f16(vmin));
        vst1_lane_f16(o, vout0, 0);
        o += batch_size;
      } while (--c != 0);
      o -= batch_size * output_channels;
      o += 1;
      i += 1;
    }
  }
}