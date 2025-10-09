#include <stdio.h>
#include "modbus_app.h"
#include "modbus_protocol.h"
#include "utils.h"
#include <arpa/inet.h>


int main() {

   // uint16_t bits_for_server_address = htons(MODBUS_PORT); // Convert to network byte order
    
    int function_code = 0;  // Example function code
    char server_ip[] = SERVER_ADDR; // Example server IP address
    //uint8_t modbus_frame[256];
    //int frame_length = sizeof(modbus_frame); // Example length of ModBus frame
    int buffer[100]; // Buffer for register values
    int numRegisters = 0;
    int startingRegister = 0;
    
    while (1) { // Function code goes at byte

        printf("Hello, you are going to connect to %s\n", server_ip);
        printf("ModBus TCP Server running on port %d\n", MODBUS_PORT);
        printf("You have 3 options:\n 1 - Read Holding Registers (Function code 3)\n 2 - Write Multiple Registers (Function code 16)\n 3 - Exit\n");
        printf("Input what kind of function you want to use (1-3): ");
        scanf("%d", &function_code);
        printf("Function code: %d\n", function_code);
        
        switch(function_code) {
            case 1:{
                printf("You chose to read holding registers (Function code 3)\n");
                
                printf("Starting register address is set to %d\n", startingRegister);
                printf("Input starting register address: ");
                scanf("%d", &startingRegister);
                printf("You chose to read multiple registers (Function code 3)\n");
                printf("Input number of registers to read: ");
                scanf("%d", &numRegisters);
                
                
                int result = Read_holding_registers(server_ip, MODBUS_PORT, startingRegister, numRegisters, buffer);

                if (result == 0) {
                    printf(BOLD_GREEN "Successfully read %d registers starting from address %d\n" RESET, numRegisters, startingRegister);
                } else if (result == -1){
                    printf(BOLD_RED "Error occurred\n" RESET);
                }


                break;
            }
            case 2:{
                
                
                printf("Starting register address is set to %d\n", startingRegister);
                printf("Input starting register address: ");
                scanf("%d", &startingRegister);
                printf("You chose to write multiple registers (Function code 16)\n");
                printf("Input number of registers to write: ");
                scanf("%d", &numRegisters);

                

                for (int i = 0; i < numRegisters; i++) {
                    printf("Input value for register %d: ", i + startingRegister);
                    scanf("%d", &buffer[i]);
                }
                int result = Write_multiple_registers(server_ip, MODBUS_PORT, startingRegister, numRegisters, buffer);

                if (result == numRegisters) {
                    printf(BOLD_GREEN "Successfully wrote %d registers starting from address %d\n" RESET, numRegisters, startingRegister);
                }
                if (result == 0) {
                    printf(BOLD_RED "You are stupid\n" RESET);
                }
                // Call the function to write multiple registers
                // Write_multiple_registers();
                break;
            }case 3:
                printf("Exiting the program.\n");
                return 0;
            
            default:
                printf("Invalid option. Please choose 1 or 2.\n");
                return -1;
        }
    }
    

    return 0;
}