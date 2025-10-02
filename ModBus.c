#include <stdio.h>
#include <sys/types.h>
//#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>


#define MODBUS_PORT 5502
#define IN_BUF_LEN 100
#define SERVER_ADDR 127.0.0.1


/*
struct sockaddr {
    // Placeholder structure for sockaddr
    int sa_family; // Address family
    u_int16_t sin_port; // Port number
    struct in_addr sin_addr; // Internet address

};
struct in_addr {
    u_int32_t s_addr; // 32 bit adderess
};

void Write_multiple_registers() {
    // check consistency of parameters
    // assembles APDU
    
    // checks the response (apdu_R or error_code)
    // returns: number of writtend registers - ok <0 -error

}
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
    uint8_t byte_value = (uint8_t)function_code;  // Cast to
    uint8_t modbus_frame[256];
    int frame_length = sizeof(modbus_frame); // Example length of ModBus frame
      // Function code goes at byte
    printf("Hello, World!\n");
    printf("ModBus TCP Server running on port %d\n", MODBUS_PORT);
    printf("Server address in network byte order: 0x%X\n", bits_for_server_address);
    printf("Function code as byte: 0x%X\n", byte_value);
    printf("ModBus Frame Hex Dump:\n");
    for(int i = 0; i < frame_length; i++) {
        modbus_frame[i] = 0; // Fill with example data
        modbus_frame[0] = function_code;
        if(i % 8 == 0) printf("\n%04X: ", i);  // New line every 8 bytes
        printf("%02X ", modbus_frame[i]);   
    }
    printf("\n");
    
    printf("modbus_frame[0]: 0x%02X\n", modbus_frame[0]);
    printf("modbus_frame[1]: 0x%02X\n", modbus_frame[1]);

    return 0;
}
    