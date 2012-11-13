CC= gcc
CFLAGS = -g -Wall

TARGET_SERVER = rudp_server  
TARGET_CLIENT = rudp_client 

all : $(TARGET_SERVER) $(TARGET_CLIENT)

$(TARGET_SERVER) : common.o crc32.o rudp_server.o
	$(CC) $(CFLAGS) -o $(TARGET_SERVER) common.o crc32.o rudp_server.o

$(TARGET_CLIENT) : common.o crc32.o rudp_client.o
	$(CC) $(CFLAGS) -o $(TARGET_CLIENT) common.o crc32.o rudp_client.o

rudp_server.o : rudp_server.c
	$(CC) $(CFLAGS) -c -o rudp_server.o rudp_server.c

rudp_client.o : rudp_client.c
	$(CC) $(CFLAGS) -c -o rudp_client.o rudp_client.c

common.o : common.c
	$(CC) $(CFLAGS) -c -o common.o common.c

crc32.o : crc32.c
	$(CC) $(CFLAGS) -c -o crc32.o crc32.c

clean:
	rm -rf *.o $(TARGET_SERVER) $(TARGET_CLIENT)



