/*
 * Author: Ken Hu
 * Date: 2024-05-19
 */

/*
 * This file  demonstrates:
 * 1. How to create C file for device_x registers description table instance
 * from reg file(eg, device_x.reg).
 * 2. How to do the registry for this table instance.
 */
#include "reg_desc_tbl.h"

#define DEV_REG_FILE "device_x.reg"
#define DEV_REGS_TBL_NAME dev_x_reg_desc_tbl
#include "reg_file_to_desc_tbl.h"

/*
 * It's highly recommended to add the device register desc tbl to registry,
 * though it's optional.
 *
 * The below exmaple illustrates the use of REG_DESC_TBL_REGISTRY() with most of its features:
 * 1. registry the same registers description table instance w/ different table names,
 *    this might help to code migration from old table name to new table name.
 * 2. specify the priority for the registry, the lower number the earlier to registry.
 * */
REG_DESC_TBL_REGISTRY(DEV_TEST_Primary, dev_x_reg_desc_tbl);
REG_DESC_TBL_REGISTRY(DEV_TEST_Backup,  dev_x_reg_desc_tbl, 200);
