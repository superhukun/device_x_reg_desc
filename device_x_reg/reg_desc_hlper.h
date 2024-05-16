/*
 * Author: Ken Hu
 * Date: 2024-05-16
 */

#include "reg_desc_tbl.h"

#ifdef GENERATE_DEVREG_IDS_ENUM
#undef GENERATE_DEVREG_IDS_ENUM
/*
 * Help to generate the following code:
 * enum dev_reg_ids_$device_name$ {
 *  reg_name_x = addr,
 *  fld_name_x = msk,
 *  opt_x = value,
 *  ......
 *
 * };
 *
 * The generated enumerators to name:
 * each register,
 * each register field mask,
 * register field optinal value
 *
 * */

#undef DEV
#undef REG
#undef FLD
#undef OPT
#undef END_DEV

//#define DEV(name, a, ...) enum dev_reg_ids_##name : uint32_t {
#define DEV(name, a, ...) enum dev_reg_ids_##name {
#define REG(name, addr, a, ...) name = addr,
#define FLD(name, msk, a, ...) name##_MSK = msk,
#define OPT(name, value, a, ...) name = value,
#define END_DEV(...) };


#elif defined (DEFINE_FLD_OPTS)
#undef DEFINE_FLD_OPTS
/*
 * Help to generate the following code:
 *
 * struct field_opt _filed_opt_$field1_name$ [] = {
 *   {.n = "GEN_CTRL1_FLD1_V0", .v = ((0x0) << 7), .short_n = "V0", .desc = "Generic control register V0" },
 *   {.n = "GEN_CTRL1_FLD1_V1", .v = ((0x1) << 7), .short_n = "V1", .desc = "Generic control register V1" },
 * };
 *  struct field_opt _filed_opt_$fieldn_name$ [] = {
 * {.n = "GEN_CTRL2_FLD1_V0", .v = ((0x0) << 7), .short_n = "V0", .desc = "Generic control register V0" },
 * };
 *
 * One struct field_opt[] is for one field of a register, containing all the optional value for this field.
 * The helper is generating all fields for all registers for the device.
 *
 */

#undef DEV
#undef REG
#undef FLD
#undef OPT
#undef END_DEV

#define DEV(name, a, ...) struct dummy_for_dev_reg_##name {
#define REG(name, a, ...) }; struct dummy_for_reg_flds_##name {
#define FLD(name, a, ...)  }; static struct field_opt _field_opts_##name [] = {
#define OPT(name, value, sn, _desc) {.n = #name, .v = value, .short_n = #sn, .desc = _desc },
#define END_DEV(...) };


#elif defined (DEFINE_REG_FLDS)
#undef DEFINE_REG_FLDS
/*
 * Help to generate the following code:
 * static struct reg_field reg_fields_$reg_name_1$ [] = {
 *     {.n = "field_name_x", .short_n = "short name x", ...  },
 *     ...
 *     {.n = "field_name_y", .short_n = "short name x", ...  },
 * };
 * .....
 * static struct reg_field reg_fields_$reg_name_n$ [] = {
 *     {.n = "field_name_x", .short_n = "short name x", ...  },
 *     ...
 *     {.n = "field_name_y", .short_n = "short name x", ...  },
 * };
 *
 * One struct reg_field[] is for one register, containing all the fields for this register.
 * The helper is generating all registers for the device.
 *
 */

#undef DEV
#undef REG
#undef FLD
#undef OPT
#undef END_DEV

#define DEV(name, a, ...) struct dummy_for_reg_flds_##name {
#define REG(name, a, ...) }; static struct reg_field _reg_fields_##name [] = {
#define FLD(name, _msk, _sn, _desc) \
    { \
           .n = #name, \
           .short_n = #_sn, \
           .desc = _desc, \
           .msk = _msk, \
           .cnt = sizeof(_field_opts_##name)/sizeof(_field_opts_##name[0]), \
           .opts = _field_opts_##name, \
   },
#define OPT(...)
#define END_DEV(...) };

#elif defined (DEFINE_DEV_REGS)
#undef DEFINE_DEV_REGS
/*
 * Help to generate the following code:
 *
 * struct reg_desc _regs_$device_name$[] = {
 *  {reg_desc of reg1},
 *  ....
 *  {reg_desc of reg2},
 * };
 *
 * //if more than one DEV()&END_DEV() in one *.reg file.
 * ....
 * struct reg_desc _regs_$devicex_name$ [] = {
 *  {reg_desc of reg1},
 *  ....
 *  {reg_desc of reg2},
 * };
 *
 */

#undef DEV
#undef REG
#undef FLD
#undef OPT
#undef END_DEV

#define DEV(name, a, ...) static struct reg_desc _regs_##name [] = {
#define REG(name, _id, _sn, _desc) \
    { .n = #name, .short_n = #_sn, .desc = _desc, .loc = _id, \
      .cnt = sizeof(_reg_fields_##name)/sizeof(_reg_fields_##name[0]), \
      .flds = _reg_fields_##name, \
    },
#define FLD(...)
#define OPT(...)
#define END_DEV(...) };


#elif defined (DEFINE_REGS_DESC_TBL)
#undef DEFINE_REGS_DESC_TBL
/*
 * Help to generate the registers description table code:
 *
 * struct reg_desc_tbl reg_desc_tbl_$device1_name$ = {
 *  .n = ..
 *  ....
 *  .regs = _regs_$device1_name$,
 * };
 * ....
 * struct reg_desc_tbl reg_desc_tbl_$devicen_name$ = {
 *  .n = ..
 *  ....
 *  .regs = _regs_$devicen_name$,
 * };
 *
 *
 */

#undef DEV
#undef REG
#undef FLD
#undef OPT
#undef END_DEV

#define _DEV_TBL(dev_name, tbl_name, _abbr, _desc) \
    static struct reg_desc_tbl reg_desc_tbl_##tbl_name = { \
        .n = #dev_name, \
        .short_n = #_abbr, \
        .desc = _desc, \
        .cnt = sizeof(_regs_##dev_name)/sizeof(_regs_##dev_name[0]), \
        .regs = _regs_##dev_name, \
    };
#define DEV_TBL(dev_name, tbl_name, _abbr, _desc) _DEV_TBL(dev_name, tbl_name, _abbr, _desc)

#if defined (DEV_REGS_TBL_NAME)
#define DEV(name, _resv, _abbr, _desc) DEV_TBL(name, DEV_REGS_TBL_NAME, _abbr, _desc)
#else
#define DEV(name, _resv, _abbr, _desc) DEV_TBL(name, name, _abbr, _desc)
#endif

#if 0
#define DEV(name, _resv, _sn, _desc) \
    static struct reg_desc_tbl reg_desc_tbl_##name = { \
        .n = #name, \
        .short_n = #_sn, \
        .desc = _desc, \
        .cnt = sizeof(_regs_##name)/sizeof(_regs_##name[0]), \
        .regs = _regs_##name, \
    };
#endif

#define REG(...)
#define FLD(...)
#define OPT(...)
#define END_DEV(...)

#endif
