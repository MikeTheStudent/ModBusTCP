#include <stdio.h>
#include "modbus_app.h"
#include "modbus_protocol.h"
#include "utils.h"
#include <arpa/inet.h>

int main() {
    char server_ip[] = SERVER_ADDR; // Server IP address
    
    // Task A: Write 'A' (0x41) to register 141
    int value_to_write = 0x41;  // ASCII 'A'
    int sendA_result = Write_multiple_registers(server_ip, MODBUS_PORT, 141, 1, &value_to_write);
    
    if (sendA_result > 0) {
        printf("Task A: Successfully wrote 'A' (0x41) to register 141\n");
    } else {
        printf("Task A: Failed to write to register 141. Error: %d\n", sendA_result);
    }
    
    // Task A verification: Read back from register 141
    int readA_buffer[1];
    int readA_result = Read_holding_registers(server_ip, MODBUS_PORT, 141, 1, readA_buffer);
    
    if (readA_result > 0) {
        printf("Task A verification: Read value %d (0x%02X) from register 141\n", readA_buffer[0], readA_buffer[0]);
        if (readA_buffer[0] == 0x41) {
            printf("Task A verification: SUCCESS - Value matches!\n");
        }
    }
    
    // Task B: Read from register 126
    int taskB_buffer[1];
    int taskB_result = Read_holding_registers(server_ip, MODBUS_PORT, 126, 1, taskB_buffer);
    
    if (taskB_result > 0) {
        printf("Task B: Read value %d (0x%02X) from register 126\n", taskB_buffer[0], taskB_buffer[0]);
        
        if (taskB_buffer[0] == 0) {
            printf("Task B: Register 126 contains 0 - OK\n");
        } else {
            printf("Task B: Register 126 contains %d\n", taskB_buffer[0]);
        }
    } else {
        printf("Task B: Failed to read from register 126. Error: %d\n", taskB_result);
    }

    return 0;
}