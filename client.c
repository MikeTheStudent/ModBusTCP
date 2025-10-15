#include <stdio.h>
#include "modbus_app.h"
#include "modbus_protocol.h"
#include "utils.h"
#include <arpa/inet.h>

int main() {
    int buffer[100]; // Buffer for register values
    // int Write_multiple_registers(char *server_ip, int port, int startingRegister, int numRegisters, int *values)
    // int read_holding_registers(char *server_ip, int port, int startingRegister, int numRegisters, int *values)
    int function_code = 0;  // Example function code
    char server_ip[] = SERVER_ADDR; // Example server IP address
    //uint8_t modbus_frame[256];
    //int frame_length = sizeof(modbus_frame); // Example length of ModBus frame
    uint16_t value;
    value = 0x41;

    int sendA = Write_multiple_registers(server_ip, MODBUS_PORT, 141, 1, &value);




    return 0;
}