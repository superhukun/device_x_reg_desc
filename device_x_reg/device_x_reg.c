/*
 * Author: Ken Hu
 * Date: 2024-05-16
 */

#include <stdio.h>

#include "reg_desc_tbl.h"

//#include "device_x_reg.h"

//#define DEV_REG "device_x.reg"

#define DEFINE_FLD_OPTS
#include "device_x.reg"

#define DEFINE_REG_FLDS
#include "device_x.reg"

#define DEFINE_DEV_REGS
#include "device_x.reg"

#define DEFINE_REGS_DESC_TBL
#define DEV_REGS_TBL_NAME test
#include "device_x.reg"

int main (int argc, char *argv[])
{
    struct reg_desc_tbl *tbl;
    struct reg_desc *reg;
    uint32_t reg_idx;

    tbl = &reg_desc_tbl_test;


    printf("DEVICE: %s:%s\n", tbl->n, tbl->short_n);
    printf("\t%s\n", tbl->desc);
    printf("\n");
    reg = tbl->regs;
    for (reg_idx = 0; reg_idx < tbl->cnt; reg_idx++) {
        printf("REG[%d %s]:%s\n", reg_idx, reg->short_n, reg->n);
        reg++;
    }

   return 0;

}
