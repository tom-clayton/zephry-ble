#include "bt_manager.h"

#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(bt_manager);

#define ADV_WAIT    1000; // mS

static struct bt_data *advertising_data; 
static size_t ad_length;
static struct bt_data *scan_response_data;
static size_t sd_length;

static uint64_t timeout;

static void (*connected_callback)(struct bt_conn *conn);
static void (*disconnected_callback)(struct bt_conn *conn, uint8_t reason);
static void (*timeout_callback)(void);

static struct k_work_delayable adv_timeout_work;
static uint64_t adv_resovled_time;
static bool advertising_active;
static K_MUTEX_DEFINE(advertising_mutex);

/*
 * Check if timeout expired before a connection was made.
 * Stop connection and call 'timeout' callback if affirmative. 
 *
 */
static void on_adv_timeout(struct k_work *work)
{
    ARG_UNUSED(work);

    k_mutex_lock(&advertising_mutex, K_FOREVER);
    if (advertising_active) {
        advertising_active = false;
        adv_resovled_time = k_uptime_get();
    } else {
        k_mutex_unlock(&advertising_mutex);
        return;
    }
    k_mutex_unlock(&advertising_mutex);

    int err = bt_le_adv_stop();
    if (err && err != -EALREADY) {
        LOG_ERR("Failed to stop advertising after timeout: %d", err);
        return;
    }
    LOG_INF("Advertising timed out");
    if (timeout_callback != NULL) {
        timeout_callback();
    }
}

/*
 * Check if connection was made before the timeout expired.
 * If so, cancel the advertising timer and call 'connected' callback.
 * If not, disconnect.
 * 
 */
static void on_connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        LOG_ERR("Connection failed: %u", err);
        return;
    }

    k_mutex_lock(&advertising_mutex, K_FOREVER);
    if (advertising_active) {
        advertising_active = false;
        adv_resovled_time = k_uptime_get();
    } else {
        k_mutex_unlock(&advertising_mutex);
        bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
        return;
    }
    k_mutex_unlock(&advertising_mutex);

    (void)k_work_cancel_delayable(&adv_timeout_work);
    LOG_INF("Connected");
    if (connected_callback != NULL) {
        connected_callback(conn);
    }
}

/*
 * Call 'disconnected' callback.
 *
 */
static void on_disconnected(struct bt_conn *conn, uint8_t reason)
{
    LOG_INF("Disconnected (reason 0x%02x)", reason);
    if (disconnected_callback != NULL) {
        disconnected_callback(conn, reason);
    }
}

static struct bt_conn_cb conn_callbacks = {
    .connected = on_connected,
    .disconnected = on_disconnected,
};

/*
 * Register advertising data and connection callbacks.
 *
 */
int bt_manager_init(
    const struct bt_data *ad, 
    size_t ad_len,
    const struct bt_data *sd, 
    size_t sd_len,
    uint64_t tmout;
    void (*connected_cb)(struct bt_conn *conn),
    void (*disconnected_cb)(struct bt_conn *conn, uint8_t reason)
    void (*timeout_cb)(void)
)
{
    advertising_data = ad;
    ad_length = ad_len;
    scan_response_data = sd;
    sd_length = sd_len;
    timeout = tmout;
    timeout_callback = timeout_cb; 
    connected_callback = connected_cb;
    disconnected_callback = disconnected_cb;
    bt_conn_cb_register(&conn_callbacks);
    k_work_init_delayable(&adv_timeout_work, on_adv_timeout);

    int err = bt_enable(NULL);
    if (err) {
        LOG_ERR("Failed to enable bluetooth: %d", err);
        return err;
    }

    return 0;
}

/*
 * Start Advetising with timer if set.
 *
 * Don't restart advertising straight after previous session has just been resolved
 * to avoid a race condition where the connection appears to be from the new session.
 */
int bt_manager_advertise()
{
    k_mutex_lock(&advetising_mutex, K_FOREVER);
    if (!advertising_active && (k_uptime_get() > adv_resolved_time + ADV_WAIT)) {
        advertising_active = true;
    } else {
        k_mutex_unlock(&advertising _mutex);
        return -1;
    }
    k_mutex_unlock(&advertising _mutex);

    int err = bt_le_adv_start(
        BT_LE_ADV_CONN_FAST_2, 
        advertising_data, 
        ad_length, 
        scan_response_data, 
        sd_length
    );
    
    if (err) {
        LOG_ERR("Failed to start advertising: %d", err);
        advertising_active = false;
        return err;
    }

    if (timeout && timeout_callback != NULL) {
        adv_timeout_callback = timeout_callback;
        (void)k_work_reschedule(&adv_timeout_work, K_MSEC(timeout));
    }

    LOG_INF("Advertising successfully started");
    return 0;
}
