#include "platform_info.h"
#include "ervp_assert.h"
#include "ervp_printf.h"
#include "ervp_blocked_matrix_op.h"
#include "ervp_matrix_op_sw.h"

#include "ip_instance_info.h"
#include "dca_matrix_conv2d.h"

static const int PRINT_DCA_MATRIX_CONV2D_FUNCTION = 0;

ervp_blocked_matrix_info_t *blocked_info_matrix_conv2d;

ervp_hwtask_busy_fx_t i_dca_matrix_conv2d00_conv_oneblock(ervp_mop_mapping_t *mop_mapping, const ErvpMatrixInfo *ma_info, const ErvpMatrixInfo *mb_info, ErvpMatrixInfo *mc_info, unsigned int conv_options)
{
  if (PRINT_DCA_MATRIX_CONV2D_FUNCTION)
    printf_function();
  return dca_matrix_conv2d_oneblock(mop_mapping, i_dca_matrix_conv2d00_info, ma_info, mb_info, mc_info, conv_options);
}

ervp_hwtask_busy_fx_t i_dca_matrix_conv2d00_conv_oneblock_sharedoutput(ervp_mop_mapping_t *mop_mapping, int num_input, const ErvpMatrixInfo **input_info_list, const ErvpMatrixInfo **kernel_info_list, ErvpMatrixInfo *output_info, unsigned int conv_options)
{
  if (PRINT_DCA_MATRIX_CONV2D_FUNCTION)
    printf_function();
  return dca_matrix_conv2d_oneblock_sharedoutput(mop_mapping, i_dca_matrix_conv2d00_info, num_input, input_info_list, kernel_info_list, output_info, conv_options);
}

////////////////////////////////////////////////////////////////////////////

ervp_hwtask_busy_fx_t i_blocked_dca_matrix_conv2d00_conv(ervp_mop_mapping_t *mop_mapping, const ErvpMatrixInfo *ma_info, const ErvpMatrixInfo *mb_info, ErvpMatrixInfo *mc_info, unsigned int conv_options)
{
  blocked_info_matrix_conv2d->block_size = i_dca_matrix_conv2d00_info->output_matrix_size + mb_info->num_row - 1;
  return blocked_matrix_conv(blocked_info_matrix_conv2d, ma_info, mb_info, mc_info, conv_options);
}

ervp_hwtask_busy_fx_t i_blocked_dca_matrix_conv2d00_conv_sharedinput(ervp_mop_mapping_t *mop_mapping, int num_output, const ErvpMatrixInfo *input_info, const ErvpMatrixInfo **kernel_info_list, ErvpMatrixInfo **output_info_list, unsigned int conv_options)
{
  blocked_info_matrix_conv2d->block_size = i_dca_matrix_conv2d00_info->output_matrix_size + kernel_info_list[0]->num_row - 1;
  return blocked_matrix_conv_sharedinput(blocked_info_matrix_conv2d, num_output, input_info, kernel_info_list, output_info_list, conv_options);
}

ervp_hwtask_busy_fx_t i_blocked_dca_matrix_conv2d00_conv_sharedoutput(ervp_mop_mapping_t *mop_mapping, int num_input, const ErvpMatrixInfo **input_info_list, const ErvpMatrixInfo **kernel_info_list, ErvpMatrixInfo *output_info, unsigned int conv_options)
{
  blocked_info_matrix_conv2d->block_size = i_dca_matrix_conv2d00_info->output_matrix_size + kernel_info_list[0]->num_row - 1;
  return blocked_matrix_conv_sharedoutput(blocked_info_matrix_conv2d, num_input, input_info_list, kernel_info_list, output_info, conv_options);
}

////////////////////////////////////////////////////////////////////////////

static void __attribute__((constructor)) construct_map_dca_matrix_conv2d()
{
  blocked_info_matrix_conv2d = blocked_matrix_info_alloc();
  blocked_info_matrix_conv2d->block_size = 0;
  blocked_info_matrix_conv2d->subop_mapping = matrix_op_mapping_alloc();

  blocked_info_matrix_conv2d->subop_mapping->matrix_conv = i_dca_matrix_conv2d00_conv_oneblock;
  blocked_info_matrix_conv2d->subop_mapping->matrix_conv_sharedoutput = i_dca_matrix_conv2d00_conv_oneblock_sharedoutput;
}

void mop_use_dca_matrix_conv2d(ervp_mop_mapping_t *mop_mapping)
{
  assert(mop_mapping != NULL);
  mop_mapping->matrix_conv = i_blocked_dca_matrix_conv2d00_conv;
  mop_mapping->matrix_conv_sharedinput = i_blocked_dca_matrix_conv2d00_conv_sharedinput;
  mop_mapping->matrix_conv_sharedoutput = i_blocked_dca_matrix_conv2d00_conv_sharedoutput;
}
