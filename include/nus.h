/*
 * nus.h: Zephyr implementation of the Bluetooth Low Energy Nordic UART Service.
 *
 * Tom Clayton 2025
 */
#ifndef NUS_H
#define NUS_H

#include <stdint.h>
#include <stddef.h>

struct bt_conn;

/*
 * Initialize the NUS service.
 * 
 * @param recieve_callback: A function to receive data from the connection.
 * 
 * @return 0 on success, error value on faliure.
 */
int nus_init(void (*recieve_callback)(
        struct bt_conn *conn,
        const void *data,
        uint16_t len,
        void *ctx
    )
);

/*
 * Send an integer value over the connection.
 *
 * @param value: A 64 bit integer to send.
 * 
 * @return the return value from bt_nus_send.
 */
int nus_send_int(uint64_t value);

/*
 * Send a string over the connection.
 *
 * @param buffer: a pointer to the string.
 * @param len: number of characters to send.
 * 
 * @return the return value from bt_nus_send.
 */
int nus_send_string(const char *buffer, size_t len);

#endif /* NUS_H */