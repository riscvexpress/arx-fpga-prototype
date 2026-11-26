#ifndef __MAP_DCA_MATRIX_CONV2D_HW_H__
#define __MAP_DCA_MATRIX_CONV2D_HW_H__

#include "ervp_matrix.h"
#include "ervp_matrix_op_sw.h"
#include "ervp_blocked_matrix_op.h"

extern ervp_blocked_matrix_info_t *blocked_info_matrix_conv2d;

void mop_use_dca_matrix_conv2d(ervp_mop_mapping_t *mop_mapping);

#endif
