/*
 * Author: Ken Hu
 * Date: 2024-05-16
 */

#if !defined(DEV_REG_FILE)
#error Please define DEV_REG_FILE before include this file.

#else

#define DEFINE_FLD_OPTS
#include DEV_REG_FILE

#define DEFINE_REG_FLDS
#include DEV_REG_FILE

#define DEFINE_DEV_REGS
#include DEV_REG_FILE


#define DEFINE_REGS_DESC_TBL
#include DEV_REG_FILE

#endif
