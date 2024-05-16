/*
 * Author: Ken Hu
 * Date: 2024-05-16
 */
/*
 * NOT BUG:
 * This file is designed to be included multiple times,
 * don't & can't have #ifndef REG_DESC_HLPER_H_ ...
 */

#ifdef GENERATE_DEVREG_IDS_ENUM
#undef GENERATE_DEVREG_IDS_ENUM

#undef DEV
#undef REG
#undef FLD
#undef OPT
#undef END_DEV

/*
 * Help to generate enumerators to name:
 * each register,
 * each register field mask,
 * each register field optinal value.
 *
 * The generated code looks like:
 * enum dev_reg_ids_$device_name$ {
 *  reg_name_x = addr,
 *  fld_name_x = msk,
 *  opt_x = value,
 *  ......
 *
 * };
 *
 * */

//#define DEV(name, a, ...) enum dev_reg_ids_##name : uint32_t {
#define DEV(name, a, ...) enum dev_reg_ids_##name {
#define REG(name, addr, a, ...) name = addr,
#define FLD(name, msk, a, ...) name##_MSK = msk,
#define OPT(name, value, a, ...) name = value,
#define END_DEV(...) };


#elif defined (DEFINE_FLD_OPTS)
/*
 * This DEFINE_FLD_OPTS and following
 *      DEFINE_REG_FLDS
 *      DEFINE_DEV_REGS
 *      DEFINE_REGS_DESC_TBL
 * are helping to generate C code for a device register description table based on a *.reg, such device_x.reg.
 * This table name is reg_desc_tbl_$device_name_defined_in_reg_file$, the table could be used to parse register value.
 *
 * The usage is:
 *
 *   #define DEFINE_FLD_OPTS
 *   #include "device_x.reg"
 *
 *   #define DEFINE_REG_FLDS
 *   #include "device_x.reg"
 *
 *   #define DEFINE_DEV_REGS
 *   #include "device_x.reg"
 *
 *   #define DEFINE_REGS_DESC_TBL
 *   #include "device_x.reg"
 *
 * */
#undef DEFINE_FLD_OPTS

#include "reg_desc_tbl.h"

#undef DEV
#undef REG
#undef FLD
#undef OPT
#undef END_DEV

/*
 * Help to generate every register-field array that contains the optional value desc.
 * One struct field_opt[] is for one field of a register, containing all the optional value for this field.
 * The helper is generating all fields for all registers for the device.
 *
 * The generated code looks like:
 * struct field_opt _filed_opt_$field1_name$ [] = {
 *   {.n = "GEN_CTRL1_FLD1_V0", .v = ((0x0) << 7), .abbr = "V0", .desc = "Generic control register V0" },
 *   {.n = "GEN_CTRL1_FLD1_V1", .v = ((0x1) << 7), .abbr = "V1", .desc = "Generic control register V1" },
 * };
 *  struct field_opt _filed_opt_$fieldn_name$ [] = {
 * {.n = "GEN_CTRL2_FLD1_V0", .v = ((0x0) << 7), .abbr = "V0", .desc = "Generic control register V0" },
 * };
 *
 *
 */
#define DEV(name, a, ...) struct dummy_for_dev_reg_##name {
#define REG(name, a, ...) }; struct dummy_for_reg_flds_##name {
#define FLD(name, a, ...)  }; static struct field_opt _field_opts_##name [] = {
#define OPT(name, value, _abbr, _desc) {.n = #name, .v = value, .abbr = #_abbr, .desc = _desc },
#define END_DEV(...) };


#elif defined (DEFINE_REG_FLDS)
#undef DEFINE_REG_FLDS

#undef DEV
#undef REG
#undef FLD
#undef OPT
#undef END_DEV

/*
 * Help to generate every register array which contains all fields of this register.
 * One struct reg_field[] is for one register, containing all the fields for this register.
 * The helper is generating all registers for the device.
 *
 * The generated code looks like:
 * static struct reg_field reg_fields_$reg_name_1$ [] = {
 *     {.n = "field_name_x", .abbr = "abbr name x", ...  },
 *     ...
 *     {.n = "field_name_y", .abbr = "abbr name x", ...  },
 * };
 * .....
 * static struct reg_field reg_fields_$reg_name_n$ [] = {
 *     {.n = "field_name_x", .abbr = "abbr name x", ...  },
 *     ...
 *     {.n = "field_name_y", .abbr = "abbr name x", ...  },
 * };
 *
 */
#define DEV(name, a, ...) struct dummy_for_reg_flds_##name {
#define REG(name, a, ...) }; static struct reg_field _reg_fields_##name [] = {
#define FLD(name, _msk, _abbr, _desc) \
    { \
           .n = #name, \
           .abbr = #_abbr, \
           .desc = _desc, \
           .msk = _msk, \
           .cnt = sizeof(_field_opts_##name)/sizeof(_field_opts_##name[0]), \
           .opts = _field_opts_##name, \
   },
#define OPT(...)
#define END_DEV(...) };

#elif defined (DEFINE_DEV_REGS)
#undef DEFINE_DEV_REGS

#undef DEV
#undef REG
#undef FLD
#undef OPT
#undef END_DEV

/*
 * Help to generate every device registers array which contains all the registers of this device.
 *
 * The generated code looks like:
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
#define DEV(name, a, ...) static struct reg_desc _regs_##name [] = {
#define REG(name, _id, _abbr, _desc) \
    { .n = #name, .abbr = #_abbr, .desc = _desc, .loc = _id, \
      .cnt = sizeof(_reg_fields_##name)/sizeof(_reg_fields_##name[0]), \
      .flds = _reg_fields_##name, \
    },
#define FLD(...)
#define OPT(...)
#define END_DEV(...) };


#elif defined (DEFINE_REGS_DESC_TBL)
#undef DEFINE_REGS_DESC_TBL

#undef DEV
#undef REG
#undef FLD
#undef OPT
#undef END_DEV

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
 * NOTE:
 * To guard duplicated device register descript tables are defined and cause memory waste unawared,
 * we intended not define reg_desc_tbl_##tbl_name as static.
 * Since the reg_desc_tbl chains reg_desc chains reg_field, other struct array instances still could be static.
 */
#define _DEV_TBL(dev_name, tbl_name, _abbr, _desc) \
    struct reg_desc_tbl reg_desc_tbl_##tbl_name = { \
        .n = #dev_name, \
        .abbr = #_abbr, \
        .desc = _desc, \
        .cnt = sizeof(_regs_##dev_name)/sizeof(_regs_##dev_name[0]), \
        .regs = _regs_##dev_name, \
    };
#define DEV_TBL(dev_name, tbl_name, _abbr, _desc) _DEV_TBL(dev_name, tbl_name, _abbr, _desc)

/*
 * The reason of having the above DEV_TBL() code instead of directly define DEV()
 * is to have the extendibility to specify suffix for the table name.
 * Eg:
 *
 * if defined (DEV_REGS_TBL_NAME)
 * #define DEV(name, _resv, _abbr, _desc) DEV_TBL(name, DEV_REGS_TBL_NAME, _abbr, _desc)
 * #endif
 * */
#define DEV(name, _resv, _abbr, _desc) DEV_TBL(name, name, _abbr, _desc)

#define REG(...)
#define FLD(...)
#define OPT(...)
#define END_DEV(...)

#elif defined (SUBMIT_DEV_REGS_DESC_TBL)
/*TODO: support register(submit) the tbl to a list */
#undef DEV
#undef REG
#undef FLD
#undef OPT
#undef END_DEV


#else
#error The usage of reg_desc_hlper.h is not correct, one of reg_desc_hlper.h needed MACRO must be defined before include this file. Please contact the author of this file.

#endif
