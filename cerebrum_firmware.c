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
    //Stuff for the SWITCH
    //DDRH |= 0x20;
    //PORTH |= 0x20;
    //s/w PWM
    TCCR0B |= _BV(CS00);
    TIMSK0 |= _BV(TOIE0);
    DDRL = 0xFF; //PWM outputs
    //The DDRs of the led matrix outputs are set in the mux loop.
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

#define PWM_COUNT 8
uint8_t pwm_cycle = 0;
uint8_t pwm_val[PWM_COUNT];
#define INPUT_COUNT 12
uint8_t debounce_timeouts[INPUT_COUNT];
uint8_t switch_states[INPUT_COUNT];

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
    uint16_t receive_state = 1;
    //primitive somewhat messy state machine of the uart interface
    do{ //Always empty the receive buffer since there are _delay_xxs in the following code and thus this might not run all that often.
        receive_state = uart_getc();
        char c = receive_state&0xFF;
        receive_state &= 0xFF00;
        //escape code format:
        // \\   - backslash
        // \n   - newline
        // \[x] - x
        //eats three commands: 's' (0x73) led value                     sets led number [led] to [value]
        //                     'b' (0x62) buffer buffer buffer buffer   sets the whole frame buffer
        //                     'a' (0x61) meter value                   sets analog meter number [meter] to [value]
        //this device will utter a "'c' (0x63) num state" when switch [num] changes state to [state]
        //commands are terminated by \n
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
        if(!receive_state){
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
                    if(c > PWM_COUNT)
                        c = 0;
                    nbuf = c;
                    state = 6;
                    break;
                case 6:
                    pwm_val[(uint8_t) nbuf] = c;
                    state = 0;
            }
        }
    }while(!receive_state);
    for(int i=0; i<8; i++){
        uint8_t Q = 0x80>>i; //select the currently active "row" of the matrix. On the protoboards I make, this actually corresponds to physical traces.
        uint8_t DDRQ = 0xFF>>i; //This is supposed to be an optimization. It is untested and will probably not work.
        //de-packing of frame data: data is packed like this: [11111117][22222266][33333555][4444----]
        if(!(i&4))
            Q |= frameBuffer[i&3] >> i;
        else
            Q |= frameBuffer[i&3] & (0xFF << (i&3));
        DDRC = DDRQ;
        PORTC = Q;
        //PORTD &= 0x0F;
        //PORTD |= Q&0xF0;
        //PORTB &= 0xF0;
        //PORTB |= Q&0x0F;
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
        _delay_ms(1); //Should result in >100Hz refresh rate
    }
    switch_states[0] |= !!(PINH&0x02);
    for(int i=0; i<INPUT_COUNT; i++){
        debounce_timeouts[i]--;
        //A #define for the debounce time would be great
        if(debounce_timeouts[i] == 0){
            uint8_t new_switch_state = switch_states[i]<<1;
            if(!(switch_states[i]^new_switch_state)){
                uart_putc('c');
                uart_putc(i);
                uart_putc(switch_states[i]&1);
                debounce_timeouts[i] = 0xFF;
                switch_states[i] = new_switch_state&3;
            }
        }
    }
}

//Called every 256 cpu clks (i.e should not get overly long)
ISR(TIMER0_OVF_vect){
    pwm_cycle++;
    //example code
    uint8_t Q = 0;
    //The following loop NEEDS to be unrolled.
    for(uint8_t i=0; i<8; i++){
        Q |= (pwm_val[i] > pwm_cycle)<<i;
    }
    PORTL = Q;
}
