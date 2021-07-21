#include <stdint.h>
#include "flash_avl.h"

const avl_bin_hdr_t hdr =
{
    .chip_id    = SMART_RTL8762D,
    .ver        = 0,
    .total_num  = 0,
    .crc16      = 0,
};

const flash_avl_t avl[] =
{
    {
        /* GD25Q127C */
        /* size,         manu, device */
        FLASH_BITS_128M, 0xC8, 0x4018,
        /* Basic Cmd
        rd_do, rd_dio, rdsr, wrsr, be64, se,   pwdn_rls, pwdn */
        {0x3b,  0xbb,  0x05, 0x01, 0xd8, 0x20, 0xab,     0xb9},
        /* Advance Cmd
        qe,   rd_qo, rd_qio, pp4, wren_vol, rdsr2, wrsr2, rdcr, hpm, rdsr3, wrsr3, rsvd0 */
        {0x09, 0x6b, 0xeb,  0x32, 0x50,     0x35,  0x31,  0,    0,   0x15,  0x11,  0x0},
        /* Query Info
        tb_oft, bp_all_lv, bp_mask, cmd_ex4b, pkg_info, en4b, rd4bsr, ads_oft */
        {5,     7,         0x07,    0,      0,        0,     0,     0}
    },
    {
        /* W25Q128JVPIQ */
        /* size,         manu, device */
        FLASH_BITS_128M, 0x17, 0x4018,
        /* Basic Cmd
        rd_do, rd_dio, rdsr, wrsr, be64, se,   pwdn_rls, pwdn */
        {0x3b,  0xbb,  0x05, 0x01, 0xd8, 0x20, 0xab,     0xb9},
        /* Advance Cmd
        qe,   rd_qo, rd_qio, pp4, wren_vol, rdsr2, wrsr2, rdcr, hpm, rdsr3, wrsr3, rsvd0 */
        {0x09, 0x6b, 0xeb,  0x32, 0x50,     0x35,  0x31,  0,    0,   0x15,  0x11,  0x0},
        /* Query Info
        tb_oft, bp_all_lv, bp_mask, cmd_ex4b, pkg_info, en4b, rd4bsr, ads_oft */
        {5,     7,         0x07,    0,      0,        0,     0,     0}
    },
    {
        /* ZB25VQ32B */
        /* size,         manu, device */
        FLASH_BITS_64M,  0x5E, 0x4016,
        /* Basic Cmd
        rd_do, rd_dio, rdsr, wrsr, be64, se,   pwdn_rls, pwdn */
        {0x3b,  0xbb,  0x05, 0x01, 0xd8, 0x20, 0xab,     0xb9},
        /* Advance Cmd
        qe,   rd_qo, rd_qio, pp4, wren_vol, rdsr2, wrsr2, rdcr, hpm, rdsr3, wrsr3, rsvd0 */
        {0x09, 0x6b, 0xeb,  0x32, 0x50,     0x35,  0x31,  0,    0,   0x15,  0x11,  0x0},
        /* Query Info
        tb_oft, bp_all_lv, bp_mask, cmd_ex4b, pkg_info, en4b, rd4bsr, ads_oft */
        {5,     7,         0x07,    0,      0,        0,     0,     0}
    },
    {
        /* ZB25VQ64A */
        /* size,         manu, device */
        FLASH_BITS_64M,  0x5E, 0x4017,
        /* Basic Cmd
        rd_do, rd_dio, rdsr, wrsr, be64, se,   pwdn_rls, pwdn */
        {0x3b,  0xbb,  0x05, 0x01, 0xd8, 0x20, 0xab,     0xb9},
        /* Advance Cmd
        qe,   rd_qo, rd_qio, pp4, wren_vol, rdsr2, wrsr2, rdcr, hpm, rdsr3, wrsr3, rsvd0 */
        {0x09, 0x6b, 0xeb,  0x32, 0x50,     0x35,  0x31,  0,    0,   0x15,  0x11,  0x0},
        /* Query Info
        tb_oft, bp_all_lv, bp_mask, cmd_ex4b, pkg_info, en4b, rd4bsr, ads_oft */
        {5,     7,         0x07,    0,      0,        0,     0,     0}
    },
    {
        /* ZB25VQ128 */
        /* size,         manu, device */
        FLASH_BITS_128M,  0x5E, 0x4018,
        /* Basic Cmd
        rd_do, rd_dio, rdsr, wrsr, be64, se,   pwdn_rls, pwdn */
        {0x3b,  0xbb,  0x05, 0x01, 0xd8, 0x20, 0xab,     0xb9},
        /* Advance Cmd
        qe,   rd_qo, rd_qio, pp4, wren_vol, rdsr2, wrsr2, rdcr, hpm, rdsr3, wrsr3, rsvd0 */
        {0x09, 0x6b, 0xeb,  0x32, 0x50,     0x35,  0x31,  0,    0,   0x15,  0x11,  0x0},
        /* Query Info
        tb_oft, bp_all_lv, bp_mask, cmd_ex4b, pkg_info, en4b, rd4bsr, ads_oft */
        {5,     7,         0x07,    0,      0,        0,     0,     0}
    },
    {
        /* P25Q32H */
        /* size,         manu, device */
        FLASH_BITS_32M,  0x85, 0x6016,
        /* Basic Cmd
        rd_do, rd_dio, rdsr, wrsr, be64, se,   pwdn_rls, pwdn */
        {0x3b,  0xbb,  0x05, 0x01, 0xd8, 0x20, 0xab,     0xb9},
        /* Advance Cmd
        qe,   rd_qo, rd_qio, pp4, wren_vol, rdsr2, wrsr2, rdcr, hpm, rdsr3, wrsr3, rsvd0 */
        {0x09, 0x6b, 0xeb,  0x32, 0x50,     0x35,  0x31,  0,    0,   0x15,  0x11,  0x0},
        /* Query Info
        tb_oft, bp_all_lv, bp_mask, cmd_ex4b, pkg_info, en4b, rd4bsr, ads_oft */
        {5,     7,         0x07,    0,      0,        0,     0,     0}
    },
    {
        /* P25Q64H */
        /* size,         manu, device */
        FLASH_BITS_64M,  0x85, 0x6017,
        /* Basic Cmd
        rd_do, rd_dio, rdsr, wrsr, be64, se,   pwdn_rls, pwdn */
        {0x3b,  0xbb,  0x05, 0x01, 0xd8, 0x20, 0xab,     0xb9},
        /* Advance Cmd
        qe,   rd_qo, rd_qio, pp4, wren_vol, rdsr2, wrsr2, rdcr, hpm, rdsr3, wrsr3, rsvd0 */
        {0x09, 0x6b, 0xeb,  0x32, 0x50,     0x35,  0x31,  0,    0,   0x15,  0x11,  0x0},
        /* Query Info
        tb_oft, bp_all_lv, bp_mask, cmd_ex4b, pkg_info, en4b, rd4bsr, ads_oft */
        {5,     7,         0x07,    0,      0,        0,     0,     0}
    },
    {
        /* BY25Q128AL */
        /* size,         manu, device */
        FLASH_BITS_128M, 0xE0, 0x6018,
        /* Basic Cmd
        rd_do, rd_dio, rdsr, wrsr, be64, se,   pwdn_rls, pwdn */
        {0x3b,  0xbb,  0x05, 0x01, 0xd8, 0x20, 0xab,     0xb9},
        /* Advance Cmd
        qe,   rd_qo, rd_qio, pp4, wren_vol, rdsr2, wrsr2, rdcr, hpm, rdsr3, wrsr3, rsvd0 */
        {0x09, 0x6b, 0xeb,  0x32, 0x50,     0x35,  0x31,  0,    0,   0x15,  0x11,  0x0},
        /* Query Info
        tb_oft, bp_all_lv, bp_mask, cmd_ex4b, pkg_info, en4b, rd4bsr, ads_oft */
        {5,     7,         0x07,    0,      0,        0,     0,     0}
    },
    {
        /* BY25Q128AS */
        /* size,         manu, device */
        FLASH_BITS_128M, 0x68, 0x4018,
        /* Basic Cmd
        rd_do, rd_dio, rdsr, wrsr, be64, se,   pwdn_rls, pwdn */
        {0x3b,  0xbb,  0x05, 0x01, 0xd8, 0x20, 0xab,     0xb9},
        /* Advance Cmd
        qe,   rd_qo, rd_qio, pp4, wren_vol, rdsr2, wrsr2, rdcr, hpm, rdsr3, wrsr3, rsvd0 */
        {0x09, 0x6b, 0xeb,  0x32, 0x50,     0x35,  0x31,  0,    0,   0x15,  0x11,  0x0},
        /* Query Info
        tb_oft, bp_all_lv, bp_mask, cmd_ex4b, pkg_info, en4b, rd4bsr, ads_oft */
        {5,     7,         0x07,    0,      0,        0,     0,     0}
    },
    {
        /* W25Q256JW-IQ */
        /* size,         manu, device */
        FLASH_BITS_256M, 0xEF, 0x6019,
        /* Basic Cmd
        rd_do, rd_dio, rdsr, wrsr, be64, se,   pwdn_rls, pwdn */
        {0x3b,  0xbb,  0x05, 0x01, 0xd8, 0x20, 0xab,     0xb9},
        /* Advance Cmd
        qe,   rd_qo, rd_qio, pp4, wren_vol, rdsr2, wrsr2, rdcr, hpm, rdsr3, wrsr3, rsvd0 */
        {0x09, 0x6b, 0xeb,  0x32, 0x50,     0x35,  0x31,  0,    0,   0x15,  0x11,  0x0},
        /* Query Info
        tb_oft, bp_all_lv, bp_mask, cmd_ex4b, pkg_info, en4b, rd4bsr, ads_oft */
        {6,     10,         0x0F,    0xe9,      0,     0xb7,  0x15,  0x01}
    },
    {
        /* MX25U25645G */
        /* size,         manu, device */
        FLASH_BITS_256M, 0xC2, 0x2539,
        /* Basic Cmd
        rd_do, rd_dio, rdsr, wrsr, be64, se,   pwdn_rls, pwdn */
        {0x3b,  0xbb,  0x05, 0x01, 0xd8, 0x20, 0xab,     0xb9},
        /* Advance Cmd
        qe,   rd_qo, rd_qio, pp4, wren_vol, rdsr2, wrsr2, rdcr, hpm, rdsr3, wrsr3, rsvd0 */
        {0x06, 0x6b, 0xeb,  0x38,    0,     0x15,   0,      0,    0,   0,     0,  0x0},
        /* Query Info
        tb_oft, bp_all_lv, bp_mask, cmd_ex4b, pkg_info, en4b, rd4bsr, ads_oft */
        {11,     10,         0x0F,    0xe9,      0,   0xb7,  0x15,  0x05}
    },
    {
        /* XM25QH64C */
        /* size,        manu, device */
        FLASH_BITS_64M, 0x20, 0x4017,
        /* Basic Cmd
        rd_do, rd_dio, rdsr, wrsr, be64, se,   pwdn_rls, pwdn */
        {0x3b,  0xbb,  0x05, 0x01, 0xd8, 0x20, 0xab,     0xb9},
        /* Advance Cmd
        qe,   rd_qo, rd_qio, pp4, wren_vol, rdsr2, wrsr2, rdcr, hpm, rdsr3, wrsr3, rsvd0 */
        {0x09, 0x6b, 0xeb,  0x32, 0x50,     0x35,  0x31,  0,    0,   0x15,  0x11,  0x0},
        /* Query Info
        tb_oft, bp_all_lv, bp_mask, cmd_ex4b, pkg_info, en4b, rd4bsr, ads_oft */
        {5,     7,         0x07,    0,      0,        0,     0,     0}
    },
    {
        /* XM25QU64C */
        /* size,        manu, device */
        FLASH_BITS_64M, 0x20, 0x4117,
        /* Basic Cmd
        rd_do, rd_dio, rdsr, wrsr, be64, se,   pwdn_rls, pwdn */
        {0x3b,  0xbb,  0x05, 0x01, 0xd8, 0x20, 0xab,     0xb9},
        /* Advance Cmd
        qe,   rd_qo, rd_qio, pp4, wren_vol, rdsr2, wrsr2, rdcr, hpm, rdsr3, wrsr3, rsvd0 */
        {0x09, 0x6b, 0xeb,  0x32, 0x50,     0x35,  0x31,  0,    0,   0x15,  0x11,  0x0},
        /* Query Info
        tb_oft, bp_all_lv, bp_mask, cmd_ex4b, pkg_info, en4b, rd4bsr, ads_oft */
        {5,     7,         0x07,    0,      0,        0,     0,     0}
    },
    {
        /* XM25QH256C */
        /* size,         manu, device */
        FLASH_BITS_256M, 0x20, 0x4019,
        /* Basic Cmd
        rd_do, rd_dio, rdsr, wrsr, be64, se,   pwdn_rls, pwdn */
        {0x3b,  0xbb,  0x05, 0x01, 0xd8, 0x20, 0xab,     0xb9},
        /* Advance Cmd
        qe,   rd_qo, rd_qio, pp4, wren_vol, rdsr2, wrsr2, rdcr, hpm, rdsr3, wrsr3, rsvd0 */
        {0x09, 0x6b, 0xeb,  0x32, 0x50,     0x35,  0x31,  0,    0,   0x15,  0x11,  0x0},
        /* Query Info
        tb_oft, bp_all_lv, bp_mask, cmd_ex4b, pkg_info, en4b, rd4bsr, ads_oft */
        {6,     10,         0x0F,    0xe9,      0,      0xb7,   0x15,  0x01}
    },
    {
        /* GD25Q256E */
        /* size,         manu, device */
        FLASH_BITS_256M, 0xC8, 0x4019,
        /* Basic Cmd
        rd_do, rd_dio, rdsr, wrsr, be64, se,   pwdn_rls, pwdn */
        {0x3b,  0xbb,  0x05, 0x01, 0xd8, 0x20, 0xab,     0xb9},
        /* Advance Cmd
        qe,   rd_qo, rd_qio, pp4, wren_vol, rdsr2, wrsr2, rdcr, hpm, rdsr3, wrsr3, rsvd0 */
        {0x09, 0x6b, 0xeb,  0x32, 0x50,     0x35,  0x31,  0,    0,   0x15,  0x11,  0x0},
        /* Query Info
        tb_oft, bp_all_lv, bp_mask, cmd_ex4b, pkg_info, en4b, rd4bsr, ads_oft */
        {6,     10,         0x0F,    0xe9,      0,     0xb7,  0x35,  0x01}
    },
};
