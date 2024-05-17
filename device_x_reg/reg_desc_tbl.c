/*
 * Author: Ken Hu
 * Date: 2024-05-16
 */

#include <stdio.h>
#include <string.h>

#include "reg_desc_tbl.h"

#define SNPRNT_SANITY_UPDT(_rc, _buf, _size) \
    do {                                     \
        if (_rc < 0 || _rc >= _size) {       \
            _size = 0;                       \
        } else {                             \
            _buf += _rc;                     \
            _size -= _rc;                    \
        }                                    \
    } while(0)                               \

static struct reg_desc* lkp_reg_desc(struct reg_desc_tbl *tbl, uint32_t reg_loc)
{
    int i;
    struct reg_desc *reg;

    reg = tbl->regs;
    for (i = 0; i < tbl->cnt; i++, reg++) {
        if (reg_loc == reg->loc) {
            return reg;
        }
    }
    return NULL;
}

/* Calculate the local value of a given global value and global mask.
 * The global mask bits could be discrete.
 * Eg:
 * Global Value|Global Mask| Local Value
 *    0x1523   | 0x0f00    | 0x5
 *    0x3521   | 0x0f0f    | 0x51
 * */
static uint32_t l_value (uint32_t g_v, uint32_t g_msk)
{
    uint32_t l_v = 0;
    uint32_t bits_cnt = sizeof(g_v) * 8;
    uint32_t g_i, l_i;

    l_i = 0;
    for (g_i = 0; g_i < bits_cnt && g_msk; g_i++) {
        if (((0x1) << g_i) & g_msk) {
            l_v |= ((((0x1) << g_i) & g_v)? 1 : 0) << l_i;
            l_i++;
            /*Clear handled bit from g_msk, when g_msk is zero then we don't need to continous. */
            g_msk ^= ((0x1) << g_i);
        }
    }
    return l_v;
}


/*
 * Return a string to describe mask value which bits are 1,
 * starting from most significant bit.
 * Eg:
 * Mask value | String
 * 0xf        | 3-0
 * 0xf0       | 7-4
 * 0xf0f      | 11-8,3-0
 * 0x555      | 10,8,6,4,2,0
 *
 * Algorithm:
 * 1. Set the begin bit of range(bgn_bit) to 0 (since MSB first)
 * 2. cursor_bit iterates mask from MSB to LSB, do the following:
 *    2.1. if mask[cursor_bit] is 1 :
 *         if bgn_bit is 0:
 *              set bgn_bit to cursor_bit, output ("%d", bgn_bit)
 *              move cursor_bit to next bit;
 *         if bgn_bit is not 0:
 *              if cursor_bit is just next one to bgn_bit,
 *                  output ("-")
 *              if cursor_bit the last one bit,
 *                  output ("%d", cursor_bit)
 *              move cursor_bit to next bit
 *    2.2  if mask[cursor_bit] is 0 :
 *         if bgn_bit is 0:
 *              move cursor_bit to next bit
 *         if bgn_bit is not 0:
 *            if cursor_bit is more than one to bgn_bit,
 *                  output ("%d", cursor_bit - 1)
 *            if cursor_bit is not the last one and the rest bits of mask NOT zero,
 *                  output (',')
 *            set bgn_bit to 0
 *            move cursor_bit to next bit
 *
 * */
#define JOINT_RANGE "-"
#define DELIM_RANGE ","
char *msk_to_str_msb (uint32_t msk)
{
    /*
     * The max count of needed char:
     * half of bits are 1 and none of them are adjacent, except the last bit1 and bit0
     * assume each bit need 3 char to describe it.
     * Eg:
     * 31,29,....,1-0
     * */
    static char str[(sizeof(msk) * 8 / 2) * 3];
    static uint32_t last_msk = 0;
    uint32_t bits_cnt = sizeof(msk) * 8;
    uint32_t m_bit, bgn_bit, cursor_bit;

    char *output;
    int size;
    int prn_rc;

    if (msk == last_msk) {
        return str;
    }

    str[0] = '\0';
    output = str;
    size = sizeof(str);

    bgn_bit = 0;
    for(m_bit = bits_cnt; m_bit > 0; m_bit--) {
        cursor_bit = m_bit - 1;
        if (msk & ((0x1) << cursor_bit)) {
            /* This msk bit is 1*/
            if (!bgn_bit) {
                /*
                 * BITn+1 | BITn
                 * 0 or NA| 1 (cursor_bit))
                 * */
                bgn_bit = cursor_bit;
                prn_rc = snprintf(output, size, "%d", cursor_bit);
                SNPRNT_SANITY_UPDT(prn_rc, output, size);
            } else if (cursor_bit + 1 == bgn_bit) {
                /*
                 * BITn+1     | BITn
                 * 1 (bgn_bit)| 1 (cursor_bit))
                 * */
                prn_rc = snprintf(output, size, JOINT_RANGE);
                SNPRNT_SANITY_UPDT(prn_rc, output, size);
            }
            if (cursor_bit == 0 && bgn_bit) {
                /*
                 * BITn+x     | ...  | BIT0
                 * 1 (bgn_bit)| 1..1 | 1 (cursor_bit))
                 * */
               /*
                * The last bit and the begin range is not staring from 0,
                * then need output itself.
                */
               prn_rc = snprintf(output, size, "%d", cursor_bit);
               SNPRNT_SANITY_UPDT(prn_rc, output, size);
            }
        } else {
            /* This msk bit is 0*/
            if (bgn_bit == 0) {
                /*
                 * BITn+1 | BITn
                 * 0 or NA| 0 (cursor_bit))
                 * */
                continue;
            }

            /*
             * | .... | BITn+1  | BITn
             * | 0/1  |    1    | 0(cursor_bit)
             */

            if (cursor_bit + 1 < bgn_bit) {
                /*
                 * BITn+1+x) | .... | BITn+1         | BITn
                 * 1(bgn_bit)| 1..1 | 1(need output) | 0(cursor_bit)
                 * */
                prn_rc = snprintf(output, size, "%d", cursor_bit+1);
                SNPRNT_SANITY_UPDT(prn_rc, output, size);
            }
            if (msk & ((0x1 << cursor_bit) - 1)) {
                /*
                 * | BITn          | BITn-1 to 0        |
                 * | 0(cursor_bit) | rest bits has 1 bit|
                 */
                prn_rc = snprintf(output, size, DELIM_RANGE);
                SNPRNT_SANITY_UPDT(prn_rc, output, size);
            }

            bgn_bit = 0;
        }
    }

    return str;
}

#define ASSIGN_LITERAL_INFO(a, b) \
    do {                          \
        (a)->n    = (b)->n;       \
        (a)->abbr = (b)->abbr;    \
        (a)->desc = (b)->desc;    \
    } while(0)                    \

#define NONE_LITERAL_INFO(a) \
    do {                     \
        (a)->n    = NULL;    \
        (a)->abbr = NULL;    \
        (a)->desc = NULL;    \
    } while(0)               \

static char * parse_result_str[] = {
    [PARSE_UNKNOWN] = "Unknown",
    [PARSE_REGISTER_FOUND] = "RegFound",
    [PARSE_ONE_FLD_FOUND] = "FieldFound",
    [PARSE_ONE_FLD_OPT_FOUND] = "FieldOptFound",
    [PARSE_ONE_FLD_OPT_NOT_FOUND] = "FieldOptNotFound",
    [PARSE_DONE] = "Done",
    [PARSE_LAST] = "",
};
char *reg_desc_parse_result_status_str (enum parse_status st) {
    return parse_result_str[st];
}

/*
 * LITERAL INFO FMT has 6 %s place holders :
 * 1st %s: if NONE flg specified, will be placed by abbr; else by "";
 * 2nd %s: if name flg specified, will be placed by name; else by "";
 * 3rd %s: if name & (abbr | desc) flg specified, will be placed by " | "; else by "";
 * 4th %s: if abbr flg specified, will be placed by abbr ; else by "";
 * 5th %s: if abbr & desc flg specified, will be placed by " | "; else by "";
 * 6th %s: if desc flg specified, will be placed by desc; else by "";
 *
 * */
#define LITERAL_INFO_FMT "%s%s%s%s%s%s"
#define LITERAL_INFO_ARG(_flg,  _obj)                                      \
    (_flg & INFO_L_ALL)?"":(_obj?(_obj)->abbr:"Unknown"),                  \
    (_flg & INFO_L_NAME)?(_obj?(_obj)->n:"Unknown"):"",                    \
    ((_flg & INFO_L_NAME) && (_flg & (INFO_L_ABBR|INFO_L_DESC)))?" | ":"", \
    (_flg & INFO_L_ABBR)?(_obj?(_obj)->abbr:"Unknown"):"",                 \
    ((_flg & INFO_L_ABBR) && (_flg & INFO_L_DESC))?" | ":"",               \
    (_flg & INFO_L_DESC)?(_obj?(_obj)->desc:"Unknown"):""                  \


//#define HEADLINE_PREFIX "[Dev/Reg/Value(lkp bits)] = "
#define HEADLINE_PREFIX ""
#define HEADLINE_NO_REG_LITERAL_FMT  HEADLINE_PREFIX LITERAL_INFO_FMT  "/0x%08X/0x%08x(lkp bits %s)\n"
#define HEADLINE_FMT  HEADLINE_PREFIX "[Device:" LITERAL_INFO_FMT  "] /[Reg: 0x%08x " LITERAL_INFO_FMT "] /[Value: 0x%08x(lkp bits %s)]\n"

#define INVOKE_CALLB(_st, _cb, _res, _ctx) \
    do {                                   \
        _res->progress = _st;              \
        if (_cb) {                         \
            _cb(_res, _ctx);               \
        }                                  \
    } while(0)                             \

void reg_desc_tbl_dump (struct reg_desc_tbl *tbl,
                        struct reg_parse_req_result *res,
                        output_decorator_cb decorator,
                        void *ctx)
{
    struct reg_desc *reg;
    uint32_t reg_idx;
    char *buf;
    uint32_t size;
    int prn_rc;

    if (!tbl || !res) {
        return;
    }

    buf = res->buf;
    size = res->buf_size;

    /*TODO:
     * Will enhance later if needed.
     * 1. Dump fields
     * 2. Dump options of fields
     * 3. Callback decorator
     * */
    if (buf) {
        prn_rc = snprintf(buf, size,
                "Device Registers Description DUMP\n");
        SNPRNT_SANITY_UPDT(prn_rc, buf, size);
        prn_rc = snprintf(buf, size,
                "Device: (" LITERAL_INFO_FMT ")\n",
                LITERAL_INFO_ARG(res->dev_info_flg, tbl));
        SNPRNT_SANITY_UPDT(prn_rc, buf, size);
    }

    reg = tbl->regs;
    for (reg_idx = 0; reg_idx < tbl->cnt; reg_idx++) {
        prn_rc = snprintf(buf, size,
                "REG[%d]0x%08x(" LITERAL_INFO_FMT "): has %d fields\n",
                reg_idx, reg->loc,
                LITERAL_INFO_ARG(res->reg_info_flg, reg),
                reg->cnt);
        reg++;
    }
}

void reg_desc_parse_reg_value (uint32_t reg_loc, uint32_t reg_value, uint32_t lkp_msk,
                               struct reg_desc_tbl *tbl,
                               struct reg_parse_req_result *res,
                               output_decorator_cb decorator, void *cb_ctx)
{
    struct reg_desc *reg;
    char *buf;
    uint32_t size;
    uint32_t fld_not_found_msk = lkp_msk;
    uint32_t fld_idx = 0;
    struct reg_field *fld;
    uint32_t fld_msk, fld_val;
    uint32_t opt_idx = 0;
    struct field_opt *opt;
    int prn_rc = 0;

    if (!res) {
        return;
    }

    buf = res->buf;
    size = res->buf_size;

    res->undef_bits_msk = lkp_msk;
    if (!tbl) {
        /* No lkp table specified */
        if (buf) {
            prn_rc = snprintf(buf, size,
                             HEADLINE_NO_REG_LITERAL_FMT,
                             LITERAL_INFO_ARG(res->dev_info_flg, tbl),
                             reg_loc, reg_value, msk_to_str_msb(lkp_msk));
            SNPRNT_SANITY_UPDT(prn_rc, buf, size);
        }
        /* Set all literal info to empty. */
        NONE_LITERAL_INFO(&res->dev);
        NONE_LITERAL_INFO(&res->reg);
        NONE_LITERAL_INFO(&res->fld);
        NONE_LITERAL_INFO(&res->opt);
        /*Parse is done*/
        INVOKE_CALLB(PARSE_DONE, decorator, res, cb_ctx);
        return;
    }

    ASSIGN_LITERAL_INFO(&res->dev, tbl);

    reg = lkp_reg_desc(tbl, reg_loc);
    if (reg == NULL) {
        /* The table doens't have the description for this register,
         *
         * if the buf is provided, we output the default output string for the parse result:
         * {device literal string} register {HEX reg loc}: {HEX reg value}}
         * */
        if (buf) {
            prn_rc = snprintf(buf, size,
                    HEADLINE_FMT,
                    LITERAL_INFO_ARG(res->dev_info_flg, tbl),
                    reg_loc,
                    LITERAL_INFO_ARG(res->reg_info_flg, reg),
                    reg_value, msk_to_str_msb(lkp_msk));
            SNPRNT_SANITY_UPDT(prn_rc, buf, size);
        }
        /* Set literal info to empty for reg, fld, opt. */
        NONE_LITERAL_INFO(&res->reg);
        NONE_LITERAL_INFO(&res->fld);
        NONE_LITERAL_INFO(&res->opt);

        /*Parse is done*/
        INVOKE_CALLB(PARSE_DONE, decorator, res, cb_ctx);
        return;
    }

    /* Find the register in the table. */
    /* Set register literal info */
    ASSIGN_LITERAL_INFO(&res->reg, reg);

    if (buf) {
        /* Output literal text to buf like:
         * (dev literal info) / (reg_loc in HEX reg literal info)/ reg value in HEX(lkp bits in string)
         * */
        prn_rc = snprintf(buf, size,
                    HEADLINE_FMT,
                    LITERAL_INFO_ARG(res->dev_info_flg, tbl),
                    reg_loc,
                    LITERAL_INFO_ARG(res->reg_info_flg, reg),
                    reg_value, msk_to_str_msb(lkp_msk));
        SNPRNT_SANITY_UPDT(prn_rc, buf, size);
    }

    INVOKE_CALLB(PARSE_REGISTER_FOUND, decorator, res, cb_ctx);

    /*
     *  Iterate each field and try to find out the matched value option for field.
     */
    fld = reg->flds;
    for (fld_idx = 0; fld_idx < reg->cnt; fld_idx++, fld++) {
        if (lkp_msk & fld->msk != fld->msk) {
            /*It's not interested field, skip it*/
            continue;
        }
        /*Matched one field in the table */
        fld_not_found_msk &= ~fld->msk;
        res->fld_msk = fld->msk;
        ASSIGN_LITERAL_INFO(&res->fld, fld);
        INVOKE_CALLB(PARSE_ONE_FLD_FOUND, decorator, res, cb_ctx);

        fld_val = reg_value & fld->msk;
        /*Check if the fld value matches w/ this field any optional values in the table. */
        opt = fld->opts;
        for (opt_idx = 0; opt_idx < fld->cnt; opt_idx++, opt++) {
            if (opt->v == fld_val) {
                /* Find the matched field value option. */
                ASSIGN_LITERAL_INFO(&res->opt, opt);
                /* Output literal text to buf like:
                 * "(fld literal text)=logical_field_value(opt literal text)\n"
                 * */
                if (buf) {
                    prn_rc = snprintf(buf, size,
                            "(BIT %s:" LITERAL_INFO_FMT ") : %d(" LITERAL_INFO_FMT ")\n",
                            msk_to_str_msb(fld->msk),
                            LITERAL_INFO_ARG(res->fld_info_flg, fld),
                            l_value(fld_val, fld->msk),
                            LITERAL_INFO_ARG(res->opt_info_flg, opt)
                            );
                    SNPRNT_SANITY_UPDT(prn_rc, buf, size);
                }
                /*invoke the callback, one opt matched.*/
                INVOKE_CALLB(PARSE_ONE_FLD_OPT_FOUND, decorator, res, cb_ctx);
                break;
            }
        }
        if (opt_idx >= fld->cnt) {
            /*Not find a matched field value option. */
            prn_rc = snprintf(buf, size,
                    "(bit %s:" LITERAL_INFO_FMT ") : %d(Undef)\n",
                    msk_to_str_msb(fld->msk),
                    LITERAL_INFO_ARG(res->fld_info_flg, fld),
                    l_value(fld_val, fld->msk));
            SNPRNT_SANITY_UPDT(prn_rc, buf, size);
            /*invoke the callback, no one opt matched.*/
            INVOKE_CALLB(PARSE_ONE_FLD_OPT_NOT_FOUND, decorator, res, cb_ctx);
        }
    }

    if (fld_not_found_msk) {
        /*This register's some fields are not defined in the table. */
        if (buf) {
            /* Output literal text to buf like:
             * "Register (reg desc) undefined bits: 15-13\n"
             **/
            prn_rc = snprintf(buf, size,
                    "Register (" LITERAL_INFO_FMT ") undefined bits: %s\n",
                    LITERAL_INFO_ARG(res->reg_info_flg, reg),
                    msk_to_str_msb(fld_not_found_msk));
            SNPRNT_SANITY_UPDT(prn_rc, buf, size);
        }
    }
    res->undef_bits_msk = fld_not_found_msk;
    INVOKE_CALLB(PARSE_DONE, decorator, res, cb_ctx);

    return;
}

//////
//

static struct reg_tbl_item *global_tbl_list = NULL;

void submit_reg_tbl_to_list (struct reg_tbl_item *list) {

    if (!list || !list->tbl || !list->tbl->n) {
        return;
    }

    if (get_reg_desc_lkptbl(list->tbl->n)) {
        /*Duplicated name table already submitted to list*/
        printf("%s device reg tbl already in the list.", list->tbl->n);
        return;

    }

    list->next = global_tbl_list;
    global_tbl_list = list;
    printf("submit %s device reg tbl to the global list.", list->tbl->n);
}

struct reg_desc_tbl *get_reg_desc_lkptbl (const char *tbl_name)
{
    struct reg_tbl_item *l;

    if (!tbl_name) {
        return NULL;
    }

    l = global_tbl_list;
    while (l) {
        if (0 == strncmp(l->tbl->n, tbl_name, strlen(l->tbl->n)+1)) {
            printf("Found %s device reg tbl in the list.", l->tbl->n);
            return l->tbl;
        }
        l = l->next;
    }

    return NULL;
}
