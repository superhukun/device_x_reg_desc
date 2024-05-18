/*
 * Author: Ken Hu
 * Date: 2024-05-16
 */

#ifndef _REG_DESC_TBL_H_
#define _REG_DESC_TBL_H_

#include <stdint.h>
#include <stddef.h>

#define desc_literal_info                        \
    const char *abbr; /* abbreviation/acronym */ \
    const char *n;    /* name/normal info */     \
    const char *desc  /* description/detail info */

/* One optional value of a register field.*/
struct field_opt {
    desc_literal_info;
    uint32_t v;
};

/* One field of a register.*/
struct reg_field {
    desc_literal_info;
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

/* Literal information level flags */
#define INFO_L_NAME ((0x1) << 0)
#define INFO_L_ABBR ((0x1) << 1)
#define INFO_L_DESC ((0x1) << 2)
#define INFO_L_ALL  ((0x7))
typedef uint8_t literal_info_flg_t;

/* Register parse Request and Result structure. */
struct reg_parse_req_result {
    /*
     * buf
     * IN : specify the address of the buf to store the parsed literal text output;
     * OUT: reg_desc tbl APIs output the parsed literal text to the buf if the buf is not NULL.
     * */
    char *buf;
    uint32_t buf_size;
    /*
     * Control what kind of level info output to buf.
     * */
    literal_info_flg_t dev_info_flg;
    literal_info_flg_t reg_info_flg;
    literal_info_flg_t fld_info_flg;
    literal_info_flg_t opt_info_flg;

    /*
     * Pass out the parsing status, literal info
     * when the callback is invoked or reg_desc_xxx() is returned.
     * */
    enum parse_status progress;
    struct reg_desc_literal_info dev;
    struct reg_desc_literal_info reg;
    uint32_t fld_msk;
    struct reg_desc_literal_info fld;
    struct reg_desc_literal_info opt;
    uint32_t undef_bits_msk;
};

typedef void (*output_decorator_cb)(struct reg_parse_req_result* res, void *ctx);

/*
 * Function: Parse the value of register w/ specified register description table.
 *
 * reg_loc  : [IN] register location ID.
 * reg_value: [IN] register value
 * lkp_msk  : [IN] specify  the mask bits to lookup for this parse.
 * tbl      : [IN] register description table to facilitate the parse.
 *                 NULL is allowed, means no any table to provide literal info.
 * res      : [IN | OUT] request and result for this parse. Refer to struct reg_parse_req_result.
 * decorator: [IN & CALLBACKED] decorator callback. NULL is allowed.
 * ctx      : [IN & PASS to decorator] decorator context. NULL is allowed.
 * */
void reg_desc_parse_reg_value(uint32_t reg_loc, uint32_t reg_value, uint32_t lkp_msk,
                              struct reg_desc_tbl *tbl,
                              struct reg_parse_req_result *res,
                              output_decorator_cb decorator,
                              void *ctx);

/*reg_desc_tbl_dump is mainly for DEBUG.*/
void reg_desc_tbl_dump(struct reg_desc_tbl *tbl,
                       struct reg_parse_req_result *res,
                       output_decorator_cb decorator,
                       void *ctx);

char *msk_to_str_msb (uint32_t msk);

/*Registry related struct, MACRO */
struct reg_tbl_item {
    const char *tbl_name;
    struct reg_desc_tbl *tbl;
    struct reg_tbl_item *next;
};
void registry_reg_desc_tbl(struct reg_tbl_item *item);
struct reg_desc_tbl *get_reg_desc_tbl_from_registry(const char *tbl_name);
typedef void (*reg_desc_tbl_regitry_iterator)(const char *tbl_name, struct reg_desc_tbl *tbl, void *ctx);
void iterate_reg_desc_tbl_registry(reg_desc_tbl_regitry_iterator iter, void *ctx);

#define REG_DESC_TBL_REGISTRY(_n, _tbl, _prio ... ) \
static struct reg_tbl_item reg_tbl_item_##_n; \
static void do_registry_reg_desc_tbl_##_n (void) __attribute__((constructor(_prio))); \
static void do_registry_reg_desc_tbl_##_n (void) \
{ \
    registry_reg_desc_tbl(&reg_tbl_item_##_n); \
} \
static struct reg_tbl_item reg_tbl_item_##_n = { \
    .tbl_name = #_n, \
    .tbl = &(_tbl), \
    .next = NULL \
} \

#endif
