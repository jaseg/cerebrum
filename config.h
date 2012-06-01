
#ifndef __CONFIG_H__
#define __CONFIG_H__

//#define HAS_PWM_SUPPORT         1
#define HAS_R0KETBEAM_SUPPORT   1
#define HAS_INPUT_SUPPORT       1
#define HAS_LED_SUPPORT         1
#define HAS_7SEG_SUPPORT        1

//#define PWM_COUNT 5
#define INPUT_COUNT 1


#define L7SEG_CLK_DDR       DDRG
#define L7SEG_CLK_PORT      PORTG
#define L7SEG_CLK_PIN       5

#define L7SEG_DATA_DDR      DDRE
#define L7SEG_DATA_PORT     PORTE
#define L7SEG_DATA_PIN      3

#define L7SEG_DIGIT1_DDR    DDRE
#define L7SEG_DIGIT1_PORT   PORTE
#define L7SEG_DIGIT1_PIN    4

#define L7SEG_DIGIT2_DDR    DDRE
#define L7SEG_DIGIT2_PORT   PORTE
#define L7SEG_DIGIT2_PIN    5

void config_setup(void);

#endif//__CONFIG_H__
