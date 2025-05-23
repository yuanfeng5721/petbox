#ifndef __NV_H__
#define __NV_H__
#include <easyflash.h>
/* nv error code */
typedef enum {
    NV_NO_ERR,
    NV_DEL_ERR,
    NV_READ_ERR,
    NV_WRITE_ERR,
    NV_ENV_NAME_ERR,
    NV_ENV_NAME_EXIST,
    NV_ENV_FULL,
    NV_ENV_INIT_FAILED,
} NvErrCode;


/* nv value type */
typedef enum {
	NV_VALUE_BYTE,
	NV_VALUE_WORD,
    NV_VALUE_DWORD,
    NV_VALUE_STRING,
    NV_VALUE_BYTES,
} NvValueType;

#define DEFAULT_NV_ITEM_NUM EF_DEFAULT_ENV_ITEM


NvErrCode nv_item_init(void);
size_t nv_item_read(const char *key, uint8_t *value, uint32_t value_len);
uint32_t nv_item_read_int(const char *key);
char *nv_item_read_string(const char *key);
size_t nv_item_write(const char *key, const uint8_t *value, uint32_t value_len);
size_t nv_item_write_int(const char *key, uint32_t value);
size_t nv_item_write_string(const char *key, const char *value);
#endif

