
#include "pwm.h"
#include "led.h"
#include "util.h"
#ifdef HAS_PWM_SUPPORT

void pwm_loop(){
#ifdef PWM_ANIMATE
    pwm_animate();
#endif//PWM_ANIMATE
}

void pwm_setup(){
    //s/w "BCM"(<== "Binary Code Modulation") timer setup
    TCCR0A |= _BV(WGM01);
    TCCR0B |= _BV(CS02);
    TIMSK0 |= _BV(OCIE0A);
    OCR0A = 1;
    pwm_output_setup();
}

ISR(TIMER0_COMPA_vect){
    pwm_unset_outputs();
    pwm_cycle<<=1;
    if(!pwm_cycle){
        pwm_cycle = 1;
    }
    uint8_t Q = 0;
    Q |= (pwm_val[0] & pwm_cycle)?1:0;
    Q |= (pwm_val[1] & pwm_cycle)?2:0;
    Q |= (pwm_val[2] & pwm_cycle)?4:0;
    Q |= (pwm_val[3] & pwm_cycle)?8:0;
    Q |= (pwm_val[4] & pwm_cycle)?16:0;
    Q |= (pwm_val[5] & pwm_cycle)?32:0;
    Q |= (pwm_val[6] & pwm_cycle)?64:0;
    Q |= (pwm_val[7] & pwm_cycle)?128:0;
    pwm_set_outputs(Q);
    TCNT0 = 0;
    OCR0A = pwm_cycle;
}

uint8_t pwm_cycle = 1;
uint8_t pwm_val[8];

rgb_value_t hsv_to_rgb(hsv_value_t k){
    int f;
    long p, q, t;
    rgb_value_t ret = {0, 0, 0};

    if(k.s==0){
        ret.r = ret.g = ret.b = k.v;
        return ret;
    }

    f = ((k.h%60)*255)/60;
    k.h /= 60;

    p = (k.v * (256 - k.s))/256;
    q = (k.v * ( 256 - (k.s * f)/256 ))/256;
    t = (k.v * ( 256 - (k.s * ( 256 - f ))/256))/256;

    switch(k.h){
        case 0:
            ret.r = k.v;
            ret.g = t;
            ret.b = p;
            break;
        case 1:
            ret.r = q;
            ret.g = k.v;
            ret.b = p;
            break;
        case 2:
            ret.r = p;
            ret.g = k.v;
            ret.b = t;
            break;
        case 3:
            ret.r = p;
            ret.g = q;
            ret.b = k.v;
            break;
        case 4:
            ret.r = t;
            ret.g = p;
            ret.b = k.v;
            break;
        default:
            ret.r = k.v;
            ret.g = p;
            ret.b = q;
            break;
    }
    return ret;
}
/*
    //The following algorithm is loosely based on http://en.wikipedia.org/wiki/HSL_and_HSV#Converting_to_RGB
    //AND BROKEN
    uint16_t c_ = (uint16_t)k.v * (uint16_t)k.s;
    uint8_t c = c_>>8;
    uint8_t m = k.v - c;
    rgb_value_t ret = {0, 0, 0};
    uint8_t branch;
    uint8_t h_;
    if(k.h<128){
        if(k.h<85){
            if(k.h<43){
                h_ = k.h;
                branch = 0;
            }else{
                h_ = 43-k.h;
                branch = 1;
            }
        }else{
            h_ = k.h-128;
            branch = 2;
        }
    }else{
        if(k.h<171){
            h_ = 171-k.h;
            branch = 3;
        }else{
            if(k.h<213){
                h_ = k.h-171;
                branch = 4;
            }else{
                h_ = 255-k.h;
                branch = 5;
            }
        }
    }
    uint16_t x_ = c * (uint16_t)h_;
    uint8_t x = x_>>8;
    switch(branch){
        case 0:
            ret.r += c;
            ret.g += x;
            break;
        case 1:
            ret.r += x;
            ret.g += c;
            break;
        case 2:
            ret.g += c;
            ret.b += x;
            break;
        case 3:
            ret.g += x;
            ret.b += c;
            break;
        case 4:
            ret.r += x;
            ret.b += c;
            break;
        case 5:
            ret.r += c;
            ret.b += x;
            break;
    }
    return ret;
}
*/

#else//HAS_PWM_SUPPORT

void pwm_setup(){}
void pwm_loop(){}

#endif//HAS_PWM_SUPPORT
