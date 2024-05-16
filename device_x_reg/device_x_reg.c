/*
 * Author: Ken Hu
 * Date: 2024-05-16
 */

#include <stdio.h>

#include "reg_desc_tbl.h"

#define DEV_REG_FILE "device_x.reg"
#include "reg_file_to_desc_tbl.h"

#if 0
#define DEFINE_FLD_OPTS
#include "device_x.reg"

#define DEFINE_REG_FLDS
#include "device_x.reg"

#define DEFINE_DEV_REGS
#include "device_x.reg"

#define DEFINE_REGS_DESC_TBL
#include "device_x.reg"
#endif

int main (int argc, char *argv[])
{
    struct reg_desc_tbl *tbl;
    struct reg_desc *reg;
    uint32_t reg_idx;

    tbl = &reg_desc_tbl_DEV_TEST;

    reg_desc_tbl_dump(tbl);

   return 0;

}
