#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
<<<<<<< HEAD
#include "uart.h"
=======
>>>>>>> feeeea7... Converted to plain c. Added a makefile.

void setup(void);
void loop(void);

int main(void){
    setup();
    for(;;) loop();
}

void setup(){
  DDRD = 0xFC;
  DDRB = 0x0F;
<<<<<<< HEAD
  uart_init(UART_BAUD_SELECT(115200, F_CPU));
=======
  //usart setup FIXME
>>>>>>> feeeea7... Converted to plain c. Added a makefile.
  //software PWM init FIXME
  sei();
}

//this scary construct is in place to make the compiler happy. if you know a better way, feel free to improve.
uint8_t _frameBuffer[4];
uint8_t _secondFrameBuffer[4];
uint8_t* frameBuffer = _frameBuffer;
uint8_t* secondFrameBuffer = _secondFrameBuffer;
char nbuf;
int state = 0;
int mode = 1;
<<<<<<< HEAD
uint8_t mcnt = 0;
=======
uint8_t mcnt;
>>>>>>> feeeea7... Converted to plain c. Added a makefile.

uint8_t pwm_cycle = 0;
uint8_t pwm_val[4];

void swapBuffers(void){
  uint8_t* tmp = frameBuffer;
  frameBuffer = secondFrameBuffer;
  secondFrameBuffer = tmp;
}

void setLED(int num, int val){
  if(num<32)
    frameBuffer[num>>3] &= 1<<(num&7);
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
  char c = 0;
<<<<<<< HEAD
  while(0){ //FIXME
/*
=======
  while((c = 42) != -1){ //FIXME
>>>>>>> feeeea7... Converted to plain c. Added a makefile.
    //primitive somewhat messy state machine.
    //eats two commands: 0x73 led value                     sets led number led to value
    //                   0x62 buffer buffer buffer buffer   sets the whole frame buffer
    switch(state){
      case 0:
        switch(c){
          case 's':
            state = 1;
            break;
          case 'b':
            nbuf = 0;
            state = 4;
            break;
          case 'r':
            mode = 1;
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
        mode = 0;
        state = 0;
        break;
      case 4:
        frameBuffer[(uint8_t) nbuf] = c;
        nbuf++;
        if(nbuf == 4){
          swapBuffers();
          mode = 0;
          state = 0;
        }
        break;
      case 5:
        nbuf = c;
        state = 6;
        if(nbuf >= 4)
          nbuf = 0;
        else
          state = 0;
        break;
      case 6:
        pwm_val[(uint8_t) nbuf] = c;
        state = 0;
    }
<<<<<<< HEAD
*/
  }
	while(1){
		PORTD = mcnt&0xF0;
		PORTB = mcnt&0x0F;
		mcnt++;
		_delay_ms(200);
	}
=======
  }
>>>>>>> feeeea7... Converted to plain c. Added a makefile.
  if(mode == 0){
    for(int i=0; i<8; i++){
      uint8_t Q = 0x80>>i;
      if(!(i&4))
        Q |= frameBuffer[i&3] >> i;
      else
        Q |= frameBuffer[i&3] & (0xFF << (i&3));
      //equivalent to the code above:
      //Q |= frameBuffer[i&3]>>(i&(0x30>>(i&4))) & (0xFF<<(i&(0x30>>(!i&4))));
      PORTD &= 0x0F;
      PORTD |= Q&0xF0;
      PORTB &= 0xF0;
      PORTB |= Q&0x0F;
      /*
      PORTD = 1<<i;
      if(i<4)
        PORTD |= frameBuffer[i&3]&~(0xFF<<i);
      else
        PORTD |= frameBuffer[3^i]<<i;
      */
      _delay_ms(1);
    }
  }else if(mode == 1){
    uint8_t Q = mcnt++;
    PORTD = Q&0xF0;
    PORTB = Q&0x0F;
    if(mcnt&0x08){
      PORTD &= 0xF3;
<<<<<<< HEAD
      PORTD |= 0x04;
    }else{
      PORTD &= 0xF3;
      PORTD |= 0x08;
=======
      PORTD |= 0xF4;
    }else{
      PORTD &= 0xF3;
      PORTD |= 0xF8;
>>>>>>> feeeea7... Converted to plain c. Added a makefile.
    }
    _delay_ms(200);
  }
}

//Software PWM margyck for the analog meters
ISR(TIMER2_OVF_vect){
  if(pwm_cycle == 0){
    PORTD &= 0xF3;
    PORTB &= 0xCF;
  }else{
    if(pwm_val[0] >= pwm_cycle)
      PORTB |= 0x04;
    if(pwm_val[1] >= pwm_cycle)
      PORTB |= 0x08;
    if(pwm_val[2] >= pwm_cycle)
      PORTB |= 0x10;
    if(pwm_val[3] > pwm_cycle)
      PORTB |= 0x20;
  }
  pwm_cycle ++;
}
