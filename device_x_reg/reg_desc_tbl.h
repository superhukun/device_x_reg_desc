/*
 * Author: Ken Hu
 * Date: 2024-05-16
 */

#ifndef _REG_DESC_TBL_H_
#define _REG_DESC_TBL_H_

#include <stdint.h>

#define desc_literal_info \
    const char *n; /* name*/ \
    const char *abbr; /* abbreviation*/ \
    const char *desc

/* One optional value of a register field.*/
struct field_opt {
    desc_literal_info;
#if 0
    const char *n; /* name*/
    const char *abbr; /* abbreviation*/
    const char *desc;
#endif
    uint32_t v;
};

/* One field of a register.*/
struct reg_field {
    desc_literal_info;
#if 0
    const char *n;
    const char *abbr;
    const char *desc;
#endif
    uint32_t msk; /*mask of this field.*/
    uint32_t cnt; /* count of optional values*/
    struct field_opt *opts;
};

/* One register description .*/
struct reg_desc {
    desc_literal_info;
    uint32_t loc; /* location: address/regID(indirect-access) or offset(direct-access).*/
    uint32_t cnt; /* count of fields*/
    struct reg_field *flds;
};

struct reg_desc_tbl {
    desc_literal_info;
    uint32_t cnt; /*count of registers*/
    struct reg_desc *regs;
};

struct reg_desc_literal_info {
    desc_literal_info;
};

enum parse_status {
    PARSE_UNKNOWN,
    PARSE_REGISTER_FOUND,
    PARSE_ONE_FLD_FOUND,
    PARSE_ONE_FLD_OPT_FOUND,
    PARSE_ONE_FLD_OPT_NOT_FOUND,
    PARSE_DONE,
    PARSE_LAST
};

#define INFO_L_NAME ((0x1) << 0)
#define INFO_L_ABBR ((0x1) << 1)
#define INFO_L_DESC ((0x1) << 2)
#define INFO_L_ALL  ((0x7))
typedef uint8_t literal_info_flg_t;

struct reg_parse_req_result {
    /*
     * buf
     * IN : specify the address of the buf to store the parsed literal text output;
     * OUT: reg_desc tbl APIs output the parsed literal text to the buf if the buf is not NULL.
     * */
    char *buf;
    uint32_t buf_size;
    /*Control what kind of level info output to buf*/
    literal_info_flg_t dev_info_flg;
    literal_info_flg_t reg_info_flg;
    literal_info_flg_t fld_info_flg;
    literal_info_flg_t opt_info_flg;

    enum parse_status progress;
    struct reg_desc_literal_info dev;
    struct reg_desc_literal_info reg;
    uint32_t fld_msk;
    struct reg_desc_literal_info fld;
    struct reg_desc_literal_info opt;
    uint32_t undef_bits_msk;
};

typedef void (*output_decorator_cb)(struct reg_parse_req_result* res, void *ctx);

/*reg_desc_tbl_dump is mainly for DEBUG.*/
void reg_desc_tbl_dump(struct reg_desc_tbl *tbl,
                       struct reg_parse_req_result *res,
                       output_decorator_cb decorator,
                       void *ctx);

void reg_desc_parse_reg_value(uint32_t reg_loc, uint32_t reg_value, uint32_t lkp_msk,
                              struct reg_desc_tbl *tbl,
                              struct reg_parse_req_result *res,
                              output_decorator_cb decorator,
                              void *ctx);

struct reg_tbl_item {
    struct reg_desc_tbl *tbl;
    struct reg_tbl_item *next;
};
void submit_reg_tbl_item(struct reg_tbl_item *item);
struct reg_desc_tbl *get_reg_desc_lkptbl(const char *tbl_name);

#define SUBMIT_REG_TBL_TO_LIST(_dev)

char *msk_to_str_msb (uint32_t msk);
#endif
