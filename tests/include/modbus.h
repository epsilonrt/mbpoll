#ifndef MODBUS_H_INCLUDED
#define MODBUS_H_INCLUDED

#include <stdint.h>
#include <sys/time.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

typedef struct modbus_struct modbus_t;

#define MODBUS_RTU_RTS_NONE 0
#define MODBUS_RTU_RTS_UP   1
#define MODBUS_RTU_RTS_DOWN 2
#define MODBUS_RTU_RS485    3

#define MODBUS_QUIRK_MAX_SLAVE 1
#define MODBUS_QUIRK_REPLY_TO_BROADCAST 2

const char *modbus_strerror(int errnum);

modbus_t *modbus_new_rtu(const char *device, int baud, char parity, int data_bit, int stop_bit);
modbus_t *modbus_new_tcp_pi(const char *node, const char *service);

int modbus_set_debug(modbus_t *ctx, int boolean);
int modbus_enable_quirks(modbus_t *ctx, int quirks);
int modbus_connect(modbus_t *ctx);
void modbus_free(modbus_t *ctx);
void modbus_close(modbus_t *ctx);
int modbus_set_slave(modbus_t *ctx, int slave);
int modbus_get_slave(modbus_t *ctx);

int modbus_write_bit(modbus_t *ctx, int addr, int status);
int modbus_write_bits(modbus_t *ctx, int addr, int nb, const uint8_t *src);
int modbus_write_register(modbus_t *ctx, int addr, const uint16_t value);
int modbus_write_registers(modbus_t *ctx, int addr, int nb, const uint16_t *src);
int modbus_read_bits(modbus_t *ctx, int addr, int nb, uint8_t *dest);
int modbus_read_input_bits(modbus_t *ctx, int addr, int nb, uint8_t *dest);
int modbus_read_registers(modbus_t *ctx, int addr, int nb, uint16_t *dest);
int modbus_read_input_registers(modbus_t *ctx, int addr, int nb, uint16_t *dest);
int modbus_report_slave_id(modbus_t *ctx, int max_dest, uint8_t *dest);

int modbus_rtu_set_serial_mode(modbus_t *ctx, int mode);
int modbus_rtu_set_rts(modbus_t *ctx, int mode);
int modbus_rtu_set_rts_delay(modbus_t *ctx, int us);
int modbus_rtu_set_custom_rts(modbus_t *ctx, void (*set_rts) (modbus_t *ctx, int on));

int modbus_get_byte_timeout(modbus_t *ctx, uint32_t *to_sec, uint32_t *to_usec);
int modbus_set_response_timeout(modbus_t *ctx, uint32_t to_sec, uint32_t to_usec);

#endif /* MODBUS_H_INCLUDED */
