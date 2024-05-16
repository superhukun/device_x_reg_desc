/*
 * Author: Ken Hu
 * Date: 2024-05-16
 */

#ifndef _REG_DESC_TBL_H_
#define _REG_DESC_TBL_H_

#include <stdint.h>

/* One optional value of a register field.*/
struct field_opt {
    const char *n; /* name*/
    const char *abbr; /* abbreviation*/
    const char *desc;
    uint32_t v;
};

/* One field of a register.*/
struct reg_field {
    const char *n;
    const char *abbr;
    const char *desc;
    uint32_t msk; /*mask of this field.*/
    uint32_t cnt; /* count of optional values*/
    struct field_opt *opts;
};

/* One register description .*/
struct reg_desc {
    const char *n;
    const char *abbr;
    const char *desc;
    uint32_t loc; /* location: address/regID(indirect-access) or offset(direct-access).*/
    uint32_t cnt; /* count of fields*/
    struct reg_field *flds;
};

struct reg_desc_tbl {
    const char *n;
    const char *abbr;
    const char *desc;
    uint32_t cnt; /*count of registers*/
    struct reg_desc *regs;
};

void reg_desc_tbl_dump(struct reg_desc_tbl *tbl);
#endif
