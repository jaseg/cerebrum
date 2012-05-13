
#include <RF24.h>

#include "r0ketbeam.h"
#include "util.h"
#include "uart.h"

#include <avr/io.h>
#include <avr/pgmspace.h>

#include <string.h>

typedef struct {
    uint8_t length;
    uint8_t protocol;
    uint8_t flags;
    uint8_t strength;
    uint32_t sequence;
    uint32_t id;
    uint8_t button1;
    uint8_t button2;
    uint16_t crc;
} beacon_data_t;

typedef struct {
    uint8_t length;
    uint8_t protocol;
    uint32_t id;
    char name[8];
    uint16_t crc;
} beacon_name_t;

#if defined(PRINT_NORESPONSE)
int inc;
#endif

void r0ketbeam_setup(void)
{
    nrf24_begin();
    nrf24_setDataRate(RF24_2MBPS);
    nrf24_setChannel(81);
    nrf24_setAutoAck(0);
    nrf24_setPayloadSize(16);
    nrf24_setCRCLength(RF24_CRC_8);
    nrf24_openReadingPipe(1, 0x0102030201LL);
    nrf24_openWritingPipe(0x0102030201LL);
    uart_puts_p(PSTR("RF24 Carrier: "));
    uint8_t carrier = nrf24_testCarrier();
    uart_putc(carrier?'Y':'N');
    uart_putc('\n');
    nrf24_startListening();
}

uint16_t flip16(uint16_t number)
{
    return number>>8|number<<8;
}

uint16_t crc16(uint8_t* buf, int len)
{
    /* code from r0ket/firmware/basic/crc.c */
    uint16_t crc = 0xffff;
    for (int i=0; i < len; i++)
    {
        crc = (unsigned char)(crc >> 8) | (crc << 8);
        crc ^= buf[i];
        crc ^= (unsigned char)(crc & 0xff) >> 4;
        crc ^= (crc << 8) << 4;
        crc ^= ((crc & 0xff) << 4) << 1;
    }
    return crc;
}

//CAUTION!!1! "id" is little-endian
void sendNick(uint32_t id, char* name)
{
    beacon_name_t pkt;
    pkt.length = 16;
    pkt.protocol = 0x23;
    pkt.id = id;
    memset(&pkt.name, 0, sizeof(pkt.name));
    strncpy((char*)&pkt.name, name, min(8,strlen(name)));
    pkt.crc = flip16(crc16((uint8_t*)&pkt, sizeof(pkt)-2));

    nrf24_stopListening();
    nrf24_openWritingPipe(0x0102030201LL);
    nrf24_write(&pkt, sizeof(pkt));
    nrf24_startListening();
}

//CAUTION!!1! "id" and "sequence" are little-endian
void sendDummyPacket(uint32_t id, uint32_t sequence)
{
    beacon_data_t pkt;
    pkt.length = 16;
    pkt.protocol = 0x17;
    pkt.id = id;
    pkt.sequence = sequence;
    pkt.flags = 0x00; 
    pkt.button1 = 0xFF;
    pkt.button2 = 0xFF;

    pkt.crc = flip16(crc16((uint8_t*)&pkt, sizeof(pkt)-2));

    nrf24_stopListening();
    nrf24_write((uint8_t*)&pkt, sizeof(pkt));
    nrf24_startListening();
}

int last_sent_packet = 0;
char* nick = "c_leuse";

void r0ketbeam_loop(void)
{
    //sorry, since we are not arduino, we do not yet have a millis() function.
    //int delta = millis() - last_sent_packet;
    /*
       if (delta > 1234)
       {
       last_sent_packet = millis(); //FIXME little endianness, anyone?
    //printf("[%i] ", delta);

    uint32_t id = 0x78563412UL;

    char myNick[9];
    sprintf((char*)&myNick, "*[%u]", (unsigned int)millis());
    myNick[sizeof(myNick)-1] = '\0';

    Serial.print("# Sending nick '");
    Serial.print(myNick);
    Serial.print("' as 0x");
    Serial.print(id, HEX);
    Serial.println(" ...");
    sendNick(id, (char*)&myNick);

    //sendDummyPacket(id, last_sent_packet);
    }
    */

    if (nrf24_available()){
        beacon_data_t buf;
        nrf24_read(&buf, sizeof(buf));

#ifdef PRINT_NORESPONSE
        if (inc > 0){
            uart_putc('\n');
        }
        inc = 0;
#endif
        uart_puts_p(PSTR("RF24 RECV "));
        uart_puthex_flip_32(buf.id);
        uart_putc(':');
        uart_putc(' ');
        uart_puthex_flip_32(buf.sequence);
        uart_putc(' ');
        uart_putc('(');
        uart_putdec(buf.strength);
        uart_putc(')');
        uart_putc(' ');
        uart_puthex(buf.protocol);
        uart_putc('\n');

        uint16_t crc = flip16(crc16((uint8_t*)&buf, sizeof(buf)-2));
        if (crc != buf.crc){
            uart_puts_p(PSTR("RF24 ! CRC mismatch (expected/got): "));
            uart_puthex_16(crc);
            uart_putc('/');
            uart_puthex_16(buf.crc);
            uart_putc('\n');
        }
    }else{
#ifdef PRINT_NORESPONSE
        if (inc++ == 80) {
            uart_putc('\n');
            inc = 0;
        }
        uart_putc('.');
        //_delay_ms(100);
#endif
    }
}


