/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2018-2020 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "trace.h"
//#include "freertos/FreeRTOS.h"
//#include "freertos/semphr.h"
//#include "freertos/task.h"
#include "qcloud_iot_export_error.h"
#include "qcloud_iot_import.h"

//#define PLATFORM_HAS_CMSIS

#ifdef PLATFORM_HAS_CMSIS
#include "cmsis_os.h"
#include "stm32l4xx_hal.h"
#else
#include "os_sync.h"
#include "os_sched.h"
#include "os_mem.h"
#include "os_task.h"
#endif

#ifdef DEBUG_USED_CMD_UART
extern void cmd_uart_print(const char *fmt, ...);
#endif

// TODO platform dependant
void HAL_SleepMs(_IN_ uint32_t ms)
{
	os_delay(ms<100?100:ms);
    return;
}

void HAL_Printf(_IN_ const char *fmt, ...)
{
	va_list list;
	char buf[256]={0};
	
	va_start(list, fmt);
	vsnprintf(buf,sizeof(buf), fmt, list);
	va_end(list);
#ifdef DEBUG_USED_CMD_UART
	cmd_uart_print("%s", buf);
#else
    DBG_DIRECT("%s", buf);
#endif
}

int HAL_Snprintf(_IN_ char *str, const int len, const char *fmt, ...)
{
    va_list args;
    int     rc;

    va_start(args, fmt);
    rc = vsnprintf(str, len, fmt, args);
    va_end(args);

    return rc;
}

int HAL_Vsnprintf(_IN_ char *str, _IN_ const int len, _IN_ const char *format, va_list ap)
{
    return vsnprintf(str, len, format, ap);
}

void *HAL_Malloc(_IN_ uint32_t size)
{
    //return pvPortMalloc(size);
	return os_mem_zalloc(RAM_TYPE_DATA_ON, size);
}

void HAL_Free(_IN_ void *ptr)
{
    os_mem_free(ptr);
}

void *HAL_MutexCreate(void)
{
#ifdef MULTITHREAD_ENABLED
	void *p_mutex = NULL;

    // Create a mutex.
    if (os_mutex_create(&p_mutex) != true)
    {
        HAL_Printf("%s: HAL_MutexCreate failed\n", __FUNCTION__);
        return NULL;
    }
	return p_mutex;
#else
    return (void *)0xFFFFFFFF;
#endif
}

void HAL_MutexDestroy(_IN_ void *mutex)
{
#ifdef MULTITHREAD_ENABLED
    os_mutex_delete(mutex);
#else
    return;
#endif
}

void HAL_MutexLock(_IN_ void *mutex)
{
#ifdef MULTITHREAD_ENABLED
    if (!mutex) {
        HAL_Printf("%s: invalid mutex\n", __FUNCTION__);
        return;
    }

    if (os_mutex_take(mutex, 0xFFFFFFFF) != true) {
        HAL_Printf("%s: HAL_MutexLock failed\n", __FUNCTION__);
        return;
    }
#else
    return;
#endif
}

int HAL_MutexTryLock(_IN_ void *mutex)
{
#ifdef MULTITHREAD_ENABLED
    if (!mutex) {
        HAL_Printf("%s: invalid mutex\n", __FUNCTION__);
        return -1;
    }

    if (os_mutex_take(mutex, 0) != true) {
        HAL_Printf("%s: HAL_MutexTryLock failed\n", __FUNCTION__);
        return -1;
    }

    return 0;
#else
    return 0;
#endif
}

void HAL_MutexUnlock(_IN_ void *mutex)
{
#ifdef MULTITHREAD_ENABLED
    if (!mutex) {
        HAL_Printf("%s: invalid mutex\n", __FUNCTION__);
        return;
    }

    if (os_mutex_give(mutex) != true) {
        HAL_Printf("%s: HAL_MutexUnlock failed\n", __FUNCTION__);
        return;
    }
#else
    return;
#endif
}

#ifdef MULTITHREAD_ENABLED

// platform-dependant thread routine/entry function
static void _HAL_thread_func_wrapper_(void *ptr)
{
    ThreadParams *params = (ThreadParams *)ptr;

    params->thread_func(params->user_arg);

    os_task_delete(NULL);
}

// platform-dependant thread create function
int HAL_ThreadCreate(ThreadParams *params)
{
    if (params == NULL)
        return QCLOUD_ERR_INVAL;

    if (params->thread_name == NULL) {
        HAL_Printf("thread name is required for FreeRTOS platform!\n");
        return QCLOUD_ERR_INVAL;
    }

	if(os_task_create((void *)&params->thread_id, params->thread_name, _HAL_thread_func_wrapper_,
                       (void *)params, params->stack_size, params->priority) != true)
	{
		HAL_Printf("%s: xTaskCreate failed\n", __FUNCTION__);
        return QCLOUD_ERR_FAILURE;
	}

    return QCLOUD_RET_SUCCESS;
}

int HAL_ThreadDelete(ThreadParams *params)
{
    if (params == NULL)
        return QCLOUD_ERR_INVAL;

    if (params->thread_id == NULL) {
        HAL_Printf("thread id is required for FreeRTOS platform!\n");
        return QCLOUD_ERR_INVAL;
    }

	if(os_task_delete(params->thread_id) != true)
	{
		HAL_Printf("%s: xTaskDelete failed\n", __FUNCTION__);
        return QCLOUD_ERR_FAILURE;
	}

    return QCLOUD_RET_SUCCESS;
}
#endif

//#if defined(PLATFORM_HAS_CMSIS) && defined(AT_TCP_ENABLED)
#if defined(AT_OS_USED)
void *HAL_SemaphoreCreate(void)
{
    void * p_sem = NULL;
	
	if (os_sem_create(&p_sem, 0, 1) == true)
		return p_sem;
	else
		return NULL;
}

void HAL_SemaphoreDestroy(void *sem)
{
	os_sem_delete(sem);
}

void HAL_SemaphorePost(void *sem)
{
	if (os_sem_give(sem) != true)
	{
		HAL_Printf("HAL_SemaphorePost err\n\r");
	}
}

int HAL_SemaphoreWait(void *sem, uint32_t timeout_ms)
{
	return os_sem_take(sem, timeout_ms);
}
#endif
