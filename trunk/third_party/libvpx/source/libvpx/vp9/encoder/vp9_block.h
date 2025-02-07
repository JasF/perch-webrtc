/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VP9_ENCODER_VP9_BLOCK_H_
#define VP9_ENCODER_VP9_BLOCK_H_

#include "vp9/common/vp9_onyx.h"
#include "vp9/common/vp9_entropymv.h"
#include "vp9/common/vp9_entropy.h"
#include "vpx_ports/mem.h"
#include "vp9/common/vp9_onyxc_int.h"

// motion search site
typedef struct {
  MV mv;
  int offset;
} search_site;

typedef struct {
  int count;
  struct {
    MB_PREDICTION_MODE mode;
    int_mv mv;
    int_mv second_mv;
  } bmi[4];
} PARTITION_INFO;

// Structure to hold snapshot of coding context during the mode picking process
// TODO Do we need all of these?
typedef struct {
  MODE_INFO mic;
  PARTITION_INFO partition_info;
  int skip;
  int_mv best_ref_mv;
  int_mv second_best_ref_mv;
  int_mv ref_mvs[MAX_REF_FRAMES][MAX_MV_REF_CANDIDATES];
  int rate;
  int distortion;
  int64_t intra_error;
  int best_mode_index;
  int rddiv;
  int rdmult;
  int hybrid_pred_diff;
  int comp_pred_diff;
  int single_pred_diff;
  int64_t txfm_rd_diff[NB_TXFM_MODES];

  // Bit flag for each mode whether it has high error in comparison to others.
  unsigned int modes_with_high_error;

  // Bit flag for each ref frame whether it has high error compared to others.
  unsigned int frames_with_high_error;
} PICK_MODE_CONTEXT;

struct macroblock_plane {
  DECLARE_ALIGNED(16, int16_t, src_diff[64*64]);
  DECLARE_ALIGNED(16, int16_t, coeff[64*64]);
  struct buf_2d src;

  // Quantizer setings
  int16_t *quant;
  uint8_t *quant_shift;
  int16_t *zbin;
  int16_t *zrun_zbin_boost;
  int16_t *round;

  // Zbin Over Quant value
  int16_t zbin_extra;
};

typedef struct macroblock MACROBLOCK;
struct macroblock {
  struct macroblock_plane plane[MAX_MB_PLANE];

  MACROBLOCKD e_mbd;
  int skip_block;
  PARTITION_INFO *partition_info; /* work pointer */
  PARTITION_INFO *pi;   /* Corresponds to upper left visible macroblock */
  PARTITION_INFO *pip;  /* Base of allocated array */

  search_site *ss;
  int ss_count;
  int searches_per_step;

  int errorperbit;
  int sadperbit16;
  int sadperbit4;
  int rddiv;
  int rdmult;
  unsigned int *mb_activity_ptr;
  int *mb_norm_activity_ptr;
  signed int act_zbin_adj;

  int mv_best_ref_index[MAX_REF_FRAMES];

  int nmvjointcost[MV_JOINTS];
  int nmvcosts[2][MV_VALS];
  int *nmvcost[2];
  int nmvcosts_hp[2][MV_VALS];
  int *nmvcost_hp[2];
  int **mvcost;

  int nmvjointsadcost[MV_JOINTS];
  int nmvsadcosts[2][MV_VALS];
  int *nmvsadcost[2];
  int nmvsadcosts_hp[2][MV_VALS];
  int *nmvsadcost_hp[2];
  int **mvsadcost;

  int mbmode_cost[MB_MODE_COUNT];
  int intra_uv_mode_cost[2][MB_MODE_COUNT];
  int y_mode_costs[VP9_INTRA_MODES][VP9_INTRA_MODES][VP9_INTRA_MODES];
  int switchable_interp_costs[VP9_SWITCHABLE_FILTERS + 1]
                             [VP9_SWITCHABLE_FILTERS];

  // These define limits to motion vector components to prevent them
  // from extending outside the UMV borders
  int mv_col_min;
  int mv_col_max;
  int mv_row_min;
  int mv_row_max;

  int skip;

  int encode_breakout;

  unsigned char *active_ptr;

  // note that token_costs is the cost when eob node is skipped
  vp9_coeff_count token_costs[TX_SIZE_MAX_SB][BLOCK_TYPES];
  vp9_coeff_count token_costs_noskip[TX_SIZE_MAX_SB][BLOCK_TYPES];

  int optimize;

  // indicate if it is in the rd search loop or encoding process
  int rd_search;

  // TODO(jingning): Need to refactor the structure arrays that buffers the
  // coding mode decisions of each partition type.
  PICK_MODE_CONTEXT ab4x4_context[4][4][4];
  PICK_MODE_CONTEXT sb8x4_context[4][4][4];
  PICK_MODE_CONTEXT sb4x8_context[4][4][4];
  PICK_MODE_CONTEXT sb8x8_context[4][4][4];
  PICK_MODE_CONTEXT sb8x16_context[4][4][2];
  PICK_MODE_CONTEXT sb16x8_context[4][4][2];
  PICK_MODE_CONTEXT mb_context[4][4];
  PICK_MODE_CONTEXT sb32x16_context[4][2];
  PICK_MODE_CONTEXT sb16x32_context[4][2];
  // when 4 MBs share coding parameters:
  PICK_MODE_CONTEXT sb32_context[4];
  PICK_MODE_CONTEXT sb32x64_context[2];
  PICK_MODE_CONTEXT sb64x32_context[2];
  PICK_MODE_CONTEXT sb64_context;
  int partition_cost[NUM_PARTITION_CONTEXTS][PARTITION_TYPES];

  BLOCK_SIZE_TYPE b_partitioning[4][4][4];
  BLOCK_SIZE_TYPE mb_partitioning[4][4];
  BLOCK_SIZE_TYPE sb_partitioning[4];
  BLOCK_SIZE_TYPE sb64_partitioning;

  void (*fwd_txm4x4)(int16_t *input, int16_t *output, int pitch);
  void (*fwd_txm8x4)(int16_t *input, int16_t *output, int pitch);
  void (*fwd_txm8x8)(int16_t *input, int16_t *output, int pitch);
  void (*fwd_txm16x16)(int16_t *input, int16_t *output, int pitch);
  void (*quantize_b_4x4)(MACROBLOCK *x, int b_idx, TX_TYPE tx_type,
                         int y_blocks);
};

#endif  // VP9_ENCODER_VP9_BLOCK_H_
