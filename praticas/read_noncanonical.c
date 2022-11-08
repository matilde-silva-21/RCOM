// Read from serial port in non-canonical mode
//
// Modified by: Eduardo Nuno Almeida [enalmeida@fe.up.pt]

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <stdbool.h>

// Baudrate settings are defined in <asm/termbits.h>, which is
// included by <termios.h>
#define BAUDRATE B38400
#define _POSIX_SOURCE 1 // POSIX compliant source

#define FALSE 0
#define TRUE 1

#define BUF_SIZE 256

volatile int STOP = FALSE;

enum state {
    STATE0,
    STATE1,
    STATE2,
    STATE3, 
    STATE4,
    STATE5      
} typedef STATE;

/**
 * Extracts a selection of string and return a new string or NULL.
 * It supports both negative and positive indexes.
 */

int main(int argc, char *argv[])
{
    // Program usage: Uses either COM1 or COM2
    const char *serialPortName = argv[1];

    

    if (argc < 2)
    {
        printf("Incorrect program usage\n"
               "Usage: %s <SerialPort>\n"
               "Example: %s /dev/ttyS1\n",
               argv[0],
               argv[0]);
        exit(1);
    }

    // Open serial port device for reading and writing and not as controlling tty
    // because we don't want to get killed if linenoise sends CTRL-C.
    int fd = open(serialPortName, O_RDWR | O_NOCTTY);
    if (fd < 0)
    {
        perror(serialPortName);
        exit(-1);
    }

    struct termios oldtio;
    struct termios newtio;

    // Save current port settings
    if (tcgetattr(fd, &oldtio) == -1)
    {
        perror("tcgetattr");
        exit(-1);
    }

    // Clear struct for new port settings
    memset(&newtio, 0, sizeof(newtio));

    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    // Set input mode (non-canonical, no echo,...)
    newtio.c_lflag = 0;
    newtio.c_cc[VTIME] = 0; // Inter-character timer unused
    newtio.c_cc[VMIN] = 1;  // Blocking read until 5 chars received

    // VTIME e VMIN should be changed in order to protect with a
    // timeout the reception of the following character(s)

    // Now clean the line and activate the settings for the port
    // tcflush() discards data written to the object referred to
    // by fd but not transmitted, or data received but not read,
    // depending on the value of queue_selector:
    //   TCIFLUSH - flushes data received but not read.
    tcflush(fd, TCIOFLUSH);

    // Set new port settings
    if (tcsetattr(fd, TCSANOW, &newtio) == -1)
    {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");

    
    unsigned char buf[1] = {0}, parcels[5] = {0}; // +1: Save space for the final '\0' char

    STATE st = STATE0;
    bool readByte = true;

    sleep(6);
    // Loop for input
    while (STOP == FALSE)
    { 
        if(readByte){
            int bytes = read(fd, buf, 1); //ler byte a byte
            if(bytes==0) continue;
        }
       
        
        switch (st)
        {
        case STATE0:
            if(buf[0] == 0x7E){
                st = STATE1;
                parcels[0] = buf[0];
            }
            break;

        case STATE1:
            if(buf[0] != 0x7E){
                st = STATE2;
                parcels[1] = buf[0];
            }
            else {
                st = STATE0;
                memset(parcels, 0, 5);
            }
            break;

        case STATE2:
            if(buf[0] != 0x7E){
                st = STATE3;
                parcels[2] = buf[0];
            }
            else {
                st = STATE0;
                memset(parcels, 0, 5);
            }
            break;

        case STATE3:
            if(buf[0] != 0x7E){
                parcels[3] = buf[0];
                st = STATE4;
            }
            else {
                st = STATE0;
                memset(parcels, 0, 5);
            }
            break;

        case STATE4:
            if(buf[0] == 0x7E){
                parcels[4] = buf[0];
                st = STATE5;
                readByte = false;
            }

            else {
                st = STATE0;
                memset(parcels, 0, 5);
            }
            break;
        case STATE5:
            if(((parcels[1])^(parcels[2]))==(parcels[3])){
                printf("\nGreat success! SET message received without errors\n\n");
                STOP = TRUE;
            }
            else {
                st = STATE0;
                memset(parcels, 0, 5);
                readByte = true;
            }
            break;
        default:
            break;
        }
    }

    parcels[2] = 0x07;
    parcels[4] = parcels[2]^parcels[3];

    //preciso de estar dentro da state machine ate receber um sinal a dizer que o UA foi corretamente recebido

    int bytes = write(fd, parcels, sizeof(parcels));
    printf("UA message sent, %d bytes written\n", bytes);






    // Restore the old port settings
    if (tcsetattr(fd, TCSANOW, &oldtio) == -1)
    {
        perror("tcsetattr");
        exit(-1);
    }

    close(fd);

    return 0;
}
