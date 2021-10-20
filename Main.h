/* 
 * File:   Main.h
 * Author: Tejas
 *
 * Created on 13 October, 2021, 5:09 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#include <xc.h>
#include "clcd.h"
#include "digital_keypad.h"
#include "timers.h"

#define WASHING_PROGRAM_DISPLAY         0x01
#define WASHING_PROGRAM_DISPLAY_RESET   0x10
#define RESET_NOTHING                   0x00
#define WATER_LEVEL                     0x02
#define WATER_LEVEL_RESET               0x11
#define START_STOP_SCREEN               0x03
#define START_PROGRAM                   0x04
#define START_PROGRAM_RESET             0x12

#define FAN                             RC2
#define FAN_DDR                         TRISC2
#define ON                              1
#define OFF                             0
#define BUZZER RC1
#define BUZZER_DDR TRISC1
#define PAUSE 0x05
#define DC_MOTOR RA0
#define DC_MOTOR_DDR TRISA
void power_on_screen(void);
void clear_screen(void);
void washing_programs_display(unsigned char);
void water_level_display(unsigned char);
void set_time_for_program(void);
void run_program(unsigned char key);
void door_status_check(void);
#endif	/* MAIN_H */

