 /* Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __AT_DEVICE_SIM76XX_H__
#define __AT_DEVICE_SIM76XX_H__

#define SIM76XX_WAIT_CONNECT_TIME  5000
#define AT_RESP_TIMEOUT_MS        (5000)
#define SIM76XX_SEND_MAX_LEN_ONCE (256)
#define SIM76XX_MAX_SOCKET_NUM    (5)

#define WIFI_SSID     "youga_wifi"  //"Honor 8"//
#define WIFI_PASSWORD "Iot@2018"    //"xy123123"//

int at_device_sim76xx_init(void);

#endif /* __AT_DEVICE_SIM76XX_H__ */
 