#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>


#define MODBUS_PORT 5502
#define IN_BUF_LEN 256
#define SERVER_ADDR "172.28.128.1"
//#define SERVER_ADDR "172.28.20.1"

// Color codes for terminal output
#define RESET       "\033[0m"
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define BLUE        "\033[34m"
#define BOLD_RED    "\033[1;31m"
#define BOLD_GREEN  "\033[1;32m"
#define BOLD_YELLOW "\033[1;33m"


int Send_Modbus_request(char *server_ip, int port, uint8_t *apdu, int apdu_len, uint8_t *apdu_response);
int Write_multiple_registers(char *server_ip, int port, int startingRegister, int numRegisters, int *values);


int Write_multiple_registers(char *server_ip, int port, int startingRegister, int numRegisters, int *values) {
    // verify parameters limit of the starting 
    if (numRegisters <= 0 || numberRegisters > 123) {
        printf(BOLD_RED "Invalid number of registers: %d. Must be between 1 and 123.\n" RESET, numRegisters);
        return -1; // invalid parameters
    }
    else if (startingRegister + numRegisters > 65536) {
        printf(BOLD_RED "Invalid starting register: %d. Must be between 0 and %d.\n" RESET, startingRegister, 65536 - numRegisters);
        return -1; // invalid parameters
    }

    uint8_t Function_code = 0x10;
    int length_of_apdu = 6 + 2 * numRegisters; // length of the apdu
    uint8_t Apdu[length_of_apdu]; // apdu to be sent
    uint8_t Apdu_R[5+7]; // 5 bytes for response  i think plus 7 for MBAP

    startingRegister = startingRegister - 1; // ModBus addresses start from 0

    Apdu[0] = Function_code;
    Apdu[1] = (startingRegister >> 8) & 0xFF; // starting address high byte
    Apdu[2] = startingRegister & 0xFF;        // starting address low byte
    Apdu[3] = (numRegisters >> 8) & 0xFF;     // number of registers high byte
    Apdu[4] = numRegisters & 0xFF;            // number of
    Apdu[5] = numRegisters * 2;               // byte count

    // filling the values to be written
    for (int i = 0; i < numRegisters; i++) {
        Apdu[6 + 2*i] = (values[i] >> 8) & 0xFF;     // high byte
        Apdu[6 + 2*i + 1] = values[i] & 0xFF;        // low byte
    }

    int Result = Send_Modbus_request(server_ip, port, Apdu, length_of_apdu, Apdu_R);

    if (Result < 0) {
        printf(BOLD_RED "Error in sending ModBus request: %d\n" RESET, Result);
        return Result;
    }
    if (Result == 5){
        printf(BOLD_GREEN "Successfully recived correct number of bytes in response: %d\n" RESET, Result);
        if (Apdu_R[0] == Function_code){
            uint16_t resp_starting_address = (Apdu_R[1] << 8) | Apdu_R[2];
            uint16_t resp_num_registers = (Apdu_R[3] << 8) | Apdu_R[4];
            if (resp_starting_address == startingRegister && resp_num_registers == numRegisters){
                printf(BOLD_GREEN "Successfully wrote %d registers starting from address %d\n" RESET, resp_num_registers, resp_starting_address);
                return resp_num_registers; // number of registers written
            } else {
                printf(BOLD_RED "Error in response: starting address or number of registers do not match\n" RESET);
                printf(BOLD_RED "Expected starting address: %d, got: %d\n" RESET, startingRegister, resp_starting_address);
                printf(BOLD_RED "Expected number of registers: %d, got: %d\n" RESET, numRegisters, resp_num_registers);
                return -1; // error in response
            }
        }
    }
    // TODO response form the server and understand what is going on with consistency of parameters
    // check consistency of parameters
    // assembles APDU
    
    // checks the response (apdu_R or error_code)
    // returns: number of writtend registers - ok <0 -error
    return 0;
}

int Read_holding_registers(char *server_ip, int port, int startingRegister, int numRegisters, int *values) {
    // check consistency of parameters
    if (startingRegister < 0 || numRegisters <= 0 || values == NULL) {
        return -1; // invalid parameters
    }

    // assembles APDU
    uint8_t Function_code = 0x03;
    int length_of_apdu = 5;
    uint8_t Apdu[length_of_apdu];
    uint8_t Apdu_R[5 + 2 * numRegisters];

    Apdu[0] = Function_code;
    Apdu[1] = (startingRegister >> 8) & 0xFF; // starting address high byte
    Apdu[2] = startingRegister & 0xFF;        // starting address low byte
    Apdu[3] = (numRegisters >> 8) & 0xFF;     // number of registers high byte
    Apdu[4] = numRegisters & 0xFF;            // number of registers low byte

    int Result = Send_Modbus_request(server_ip, port, Apdu, length_of_apdu, Apdu_R);

    if (Result < 0) {

        printf(BOLD_RED "Error in sending ModBus request: %d\n" RESET, Result);
        return -2; // error in request
    }

    // checks the response (apdu_R or error_code)
    if (Result == sizeof(Apdu_R)){
        if (Apdu_R[0] == Function_code){
            printf(BOLD_GREEN "Successfully recived correct number of bytes in response: %d\n" RESET, Result);
        }
    }
    return 0;

}


int Send_Modbus_request(char *server_ip, int port, uint8_t *Apdu, int length_of_apdu, uint8_t *Apdu_R) {

    uint8_t Mbap[7]; // mbap header
    uint8_t Modbus_frame[7 + length_of_apdu]; // complete modbus
    int length_of_frame = 7 + length_of_apdu; // length of the complete modbus frame

    static uint16_t transaction_id = 0; // transaction id
    uint16_t protocol_id = 0; // protocol id
    uint16_t length = length_of_apdu + 1; // length of the remaining bytes (unit id + apdu)
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
	int Time_start = time(NULL);
	
    //Check if socket is created
	if (socket_desc == -1)
	{
		printf(BOLD_RED "Socket creation failed...\n" RESET);
		return -1;
	}
	else
		printf(BOLD_GREEN "Socket successfully created...\n" RESET);

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(server_ip);  // Use parameter instead of SERVER_ADDR
	server.sin_port = htons(port);

	//Connect to remote server
	if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		printf(BOLD_RED "Connection with the server failed: %s\n" RESET, strerror(errno)); 
        printf("Make sure that the server is running and reachable at address ("BLUE "%s" RESET ") port ("BLUE "%d" RESET ")\n", server_ip, port);

        for (int i = 0; i < length_of_frame; i++) {
            printf(BOLD_YELLOW "0x%02X ", Modbus_frame[i]);
        }
        printf("\n");
        close(socket_desc);
		return 1;
	}
	else
		printf(BOLD_GREEN "Connected to the server at address (%s) port (%d)...\n" RESET, server_ip, port); 	
	
	int out = write(socket_desc , Modbus_frame , length_of_frame);

    if (out < 0) {
        printf(BOLD_RED "Sending data to the server failed...\n" RESET);
        close(socket_desc);
        return -2;
    }
    printf(BOLD_GREEN "Data sent to the server successfully...\n" RESET);
    
    // Debug: Print the sent frame
    printf(BLUE "Sent frame (%d bytes): ", length_of_frame);

    for (int i = 0; i < length_of_frame; i++) {
        printf("0x%02X ", Modbus_frame[i]);
    }

    printf("\n" RESET);

    uint8_t Mbap_R[7]; // mbap header for response

    //int in = recv(socket_desc, Mbap_R, sizeof(Mbap_R), 0);
    int HearedIN = read(socket_desc, Mbap_R, 7);
    int in = 0;

    if (Mbap_R[0] != Mbap[0] || Mbap_R[1] != Mbap[1]) {
        printf(BOLD_RED "Transaction ID mismatch: sent 0x%02X%02X, received 0x%02X%02X\n" RESET, Mbap[0], Mbap[1], Mbap_R[0], Mbap_R[1]);
        close(socket_desc);
        return 0; // transaction ID mismatch
    }
    else {
        uint16_t waiting_bytes = (Mbap_R[4] << 8) | Mbap_R[5];
        waiting_bytes -= 1; // subtract unit id byte
        in = read(socket_desc, Apdu_R, waiting_bytes);
    }
    // Transaction id should be the same as sent

    printf(BOLD_GREEN "Response received from the server successfully...\n" RESET);

    printf(BOLD_YELLOW "APDU Response\n" RESET);
    for (int i = 0; i < in; i++) {
        Apdu_R[i] = Apdu_R[i];
        printf(BLUE "0x%02X ", Apdu_R[i]);
    }
    printf("\n");
    close(socket_desc);
    printf(BOLD_GREEN "Connection closed...\n" RESET);
    return in; // return length of the apdu response
}

int main() {

    u_int16_t bits_for_server_address = htons(MODBUS_PORT); // Convert to network byte order
    
    int function_code = 3;  // Example function code
    char server_ip[] = SERVER_ADDR; // Example server IP address
    uint8_t modbus_frame[256];
    int frame_length = sizeof(modbus_frame); // Example length of ModBus frame
    int buffer[100]; // Buffer for register values
    int numRegisters = 0;
    int startingRegister = 0;
    
      // Function code goes at byte
    printf("Hello, you are going to connect to %s\n", server_ip);
    printf("ModBus TCP Server running on port %d\n", MODBUS_PORT);
    printf("You have 2 options:\n 1 - Read Holding Registers (Function code 3)\n 2 - Write Multiple Registers (Function code 16)\n");
    printf("Input what kind of function you want to use (1-2): ");
    scanf("%d", &function_code);
    printf("Function code: %d\n", function_code);
    
    switch(function_code) {
        case 1:{
            printf("You chose to read holding registers (Function code 3)\n");
            
            printf("Starting register address is set to %d\n", startingRegister);
            printf("Input starting register address: ");
            scanf("%d", &startingRegister);
            printf("You chose to write multiple registers (Function code 16)\n");
            printf("Input number of registers to write: ");
            scanf("%d", &numRegisters);
            
            
            int result = Read_holding_registers(server_ip, MODBUS_PORT, startingRegister, numRegisters, buffer);

            if (result == numRegisters) {
                printf(BOLD_GREEN "Successfully read %d registers starting from address %d\n" RESET, numRegisters, startingRegister);
                for (int i = 0; i < numRegisters; i++) {
                    printf("Register %d: %d\n", i + startingRegister, buffer[i]);
                }
            }
            
            if (result == 0) {
                printf(BOLD_RED "You are stupid\n" RESET);
            }

            printf(BOLD_YELLOW "Why are you Here?" RESET "\n");
            printf(BOLD_YELLOW "This is a result of reading register %d s\n" RESET, result);

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
        }
        default:
            printf("Invalid option. Please choose 1 or 2.\n");
            return -1;
    }
    

    return 0;
}
    