#ifndef UTILS_H
#define UTILS_H

// Network configuration
#define MODBUS_PORT 5502
#define IN_BUF_LEN 256
#define SERVER_ADDR "172.28.128.1"

// Color codes for terminal output
#define RESET       "\033[0m"
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define BLUE        "\033[34m"
#define BOLD_RED    "\033[1;31m"
#define BOLD_GREEN  "\033[1;32m"
#define BOLD_YELLOW "\033[1;33m"

// External DEBUG variable declaration
extern int DEBUG;

#endif
