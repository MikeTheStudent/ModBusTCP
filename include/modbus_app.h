#ifndef MODBUS_APP_H
#define MODBUS_APP_H

#include <stdint.h>

// Function declarations
int Read_holding_registers(char *server_ip, int port, int startingRegister, int numRegisters, int *values);
int Write_multiple_registers(char *server_ip, int port, int startingRegister, int numRegisters, int *values);

#endif
