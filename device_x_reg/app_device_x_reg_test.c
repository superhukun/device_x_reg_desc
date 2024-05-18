/*
 * Author: Ken Hu
 * Date: 2024-05-16
 */

#include <stdio.h>
#include <string.h>

#include "reg_desc_tbl.h"

#include "device_x_reg.h"

static char buf[1024];

static void test_decorator_cb (struct reg_parse_req_result* res, void *ctx)
{
    printf("DECORATOR CALLBACK\n");
    printf("result status:%s\n", reg_desc_parse_result_status_str(res->progress));
    printf("result :%s\n", res->buf);
}

struct reg_parse_test_item {
    uint32_t reg_loc;
    uint32_t reg_value;
    uint32_t intested_msk;
    char *tbl;
    void (*cb)(struct reg_parse_req_result* res, void *ctx);
};

struct reg_parse_test_item test_cases [] = {
    {.reg_loc = 0xfffff, .reg_value = 0x5a5a, .intested_msk = 0xffff, .tbl = NULL, .cb = NULL },
    {.reg_loc = 0xfffff, .reg_value = 0x5a5a, .intested_msk = 0xffff, .tbl = "DEV_TEST", .cb = NULL },
    {.reg_loc = 0x12345, .reg_value = 0x1234, .intested_msk = 0xffff, .tbl = "DEV_TEST", .cb = NULL },
    {.reg_loc = 0x12345, .reg_value = 0xff32, .intested_msk = 0xff,   .tbl = "DEV_TEST", .cb = NULL },
    {.reg_loc = 0x12345, .reg_value = 0xff23, .intested_msk = 0xff,   .tbl = "DEV_TEST", .cb = NULL },
    {.reg_loc = 0x12345, .reg_value = 0x5a5a, .intested_msk = 0xffff, .tbl = "DEV_TEST", .cb = NULL },
    {.reg_loc = 0x8877,  .reg_value = 0xa5a5, .intested_msk = 0xffff, .tbl = "DEV_TEST", .cb = NULL },
    {.reg_loc = 0x12345, .reg_value = 0x5a5a, .intested_msk = 0xffff, .tbl = "DEV_TEST", .cb = test_decorator_cb},
    {.reg_loc = 0x8877,  .reg_value = 0xa5a5, .intested_msk = 0xffff, .tbl = "DEV_TEST", .cb = test_decorator_cb},
};

static uint32_t masks[] = {
    0x0,
    ~0x0,
    0x1,
    ~0x1,
    0xff00ff,
    ~0xff00ff,
    0x5a5a5a5a,
    ~0x5a5a5a5a,
};

static void test_msk_to_str_msb (void)
{
    uint32_t i;
    printf("| Mask HEX | MSB Bits Description |\n");
    for (i = 0; i < sizeof(masks)/sizeof(masks[0]); i++) {
        printf("| 0X%08X | %10s | \n", masks[i], msk_to_str_msb(masks[i]));
    }
}

static void test_reg_desc_tbl_parse (void)
{
    struct reg_parse_req_result result = {0};
    int i;
    struct reg_desc_tbl *lkptbl;

    result.buf = buf;
    result.buf_size = sizeof(buf);

    result.dev_info_flg = INFO_L_ALL;
    result.reg_info_flg = INFO_L_ALL;
    result.fld_info_flg = INFO_L_ALL;
    result.opt_info_flg = INFO_L_ALL;

    for (i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        printf("TestCase[%d], lkp table name %s\n", i, test_cases[i].tbl);
        lkptbl = get_reg_desc_tbl_from_registry(test_cases[i].tbl);

        reg_desc_parse_reg_value(test_cases[i].reg_loc, test_cases[i].reg_value, test_cases[i].intested_msk,
                                 lkptbl,
                                 &result,
                                 test_cases[i].cb, NULL);
        printf("result status:%s\n", reg_desc_parse_result_status_str(result.progress));
        printf("result :\n%s\n", result.buf);
    }
}

static void test_reg_enum_ids (void)
{
    uint32_t regid;
    uint32_t value, msk;

    char buf[1024];
    struct reg_parse_req_result result = {0};
    struct reg_desc_tbl *lkptbl;

    regid = FOOD_SELECT;
    value = FOOD_SLCT_FRUIT_PINEAPPLE | FOOD_SLCT_DRINK_TEA;
    msk = FOOD_SLCT_FRUIT_MSK | FOOD_SLCT_DRINK_MSK;

    printf("regid %#08x, value %#08x mask %#08x\n",
            regid, value, msk);

    result.buf = buf;
    result.buf_size = sizeof(buf);

    lkptbl = get_reg_desc_tbl_from_registry("DEV_TEST");
    reg_desc_parse_reg_value(regid, value, msk,
                             lkptbl,
                             &result,
                             NULL, NULL);
    printf("%s", buf);
}

static void show_usage (char *app)
{
    printf("%s [cmd]\n", app);
    printf("cmd is one of the following:\n");
    printf("\n");
    printf("\ttest_msk          : do the test cases for converting mask value to string desc\n");
    printf("\ttest_reg_enum     : do the test bases for register related(including fields, field optional values) enum IDs\n");
    printf("\ttest_reg_vl_parse : do the test bases for register value parsing\n");
}

int main (int argc, char *argv[])
{
    if (argc > 1) {
        if (0 == strncmp(argv[1], "test_msk", strlen("test_msk")+1)) {
            test_msk_to_str_msb();
        } else if (0 == strncmp(argv[1], "test_reg_enum", strlen("test_reg_enum")+1)) {
            test_reg_enum_ids();
        } else if (0 == strncmp(argv[1], "test_reg_vl_parse", strlen("test_reg_vl_parse")+1)) {
            test_reg_desc_tbl_parse();
        } else if (0 == strncmp(argv[1], "help", strlen("help")+1)) {
            show_usage(argv[0]);
        } else {
            show_usage(argv[0]);
        }
        return 0;
    }

    show_usage(argv[0]);
    return 0;
}
