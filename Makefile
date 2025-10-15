CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRCDIR = src

# Source files (excluding main programs)
LIB_SOURCES = $(SRCDIR)/modbus_app.c $(SRCDIR)/modbus_protocol.c $(SRCDIR)/globals.c
LIB_OBJECTS = $(LIB_SOURCES:.c=.o)

# Main programs
MAIN_SOURCE = $(SRCDIR)/main.c
CLIENT_SOURCE = client.c

# Targets
TARGET_MAIN = ModBus
TARGET_CLIENT = Client
TARGET_MAIN_EXE = ModBus.exe
TARGET_CLIENT_EXE = Client.exe

# Default target - build both executables
all: $(TARGET_MAIN) $(TARGET_CLIENT) $(TARGET_MAIN_EXE) $(TARGET_CLIENT_EXE)

# Build main program (with menu)
$(TARGET_MAIN): $(LIB_OBJECTS) $(MAIN_SOURCE:.c=.o)
	$(CC) $(LIB_OBJECTS) $(MAIN_SOURCE:.c=.o) -o $(TARGET_MAIN)

# Build client program (exam version)
$(TARGET_CLIENT): $(LIB_OBJECTS) $(CLIENT_SOURCE:.c=.o)
	$(CC) $(LIB_OBJECTS) $(CLIENT_SOURCE:.c=.o) -o $(TARGET_CLIENT)

# Build .exe versions (for Windows compatibility)
$(TARGET_MAIN_EXE): $(LIB_OBJECTS) $(MAIN_SOURCE:.c=.o)
	$(CC) $(LIB_OBJECTS) $(MAIN_SOURCE:.c=.o) -o $(TARGET_MAIN_EXE)

$(TARGET_CLIENT_EXE): $(LIB_OBJECTS) $(CLIENT_SOURCE:.c=.o)
	$(CC) $(LIB_OBJECTS) $(CLIENT_SOURCE:.c=.o) -o $(TARGET_CLIENT_EXE)

# Compile object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Individual targets
main: $(TARGET_MAIN)
client: $(TARGET_CLIENT) 
exe: $(TARGET_MAIN_EXE) $(TARGET_CLIENT_EXE)

clean:
	rm -f $(SRCDIR)/*.o *.o $(TARGET_MAIN) $(TARGET_CLIENT) $(TARGET_MAIN_EXE) $(TARGET_CLIENT_EXE)

.PHONY: all clean main client exe

