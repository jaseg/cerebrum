
#include "led.h"
#include <util/delay.h>

//The DDRs of the led matrix outputs are set in the mux loop.
void led_setup(void){}

#ifdef HAS_LED_SUPPORT

void swapBuffers(void){
    uint8_t* tmp = frameBuffer;
    frameBuffer = secondFrameBuffer;
    secondFrameBuffer = tmp;
}

void setLED(int num, int val){
    if(num<32){
        frameBuffer[num>>3] &= ~(1<<(num&7));
        if(val)
            frameBuffer[num>>3] |= 1<<(num&7);
    }
}

void led_loop(){
    for(int i=0; i<8; i++){
        uint8_t Q = 0x80>>i; //select the currently active "row" of the matrix. On the protoboards I make, this actually corresponds to physical traces.
        //uint8_t DDRQ = 0xFF>>i; //This is supposed to be an optimization. It is untested and will probably not work.
        uint8_t DDRQ = 0xFF; //Just for testing: reactivating the old behavioor
        //unpacking of frame data: data is packed like this: [11111117][22222266][33333555][4444----]
        if(!(i&4))
            Q |= frameBuffer[i&3] >> (i+1);
        else
            Q |= frameBuffer[i&3] & (0xFF << ((i&3)+1));
        //FIXME this whole mapping shit should be done in h/w!!1!
        //FIXME this whole mapping is not even correct!!1!
        //FIXME IT DOES NOT WORK!!1!
        // Q&0x01 ==> PG5
        //     02 ==> PE3
        //     04 ==> PH3
        //     08 ==> PH4
        //     10 ==> PH5
        //     20 ==> PH6
        //     40 ==> PB4
        //     80 ==> PB5
        DDRG&=~(1<<5);
        DDRG|=(DDRQ&1)<<5;
        PORTG&=~(1<<5);
        PORTG|=(Q&1)<<5;
        DDRE&=~(1<<3);
        DDRE|=(DDRQ&2)<<2;
        PORTE&=~(1<<3);
        PORTE|=(Q&2)<<2;
        DDRH&=0xC3;
        DDRH|=(DDRQ&0x3C);
        PORTH&=0xC3;
        PORTH|=(Q&0x3C);
        DDRB&=0xCF;
        DDRB|=(DDRQ&0xC0)>>2;
        PORTB&=0xCF;
        PORTB|=(Q&0xC0)>>2;
        /* second channel skeleton
        Q = 0x80>>i;
        if(!(i&4))
            Q |= frameBuffer[4+(i&3)] >> i;
        else
            Q |= frameBuffer[4+(i&3)] & (0xFF << (i&3));
        DDRA = DDRQ;
        PORTA = Q;
        //PORTD &= 0x0F;
        //PORTD |= Q&0xF0;
        //PORTB &= 0xF0;
        //PORTB |= Q&0x0F;
        */
        _delay_ms(1);
    }
}

//this scary construct is in place to make the compiler happy. if you know a better way, feel free to improve.
uint8_t _frameBuffer[] = {0,0,0,0};
uint8_t _secondFrameBuffer[] = {0,0,0,0};
uint8_t* frameBuffer = _frameBuffer;
uint8_t* secondFrameBuffer = _secondFrameBuffer;

#endif//HAS_LED_SUPPORT
