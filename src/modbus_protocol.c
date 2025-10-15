#include "modbus_app.h"
#include "modbus_protocol.h" 
#include "utils.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>

int Send_Modbus_request(char *server_ip, int port, uint8_t *Apdu, int length_of_apdu, uint8_t *Apdu_R) {

    uint8_t Mbap[7]; // mbap header
    uint8_t *Modbus_frame = malloc(7 + length_of_apdu); // complete modbus
    int length_of_frame = 7 + length_of_apdu; // length of the complete modbus frame

    static uint16_t transaction_id = 0; // transaction id
    uint16_t protocol_id = 0; // protocol id
    uint16_t length = length_of_apdu + 1; // length of the remaining bytes (unit id + apdu)
    uint8_t unit_id = 1; // unit id

    uint8_t Mbap_R[7];

    if (DEBUG) printf(YELLOW "Transaction ID: %d\n" RESET, transaction_id);


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
	
    //Check if socket is created
	if (socket_desc == -1)
	{
		if (DEBUG) printf(BOLD_RED "Socket creation failed...\n" RESET);
        free(Modbus_frame);
		return -1;
	}
	else
		if (DEBUG) printf(BOLD_GREEN "Socket successfully created...\n" RESET);

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(server_ip);  // Use parameter instead of SERVER_ADDR
	server.sin_port = htons(port);

	//Connect to remote server
	if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		if (DEBUG) printf(BOLD_RED "Connection with the server failed: %s\n" RESET, strerror(errno)); 
        if (DEBUG) printf("Make sure that the server is running and reachable at address ("BLUE "%s" RESET ") port ("BLUE "%d" RESET ")\n", server_ip, port);
        
        if (DEBUG) {
            for (int i = 0; i < length_of_frame; i++) {
                printf(BOLD_YELLOW "0x%02X ", Modbus_frame[i]);
            }
            printf("\n");
        }
        
        close(socket_desc);
        free(Modbus_frame);
		return 1;
	}
	else
		if (DEBUG) printf(BOLD_GREEN "Connected to the server at address (%s) port (%d)...\n" RESET, server_ip, port); 	
	
	int out = write(socket_desc , Modbus_frame , length_of_frame);
    


    if (out < 0) {
        if (DEBUG) printf(BOLD_RED "Sending data to the server failed...\n" RESET);
        close(socket_desc);
        free(Modbus_frame);
        return -2;
    }
    if (DEBUG) printf(BOLD_GREEN "Data sent to the server successfully...\n" RESET);
    
    // Debug: Print the sent frame
    if (DEBUG) {
        printf(BLUE "Sent frame (%d bytes): ", length_of_frame);
        
        for (int i = 0; i < length_of_frame; i++) {
            printf("0x%02X ", Modbus_frame[i]);
        }
        printf("\n" RESET);
    }
    

    

     // mbap header for response

    //int in = recv(socket_desc, Mbap_R, sizeof(Mbap_R), 0);
    int HearedIN = read(socket_desc, Mbap_R, 7);
    int in = 0;
    if (HearedIN < 0) {
        if (DEBUG) printf(BOLD_RED "Receiving data from the server failed...\n" RESET);
        close(socket_desc);
        free(Modbus_frame);
        return -3;
    }
    if (Mbap_R[0] != Mbap[0] || Mbap_R[1] != Mbap[1]) {
        if (DEBUG) printf(BOLD_RED "Transaction ID mismatch: sent 0x%02X%02X, received 0x%02X%02X\n" RESET, Mbap[0], Mbap[1], Mbap_R[0], Mbap_R[1]);
        close(socket_desc);
        free(Modbus_frame);
        return 0; // transaction ID mismatch
    }
    else {
        uint16_t waiting_bytes = (Mbap_R[4] << 8) | Mbap_R[5];
        waiting_bytes -= 1; // subtract unit id byte
        in = read(socket_desc, Apdu_R, waiting_bytes);
    }
    // Transaction id should be the same as sent

    if (DEBUG) printf(BOLD_GREEN "Response received from the server successfully...\n" RESET);

    if (DEBUG) {
        printf(BOLD_YELLOW "APDU Response\n" RESET);
        for (int i = 0; i < in; i++) {
            Apdu_R[i] = Apdu_R[i];
            printf(BLUE "0x%02X ", Apdu_R[i]);
        }
        printf("\n");
    }
    close(socket_desc);
    if (DEBUG) printf(BOLD_GREEN "Connection closed...\n" RESET);
    free(Modbus_frame);
    return in; // return length of the apdu response
}