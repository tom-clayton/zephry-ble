/*
 * bt_manager.h: Controls Bluetooth Low Energy advertising and connections.
 *
 * Tom Clayton 2025
 *
 */
#ifndef BT_MANAGER_H
#define BT_MANAGER_H

#include <stdint.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>

/*
 * Intialise the Bluetooth subsystem.
 *
 * @param ad: Advertising data array.
 * @param ad_len: Length of the advertising data array.
 * @param sd: Scan response data array.
 * @param sd_len: Length of the scan response data array.
 * @param timeout: Time to advertise for, 0 for permenant.
 * @param connected_callback:    A function to call when a connection is established.
 * @param disconnected_callback: A function to call when a connection is lost.
 * @param timeout callback: A function to call when a advertising times out.
 * 
 */
int bt_manager_init(
  const struct bt_data *advertising_data, 
  size_t ad_length,
  const struct bt_data *scan_response_data, 
  size_t sd_length,
  uint64_t timeout;
  void (*connected_callback)(struct bt_conn *conn),
  void (*disconnected_callback)(struct bt_conn *conn, uint8_t reason),
  void (*timeout_callback)(void)
);

/*
 * Start advertising.
 * 
 */
int bt_manager_advertise();

#endif /* BT_MANAGER_H */
