# This file is generated. Do not edit.
# Copyright (c) 2013 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'sources': [
    '<(libvpx_source)/vp8/common/alloccommon.c',
    '<(libvpx_source)/vp8/common/alloccommon.h',
    '<(libvpx_source)/vp8/common/arm/armv6/bilinearfilter_v6.asm',
    '<(libvpx_source)/vp8/common/arm/armv6/copymem16x16_v6.asm',
    '<(libvpx_source)/vp8/common/arm/armv6/copymem8x4_v6.asm',
    '<(libvpx_source)/vp8/common/arm/armv6/copymem8x8_v6.asm',
    '<(libvpx_source)/vp8/common/arm/armv6/dc_only_idct_add_v6.asm',
    '<(libvpx_source)/vp8/common/arm/armv6/dequant_idct_v6.asm',
    '<(libvpx_source)/vp8/common/arm/armv6/dequantize_v6.asm',
    '<(libvpx_source)/vp8/common/arm/armv6/filter_v6.asm',
    '<(libvpx_source)/vp8/common/arm/armv6/idct_blk_v6.c',
    '<(libvpx_source)/vp8/common/arm/armv6/idct_v6.asm',
    '<(libvpx_source)/vp8/common/arm/armv6/intra4x4_predict_v6.asm',
    '<(libvpx_source)/vp8/common/arm/armv6/iwalsh_v6.asm',
    '<(libvpx_source)/vp8/common/arm/armv6/loopfilter_v6.asm',
    '<(libvpx_source)/vp8/common/arm/armv6/simpleloopfilter_v6.asm',
    '<(libvpx_source)/vp8/common/arm/armv6/sixtappredict8x4_v6.asm',
    '<(libvpx_source)/vp8/common/arm/armv6/vp8_sad16x16_armv6.asm',
    '<(libvpx_source)/vp8/common/arm/armv6/vp8_variance16x16_armv6.asm',
    '<(libvpx_source)/vp8/common/arm/armv6/vp8_variance8x8_armv6.asm',
    '<(libvpx_source)/vp8/common/arm/armv6/vp8_variance_halfpixvar16x16_h_armv6.asm',
    '<(libvpx_source)/vp8/common/arm/armv6/vp8_variance_halfpixvar16x16_hv_armv6.asm',
    '<(libvpx_source)/vp8/common/arm/armv6/vp8_variance_halfpixvar16x16_v_armv6.asm',
    '<(libvpx_source)/vp8/common/arm/bilinearfilter_arm.c',
    '<(libvpx_source)/vp8/common/arm/bilinearfilter_arm.h',
    '<(libvpx_source)/vp8/common/arm/dequantize_arm.c',
    '<(libvpx_source)/vp8/common/arm/filter_arm.c',
    '<(libvpx_source)/vp8/common/arm/loopfilter_arm.c',
    '<(libvpx_source)/vp8/common/arm/neon/bilinearpredict16x16_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/bilinearpredict4x4_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/bilinearpredict8x4_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/bilinearpredict8x8_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/buildintrapredictorsmby_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/copymem16x16_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/copymem8x4_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/copymem8x8_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/dc_only_idct_add_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/dequant_idct_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/dequantizeb_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/idct_blk_neon.c',
    '<(libvpx_source)/vp8/common/arm/neon/idct_dequant_0_2x_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/idct_dequant_full_2x_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/iwalsh_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/loopfilter_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/loopfiltersimplehorizontaledge_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/loopfiltersimpleverticaledge_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/mbloopfilter_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/sad16_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/sad8_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/save_reg_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/shortidct4x4llm_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/sixtappredict16x16_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/sixtappredict4x4_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/sixtappredict8x4_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/sixtappredict8x8_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/variance_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/vp8_subpixelvariance16x16_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/vp8_subpixelvariance16x16s_neon.asm',
    '<(libvpx_source)/vp8/common/arm/neon/vp8_subpixelvariance8x8_neon.asm',
    '<(libvpx_source)/vp8/common/arm/reconintra_arm.c',
    '<(libvpx_source)/vp8/common/arm/variance_arm.c',
    '<(libvpx_source)/vp8/common/blockd.c',
    '<(libvpx_source)/vp8/common/blockd.h',
    '<(libvpx_source)/vp8/common/coefupdateprobs.h',
    '<(libvpx_source)/vp8/common/common.h',
    '<(libvpx_source)/vp8/common/debugmodes.c',
    '<(libvpx_source)/vp8/common/default_coef_probs.h',
    '<(libvpx_source)/vp8/common/dequantize.c',
    '<(libvpx_source)/vp8/common/entropy.c',
    '<(libvpx_source)/vp8/common/entropy.h',
    '<(libvpx_source)/vp8/common/entropymode.c',
    '<(libvpx_source)/vp8/common/entropymode.h',
    '<(libvpx_source)/vp8/common/entropymv.c',
    '<(libvpx_source)/vp8/common/entropymv.h',
    '<(libvpx_source)/vp8/common/extend.c',
    '<(libvpx_source)/vp8/common/extend.h',
    '<(libvpx_source)/vp8/common/filter.c',
    '<(libvpx_source)/vp8/common/filter.h',
    '<(libvpx_source)/vp8/common/findnearmv.c',
    '<(libvpx_source)/vp8/common/findnearmv.h',
    '<(libvpx_source)/vp8/common/generic/systemdependent.c',
    '<(libvpx_source)/vp8/common/header.h',
    '<(libvpx_source)/vp8/common/idct_blk.c',
    '<(libvpx_source)/vp8/common/idctllm.c',
    '<(libvpx_source)/vp8/common/invtrans.h',
    '<(libvpx_source)/vp8/common/loopfilter.c',
    '<(libvpx_source)/vp8/common/loopfilter_filters.c',
    '<(libvpx_source)/vp8/common/loopfilter.h',
    '<(libvpx_source)/vp8/common/mbpitch.c',
    '<(libvpx_source)/vp8/common/mfqe.c',
    '<(libvpx_source)/vp8/common/modecont.c',
    '<(libvpx_source)/vp8/common/modecont.h',
    '<(libvpx_source)/vp8/common/mv.h',
    '<(libvpx_source)/vp8/common/onyxc_int.h',
    '<(libvpx_source)/vp8/common/onyxd.h',
    '<(libvpx_source)/vp8/common/onyx.h',
    '<(libvpx_source)/vp8/common/postproc.c',
    '<(libvpx_source)/vp8/common/postproc.h',
    '<(libvpx_source)/vp8/common/ppflags.h',
    '<(libvpx_source)/vp8/common/pragmas.h',
    '<(libvpx_source)/vp8/common/quant_common.c',
    '<(libvpx_source)/vp8/common/quant_common.h',
    '<(libvpx_source)/vp8/common/reconinter.c',
    '<(libvpx_source)/vp8/common/reconinter.h',
    '<(libvpx_source)/vp8/common/reconintra4x4.c',
    '<(libvpx_source)/vp8/common/reconintra4x4.h',
    '<(libvpx_source)/vp8/common/reconintra.c',
    '<(libvpx_source)/vp8/common/rtcd.c',
    '<(libvpx_source)/vp8/common/sad_c.c',
    '<(libvpx_source)/vp8/common/setupintrarecon.c',
    '<(libvpx_source)/vp8/common/setupintrarecon.h',
    '<(libvpx_source)/vp8/common/swapyv12buffer.c',
    '<(libvpx_source)/vp8/common/swapyv12buffer.h',
    '<(libvpx_source)/vp8/common/systemdependent.h',
    '<(libvpx_source)/vp8/common/threading.h',
    '<(libvpx_source)/vp8/common/treecoder.c',
    '<(libvpx_source)/vp8/common/treecoder.h',
    '<(libvpx_source)/vp8/common/variance_c.c',
    '<(libvpx_source)/vp8/common/variance.h',
    '<(libvpx_source)/vp8/common/vp8_entropymodedata.h',
    '<(libvpx_source)/vp8/decoder/dboolhuff.c',
    '<(libvpx_source)/vp8/decoder/dboolhuff.h',
    '<(libvpx_source)/vp8/decoder/decodemv.c',
    '<(libvpx_source)/vp8/decoder/decodemv.h',
    '<(libvpx_source)/vp8/decoder/decoderthreading.h',
    '<(libvpx_source)/vp8/decoder/decodframe.c',
    '<(libvpx_source)/vp8/decoder/detokenize.c',
    '<(libvpx_source)/vp8/decoder/detokenize.h',
    '<(libvpx_source)/vp8/decoder/onyxd_if.c',
    '<(libvpx_source)/vp8/decoder/onyxd_int.h',
    '<(libvpx_source)/vp8/decoder/threading.c',
    '<(libvpx_source)/vp8/decoder/treereader.h',
    '<(libvpx_source)/vp8/encoder/arm/armv5te/boolhuff_armv5te.asm',
    '<(libvpx_source)/vp8/encoder/arm/armv5te/vp8_packtokens_armv5.asm',
    '<(libvpx_source)/vp8/encoder/arm/armv5te/vp8_packtokens_mbrow_armv5.asm',
    '<(libvpx_source)/vp8/encoder/arm/armv5te/vp8_packtokens_partitions_armv5.asm',
    '<(libvpx_source)/vp8/encoder/arm/armv6/vp8_fast_quantize_b_armv6.asm',
    '<(libvpx_source)/vp8/encoder/arm/armv6/vp8_mse16x16_armv6.asm',
    '<(libvpx_source)/vp8/encoder/arm/armv6/vp8_short_fdct4x4_armv6.asm',
    '<(libvpx_source)/vp8/encoder/arm/armv6/vp8_subtract_armv6.asm',
    '<(libvpx_source)/vp8/encoder/arm/armv6/walsh_v6.asm',
    '<(libvpx_source)/vp8/encoder/arm/boolhuff_arm.c',
    '<(libvpx_source)/vp8/encoder/arm/dct_arm.c',
    '<(libvpx_source)/vp8/encoder/arm/neon/fastquantizeb_neon.asm',
    '<(libvpx_source)/vp8/encoder/arm/neon/picklpf_arm.c',
    '<(libvpx_source)/vp8/encoder/arm/neon/shortfdct_neon.asm',
    '<(libvpx_source)/vp8/encoder/arm/neon/subtract_neon.asm',
    '<(libvpx_source)/vp8/encoder/arm/neon/vp8_memcpy_neon.asm',
    '<(libvpx_source)/vp8/encoder/arm/neon/vp8_mse16x16_neon.asm',
    '<(libvpx_source)/vp8/encoder/arm/neon/vp8_shortwalsh4x4_neon.asm',
    '<(libvpx_source)/vp8/encoder/arm/quantize_arm.c',
    '<(libvpx_source)/vp8/encoder/bitstream.c',
    '<(libvpx_source)/vp8/encoder/bitstream.h',
    '<(libvpx_source)/vp8/encoder/block.h',
    '<(libvpx_source)/vp8/encoder/boolhuff.h',
    '<(libvpx_source)/vp8/encoder/dct.c',
    '<(libvpx_source)/vp8/encoder/dct_value_cost.h',
    '<(libvpx_source)/vp8/encoder/dct_value_tokens.h',
    '<(libvpx_source)/vp8/encoder/defaultcoefcounts.h',
    '<(libvpx_source)/vp8/encoder/denoising.c',
    '<(libvpx_source)/vp8/encoder/denoising.h',
    '<(libvpx_source)/vp8/encoder/encodeframe.c',
    '<(libvpx_source)/vp8/encoder/encodeframe.h',
    '<(libvpx_source)/vp8/encoder/encodeintra.c',
    '<(libvpx_source)/vp8/encoder/encodeintra.h',
    '<(libvpx_source)/vp8/encoder/encodemb.c',
    '<(libvpx_source)/vp8/encoder/encodemb.h',
    '<(libvpx_source)/vp8/encoder/encodemv.c',
    '<(libvpx_source)/vp8/encoder/encodemv.h',
    '<(libvpx_source)/vp8/encoder/ethreading.c',
    '<(libvpx_source)/vp8/encoder/firstpass.h',
    '<(libvpx_source)/vp8/encoder/lookahead.c',
    '<(libvpx_source)/vp8/encoder/lookahead.h',
    '<(libvpx_source)/vp8/encoder/mcomp.c',
    '<(libvpx_source)/vp8/encoder/mcomp.h',
    '<(libvpx_source)/vp8/encoder/modecosts.c',
    '<(libvpx_source)/vp8/encoder/modecosts.h',
    '<(libvpx_source)/vp8/encoder/mr_dissim.c',
    '<(libvpx_source)/vp8/encoder/mr_dissim.h',
    '<(libvpx_source)/vp8/encoder/onyx_if.c',
    '<(libvpx_source)/vp8/encoder/onyx_int.h',
    '<(libvpx_source)/vp8/encoder/pickinter.c',
    '<(libvpx_source)/vp8/encoder/pickinter.h',
    '<(libvpx_source)/vp8/encoder/picklpf.c',
    '<(libvpx_source)/vp8/encoder/psnr.c',
    '<(libvpx_source)/vp8/encoder/psnr.h',
    '<(libvpx_source)/vp8/encoder/quantize.c',
    '<(libvpx_source)/vp8/encoder/quantize.h',
    '<(libvpx_source)/vp8/encoder/ratectrl.c',
    '<(libvpx_source)/vp8/encoder/ratectrl.h',
    '<(libvpx_source)/vp8/encoder/rdopt.c',
    '<(libvpx_source)/vp8/encoder/rdopt.h',
    '<(libvpx_source)/vp8/encoder/segmentation.c',
    '<(libvpx_source)/vp8/encoder/segmentation.h',
    '<(libvpx_source)/vp8/encoder/tokenize.c',
    '<(libvpx_source)/vp8/encoder/tokenize.h',
    '<(libvpx_source)/vp8/encoder/treewriter.c',
    '<(libvpx_source)/vp8/encoder/treewriter.h',
    '<(libvpx_source)/vp8/vp8_cx_iface.c',
    '<(libvpx_source)/vp8/vp8_dx_iface.c',
    '<(libvpx_source)/vp9/common/generic/vp9_systemdependent.c',
    '<(libvpx_source)/vp9/common/vp9_alloccommon.c',
    '<(libvpx_source)/vp9/common/vp9_alloccommon.h',
    '<(libvpx_source)/vp9/common/vp9_blockd.h',
    '<(libvpx_source)/vp9/common/vp9_common.h',
    '<(libvpx_source)/vp9/common/vp9_convolve.c',
    '<(libvpx_source)/vp9/common/vp9_convolve.h',
    '<(libvpx_source)/vp9/common/vp9_debugmodes.c',
    '<(libvpx_source)/vp9/common/vp9_default_coef_probs.h',
    '<(libvpx_source)/vp9/common/vp9_entropy.c',
    '<(libvpx_source)/vp9/common/vp9_entropy.h',
    '<(libvpx_source)/vp9/common/vp9_entropymode.c',
    '<(libvpx_source)/vp9/common/vp9_entropymode.h',
    '<(libvpx_source)/vp9/common/vp9_entropymv.c',
    '<(libvpx_source)/vp9/common/vp9_entropymv.h',
    '<(libvpx_source)/vp9/common/vp9_enums.h',
    '<(libvpx_source)/vp9/common/vp9_extend.c',
    '<(libvpx_source)/vp9/common/vp9_extend.h',
    '<(libvpx_source)/vp9/common/vp9_filter.c',
    '<(libvpx_source)/vp9/common/vp9_filter.h',
    '<(libvpx_source)/vp9/common/vp9_findnearmv.c',
    '<(libvpx_source)/vp9/common/vp9_findnearmv.h',
    '<(libvpx_source)/vp9/common/vp9_idct.c',
    '<(libvpx_source)/vp9/common/vp9_idct.h',
    '<(libvpx_source)/vp9/common/vp9_loopfilter.c',
    '<(libvpx_source)/vp9/common/vp9_loopfilter_filters.c',
    '<(libvpx_source)/vp9/common/vp9_loopfilter.h',
    '<(libvpx_source)/vp9/common/vp9_mbpitch.c',
    '<(libvpx_source)/vp9/common/vp9_modecont.c',
    '<(libvpx_source)/vp9/common/vp9_modecontext.c',
    '<(libvpx_source)/vp9/common/vp9_modecont.h',
    '<(libvpx_source)/vp9/common/vp9_mv.h',
    '<(libvpx_source)/vp9/common/vp9_mvref_common.c',
    '<(libvpx_source)/vp9/common/vp9_mvref_common.h',
    '<(libvpx_source)/vp9/common/vp9_onyxc_int.h',
    '<(libvpx_source)/vp9/common/vp9_onyx.h',
    '<(libvpx_source)/vp9/common/vp9_postproc.c',
    '<(libvpx_source)/vp9/common/vp9_postproc.h',
    '<(libvpx_source)/vp9/common/vp9_ppflags.h',
    '<(libvpx_source)/vp9/common/vp9_pragmas.h',
    '<(libvpx_source)/vp9/common/vp9_pred_common.c',
    '<(libvpx_source)/vp9/common/vp9_pred_common.h',
    '<(libvpx_source)/vp9/common/vp9_quant_common.c',
    '<(libvpx_source)/vp9/common/vp9_quant_common.h',
    '<(libvpx_source)/vp9/common/vp9_reconinter.c',
    '<(libvpx_source)/vp9/common/vp9_reconinter.h',
    '<(libvpx_source)/vp9/common/vp9_reconintra.c',
    '<(libvpx_source)/vp9/common/vp9_reconintra.h',
    '<(libvpx_source)/vp9/common/vp9_rtcd.c',
    '<(libvpx_source)/vp9/common/vp9_sadmxn.h',
    '<(libvpx_source)/vp9/common/vp9_seg_common.c',
    '<(libvpx_source)/vp9/common/vp9_seg_common.h',
    '<(libvpx_source)/vp9/common/vp9_subpelvar.h',
    '<(libvpx_source)/vp9/common/vp9_systemdependent.h',
    '<(libvpx_source)/vp9/common/vp9_textblit.h',
    '<(libvpx_source)/vp9/common/vp9_tile_common.c',
    '<(libvpx_source)/vp9/common/vp9_tile_common.h',
    '<(libvpx_source)/vp9/common/vp9_treecoder.c',
    '<(libvpx_source)/vp9/common/vp9_treecoder.h',
    '<(libvpx_source)/vp9/decoder/vp9_dboolhuff.c',
    '<(libvpx_source)/vp9/decoder/vp9_dboolhuff.h',
    '<(libvpx_source)/vp9/decoder/vp9_decodemv.c',
    '<(libvpx_source)/vp9/decoder/vp9_decodemv.h',
    '<(libvpx_source)/vp9/decoder/vp9_decodframe.c',
    '<(libvpx_source)/vp9/decoder/vp9_decodframe.h',
    '<(libvpx_source)/vp9/decoder/vp9_detokenize.c',
    '<(libvpx_source)/vp9/decoder/vp9_detokenize.h',
    '<(libvpx_source)/vp9/decoder/vp9_idct_blk.c',
    '<(libvpx_source)/vp9/decoder/vp9_idct_blk.h',
    '<(libvpx_source)/vp9/decoder/vp9_onyxd.h',
    '<(libvpx_source)/vp9/decoder/vp9_onyxd_if.c',
    '<(libvpx_source)/vp9/decoder/vp9_onyxd_int.h',
    '<(libvpx_source)/vp9/decoder/vp9_read_bit_buffer.h',
    '<(libvpx_source)/vp9/decoder/vp9_treereader.h',
    '<(libvpx_source)/vp9/vp9_dx_iface.c',
    '<(libvpx_source)/vp9/vp9_iface_common.h',
    '<(libvpx_source)/vpx/internal/vpx_codec_internal.h',
    '<(libvpx_source)/vpx_mem/include/vpx_mem_intrnl.h',
    '<(libvpx_source)/vpx_mem/vpx_mem.c',
    '<(libvpx_source)/vpx_mem/vpx_mem.h',
    '<(libvpx_source)/vpx_ports/arm_cpudetect.c',
    '<(libvpx_source)/vpx_ports/arm.h',
    '<(libvpx_source)/vpx_ports/asm_offsets.h',
    '<(libvpx_source)/vpx_ports/emmintrin_compat.h',
    '<(libvpx_source)/vpx_ports/mem.h',
    '<(libvpx_source)/vpx_ports/vpx_once.h',
    '<(libvpx_source)/vpx_ports/vpx_timer.h',
    '<(libvpx_source)/vpx_scale/arm/neon/vp8_vpxyv12_copyframe_func_neon.asm',
    '<(libvpx_source)/vpx_scale/arm/neon/vp8_vpxyv12_copysrcframe_func_neon.asm',
    '<(libvpx_source)/vpx_scale/arm/neon/vp8_vpxyv12_copy_y_neon.asm',
    '<(libvpx_source)/vpx_scale/arm/neon/vp8_vpxyv12_extendframeborders_neon.asm',
    '<(libvpx_source)/vpx_scale/arm/neon/yv12extend_arm.c',
    '<(libvpx_source)/vpx_scale/generic/gen_scalers.c',
    '<(libvpx_source)/vpx_scale/generic/vpx_scale.c',
    '<(libvpx_source)/vpx_scale/generic/yv12config.c',
    '<(libvpx_source)/vpx_scale/generic/yv12extend.c',
    '<(libvpx_source)/vpx_scale/vpx_scale.h',
    '<(libvpx_source)/vpx_scale/vpx_scale_rtcd.c',
    '<(libvpx_source)/vpx_scale/yv12config.h',
    '<(libvpx_source)/vpx/src/vpx_codec.c',
    '<(libvpx_source)/vpx/src/vpx_decoder.c',
    '<(libvpx_source)/vpx/src/vpx_encoder.c',
    '<(libvpx_source)/vpx/src/vpx_image.c',
    '<(libvpx_source)/vpx/vp8cx.h',
    '<(libvpx_source)/vpx/vp8dx.h',
    '<(libvpx_source)/vpx/vp8.h',
    '<(libvpx_source)/vpx/vpx_codec.h',
    '<(libvpx_source)/vpx/vpx_codec_impl_bottom.h',
    '<(libvpx_source)/vpx/vpx_codec_impl_top.h',
    '<(libvpx_source)/vpx/vpx_decoder.h',
    '<(libvpx_source)/vpx/vpx_encoder.h',
    '<(libvpx_source)/vpx/vpx_image.h',
    '<(libvpx_source)/vpx/vpx_integer.h',
  ],
}
