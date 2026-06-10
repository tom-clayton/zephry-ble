# zephyr-ble

A Zephyr Bluetooth Low Energy periphral manager.
Manages the advertising, conections and services of a Bluetooth Low Energy connection.

### Usage

Bluetooth manager must be used. It should be sent a advertising data array, a scan response array, a timeout and callbacks for connection, disconnection and timeout. Pass NULL if not using any of these.

```C
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
```

Then initialise the relevant service: (currently only the NUS service is supported)

### nus

Initialise with a callback function that will receive incoming data. It must accept the bluetooth connection, a pointer to the data, the length of the data and a void pointer.

```C
int nus_init(void (*recieve_callback)(
        struct bt_conn *conn,
        const void *data,
        uint16_t len,
        void *ctx
    )
);
```
Send integers or strings over the connection with the following functions:

```C
int nus_send_int(uint64_t value);
```

```C
int nus_send_string(const char *buffer, size_t len);
```

### Example

```C
if (bt_manager_init(
    ad, 
    ARRAY_SIZE(ad), 
    sd,
    ADVERTISING_TIMEOUT, 
    ARRAY_SIZE(sd), 
    on_connected, 
    on_disconnected, 
    NULL
) != 0) {
    LOG_ERR("BT manager error\n\r");
    return -1;
}

static void nus_callback(struct bt_conn *conn, const void *data, uint16_t len, void *ctx)
{
    ARG_UNUSED(conn);
    ARG_UNUSED(ctx);

    printf("Data received (%u B): %.*s", len, (int)len, (const char *)data);
}

if (nus_init(callback) == 0) {
    nus_send_string("Hello World!", 12);
    nus_send_int(10);
}
```

### Todo

Add other services and a 'custom' service.<br>
Send other data typeso over nus.
