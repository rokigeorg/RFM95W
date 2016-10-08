#include <string>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
//#include <sys/socket.h>
//#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <cstdlib>
//#include <sys/time.h>
#include <cstring>

#include <sys/ioctl.h>
//#include <net/if.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>

using namespace std;

// SX1276 - Raspberry connections
int ssPin = 6;
int dio0  = 7;
int RST   = 0;

static const int CHANNEL =0;


//################# copyed from single channel gateway Copyright (c) 2015 Thomas Telkamp ########################## 
	

#define REG_FIFO                    0x00
#define REG_FIFO_ADDR_PTR           0x0D
#define REG_FIFO_TX_BASE_AD         0x0E
#define REG_FIFO_RX_BASE_AD         0x0F
#define REG_RX_NB_BYTES             0x13
#define REG_OPMODE                  0x01
#define REG_FIFO_RX_CURRENT_ADDR    0x10
#define REG_IRQ_FLAGS               0x12
#define REG_DIO_MAPPING_1           0x40
#define REG_DIO_MAPPING_2           0x41
#define REG_MODEM_CONFIG            0x1D
#define REG_MODEM_CONFIG2           0x1E
#define REG_MODEM_CONFIG3           0x26
#define REG_SYMB_TIMEOUT_LSB  		0x1F
#define REG_PKT_SNR_VALUE			0x19
#define REG_PAYLOAD_LENGTH          0x22
#define REG_IRQ_FLAGS_MASK          0x11
#define REG_MAX_PAYLOAD_LENGTH 		0x23
#define REG_HOP_PERIOD              0x24
#define REG_SYNC_WORD				0x39
#define REG_VERSION	  				0x42

#define SX72_MODE_RX_CONTINUOS      0x85
#define SX72_MODE_TX                0x83
#define SX72_MODE_SLEEP             0x80
#define SX72_MODE_STANDBY           0x81

#define RH_RF95_LONG_RANGE_MODE	    0x80
#define RH_RF95_MODE_SLEEP	    0x01

#define PAYLOAD_LENGTH              0x40

// LOW NOISE AMPLIFIER
#define REG_LNA                     0x0C
#define LNA_MAX_GAIN                0x23
#define LNA_OFF_GAIN                0x00
#define LNA_LOW_GAIN		    	0x20

// CONF REG
#define REG1                        0x0A
#define REG2                        0x84

#define SX72_MC2_FSK                0x00
#define SX72_MC2_SF7                0x70
#define SX72_MC2_SF8                0x80
#define SX72_MC2_SF9                0x90
#define SX72_MC2_SF10               0xA0
#define SX72_MC2_SF11               0xB0
#define SX72_MC2_SF12               0xC0

#define SX72_MC1_LOW_DATA_RATE_OPTIMIZE  0x01 // mandated for SF11 and SF12

// FRF
#define        REG_FRF_MSB              0x06
#define        REG_FRF_MID              0x07
#define        REG_FRF_LSB              0x08

#define        FRF_MSB                  0xD9 // 868.1 Mhz
#define        FRF_MID                  0x06
#define        FRF_LSB                  0x66

#define BUFLEN 2048  //Max length of buffer

#define PROTOCOL_VERSION  1
#define PKT_PUSH_DATA 0
#define PKT_PUSH_ACK  1
#define PKT_PULL_DATA 2
#define PKT_PULL_RESP 3
#define PKT_PULL_ACK  4

#define TX_BUFF_SIZE  2048
#define STATUS_SIZE	  1024

//#######################################################################################

void die(const char *s)
{
    
    exit(1);
}

void selectreceiver()
{
    digitalWrite(ssPin, LOW);
}

void unselectreceiver()
{
    digitalWrite(ssPin, HIGH);
}


uint8_t readRegister(uint8_t addr)
{
    unsigned char spibuf[2];

    selectreceiver();
    spibuf[0] = addr & 0x7F;
    spibuf[1] = 0x00;
    wiringPiSPIDataRW(CHANNEL, spibuf, 2);
    unselectreceiver();

    return spibuf[1];
}


void writeRegister(uint8_t addr, uint8_t value)
{
    unsigned char spibuf[2];

    spibuf[0] = addr | 0x80;
    spibuf[1] = value;
    selectreceiver();
    wiringPiSPIDataRW(CHANNEL, spibuf, 2);

    unselectreceiver();
}


void SetupLoRa()
{
    	digitalWrite(RST, HIGH);
    	delay(100);
    	digitalWrite(RST, LOW);
    	delay(100);

	printf("SX1276 detected, starting.\n");
        
	// sx1276?
        digitalWrite(RST, LOW);
        delay(100);
        digitalWrite(RST, HIGH);
        delay(100);
        uint8_t version = readRegister(REG_VERSION);
        if (version == 0x12) {
            // sx1276
            printf("SX1276 detected, starting.\n");
        } else {
            printf("Unrecognized transceiver.\n");
            //printf("Version: 0x%x\n",version);
            exit(1);
        	}
    	

	// Set Continous Sleep Mode
   	writeRegister(REG_OPMODE, RH_RF95_LONG_RANGE_MODE);
}



int main (void){
	printf("Start main function.");
	wiringPiSetup();
	pinMode(ssPin, OUTPUT);
    	pinMode(dio0, INPUT);
    	pinMode(RST, OUTPUT);

	//int fd = 
    	wiringPiSPISetup(CHANNEL, 500000);
    	//cout << "Init result: " << fd << endl;

	SetupLoRa();	
	printf("Start WiringPi function.\n");
	
	//registers I want to read
	//uint8_t registers[] = {0x01,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x42};
	uint8_t registers[] = {REG_OPMODE, REG_FIFO};

	int i;
	for(i=0; i < sizeof(registers);i++){
		printf("%X", registers[i]);
		printf("%s",(char *)(": ") );
		printf("%X", readRegister(registers[i]) );
	}
	
	//while(1);
 

	return 0;
}