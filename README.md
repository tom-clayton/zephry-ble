# nus

A Zephyr implementation of the Bluetooth Low Energy Nordic UART Service. Allows bytes to be sent and received over bluetooth as a serial connection.

### Usage


The NUS service must be included in the bluetooth advertising data.<br>
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
static void callback(struct bt_conn *conn, const void *data, uint16_t len, void *ctx)
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

Send other data types.