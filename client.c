#include <stdio.h>
#include "modbus_app.h"
#include "modbus_protocol.h"
#include "utils.h"
#include <arpa/inet.h>

int main() {
    char server_ip[] = SERVER_ADDR; // Server IP address
    
    // Task A: Write 'A' (0x41) to register 141
    int value_to_write = 0x41;  // ASCII 'A'
    int sendA_result = Write_multiple_registers(server_ip, MODBUS_PORT, 121, 1, &value_to_write);
    if (sendA_result < 0){
        if (sendA_result == -1){
            printf("TCP or other error occurred\n");
        }
        else if (sendA_result == -2){
            return 1;
        }
        else if (sendA_result == -3){
            return 2;
        }
        else if (sendA_result == -4){
            return 3;
        }
        else if (sendA_result == -5){
            return 4;
        }
    }
    

    return 0;
}