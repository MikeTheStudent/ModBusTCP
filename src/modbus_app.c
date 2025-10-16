#include "modbus_app.h"
#include "modbus_protocol.h" 
#include "utils.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


int Write_multiple_registers(char *server_ip, int port, int startingRegister, int numRegisters, int *values) {
    // verify parameters limit of the starting 
    if (numRegisters <= 0 || numRegisters > 123) {
        printf(BOLD_RED "Invalid number of registers: %d. Must be between 1 and 123.\n" RESET, numRegisters);
        return -1; // invalid parameters
    }
    else if (startingRegister + numRegisters > 65536) {
        printf(BOLD_RED "Invalid starting register: %d. Must be between 0 and %d.\n" RESET, startingRegister, 65536 - numRegisters);
        return -1; // invalid parameters
    }

    uint8_t Function_code = 0x10;
    int length_of_apdu = 6 + 2 * numRegisters; // length of the apdu
    uint8_t *Apdu = malloc(length_of_apdu); // apdu to be sent THIS SHOULD BE MALLOC2
    uint8_t *Apdu_R = malloc(5 + 7); // 5 bytes for response  i think plus 7 for MBAP

    //Server registers start form 0
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
        if (DEBUG) printf(BOLD_RED "Error in sending ModBus request: %d\n" RESET, Result);
        free(Apdu);
        free(Apdu_R);
        return Result;
    }
    if (Result == 5){
        if (DEBUG) printf(BOLD_GREEN "Successfully recived correct number of bytes in response: %d\n" RESET, Result);
        if (Apdu_R[0] == Function_code){
            uint16_t resp_starting_address = (Apdu_R[1] << 8) | Apdu_R[2];
            uint16_t resp_num_registers = (Apdu_R[3] << 8) | Apdu_R[4];
            if (resp_starting_address == startingRegister && resp_num_registers == numRegisters){
                if (DEBUG) printf(BOLD_GREEN "Successfully wrote %d registers starting from address %d\n" RESET, resp_num_registers, resp_starting_address);

                free(Apdu);
                free(Apdu_R);
                return resp_num_registers; // number of registers written
            } else {
                if (DEBUG) printf(BOLD_RED "Error in response: starting address or number of registers do not match\n" RESET);
                if (DEBUG) printf(BOLD_RED "Expected starting address: %d, got: %d\n" RESET, startingRegister, resp_starting_address);
                if (DEBUG) printf(BOLD_RED "Expected number of registers: %d, got: %d\n" RESET, numRegisters, resp_num_registers);
                free(Apdu);
                free(Apdu_R);
                return -1; // error in response
            }
        }
    }
    else if (Result == 2){
        if (Apdu_R[0] == (Function_code + 0x80)){
            uint8_t exception_code = Apdu_R[1];
            if (DEBUG) printf(BOLD_RED "ModBus Exception response received. Exception code: %d\n" RESET, exception_code);
            switch(exception_code) {
                case 1:
                    printf(BOLD_RED "Illegal Function\n" RESET);
                    free(Apdu);
                    free(Apdu_R);
                    return -2;

                case 2:
                    printf(BOLD_RED "Illegal Data Address\n" RESET);
                    free(Apdu);
                    free(Apdu_R);
                    return -3;
                    
                case 3:
                    printf(BOLD_RED "Illegal Data Value\n" RESET);
                    free(Apdu);
                    free(Apdu_R);
                    return -4;
                    
                case 4:
                    printf(BOLD_RED "Slave Device Failure\n" RESET);
                    free(Apdu);
                    free(Apdu_R);
                    return -5;
                default:
                    if (DEBUG) printf(BOLD_RED "Unknown Exception Code\n" RESET);
            }
            free(Apdu);
            free(Apdu_R);
            return -1; // return negative exception code
        } else {
            if (DEBUG) printf(BOLD_RED "Error in response: unexpected function code in exception response\n" RESET);
            free(Apdu);
            free(Apdu_R);
            return -1; // error in response
        }
    }
    else {
        if (DEBUG) printf(BOLD_RED "Error in response: unexpected number of bytes received: %d\n" RESET, Result);
        free(Apdu);
        free(Apdu_R);
        return -1; // error in response
    }
    return 0;
}

int Read_holding_registers(char *server_ip, int port, int startingRegister, int numRegisters, int *values) {
    // check consistency of parameters
    if (startingRegister + numRegisters > 65536 || numRegisters <= 0 || numRegisters > 125) {
        printf(BOLD_RED "Invalid parameters: startingRegister=%d, numRegisters=%d\n" RESET, startingRegister, numRegisters);
        return -1; // invalid parameters
    }

    // assembles APDU
    uint8_t Function_code = 0x03;
    int length_of_apdu = 5;
    int length_of_response = 2 + 2 * numRegisters; // function code + byte count + 2 bytes per register
    uint8_t *Apdu = malloc(length_of_apdu);
    uint8_t *Apdu_R = malloc(length_of_response);
    

    startingRegister = startingRegister - 1; // ModBus addresses start from 0

    Apdu[0] = Function_code;
    Apdu[1] = (startingRegister >> 8) & 0xFF; // starting address high byte
    Apdu[2] = startingRegister & 0xFF;        // starting address low byte
    Apdu[3] = (numRegisters >> 8) & 0xFF;     // number of registers high byte
    Apdu[4] = numRegisters & 0xFF;            // number of registers low byte

    int Result = Send_Modbus_request(server_ip, port, Apdu, length_of_apdu, Apdu_R);

    // checks the response (apdu_R or error_code)
    if (Result == length_of_response){
        if (Apdu_R[0] == Apdu[0]){
            if (DEBUG) printf(BOLD_GREEN "Successfully recived correct number of bytes in response: %d\n" RESET, Result);
            }
        for (int i = 0; i < numRegisters; i++) {
            int register_number = i + startingRegister + 1; // Adjust for 1-based addressing in output
            values[i] = (Apdu_R[2*i + 2] << 8) | Apdu_R[2*i + 3];
            if (DEBUG) printf("Register %d: %d\n", register_number, values[i]);
         } // combine high and low byte
        free(Apdu);
        free(Apdu_R);
        return numRegisters; // number of registers read
    } else if (Apdu_R[0] == (Function_code + 0x80)){
            uint8_t exception_code = Apdu_R[1];
            if (DEBUG) printf(BOLD_RED "ModBus Exception response received. Exception code: %d\n" RESET, exception_code);
            switch(exception_code) {
                case 1:
                    printf(BOLD_RED "Illegal Function\n" RESET);
                    free(Apdu);
                    free(Apdu_R);
                    return -2;
                case 2:
                    printf(BOLD_RED "Illegal Data Address\n" RESET);
                    free(Apdu);
                    free(Apdu_R);
                    return -3;
                case 3:
                    printf(BOLD_RED "Illegal Data Value\n" RESET);
                    free(Apdu);
                    free(Apdu_R);
                    return -4;
                case 4:
                    printf(BOLD_RED "Slave Device Failure\n" RESET);
                    free(Apdu);
                    free(Apdu_R);
                    return -5;
                default:
                    if (DEBUG) printf(BOLD_RED "Unknown Exception Code\n" RESET);
            }
            free(Apdu);
            free(Apdu_R);
            return -1; // return negative exception code
    } else {
            if (DEBUG) printf(BOLD_RED "Error in response: unexpected function code in exception response\n" RESET);
            free(Apdu);
            free(Apdu_R);
            return -1; // error in response
        }
    free(Apdu);
    free(Apdu_R);
    return 0;

}