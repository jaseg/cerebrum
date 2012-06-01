
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
    uint8_t row = 2; //selects the currently active "row" of the matrix. On the protoboards I make, this actually corresponds to physical traces.
    for(int i=0; i<7; i++){
        uint8_t DDRQ = frameBuffer[i];
        uint8_t Q = ~DDRQ;
        Q |= row;
        DDRQ |= row;
        row <<= 1;
        led_output_stuff1(Q, DDRQ);
        _delay_ms(1);
    }
}

//this scary construct is in place to make the compiler happy. if you know a better way, feel free to improve.
uint8_t _frameBuffer[] = {0,0,0,0,0,0,0};
uint8_t _secondFrameBuffer[] = {0,0,0,0,0,0,0};
uint8_t* frameBuffer = _frameBuffer;
uint8_t* secondFrameBuffer = _secondFrameBuffer;

#else//HAS_LED_SUPPORT

void led_loop(){}

#endif//HAS_LED_SUPPORT
