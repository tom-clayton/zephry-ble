/*
 * nus.c: Zephyr implementation of the Bluetooth Low Energy Nordic UART Service.
 *
 * Tom Clayton 2025
 */
#include <zephyr/bluetooth/services/nus.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/conn.h>

LOG_MODULE_REGISTER(nus);

static struct bt_nus_cb nus_listener;

int nus_init(void (*receive_callback)(struct bt_conn *conn, const void *data, uint16_t len, void *ctx))
{
    nus_listener.received = receive_callback;

    int err = bt_nus_cb_register(&nus_listener, NULL);
    if (err) {
        LOG_ERR("Failed to register NUS callback: %d", err);
        return err;
    }

    LOG_INF("NUS Initialization complete, callback registered");
    return 0;
}

int nus_send_int(uint64_t value)
{
  uint8_t buffer[32];
  size_t len = snprintf((char *)buffer, sizeof(buffer), "%llu\n", value);
  return bt_nus_send(NULL, buffer, len);
}

int nus_send_string(const char *buf, size_t len)
{
  return bt_nus_send(NULL, (const uint8_t *)buf, len);
}
