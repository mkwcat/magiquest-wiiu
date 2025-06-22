// From yuv2rgb - https://github.com/descampsa/yuv2rgb
// Modified by Palapeli
//
// Copyright (c) 2016 Adrien Descamps
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include <gctypes.h>
#include <stdint.h>

typedef struct {
    uint8_t cb_factor; // [(255*CbNorm)/CbRange]
    uint8_t cr_factor; // [(255*CrNorm)/CrRange]
    uint8_t g_cb_factor; // [Bf/Gf*(255*CbNorm)/CbRange]
    uint8_t g_cr_factor; // [Rf/Gf*(255*CrNorm)/CrRange]
    uint8_t y_factor; // [(YMax-YMin)/255]
    uint8_t y_offset; // YMin
} YUV2RGBParam;

#define FIXED_POINT_VALUE(value, precision) ((int) (((value) * (1 << precision)) + 0.5))

#define YUV2RGB_PARAM(Rf, Bf, YMin, YMax, CbCrRange)                                               \
    {.cb_factor = FIXED_POINT_VALUE(255.0 * (2.0 * (1 - Bf)) / CbCrRange, 6),                      \
      .cr_factor = FIXED_POINT_VALUE(255.0 * (2.0 * (1 - Rf)) / CbCrRange, 6),                     \
      .g_cb_factor =                                                                               \
        FIXED_POINT_VALUE(Bf / (1.0 - Bf - Rf) * 255.0 * (2.0 * (1 - Bf)) / CbCrRange, 7),         \
      .g_cr_factor =                                                                               \
        FIXED_POINT_VALUE(Rf / (1.0 - Bf - Rf) * 255.0 * (2.0 * (1 - Rf)) / CbCrRange, 7),         \
      .y_factor = FIXED_POINT_VALUE(255.0 / (YMax - YMin), 7),                                     \
      .y_offset = YMin}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"

static const YUV2RGBParam YUV2RGB[3] = {
  // ITU-T T.871 (JPEG)
  YUV2RGB_PARAM(0.299, 0.114, 0.0, 255.0, 255.0),
  // ITU-R BT.601-7
  YUV2RGB_PARAM(0.299, 0.114, 16.0, 235.0, 224.0),
  // ITU-R BT.709-6
  YUV2RGB_PARAM(0.2126, 0.0722, 16.0, 235.0, 224.0)};

#pragma GCC diagnostic pop

static inline uint8_t clamp(int16_t value)
{
    return value < 0 ? 0 : (value > 255 ? 255 : value);
}

static inline void nv12_rgba32_std(uint32_t width, uint32_t height, const uint8_t* Y,
  const uint8_t* UV, uint32_t Y_stride, uint32_t UV_stride, uint8_t* __restrict RGB,
  uint32_t RGB_stride, int yuv_type)
{
    const YUV2RGBParam* const param = &(YUV2RGB[yuv_type]);
    uint8_t cb_factor = param->cb_factor;
    uint8_t cr_factor = param->cr_factor;

    uint32_t x, y;
    for (y = 0; y < (height - 1); y += 2) {
        const uint8_t *y_ptr1 = Y + y * Y_stride, *y_ptr2 = Y + (y + 1) * Y_stride,
                      *uv_ptr = UV + (y / 2) * UV_stride;

        u32* __restrict rgb_ptr1 = (u32*) (RGB + y * RGB_stride);
        u32* __restrict rgb_ptr2 = (u32*) (RGB + (y + 1) * RGB_stride);

        for (x = 0; x < (width - 1); x += 2) {
            int16_t uv_tmp = *(uint16_t*) uv_ptr;
            int8_t u_tmp = (uv_tmp >> 8) - 128;
            int8_t v_tmp = (uv_tmp & 0xFF) - 128;

            // compute Cb Cr color offsets, common to four pixels
            int16_t b_cb_offset, r_cr_offset, g_cbcr_offset;
            b_cb_offset = (cb_factor * u_tmp) >> 6;
            r_cr_offset = (cr_factor * v_tmp) >> 6;
            g_cbcr_offset = (param->g_cb_factor * u_tmp + param->g_cr_factor * v_tmp) >> 7;

            int16_t y_tmp = (param->y_factor * (y_ptr1[0] - param->y_offset)) >> 7;
            rgb_ptr1[0] = 0xFF | (clamp(y_tmp + r_cr_offset) << 24) |
                          (clamp(y_tmp - g_cbcr_offset) << 16) | (clamp(y_tmp + b_cb_offset) << 8);

            y_tmp = (param->y_factor * (y_ptr1[1] - param->y_offset)) >> 7;
            rgb_ptr1[1] = 0xFF | (clamp(y_tmp + r_cr_offset) << 24) |
                          (clamp(y_tmp - g_cbcr_offset) << 16) | (clamp(y_tmp + b_cb_offset) << 8);

            y_tmp = (param->y_factor * (y_ptr2[0] - param->y_offset)) >> 7;
            rgb_ptr2[0] = 0xFF | (clamp(y_tmp + r_cr_offset) << 24) |
                          (clamp(y_tmp - g_cbcr_offset) << 16) | (clamp(y_tmp + b_cb_offset) << 8);

            y_tmp = (param->y_factor * (y_ptr2[1] - param->y_offset)) >> 7;
            rgb_ptr2[1] = 0xFF | (clamp(y_tmp + r_cr_offset) << 24) |
                          (clamp(y_tmp - g_cbcr_offset) << 16) | (clamp(y_tmp + b_cb_offset) << 8);

            rgb_ptr1 += 2;
            rgb_ptr2 += 2;
            y_ptr1 += 2;
            y_ptr2 += 2;
            uv_ptr += 2;
        }
    }
}
