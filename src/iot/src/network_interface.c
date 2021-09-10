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

#ifdef __cplusplus
extern "C" {
#endif

#include "network_interface.h"

#include "qcloud_iot_export_error.h"
#include "utils_param_check.h"

int is_network_connected(int fd)
{
    return fd == AT_NO_CONNECTED_FD ? 0 : fd == AT_NO_CONNECTED_FD;
}

#ifdef AT_TCP_ENABLED
int is_network_at_connected(int fd)
{
    return fd == AT_NO_CONNECTED_FD ? 0 : fd == AT_NO_CONNECTED_FD;
}
#endif
/**
 * network initialize.
 *
 * @param null
 * @return @see IoT_Return_Code
 */
int network_init(void)
{	
    return network_at_tcp_init();
}

/**
 * network deinitialize.
 *
 * @param null
 * @return @see IoT_Return_Code
 */
int network_deinit(void)
{
	return network_at_tcp_deinit();
}

/**
 * network connect.
 *
 * @param addr is socket address struct
 * @return network fd handle
 */
int network_connect(const sockaddr_t *addr)
{
	POINTER_SANITY_CHECK(addr, QCLOUD_ERR_INVAL);
	int fd = AT_NO_CONNECTED_FD;
	
	switch(addr->type)
	{
		case NETWORK_TCP:
			fd = network_at_tcp_connect(addr);
		break;
		
		case NETWORK_UDP:
			//udp connect
		break;
		
		default:
			Log_e("network type isn't tcp or udp: %s", (addr->type==NETWORK_TCP)?"tcp":((addr->type==NETWORK_UDP)?"udp":"unknown"));
		break;
	}
	return fd;
}

/**
 * network prase domain.
 *
 * @param domain is server url
 * @param addr is socket address struct
 * @return @see IoT_Return_Code
 */
int network_prase_domain(const char *domain, sockaddr_t *addr)
{
	return network_at_tcp_parse_domain(domain, addr);
}

/**
 * network receive data.
 *
 * @param fd is socket handle
 * @param data is recevice buffer point
 * @param datalen is want recevice data len
 * @return receive data len
 */
size_t network_recv(int fd, uint8_t *data, size_t datalen)
{
	return network_at_tcp_read(fd, data, datalen, 1000);
}

/**
 * network send data.
 *
 * @param fd is socket handle
 * @param data is send data buffer
 * @param datalen is want recevice data len
 * @return send data len
 */
size_t network_send(int fd, uint8_t *data, size_t datalen)
{
	return network_at_tcp_write(fd, data, datalen, 1000);
}

/**
 * network disconnect.
 *
 * @param fd is socket handle
 * @return @see IoT_Return_Code
 */
int network_disconnect(int fd)
{
	return network_at_tcp_disconnect(fd);
}
#ifdef __cplusplus
}
#endif
