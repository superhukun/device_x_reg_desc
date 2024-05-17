/*
 * Author: Ken Hu
 * Date: 2024-05-16
 */

#include <stdio.h>

#include "reg_desc_tbl.h"

#define DEV_REG_FILE "device_x.reg"
#include "reg_file_to_desc_tbl.h"

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
    struct reg_desc_tbl *tbl;
    void (*cb)(struct reg_parse_req_result* res, void *ctx);
};

struct reg_parse_test_item test_cases [] = {
    {.reg_loc = 0xfffff, .reg_value = 0x5a5a, .intested_msk = 0xffff, .tbl = NULL, .cb = NULL },
    {.reg_loc = 0xfffff, .reg_value = 0x5a5a, .intested_msk = 0xffff, .tbl = &reg_desc_tbl_DEV_TEST, .cb = NULL },
    {.reg_loc = 0x12345, .reg_value = 0x1234, .intested_msk = 0xffff, .tbl = &reg_desc_tbl_DEV_TEST, .cb = NULL },
    {.reg_loc = 0x12345, .reg_value = 0xff32, .intested_msk = 0xff, .tbl = &reg_desc_tbl_DEV_TEST, .cb = NULL },
    {.reg_loc = 0x12345, .reg_value = 0xff23, .intested_msk = 0xff, .tbl = &reg_desc_tbl_DEV_TEST, .cb = NULL },
    {.reg_loc = 0x12345, .reg_value = 0x5a5a, .intested_msk = 0xffff, .tbl = &reg_desc_tbl_DEV_TEST, .cb = NULL },
    {.reg_loc = 0x8877, .reg_value = 0xa5a5, .intested_msk = 0xffff, .tbl = &reg_desc_tbl_DEV_TEST, .cb = NULL },
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

void test_msk_to_str_msb (void)
{
    uint32_t i;
    for (i = 0; i < sizeof(masks)/sizeof(masks[0]); i++) {
        printf("%#08x bits set:%s\n", masks[i], msk_to_str_msb(masks[i]));
    }
}

void test_reg_desc_tbl_parse (void)
{
    struct reg_parse_req_result result = {0};
    int i;

    result.buf = buf;
    result.buf_size = sizeof(buf);

    result.dev_info_flg = INFO_L_ALL;
    result.reg_info_flg = INFO_L_ALL;
    result.fld_info_flg = INFO_L_ALL;
    result.opt_info_flg = INFO_L_ALL;

    for (i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        printf("\n\nTestCase[%d]\n", i);
        reg_desc_parse_reg_value(test_cases[i].reg_loc, test_cases[i].reg_value, test_cases[i].intested_msk,
                                 test_cases[i].tbl,
                                 &result,
                                 test_cases[i].cb, NULL);
        printf("result status:%s\n", reg_desc_parse_result_status_str(result.progress));
        printf("result :\n\n%s\n\n", result.buf);
    }
}

int main (int argc, char *argv[])
{
    //test_msk_to_str_msb();

    test_reg_desc_tbl_parse();

    return 0;

}
