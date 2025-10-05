#include <stdio.h>
#include <sys/types.h>
//#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>


#define MODBUS_PORT 5502
#define IN_BUF_LEN 256
#define SERVER_ADDR "127.0.0.1"




int Write_multiple_registers(char *server_ip, int port, int startingRegister, int numRegisters, int *values) {

    uint8_t Function_code = 0x10;
    int length_of_apdu = 9 + 2 * numRegisters; // length of the apdu
    uint8_t Apdu[length_of_apdu]; // apdu to be sent
    uint8_t Apdu_R[5];
    Apdu[0] = Function_code;
    Apdu[1] = (startingRegister >> 8) & 0xFF; // starting address high byte
    Apdu[2] = startingRegister & 0xFF;        // starting address low byte
    Apdu[3] = (numRegisters >> 8) & 0xFF;     // number of registers high byte
    Apdu[4] = numRegisters & 0xFF;            // number of
    
    for (int i = 5; i < length_of_apdu; i++) {
        uint8_t High_byte = (values[(i - 5) / 2] >> 8) & 0xFF; // high byte of the value
        uint8_t Low_byte = values[(i - 5) / 2] & 0xFF;         // low byte of the value
        if ((i - 5) % 2 == 0) {
            Apdu[i] = High_byte;
        } else {
            Apdu[i] = Low_byte;
        }
    }
    // TODO response form the server and understand what is going on with consistency of parameters
    // check consistency of parameters
    // assembles APDU
    
    // checks the response (apdu_R or error_code)
    // returns: number of writtend registers - ok <0 -error
    return 0;
}
/*
void Read_holding_registers() {
    // check consistency of parameters
    // assembles APDU
    
    // checks the response (apdu_R or error_code)
    // returns: number of read registers - ok <0 -error

}
void Send_Modbus_request() {
    // sends the request to the slave
    // waits for the response
    // returns: number of bytes in the response - ok <0 -error

}
int socket (int domain, int type, int protocol) {
    // Create a socket
    // domain: pf_inet for IPv4
    // type: channel properties - in practice SOCK_STREAM for TCP, SOCK_DGRAM for UDP
    // protocol: which protocol - in practice 0 = IPPROTO_TCP

    // returns a socket local identifier or -1 for error
    return 0; // Placeholder return value
}
int bind (int sockfd, const struct sockaddr *my_addr, socklen_t addrlen) {
    
    // Bind a socket to an address
    // sockfd: socket file descriptor ( returned by socket() )
    // addr: pointer to sockaddr structure with the address to bind to
    // addrlen: length of the address structure

    // returns 0 on success, -1 on error
    // mentarory in the server side optional in the client

    return 0; // Placeholder return value
}
*/



int main() {

    u_int16_t bits_for_server_address = htons(MODBUS_PORT); // Convert to network byte order
    
    int function_code = 3;  // Example function code
    char server_ip[] = SERVER_ADDR; // Example server IP address
    uint8_t modbus_frame[256];
    int frame_length = sizeof(modbus_frame); // Example length of ModBus frame
    int startingRegister = 0; // Starting register address
    int numRegisters = 0; // Number of registers to read/write
    int buffer[numRegisters]; // Buffer for register values
      // Function code goes at byte
    printf("Hello, you are going to connect to %s\n", server_ip);
    printf("ModBus TCP Server running on port %d\n", MODBUS_PORT);
    printf("You have 2 options:\n 1 - Read Holding Registers (Function code 3)\n 2 - Write Multiple Registers (Function code 16)\n");
    printf("Input what kind of function you want to use (1-2): ");
    scanf("%d", &function_code);
    printf("Function code: %d\n", function_code);
    switch(function_code) {
        case 1:
            printf("You chose to read holding registers (Function code 3)\n");
            // Call the function to read holding registers
            // Read_holding_registers();
            break;
        case 2:
            printf("You chose to write multiple registers (Function code 16)\n");
            printf("Input number of registers to write: ");
            scanf("%d", &numRegisters);
            for (int i = 0; i < numRegisters; i++) {
                printf("Input value for register %d: ", i + startingRegister);
                scanf("%d", &buffer[i]);
            }
            int result = Write_multiple_registers(server_ip, MODBUS_PORT, startingRegister, numRegisters, buffer);
            if (result == numRegisters) {
                printf("Successfully wrote %d registers starting from address %d\n", numRegisters, startingRegister);
            }
            if (result == 0) {
                printf("You are stupid\n");
            }
            // Call the function to write multiple registers
            // Write_multiple_registers();
            break;
        default:
            printf("Invalid option. Please choose 1 or 2.\n");
            return -1;
    }
    

    return 0;
}
    