#include <stdio.h>
#include <sys/types.h>
//#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>


#define MODBUS_PORT 502
#define IN_BUF_LEN 256
#define SERVER_ADDR "127.0.0.1"


int Send_Modbus_request(char *server_ip, int port, uint8_t *apdu, int apdu_len, uint8_t *apdu_response);
int Write_multiple_registers(char *server_ip, int port, int startingRegister, int numRegisters, int *values);


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
    Apdu[5] = numRegisters * 2;               // byte count

    // filling the values to be written
    for (int i = 6; i < length_of_apdu; i++) {
        uint8_t High_byte = (values[(i - 5) / 2] >> 8) & 0xFF; // high byte of the value
        uint8_t Low_byte = values[(i - 5) / 2] & 0xFF;         // low byte of the value
        if ((i - 5) % 2 == 0) {
            Apdu[i] = High_byte;
        } else {
            Apdu[i] = Low_byte;
        }
    }

    int Result = Send_Modbus_request(server_ip, port, Apdu, length_of_apdu, Apdu_R);

    if (Apdu_R[0] = Function_code) {
        if (Apdu_R[1] == Apdu[1] && Apdu_R[2] == Apdu[2]) {
            if (Apdu_R[3] == Apdu[3] && Apdu_R[4] == Apdu[4]) {
                // all good
                return numRegisters; // number of registers written
            } else {
                // error in number of registers
                return -3; // error in number of registers
            }
        } else {
            // error in starting address
            return -2; // error in starting address
        }
    } else {
        // error in function code
        return -1; // error in function code
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
*/

int Send_Modbus_request(char *server_ip, int port, uint8_t *Apdu, int length_of_apdu, uint8_t *Apdu_R) {

    uint8_t Mbap[7]; // mbap header
    uint8_t Modbus_frame[7 + length_of_apdu]; // complete modbus
    int length_of_frame = 7 + length_of_apdu; // length of the complete modbus frame

    static uint16_t transaction_id = 0; // transaction id
    uint16_t protocol_id = 0; // protocol id
    uint16_t length = htons(length_of_apdu + 1); // length of the remaining bytes (unit id + apdu)
    uint8_t unit_id = 1; // unit id

    // filling the mbap header
    Mbap[0] = (transaction_id >> 8) & 0xFF; // transaction id high byte
    Mbap[1] = transaction_id & 0xFF;        // transaction id
    Mbap[2] = (protocol_id >> 8) & 0xFF;    // protocol id high byte
    Mbap[3] = protocol_id & 0xFF;           // protocol id
    Mbap[4] = (length >> 8) & 0xFF;         // length high byte
    Mbap[5] = length & 0xFF;                // length low byte
    Mbap[6] = unit_id;                      // unit id
    transaction_id++;                       // increment transaction id for next request
    // filling the complete modbus frame
    for (int i = 0; i < 7; i++) {
        Modbus_frame[i] = Mbap[i];
    }

    // filling the APDU
    for (int i = 0; i < length_of_apdu; i++) {
        Modbus_frame[7 + i] = Apdu[i];
    }

    // Socket setup and comunication
    int socket_desc;
	struct sockaddr_in server;
	
	
	//Create socket
	socket_desc = socket(PF_INET , SOCK_STREAM , IPPROTO_TCP);
	if (socket_desc == -1)
	{
		printf("Socket creation failed...\n");
		return -1;
	}
	else
		printf("Socket successfully created...\n");

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(server_ip);  // Use parameter instead of SERVER_ADDR
	server.sin_port = htons(port);

	//Connect to remote server
	
	if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		printf("Connection with the server failed...\n"); 
        printf("Make sure that the server is running and reachable at address (%s) port (%d)\n", server_ip, port);

        for (int i = 0; i < length_of_frame; i++) {
            printf("0x%02X ", Modbus_frame[i]);
        }
        printf("\n");
        close(socket_desc);
		return 1;
	}
	else
		printf("Connected to the server at address (%s) port (%d)...\n", server_ip, port); 	
	
	

    // sends the request to the slave
    // waits for the response
    // returns: number of bytes in the response - ok <0 -error
    close(socket_desc);
    return 0;
}
/*
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
    int buffer[100]; // Buffer for register values
    int numRegisters = 0;
    
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
            int startingRegister = 0; // Starting register address
             // Number of registers to read/write
            
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
    