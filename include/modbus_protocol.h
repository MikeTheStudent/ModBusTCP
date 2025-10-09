#ifndef MODBUS_PROTOCOL_H
#define MODBUS_PROTOCOL_H

#include <stdint.h>

// Function declarations
int Send_Modbus_request(char *server_ip, int port, uint8_t *apdu, int apdu_len, uint8_t *apdu_response);

// ModBus function codes
#define MODBUS_FC_READ_HOLDING_REGISTERS  0x03
#define MODBUS_FC_WRITE_MULTIPLE_REGISTERS 0x10

#endif
