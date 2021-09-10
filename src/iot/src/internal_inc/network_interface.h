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

#ifndef _NETWORK_INTERFACE_H_
#define _NETWORK_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "qcloud_iot_import.h"

#define MAX_SOCKET_IPADRESS_LEN   20
#define MAX_SOCKET_DOMAIN_LEN     128

#define AT_NO_CONNECTED_FD 0xffffffff
/*
 * Type of network interface
 */

typedef enum { NETWORK_TCP = 0, NETWORK_UDP = 1/*, NETWORK_TLS = 2, NETWORK_DTLS = 3*/ } NETWORK_TYPE;

/**
 * @brief Define structure for network stack
 */
typedef struct Network Network;

typedef struct ip4_addr
{
    uint32_t addr;
} ip4_addr_t;

/**
 * @brief Define structure for network stack
 *
 * Network init/connect/read/write/disconnect/state
 */
struct Network {
    int (*init)(Network *);

    int (*connect)(Network *);

    int (*read)(Network *, unsigned char *, size_t, uint32_t, size_t *);

    int (*write)(Network *, unsigned char *, size_t, uint32_t, size_t *);

	int (*prase_domain)(Network *);
	
    void (*disconnect)(Network *);

    int (*is_connected)(Network *);

    // connetion handle:
    // for non-AT: 0 = not connected, non-zero = connected
    // for AT: 0 = valid connection, MAX_UNSINGED_INT = invalid
    uintptr_t handle;

#ifndef AUTH_WITH_NOTLS
    SSLConnectParams ssl_connect_params;
#endif
	char * domain;
    char * host;  // server address
    int          port;  // server port
    NETWORK_TYPE type;
};

typedef struct sockaddr{
	int port;
	char host[MAX_SOCKET_IPADRESS_LEN];
	NETWORK_TYPE type;
}sockaddr_t;
/*
 * Init network stack
 */
int network_init(void);
int network_deinit(void);
int network_connect(const sockaddr_t *addr);
int network_prase_domain(const char *domain, sockaddr_t *addr);
size_t network_recv(int fd, uint8_t *data, size_t datalen);
size_t network_send(int fd, uint8_t *data, size_t datalen);
int network_disconnect(int fd);
/* return the handle */
int is_network_connected(int fd);

/* network stack API */
#ifdef AT_TCP_ENABLED
size_t  network_at_tcp_read(int fd, unsigned char *data, size_t datalen, uint32_t timeout_ms);
size_t  network_at_tcp_write(int fd, unsigned char *data, size_t datalen, uint32_t timeout_ms);
int network_at_tcp_disconnect(int fd);
int  network_at_tcp_connect(const sockaddr_t *addr);
int  network_at_tcp_init(void);
int  network_at_tcp_deinit(void);
int  network_at_tcp_parse_domain(const char *domain, sockaddr_t *addr);
#endif



#ifdef AT_UDP_ENABLED
int network_udp_read(Network *pNetwork, unsigned char *data, size_t datalen, uint32_t timeout_ms, size_t *read_len);
int network_udp_write(Network *pNetwork, unsigned char *data, size_t datalen, uint32_t timeout_ms, size_t *written_len);
void network_udp_disconnect(Network *pNetwork);
int  network_udp_connect(Network *pNetwork);
int  network_udp_init(Network *pNetwork);
#endif

#ifdef __cplusplus
}
#endif
#endif /* _NETWORK_INTERFACE_H_ */
