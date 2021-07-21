#ifndef _FLASH_AVL_H_
#define _FLASH_AVL_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* define flash size (bit) */
#define FLASH_BITS_2M       (0x12U)
#define FLASH_BITS_4M       (0x13U)
#define FLASH_BITS_8M       (0x14U)
#define FLASH_BITS_16M      (0x15U)
#define FLASH_BITS_32M      (0x16U)
#define FLASH_BITS_64M      (0x17U)
#define FLASH_BITS_128M     (0x18U)
#define FLASH_BITS_256M     (0x19U)

#define FLASH_AVL_BIN_BASE  (0x801A00U)
#define SMART_RTL8762C      (0x8762CU)
#define SMART_RTL8762D      (0x8762DU)

/*
please reference bb2 simplified struct

flash avl interface parse avl binary and get avl header, then copy to flash basic, adv, query_info
*/

typedef struct flash_basic_cmd
{
//    uint8_t cmd_rd_data;            /*!< Specifies single data read cmd */
    uint8_t cmd_rd_dual_o;          /*!< Specifies dual data read cmd */
    uint8_t cmd_rd_dual_io;         /*!< Specifies dual data/addr read cmd */
//    uint8_t cmd_pp;                 /*!< Specifies single page program cmd*/
//    uint8_t cmd_wr_en;              /*!< Specifies the Write Enable(WREN) instruction*/
//    uint8_t cmd_rd_id;              /*!< Specifies the Read ID instruction*/
    uint8_t cmd_rd_status;          /*!< Specifies the Read Status Register*/
    uint8_t cmd_wr_status;          /*!< Specifies the Write Status Register*/
//    uint8_t cmd_chip_e;             /*!< Specifies the Erase Chip instruction which is for erasing whole chip*/
    uint8_t cmd_block_e;            /*!< Specifies the Erase Block instruction which is for erasing 64kB*/
    uint8_t cmd_sector_e;           /*!< Specifies the Erase Sector instruction which is for erasing 4kB*/
    uint8_t cmd_pwdn_release;       /*!< Specifies the Release from Deep Power Down instruction*/
    uint8_t cmd_pwdn;               /*!< Specifies the Deep Power Down instruction*/
//    uint8_t cmd_en_reset;           /*!< Specifies the Enable Reset instruction*/
//    uint8_t cmd_reset;              /*!< Specifies the Reset instruction*/
//    uint8_t rsvd0;                  /*!< Reserved */
} flash_basic_cmd_t;

typedef struct flash_adv_cmd
{
    uint8_t flash_qe_bit;           /*!< Specifies the QE bit in status register which is used to
                                         enable Quad I/O mode . */
    uint8_t cmd_rd_quad_o;          /*!< Specifies quad data read cmd */
    uint8_t cmd_rd_quad_io;         /*!< Specifies quad data/addr read cmd */
    uint8_t cmd_ppx4;               /*!< Specifies quad page program cmd */
    uint8_t cmd_wr_en_vol;          /*!< Specifies the Write Enable for Volatile Status Register instruction*/
    uint8_t cmd_rd_status2;         /*!< Specifies the Read Status Register2*/
    uint8_t cmd_wr_status2;         /*!< Specifies the Write Status Register2*/
    uint8_t cmd_rdcr;               /*!< Specifies read config command, it's used on MXIC for high power mode*/
    uint8_t cmd_hpm;                /*!< Specifies the high performance command.*/
    uint8_t cmd_rd_status3;         /*!< Specifies the Read Status Register3 */
    uint8_t cmd_wr_status3;         /*!< Specifies the Write Status Register3 */
    uint8_t rsvd0;                  /*!< Reserved */
} flash_adv_cmd_t;

/* query_info */
typedef struct flash_query_info
{
    /* Top/Bottom bit offset of status reg. (some chip may use BPx as TB) */
    /* set "0xFF" means BP only support lock / unlock all (no level) */
    /* set "0x7F" means BP support lock / unlock all, half, none */
    uint8_t         tb_offset;
    uint8_t         bp_all_lv;      /* Block Protect All Level (depends on flash_size) */
    uint8_t         bp_mask;        /* Block Protect (BPx~BP0); BP2~BP0=0x07, BP3~BP0=0x0F */
    uint8_t         pkg_id;
    /* use pkg_info bit 0 for IS_EXT_flash flag */
    /* use pkg_info bit 1 for support 4 bit-mode flag */
    /* use pkg_info bit 2 for enter 4 byte address mode flag */
    /* use pkg_info bit 7 to notify package ID is valid */
    uint8_t         pkg_info;
    uint8_t         cmd_en4b;       /*!< Specifies enter 4 byte addr mode cmd */
    uint8_t         cmd_rd4bsr;     /*!< Specifies the status register for read addr mode */
    uint8_t         ads_offset;     /*!< Specifies ADS bit offset */
} flash_query_info_t;

typedef struct
{
    uint8_t            size;
    uint8_t            manu_id;
    uint16_t           device_id;
    flash_basic_cmd_t  basic;
    flash_adv_cmd_t    adv;
    flash_query_info_t query_info;
} flash_avl_t;

typedef struct
{
    uint32_t    chip_id;
    uint8_t     ver;
    uint8_t     total_num;
    uint16_t    crc16;
} avl_bin_hdr_t;

#ifdef __cplusplus
}
#endif

#endif /* _FLASH_AVL_H_ */
