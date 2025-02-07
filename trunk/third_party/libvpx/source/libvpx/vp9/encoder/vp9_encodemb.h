/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VP9_ENCODER_VP9_ENCODEMB_H_
#define VP9_ENCODER_VP9_ENCODEMB_H_

#include "./vpx_config.h"
#include "vp9/encoder/vp9_block.h"
#include "vp9/encoder/vp9_onyx_int.h"
#include "vp9/common/vp9_onyxc_int.h"

typedef struct {
  MB_PREDICTION_MODE mode;
  MV_REFERENCE_FRAME ref_frame;
  MV_REFERENCE_FRAME second_ref_frame;
} MODE_DEFINITION;

struct optimize_ctx {
  ENTROPY_CONTEXT ta[MAX_MB_PLANE][16];
  ENTROPY_CONTEXT tl[MAX_MB_PLANE][16];
};

void vp9_optimize_init(MACROBLOCKD *xd, BLOCK_SIZE_TYPE bsize,
                       struct optimize_ctx *ctx);
void vp9_optimize_b(int plane, int block, BLOCK_SIZE_TYPE bsize,
                    int ss_txfrm_size, VP9_COMMON *cm, MACROBLOCK *x,
                    struct optimize_ctx *ctx);
void vp9_optimize_sby(VP9_COMMON *cm, MACROBLOCK *x, BLOCK_SIZE_TYPE bsize);
void vp9_optimize_sbuv(VP9_COMMON *cm, MACROBLOCK *x, BLOCK_SIZE_TYPE bsize);

void vp9_encode_sb(VP9_COMMON *cm, MACROBLOCK *x, BLOCK_SIZE_TYPE bsize);
void vp9_encode_sby(VP9_COMMON *cm, MACROBLOCK *x, BLOCK_SIZE_TYPE bsize);
void vp9_encode_sbuv(VP9_COMMON *cm, MACROBLOCK *x, BLOCK_SIZE_TYPE bsize);

void vp9_xform_quant_sby(VP9_COMMON *cm, MACROBLOCK *x, BLOCK_SIZE_TYPE bsize);
void vp9_xform_quant_sbuv(VP9_COMMON *cm, MACROBLOCK *x, BLOCK_SIZE_TYPE bsize);

void vp9_subtract_sby(MACROBLOCK *x, BLOCK_SIZE_TYPE bsize);
void vp9_subtract_sbuv(MACROBLOCK *x, BLOCK_SIZE_TYPE bsize);
void vp9_subtract_sb(MACROBLOCK *xd, BLOCK_SIZE_TYPE bsize);

#endif  // VP9_ENCODER_VP9_ENCODEMB_H_
