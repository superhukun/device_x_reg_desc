/*
 * Author: Ken Hu
 * Date: 2024-05-16
 */

#include <stdio.h>

#include "reg_desc_tbl.h"

void reg_desc_tbl_dump (struct reg_desc_tbl *tbl)
{
    struct reg_desc *reg;
    uint32_t reg_idx;

    printf("Device Registers Description DUMP\n");
    printf("DEVICE %s(%s): %s\n", tbl->n, tbl->abbr, tbl->desc);
    printf("\n");
    reg = tbl->regs;
    for (reg_idx = 0; reg_idx < tbl->cnt; reg_idx++) {
        printf("REG[%d %s]:%s\n", reg_idx, reg->abbr, reg->n);
        reg++;
    }
}
