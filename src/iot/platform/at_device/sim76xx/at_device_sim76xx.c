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
#include "at_device_sim76xx.h"

#include <stdio.h>
#include <string.h>

#include "at_client.h"
#include "at_socket_inf.h"
#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"
#include "utils_param_check.h"

//#define USING_RTC

#undef DEVICE_NAME
#define DEVICE_NAME    "SIM76XX"
#define WIFI_CONN_FLAG (1 << 0)
#define SEND_OK_FLAG   (1 << 1)
#define SEND_FAIL_FLAG (1 << 2)

static uint8_t sg_SocketBitMap = 0;

static at_evt_cb_t at_evt_cb_table[] = {
    [AT_SOCKET_EVT_RECV]   = NULL,
    [AT_SOCKET_EVT_CLOSED] = NULL,
};


/* power up sim76xx modem */
static void sim76xx_power_on(void)
{
//    struct at_device_sim76xx *sim76xx = RT_NULL;

//    sim76xx = (struct at_device_sim76xx *) device->user_data;

//    /* not nead to set pin configuration for m26 device power on */
//    if (sim76xx->power_pin == -1 || sim76xx->power_status_pin == -1)
//    {
//        return;
//    }

//    if (rt_pin_read(sim76xx->power_status_pin) == PIN_HIGH)
//    {
//        return;
//    }
//    rt_pin_write(sim76xx->power_pin, PIN_HIGH);

//    while (rt_pin_read(sim76xx->power_status_pin) == PIN_LOW)
//    {
//        rt_thread_mdelay(10);
//    }
//    rt_pin_write(sim76xx->power_pin, PIN_LOW);
}

/* power off sim76xx modem */
static void sim76xx_power_off(void)
{
//    struct at_device_sim76xx *sim76xx = RT_NULL;

//    sim76xx = (struct at_device_sim76xx *) device->user_data;

//    /* not nead to set pin configuration for m26 device power on */
//    if (sim76xx->power_pin == -1 || sim76xx->power_status_pin == -1)
//    {
//        return;
//    }

//    if (rt_pin_read(sim76xx->power_status_pin) == PIN_LOW)
//    {
//        return;
//    }
//    rt_pin_write(sim76xx->power_pin, PIN_HIGH);

//    while (rt_pin_read(sim76xx->power_status_pin) == PIN_HIGH)
//    {
//        rt_thread_mdelay(10);
//    }
//    rt_pin_write(sim76xx->power_pin, PIN_LOW);
}

static int alloc_fd(void)
{
    uint8_t i;
    int     fd;

    for (i = 0; i < SIM76XX_MAX_SOCKET_NUM; i++) {
        if (0 == ((sg_SocketBitMap >> i) & 0x01)) {
            sg_SocketBitMap |= (1 << i) & 0xff;
            break;
        }
    }

    return (i < SIM76XX_MAX_SOCKET_NUM) ? (fd = i) : (fd = UNUSED_SOCKET);
}

static void free_fd(int fd)
{
    uint8_t i = fd;

    if ((fd != UNUSED_SOCKET) && fd < SIM76XX_MAX_SOCKET_NUM) {
        sg_SocketBitMap &= ~((1 << i) & 0xff);
    }
}

static void urc_send_func(const char *data, size_t size)
{
    POINTER_SANITY_CHECK_RTN(data);

    if (strstr(data, "SEND OK")) {
        at_setFlag(SEND_OK_FLAG);
    } else if (strstr(data, "SEND FAIL")) {
        at_setFlag(SEND_FAIL_FLAG);
    }
}

static void urc_send_bfsz_func(const char *data, size_t size)
{
    POINTER_SANITY_CHECK_RTN(data);
    int send_bfsz = 0;

    sscanf(data, "Recv %d bytes", &send_bfsz);
}

static void urc_close_func(const char *data, size_t size)
{
    int fd = 0;
    POINTER_SANITY_CHECK_RTN(data);

    sscanf(data, "%d,CLOSED", &fd);

    /* notice the socket is disconnect by remote */
    if (at_evt_cb_table[AT_SOCKET_EVT_CLOSED]) {
        at_evt_cb_table[AT_SOCKET_EVT_CLOSED](fd, AT_SOCKET_EVT_CLOSED, NULL, 0);
    }

    free_fd(fd);
}

static void urc_recv_func(const char *data, size_t size)
{
    int      fd;
    size_t   bfsz = 0, temp_size = 0;
    uint32_t timeout;
    char *   recv_buf, temp[8];

    POINTER_SANITY_CHECK_RTN(data);

    /* get the current socket and receive buffer size by receive data */
    sscanf(data, "+IPD,%d,%d:", &fd, (int *)&bfsz);

    /* get receive timeout by receive buffer length */
    timeout = bfsz;

    if (fd < 0 || bfsz == 0)
        return;

    recv_buf = HAL_Malloc(bfsz);
    if (NULL == recv_buf) {
        Log_e("no memory for URC receive buffer (%d)!", bfsz);
        /* read and clean the coming data */
        while (temp_size < bfsz) {
            if (bfsz - temp_size > sizeof(temp)) {
                at_client_recv(temp, sizeof(temp), timeout);
            } else {
                at_client_recv(temp, bfsz - temp_size, timeout);
            }
            temp_size += sizeof(temp);
        }
        return;
    }

    /* sync receive data */
    if (at_client_recv(recv_buf, bfsz, timeout) != bfsz) {
        Log_e("receive size(%d) data failed!", bfsz);
        HAL_Free(recv_buf);
        return;
    }

    /* notice the receive buffer and buffer size */
    if (at_evt_cb_table[AT_SOCKET_EVT_RECV]) {
        at_evt_cb_table[AT_SOCKET_EVT_RECV](fd, AT_SOCKET_EVT_RECV, recv_buf, bfsz);
    }
}

static void urc_busy_p_func(const char *data, size_t size)
{
    POINTER_SANITY_CHECK_RTN(data);

    Log_d("system is processing a commands and it cannot respond to the current commands.");
}

static void urc_busy_s_func(const char *data, size_t size)
{
    POINTER_SANITY_CHECK_RTN(data);

    Log_d("system is sending data and it cannot respond to the current commands.");
}

static void urc_func(const char *data, size_t size)
{
    POINTER_SANITY_CHECK_RTN(data);
    int i;

    if (strstr(data, "WIFI CONNECTED")) {
        at_setFlag(WIFI_CONN_FLAG);
    } else if (strstr(data, "WIFI DISCONNECT")) {
        /* notice the socket is disconnect by remote */
        if (at_evt_cb_table[AT_SOCKET_EVT_CLOSED]) {
            for (i = 0; i < SIM76XX_MAX_SOCKET_NUM; i++) {
                at_evt_cb_table[AT_SOCKET_EVT_CLOSED](i, AT_SOCKET_EVT_CLOSED, NULL, 0);
                free_fd(i);
            }
        }
    }
}

static at_urc urc_table[] = {
    {"SEND OK", "\r\n", urc_send_func},
    {"SEND FAIL", "\r\n", urc_send_func},
    {"Recv", "bytes\r\n", urc_send_bfsz_func},
    {"", ",CLOSED\r\n", urc_close_func},
    {"+IPD", ":", urc_recv_func},
    {"busy p", "\r\n", urc_busy_p_func},
    {"busy s", "\r\n", urc_busy_s_func},
    {"WIFI CONNECTED", "\r\n", urc_func},
    {"WIFI DISCONNECT", "\r\n", urc_func},
};

//static struct at_urc urc_table[] =
//{
//    {"+CIPSEND:",      "\r\n",           urc_send_func},
//    {"+CIPOPEN:",      "\r\n",           urc_connect_func},
//    {"+IPCLOSE",       "\r\n",           urc_close_func},
//    {"+IPD",           "\r\n",           urc_recv_func},
//};
static void sim76xx_set_event_cb(at_socket_evt_t event, at_evt_cb_t cb)
{
    if (event < sizeof(at_evt_cb_table) / sizeof(at_evt_cb_table[0])) {
        at_evt_cb_table[event] = cb;
    }
}

/* =============================  sim76xx network interface operations ============================= */

/* set sim76xx network interface device status and address information */
static int sim76xx_get_ipaddr(char *ipaddr)
{
#define SIM76XX_IMEI_RESP_SIZE      256
#define SIM76XX_IPADDR_RESP_SIZE    64
#define SIM76XX_DNS_RESP_SIZE       96
#define SIM76XX_INFO_RESP_TIMO      (300)

    int result = QCLOUD_RET_SUCCESS;
    at_response_t resp = NULL;


    /* set network interface device status */
    resp = at_create_resp(SIM76XX_IMEI_RESP_SIZE, 0, SIM76XX_INFO_RESP_TIMO);
    if (resp == NULL)
    {
        Log_e("no memory for resp create.");
        result = QCLOUD_ERR_MALLOC;
        goto __exit;
    }
#if 0
    /* set network interface device hardware address(IMEI) */
    {
        #define SIM76XX_NETDEV_HWADDR_LEN   8
        #define SIM76XX_IMEI_LEN            15

        char imei[SIM76XX_IMEI_LEN] = {0};
        int i = 0, j = 0;

        /* send "ATI" commond to get device IMEI */
        if (at_obj_exec_cmd(resp, "ATI") < 0)
        {
            result = QCLOUD_ERR_FAILURE;
            goto __exit;
        }

        if (at_resp_parse_line_args_by_kw(resp, "IMEI:", "IMEI: %s", imei) <= 0)
        {
            Log_e("%s device prase \"ATI\" cmd error.", DEVICE_NAME);
            result = QCLOUD_ERR_FAILURE;
            goto __exit;
        }

        Log_d("%s device IMEI number: %s", DEVICE_NAME, imei);
    }
#endif
    /* set network interface device IP address */
    {
        #define IP_ADDR_SIZE_MAX    16
        char ipaddr[IP_ADDR_SIZE_MAX] = {0};

        at_resp_set_info(resp, 2, SIM76XX_INFO_RESP_TIMO);

        /* send "AT+IPADDR" commond to get IP address */
        if (at_obj_exec_cmd(resp, "AT+IPADDR") < 0)
        {
            result = QCLOUD_ERR_FAILURE;
            goto __exit;
        }

        if (at_resp_parse_line_args_by_kw(resp, "+IPADDR:", "+IPADDR: %s", ipaddr) <= 0)
        {
            Log_e("%s device prase \"AT+IPADDR\" cmd error!", DEVICE_NAME);
            result = QCLOUD_ERR_FAILURE;
            goto __exit;
        }

        Log_d("%s device IP address: %s", DEVICE_NAME, ipaddr);

        /* set network interface address information */
        //inet_aton(ipaddr, &addr);
        //netdev_low_level_set_ipaddr(netdev, &addr);
    }

__exit:
    if (resp)
    {
        at_delete_resp(resp);
    }

    return result;
}

static int sim76xx_init(void)
{
#define INIT_RETRY                     10
#define CPIN_RETRY                     5
#define CSQ_RETRY                      10
#define CREG_RETRY                     10
#define CGREG_RETRY                    10
#define CGATT_RETRY                    10
#define CCLK_RETRY                     10
    at_response_t resp = NULL;
    int           ret;
    int           i;
	int 		  qi_arg[3] = {0};
	char          parsed_data[20] = {0};
	int           retry_num = INIT_RETRY;
	
    resp = at_create_resp(128, 0, AT_RESP_TIMEOUT_MS);
    if (NULL == resp) {
        Log_e("No memory for response structure!");
        ret = QCLOUD_ERR_FAILURE;
        goto __exit;
    }

	while(retry_num--)
	{
		/* power-up sim76xx */
		sim76xx_power_on();
		
		/* wait SIM76XX startup finish, Send AT every 5s, if receive OK, SYNC success*/
		if(at_obj_wait_connect(SIM76XX_WAIT_CONNECT_TIME))
		{
			Log_e("sim76xx connect timeout!");
			ret = QCLOUD_ERR_TIMEOUT;
			goto __exit;
		}

		/* disable echo */

		ret = at_exec_cmd(resp, "ATE0");
		if (QCLOUD_RET_SUCCESS != ret) {
			Log_e("cmd ATE0 exec err");
			// goto exit;
		}

		at_delayms(100);
		/* get module version */
		ret = at_exec_cmd(resp, "ATI");
		if (QCLOUD_RET_SUCCESS != ret) {
			Log_e("cmd ATI exec err");
			// goto exit;
		}

		/* show module version */
		for (i = 0; i < resp->line_counts - 1; i++) {
			Log_d("%s", at_resp_get_line(resp, i + 1));
		}

		
		/* check SIM card */
		at_delayms(1000);
		for (i = 0; i < CPIN_RETRY; i++)
		{
			at_exec_cmd(resp, "AT+CPIN?");
			if (at_resp_get_line_by_kw(resp, "READY"))
			{
				Log_d("device SIM card detection success.");
				break;
			}
			Log_i("\"AT+CPIN\" commands send retry...");
			at_delayms(1000);
		}

		if (i == CPIN_RETRY)
		{
			ret = QCLOUD_ERR_FAILURE;
			goto __exit;
		}
		
		
		/* waiting for dirty data to be digested */
		at_delayms(10);
		/* check signal strength */
		for (i = 0; i < CSQ_RETRY; i++)
		{
			at_exec_cmd(resp, "AT+CSQ");
			at_resp_parse_line_args_by_kw(resp, "+CSQ:", "+CSQ: %d,%d", &qi_arg[0], &qi_arg[1]);
			if (qi_arg[0] != 99)
			{
				Log_d("device signal strength: %d  Channel bit error rate: %d", qi_arg[0], qi_arg[1]);
				break;
			}
			at_delayms(1000);
		}

		if (i == CSQ_RETRY)
		{
			Log_e("device signal strength check failed (%s).", parsed_data);
			ret = QCLOUD_ERR_FAILURE;
			goto __exit;
		}
		
		/* do not show the prompt when receiving data */
		at_exec_cmd(resp, "AT+CIPSRIP=0");

		/* check the GSM network is registered */
		for (i = 0; i < CREG_RETRY; i++)
		{
			at_exec_cmd(resp, "AT+CREG?");
			at_resp_parse_line_args_by_kw(resp, "+CREG:", "+CREG: %s", &parsed_data);
			if (!strncmp(parsed_data, "0,1", sizeof(parsed_data)) || !strncmp(parsed_data, "0,5", sizeof(parsed_data)))
			{
				Log_d("%s device GSM is registered(%s).", DEVICE_NAME, parsed_data);
				break;
			}
			at_delayms(1000);
		}
		if (i == CREG_RETRY)
		{
			Log_e("%s device GSM is register failed(%s).", DEVICE_NAME, parsed_data);
			ret = QCLOUD_ERR_FAILURE;
			goto __exit;
		}
		
		
		/* check the GPRS network is registered */
        for (i = 0; i < CGREG_RETRY; i++)
        {
            at_exec_cmd(resp, "AT+CGREG?");
            at_resp_parse_line_args_by_kw(resp, "+CGREG:", "+CGREG: %s", &parsed_data);
            if (!strncmp(parsed_data, "0,1", sizeof(parsed_data)) || !strncmp(parsed_data, "0,5", sizeof(parsed_data)))
            {
                Log_d("%s device GPRS is registered(%s).", DEVICE_NAME, parsed_data);
                break;
            }
            at_delayms(1000);
        }

        if (i == CGREG_RETRY)
        {
            Log_e("%s device GPRS is register failed(%s).", DEVICE_NAME, parsed_data);
            ret = QCLOUD_ERR_FAILURE;
            goto __exit;
        }

        /* check packet domain attach or detach */
        for (i = 0; i < CGATT_RETRY; i++)
        {
            at_exec_cmd(resp, "AT+CGATT?");
            at_resp_parse_line_args_by_kw(resp, "+CGATT:", "+CGATT: %s", &parsed_data);
            if (!strncmp(parsed_data, "1", 1))
            {
                Log_d("%s device Packet domain attach.", DEVICE_NAME);
                break;
            }

            at_delayms(1000);
        }

        if (i == CGATT_RETRY)
        {
            Log_e("%s device GPRS attach failed.", DEVICE_NAME);
            ret = QCLOUD_ERR_FAILURE;
            goto __exit;
        }

        /* configure context */
        at_exec_cmd(resp, "AT+CGDCONT=1,\"IP\",\"CMNET\"");

        /* activate context */
        {
            int net_status = 0;

            at_exec_cmd(resp, "AT+NETOPEN?");
            at_resp_parse_line_args_by_kw(resp, "+NETOPEN:", "+NETOPEN: %d", &net_status);
            /* 0 - netwoek close, 1 - network open */
            if (net_status == 0)
            {
                at_exec_cmd(resp, "AT+NETOPEN");
            }
        }

        /* set active PDP context's profile number */
        //AT_SEND_CMD(client, resp, "AT+CSOCKSETPN=1");

#ifdef USING_RTC
        /* get real time */
        int year, month, day, hour, min, sec;

        for (i = 0; i < CCLK_RETRY; i++)
        {
            if (at_obj_exec_cmd(resp, "AT+CCLK?") < 0)
            {
                at_delayms(500);
                continue;
            }

            /* +CCLK: "18/12/22,18:33:12+32" */
            if (at_resp_parse_line_args_by_kw(resp, "+CCLK:", "+CCLK: \"%d/%d/%d,%d:%d:%d",
                                              &year, &month, &day, &hour, &min, &sec) < 0)
            {
                at_delayms(500);
                continue;
            }

            set_date(year + 2000, month, day);
            set_time(hour, min, sec);

            break;
        }

        if (i == CCLK_RETRY)
        {
            Log_e("%s device GPRS attach failed.", DEVICE_NAME);
            ret = QCLOUD_ERR_FAILURE;
            goto __exit;
        }
#endif /* USING_RTC */

        /* initialize successfully  */
        ret = QCLOUD_RET_SUCCESS;
        break;

    __exit:
        if (ret != QCLOUD_RET_SUCCESS)
        {
            /* power off the sim76xx device */
            sim76xx_power_off();
            at_delayms(1000);

            Log_i("%s device initialize retry...", DEVICE_NAME);
        }
	}

    if (resp) {
        at_delete_resp(resp);
    }

    return ret;
}

static int sim76xx_close(int fd)
{
    at_response_t resp;
    int           ret;

    resp = at_create_resp(128, 0, AT_RESP_TIMEOUT_MS);
    if (NULL == resp) {
        Log_e("No memory for response structure!");
        return QCLOUD_ERR_FAILURE;
    }

    ret = at_exec_cmd(resp, "AT+CIPCLOSE=%d", fd);

    if (QCLOUD_RET_SUCCESS != ret) {  // fancyxu
        Log_e("close socket(%d) fail", fd);
    }

    if (resp) {
        at_delete_resp(resp);
    }

    return ret;
}

static int sim76xx_connect(const char *ip, uint16_t port, eNetProto proto)
{
    at_response_t resp;
    bool          retryed = false;
    int           fd, ret;

    POINTER_SANITY_CHECK(ip, QCLOUD_ERR_INVAL);
    resp = at_create_resp(128, 0, AT_RESP_TIMEOUT_MS);
    if (NULL == resp) {
        Log_e("No memory for response structure!");
        return QCLOUD_ERR_FAILURE;
    }

    fd = alloc_fd();
    if (fd < 0) {
        Log_e("esp8226 support max %d chain", SIM76XX_MAX_SOCKET_NUM);
        return QCLOUD_ERR_FAILURE;
    }

__retry:

    switch (proto) {
        case eNET_TCP:
            /* send AT commands to connect TCP server */
            ret = at_exec_cmd(resp, "AT+CIPSTART=%d,\"TCP\",\"%s\",%d,60", fd, ip, port);
            if (QCLOUD_RET_SUCCESS != ret) {
                Log_e("start tcp connect failed, fd=%d,ip(%s),port(%d)", fd, ip, port);
            }
            break;

        case eNET_UDP:
            ret = at_exec_cmd(resp, "AT+CIPSTART=%d,\"UDP\",\"%s\",%d", fd, ip, port);
            if (QCLOUD_RET_SUCCESS != ret) {
                Log_e("start udp connect failed, fd=%d,ip(%s),port(%d)", fd, ip, port);
            }
            break;

        default:
            Log_e("Not supported connect type : %d", proto);
            ret = QCLOUD_ERR_FAILURE;
            goto __exit;
    }

    if ((QCLOUD_RET_SUCCESS != ret) && !retryed) {
        Log_e("socket(%d) connect failed, maybe the socket was not be closed at the last time and now will retry.", fd);
        if (QCLOUD_RET_SUCCESS != sim76xx_close(fd)) {
            goto __exit;
        }
        retryed = true;
        at_delayms(100);
        goto __retry;
    }

__exit:

    if (resp) {
        at_delete_resp(resp);
    }

    if (QCLOUD_RET_SUCCESS != ret) {
        free_fd(fd);
        fd = UNUSED_SOCKET;
    }

    at_delayms(200);
    return fd;
}

static int sim76xx_send(int fd, const void *buff, size_t len)
{
    int           ret;
    at_response_t resp;
    size_t        cur_pkt_size = 0;
    size_t        sent_size    = 0;
    size_t        temp_size    = 0;

    POINTER_SANITY_CHECK(buff, QCLOUD_ERR_INVAL);
    resp = at_create_resp(128, 2, AT_RESP_TIMEOUT_MS);
    if (NULL == resp) {
        Log_e("No memory for response structure!");
        return QCLOUD_ERR_FAILURE;
    }

    /* set AT client end sign to deal with '>' sign.*/
    at_set_end_sign('>');

    while (sent_size < len) {
        if (len - sent_size < SIM76XX_SEND_MAX_LEN_ONCE) {
            cur_pkt_size = len - sent_size;
        } else {
            cur_pkt_size = SIM76XX_SEND_MAX_LEN_ONCE;
        }

        at_clearFlag(SEND_OK_FLAG);
        at_clearFlag(SEND_FAIL_FLAG);
        /* send the "AT+CIPSEND" commands to AT server than receive the '>' response on the first line. */
        ret = at_exec_cmd(resp, "AT+CIPSEND=%d,%d", fd, cur_pkt_size);
        if (QCLOUD_RET_SUCCESS != ret) {
            Log_e("cmd AT+CIPSEND exec err");
            goto __exit;
        }
        at_set_end_sign(0);
        /* send the real data to server or client */
        temp_size = at_client_send(at_client_get(), (char *)buff + sent_size, cur_pkt_size, AT_RESP_TIMEOUT_MS);

        if (cur_pkt_size != temp_size) {
            Log_e("at send real data failed");
            goto __exit;
        }

#ifndef AT_OS_USED
        at_urc urc_send = {.cmd_prefix = "SEND OK", .cmd_suffix = "\r\n", NULL};
        at_client_yeild(&urc_send, resp->timeout);
        if (at_client_get()->resp_status != AT_RESP_OK) {
            Log_e("send fail");
            ret = QCLOUD_ERR_FAILURE;
            goto __exit;
        }
#else
        if (at_waitFlag(SEND_OK_FLAG | SEND_FAIL_FLAG, AT_RESP_TIMEOUT_MS)) {
            if (at_waitFlag(SEND_FAIL_FLAG, AT_RESP_TIMEOUT_MS)) {
                Log_e("send fail");
                ret = QCLOUD_ERR_FAILURE;
                goto __exit;
            }
        }
#endif
        sent_size += cur_pkt_size;
    }

    ret = QCLOUD_RET_SUCCESS;

__exit:
    /* reset the end sign for data */
    if (resp) {
        at_delete_resp(resp);
    }

    return sent_size;  // fancyxu
}

static int sim76xx_recv_timeout(int fd, void *buf, size_t len, uint32_t timeout)
{
#ifndef AT_OS_USED
    at_client_yeild(NULL, timeout);
#endif
    return QCLOUD_RET_SUCCESS;
}

static int sim76xx_parse_domain(const char *host_name, char *host_ip, size_t host_ip_len)
{
#define RESOLVE_RETRY 5
	char domain[32] = {0};
    char          recv_ip[16] = {0};
    at_response_t resp;
    int           ret, i;

    POINTER_SANITY_CHECK(host_name, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(host_ip, QCLOUD_ERR_INVAL);

    if (host_ip_len < 16) {
        Log_e("host ip buff too short");
        return QCLOUD_ERR_FAILURE;
    }

    resp = at_create_resp(128, 0, 4 * AT_RESP_TIMEOUT_MS);
    if (NULL == resp) {
        Log_e("No memory for response structure!");
        return QCLOUD_ERR_FAILURE;
    }

    for (i = 0; i < RESOLVE_RETRY; i++) {
        ret = at_exec_cmd(resp, "AT+CDNSGIP=\"%s\"", host_name);
        if (QCLOUD_RET_SUCCESS != ret) {
            Log_e("exec AT+CDNSGIP=\"%s\" fail", host_name);
            goto __exit;
        }

        /* parse the third line of response data, get the IP address */
        if (at_resp_parse_line_args_by_kw(resp, "+CDNSGIP:", "+CDNSGIP: 1,%[^,],\"%[^\"]", domain, recv_ip) < 0) {
            at_delayms(100);
            /* resolve failed, maybe receive an URC CRLF */
            continue;
        }

        if (strlen(recv_ip) < 8) {
            at_delayms(100);
            /* resolve failed, maybe receive an URC CRLF */
            continue;
        } else {
            strncpy(host_ip, recv_ip, 15);
            host_ip[15] = '\0';
            break;
        }
    }

__exit:
	if(i == RESOLVE_RETRY)
	{
		Log_e("parse domain retry max times!");
		ret = QCLOUD_ERR_FAILURE;
	}
	
    if (resp) {
        at_delete_resp(resp);
    }

#undef RESOLVE_RETRY

    return ret;
}

at_device_op_t at_ops_sim76xx = {
    .init         = sim76xx_init,
	.get_local_ip = NULL,
    .connect      = sim76xx_connect,
    .send         = sim76xx_send,
    .recv_timeout = sim76xx_recv_timeout,
    .close        = sim76xx_close,
    .parse_domain = sim76xx_parse_domain,
    .set_event_cb = sim76xx_set_event_cb,
    .deviceName   = "sim76xx",
};

int at_device_sim76xx_init(void)
{
    int         i;
    int         ret;
    at_client_t p_client;

    ret = HAL_AT_Uart_Init();
    if (QCLOUD_RET_SUCCESS != ret) {
        Log_e("at uart init fail!");
    } else {
        Log_d("at uart init success!");
    }

    /* initialize AT client */
    ret = at_client_init(&p_client);

    if (QCLOUD_RET_SUCCESS != ret) {
        Log_e("at client init fail,ret:%d", ret);
        goto exit;
    } else {
        Log_d("at client init success");
    }

    /* register URC data execution function  */
    at_set_urc_table(p_client, urc_table, sizeof(urc_table) / sizeof(urc_table[0]));

    Log_d("urc table addr:%p, size:%d", p_client->urc_table, p_client->urc_table_size);
    for (i = 0; i < p_client->urc_table_size; i++) {
        Log_d("%s", p_client->urc_table[i].cmd_prefix);
    }

    ret = at_device_op_register(&at_ops_sim76xx);
    if (QCLOUD_RET_SUCCESS != ret) {
        Log_e("at device driver register fail");
        goto exit;
    }

exit:
    if (QCLOUD_RET_SUCCESS != ret) {
        if (NULL != p_client) {
            at_client_deinit(p_client);
        }
    }

    return ret;
}

/*at device driver must realize this api which called by HAL_AT_TCP_Init*/
//int at_device_init(void)
//{
//    return at_device_sim76xx_init();
//}
