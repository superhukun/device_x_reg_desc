/*
 * Author: Ken Hu
 * Date: 2024-05-20
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "reg_desc_tbl.h"

struct reg_value_lkp_cfg {
    char *tbl_name;
    uint32_t reg;
    uint32_t val;
    uint32_t msk;

    literal_info_flg_t dev_info_flg;
    literal_info_flg_t reg_info_flg;
    literal_info_flg_t fld_info_flg;
    literal_info_flg_t opt_info_flg;
};

static void show_usage (char *app)
{
    printf("%s [OPTIONS]\n", app);
    printf("Options are:\n");
    printf("--tbl | -t {table_name} : Specify the lookup table to parse the register value.\n");
    printf("--reg | -r {U32 in Hex} : Specify the location/address/regid of the register.\n");
    printf("--val | -v {U32 in Hex} : Specify the value of the register.\n");
    printf("--msk | -m {U32 in Hex} : Specify the mask of the value to parse.\n");
    printf("--[dev|reg|fld|opt|ALL]_[a|n|d|A]_literal: specify literal information level for different components.\n");
    printf("                                 [Components]\n");
    printf("                                     dev : the device of this register\n");
    printf("                                     reg : the register\n");
    printf("                                     fld : register field\n");
    printf("                                     opt : optional values of field\n");
    printf("                                     ALL : All of the components\n");

    printf("                                 [Levels]\n");
    printf("                                     a: abbreviation/acry of component\n");
    printf("                                     n: normally show name of component\n");
    printf("                                     d: detail description of component\n");
    printf("                                     A: all, a, n, d\n");
    printf("--help | -h : Show this help information.\n");
    printf("\n");
}

static void load_default_cfg (struct reg_value_lkp_cfg *cfg)
{
    cfg->tbl_name = "";
    cfg->reg = 0;
    cfg->val = 0x0;
    cfg->msk = 0xffffffff;
}

#define info_opt(_comp, _l) ( (((_comp) & 0xff ) << 8 ) | ((_l) & 0xff) )
static void build_cfg_from_args (int argc, char *argv[], struct reg_value_lkp_cfg *cfg)
{
    int c;
    while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"tbl",           required_argument, 0,  't' },
            {"reg",           required_argument, 0,  'r' },
            {"val",           required_argument, 0,  'v' },
            {"msk",           required_argument, 0,  'm' },
            {"help",          no_argument,       0,  'h' },

            {"dev_a_literal", no_argument,       0,  info_opt('d', 'a') },
            {"dev_n_literal", no_argument,       0,  info_opt('d', 'n') },
            {"dev_d_literal", no_argument,       0,  info_opt('d', 'd') },
            {"dev_A_literal", no_argument,       0,  info_opt('d', 'A') },

            {"reg_a_literal", no_argument,       0,  info_opt('r', 'a') },
            {"reg_n_literal", no_argument,       0,  info_opt('r', 'n') },
            {"reg_d_literal", no_argument,       0,  info_opt('r', 'd') },
            {"reg_A_literal", no_argument,       0,  info_opt('r', 'A') },

            {"fld_n_literal", no_argument,       0,  info_opt('f', 'n') },
            {"fld_a_literal", no_argument,       0,  info_opt('f', 'a') },
            {"fld_d_literal", no_argument,       0,  info_opt('f', 'd') },
            {"fld_A_literal", no_argument,       0,  info_opt('f', 'A') },

            {"opt_a_literal", no_argument,       0,  info_opt('o', 'a') },
            {"opt_n_literal", no_argument,       0,  info_opt('o', 'n') },
            {"opt_d_literal", no_argument,       0,  info_opt('o', 'd') },
            {"opt_A_literal", no_argument,       0,  info_opt('o', 'A') },

            {"ALL_a_literal", no_argument,       0,  info_opt('A', 'a') },
            {"ALL_n_literal", no_argument,       0,  info_opt('A', 'n') },
            {"ALL_d_literal", no_argument,       0,  info_opt('A', 'd') },
            {"ALL_A_literal", no_argument,       0,  info_opt('A', 'A') },

            {0,         0,                 0,  0 }
        };

        c = getopt_long(argc, argv, "t:r:v:m:h",
                long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            case 't':
                cfg->tbl_name = optarg;
                break;
            case 'r':
                cfg->reg = strtol(optarg, NULL, 16);
                break;
            case 'v':
                cfg->val = strtol(optarg, NULL, 16);
                break;
            case 'm':
                cfg->msk = strtol(optarg, NULL, 16);
                break;

            case info_opt('d', 'a'):
                cfg->dev_info_flg |= INFO_L_ABBR;
                break;
            case info_opt('d', 'n'):
                cfg->dev_info_flg |= INFO_L_NAME;
                break;
            case info_opt('d', 'd'):
                cfg->dev_info_flg |= INFO_L_DESC;
                break;
            case info_opt('d', 'A'):
                cfg->dev_info_flg |= INFO_L_ALL;
                break;

            case info_opt('r', 'a'):
                cfg->reg_info_flg |= INFO_L_ABBR;
                break;
            case info_opt('r', 'n'):
                cfg->reg_info_flg |= INFO_L_NAME;
                break;
            case info_opt('r', 'd'):
                cfg->reg_info_flg |= INFO_L_DESC;
                break;
            case info_opt('r', 'A'):
                cfg->reg_info_flg |= INFO_L_ALL;
                break;

            case info_opt('f', 'a'):
                cfg->fld_info_flg |= INFO_L_ABBR;
                break;
            case info_opt('f', 'n'):
                cfg->fld_info_flg |= INFO_L_NAME;
                break;
            case info_opt('f', 'd'):
                cfg->fld_info_flg |= INFO_L_DESC;
                break;
            case info_opt('f', 'A'):
                cfg->fld_info_flg |= INFO_L_ALL;
                break;

            case info_opt('o', 'a'):
                cfg->opt_info_flg |= INFO_L_ABBR;
                break;
            case info_opt('o', 'n'):
                cfg->opt_info_flg |= INFO_L_NAME;
                break;
            case info_opt('o', 'd'):
                cfg->opt_info_flg |= INFO_L_DESC;
                break;
            case info_opt('o', 'A'):
                cfg->opt_info_flg |= INFO_L_ALL;
                break;

            case info_opt('A', 'a'):
                cfg->dev_info_flg |= INFO_L_ABBR;
                cfg->reg_info_flg |= INFO_L_ABBR;
                cfg->fld_info_flg |= INFO_L_ABBR;
                cfg->opt_info_flg |= INFO_L_ABBR;
                break;
            case info_opt('A', 'n'):
                cfg->dev_info_flg |= INFO_L_NAME;
                cfg->reg_info_flg |= INFO_L_NAME;
                cfg->fld_info_flg |= INFO_L_NAME;
                cfg->opt_info_flg |= INFO_L_NAME;
                break;
            case info_opt('A', 'd'):
                cfg->dev_info_flg |= INFO_L_DESC;
                cfg->reg_info_flg |= INFO_L_DESC;
                cfg->fld_info_flg |= INFO_L_DESC;
                cfg->opt_info_flg |= INFO_L_DESC;
                break;
            case info_opt('A', 'A'):
                cfg->dev_info_flg |= INFO_L_ALL;
                cfg->reg_info_flg |= INFO_L_ALL;
                cfg->fld_info_flg |= INFO_L_ALL;
                cfg->opt_info_flg |= INFO_L_ALL;
                break;

            case '?':
                show_usage(argv[0]);
                exit(0);
                break;

            case 'h':
            default:
                show_usage(argv[0]);
                exit(0);
        }
    }
}

static void show_cfg (struct reg_value_lkp_cfg *cfg)
{
    printf("Table to use : %s\n", cfg->tbl_name);
    printf("Register loc  : 0x%08x\n", cfg->reg);
    printf("Value : 0x%08x\n", cfg->val);
    printf("Mask : 0x%08x\n", cfg->msk);

    printf("dev info literal flg: 0x%08x\n", cfg->dev_info_flg);
    printf("reg info literal flg: 0x%08x\n", cfg->reg_info_flg);
    printf("fld info literal flg: 0x%08x\n", cfg->fld_info_flg);
    printf("opt info literal flg: 0x%08x\n", cfg->opt_info_flg);

}

static char buf[1024];
int main (int argc, char *argv[])
{
    struct reg_value_lkp_cfg cfg = {0};
    struct reg_desc_tbl *tbl;
    struct reg_parse_req_result req_res = {0};

    load_default_cfg(&cfg);
    build_cfg_from_args(argc, argv, &cfg);
    show_cfg(&cfg);

    tbl = get_reg_desc_tbl_from_registry(cfg.tbl_name);

    req_res.buf = buf;
    req_res.buf_size = sizeof(buf);
    req_res.dev_info_flg = cfg.dev_info_flg;
    req_res.reg_info_flg = cfg.reg_info_flg;
    req_res.fld_info_flg = cfg.fld_info_flg;
    req_res.opt_info_flg = cfg.opt_info_flg;

    reg_desc_parse_reg_value(cfg.reg, cfg.val, cfg.msk, tbl, &req_res, NULL, NULL);
    printf("%s\n", buf);

    return 0;
}
