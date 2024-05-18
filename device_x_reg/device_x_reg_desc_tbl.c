/*
 * Author: Ken Hu
 * Date: 2024-05-19
 */

#include "reg_desc_tbl.h"

#define DEV_REG_FILE "device_x.reg"
#include "reg_file_to_desc_tbl.h"

/*
 * It's highly to add the device register desc tbl to registry,
 * though it's optional.
 * */
REG_DESC_TBL_REGISTRY(reg_desc_tbl_DEV_TEST);
