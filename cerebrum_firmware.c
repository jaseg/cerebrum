#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "uart.h"

void setup(void);
void loop(void);

int main(void){
    setup();
    for(;;) loop();
}

void setup(){
    uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(115201, F_CPU));
    //software PWM init FIXME
    DDRH |= 0x20;
    PORTH |= 0x20;
    sei();
}

//this scary construct is in place to make the compiler happy. if you know a better way, feel free to improve.
uint8_t _frameBuffer[] = {0,0,0,0};
uint8_t _secondFrameBuffer[] = {0,0,0,0};
uint8_t* frameBuffer = _frameBuffer;
uint8_t* secondFrameBuffer = _secondFrameBuffer;
char nbuf;
int state = 0;
uint8_t switch_last_state = 0;
int switch_debounce_timeout = 0;
uint8_t mcnt = 0;
uint8_t ccnt = 0;

uint8_t pwm_cycle = 0;
uint8_t pwm_val[4];

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

int parseHex(char* buf){
    int result = 0;
    int len = 2;
    for(int i=0; i<len; i++){
        char c = buf[len-i];
        int v = 0;
        if(c>='0' && c<='9'){
            v=(c-'0');
        }else if(c>='a' && c<= 'f'){
            v=(c-'a'+10);
        }else if(c>='A' && c<= 'F'){
            v=(c-'A'+10);
        }
        result |= v<<(4*i);
    }
    return result;
}

void loop(){ //one frame
    static uint8_t escape_state = 0;
    uint16_t receive_status = 1;
    //primitive somewhat messy state machine of the uart interface
    do{ //Always empty the receive buffer since there are _delay_xxs in the following code and thus this might not run all that often.
        receive_status = uart_getc();
        char c = receive_status&0xFF;
        receive_status &= 0xFF00;
        //eats three commands: 's' (0x73) led value                     sets led number [led] to [value]
        //                     'b' (0x62) buffer buffer buffer buffer   sets the whole frame buffer
        //                     'a' (0x61) meter value                   sets analog meter number [meter] to [value]
        if(!receive_state){
            if(!escape_state){
                receive_state |= 0x02;
                if(c == '\\'){
                    escape_state = 1;
                }else if(c == '\n'){
                    state = 0;
                }
            }else{
                receive_state = 0;
                escape_state = 0;
                switch(c){
                    case '\\':
                        break;
                    case 'n':
                        c = '\n';
                        break;
                }
            }
        }
        if(!receive_status){
            switch(state){
                case 0: //Do not assume anything about the variables used
                    switch(c){
                        case 's':
                            state = 2;
                            break;
                        case 'b':
                            nbuf = 0;
                            state = 4;
                            break;
                        case 'a':
                            state = 5;
                            nbuf = 0;
                            break;
                    }
                    break;
                case 2:
                    nbuf=c;
                    state = 3;
                    break;
                case 3:
                    setLED(nbuf, c);
                    state = 0;
                    break;
                case 4:
                    secondFrameBuffer[(uint8_t) nbuf] = c;
                    nbuf++;
                    if(nbuf == 4){
                        swapBuffers();
                        state = 0;
                    }
                    break;
                case 5:
                    nbuf = c;
                    state = 6;
                    if(nbuf >= 4) //prevent array overflows
                        nbuf = 0;
                    break;
                case 6:
                    pwm_val[(uint8_t) nbuf] = c;
                    state = 0;
            }
        }
    }while(!receive_status);
    for(int i=0; i<8; i++){
        uint8_t Q = 0x80>>i; //select the currently active "row" of the matrix. On the protoboards I make, this actually corresponds to physical traces.
        //de-packing of frame data: data is packed like this: [11111117][22222266][33333555][4444----]
        if(!(i&4))
            Q |= frameBuffer[i&3] >> i;
        else
            Q |= frameBuffer[i&3] & (0xFF << (i&3));
        //equivalent to the code above: <== revisiting this, I'm not quite sure about this anymore.
        //Q |= frameBuffer[i&3]>>(i&(0x30>>(i&4))) & (0xFF<<(i&(0x30>>(!i&4))));
        PORTD &= 0x0F;
        PORTD |= Q&0xF0;
        PORTB &= 0xF0;
        PORTB |= Q&0x0F;
        _delay_ms(1); //Should result in >100Hz refresh rate
    }
}
