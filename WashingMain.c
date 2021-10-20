/*
 * File:   WashingMain.c
 * Author: Tejas 
 *
 * Created on 13 October, 2021, 4:37 PM
 */
#include "Main.h"
#pragma config WDTE = OFF                                                       // Watchdog Timer Enable bit (WDT enabled)

int wash_time, rinse_time, spin_time;

unsigned char reset_mode, program_no = 0,water_level_idex = 0;
char *washing_prog[] = {"Daily","Heavy", "Delicates", "Whites", "StainWash", 
                        "EcoCottons","Woolen","BedSheets","Rinse+Dry",
                        "DRY Only", "WASH Only", "AQUA Store"};

char *water_level_arr[] = {"Auto", "Low", "Medium", "High", "MAX"};
unsigned char operation_mode = WASHING_PROGRAM_DISPLAY;
unsigned char sec, min;

static void init_config(void){
    init_clcd();                                                                //Initialization of CLCD
    init_digital_keypad(); 
    init_timer2();
    FAN_DDR = 0;
    BUZZER_DDR = 0;
    PEIE = 1;
    GIE = 1;
    ADCON1 = 0x06;
    DC_MOTOR_DDR = 0;
    DC_MOTOR = OFF;
    //Initialization of Digital Keypad
}
void main(void) 
{
    unsigned char key = 0x3F, key_copy;
    
    reset_mode = WASHING_PROGRAM_DISPLAY_RESET;
    init_config();
    
    clcd_print(" PRESS KEY5 TO  ", LINE1(0));
    clcd_print("   POWER ON     ", LINE2(0));
    clcd_print("WASHING MACHINE ", LINE4(0));
    key = read_digital_keypad(STATE);
    while (key != SW5)
    {
        key = read_digital_keypad(STATE);
        for(int j = 3000; j--;);  
    }
    
    clear_screen();
    power_on_screen();
    
    while(1)
    {
        key = read_digital_keypad(STATE);
        for(int j = 3000; j--;);
        if(key == LSW4 && operation_mode == WASHING_PROGRAM_DISPLAY)
        {
            //Display Water Level screen
            operation_mode = WATER_LEVEL;
            reset_mode = WATER_LEVEL_RESET;
        }
        else if(key == LSW4 && operation_mode == WATER_LEVEL)
        {
            set_time_for_program();
            clear_screen();
            clcd_print("PRESS SWITCH",LINE1(0));
            clcd_print("----------------",LINE2(0));
            clcd_print("SW5 - START",LINE3(0));
            clcd_print("SW6 - STOP",LINE4(0));
            operation_mode = START_STOP_SCREEN;
        }
        else if(key == SW4 && operation_mode == PAUSE)
        {
            TMR2ON = 1;
            FAN = ON;
            DC_MOTOR = ON;
            operation_mode = START_PROGRAM;
        }
        switch(operation_mode)
        {
            case WASHING_PROGRAM_DISPLAY:
                washing_programs_display(key);
                break;
                
            case WATER_LEVEL:
                water_level_display(key);
                break;
                
            case START_STOP_SCREEN:
                if(key == SW5)
                {
                    operation_mode = START_PROGRAM;
                    reset_mode = START_PROGRAM_RESET;
                    continue;
                }
                else if(key == SW6)
                {
                    operation_mode = WASHING_PROGRAM_DISPLAY;
                    reset_mode = WASHING_PROGRAM_DISPLAY_RESET;
                    continue;
                }  
                break;
            case START_PROGRAM:
                run_program(key);
                break;
        }
        reset_mode = RESET_NOTHING;
    }
    return;
}
void clear_screen(void)
{
    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
    __delay_us(100);
}

void power_on_screen(void){
    for(unsigned char i = 0; i < 16; i++)
        clcd_putch(BLOCK, LINE1(i));

    clcd_print("  Powering ON   ", LINE2(0));
    clcd_print("Washing Machine ", LINE3(0));
    
    for(unsigned char i = 0; i < 16; i++)
        clcd_putch(BLOCK, LINE4(i));
    
    __delay_ms(2000);
}

void washing_programs_display(unsigned char key){
    if(reset_mode == WASHING_PROGRAM_DISPLAY_RESET){
        clear_screen();
        program_no = 0;
    }
    if(key == SW4)
    {
        program_no++;
        
        if(program_no == 12)
            program_no = 0;
        
        clear_screen();
    }
    clcd_print("WASHING-PROGRAMS", LINE1(0));
    clcd_print("->", LINE2(0));
    if(program_no <= 9)
    {
        clcd_print(washing_prog[program_no],LINE2(2));
        clcd_print(washing_prog[program_no + 1],LINE3(2));
        clcd_print(washing_prog[program_no + 2],LINE4(2));
    }
    else if(program_no == 10)
    {
        clcd_print(washing_prog[program_no],LINE2(2));
        clcd_print(washing_prog[program_no + 1],LINE3(2));
        clcd_print(washing_prog[0],LINE4(2));
    }
    else if(program_no == 11)
    {
        clcd_print(washing_prog[program_no],LINE2(2));
        clcd_print(washing_prog[0],LINE3(2));
        clcd_print(washing_prog[1],LINE4(2));
    }
}

void water_level_display(unsigned char key)
{
    if(reset_mode == WATER_LEVEL_RESET){
        water_level_idex = 0;
        clear_screen();
    }
    
    if(key == SW4)
    {
        water_level_idex++;
        
        if(water_level_idex == 5)
            water_level_idex = 0;
        
        clear_screen();
    }
    clcd_print("WATER LEVEL", LINE1(0));
    clcd_print("->", LINE2(0));
    
    if(water_level_idex <= 2)
    {
        clcd_print(water_level_arr[water_level_idex],LINE2(2));
        clcd_print(water_level_arr[water_level_idex + 1],LINE3(2));
        clcd_print(water_level_arr[water_level_idex + 2],LINE4(2));
    }
    else if(water_level_idex == 3)
    {
        clcd_print(water_level_arr[water_level_idex],LINE2(2));
        clcd_print(water_level_arr[water_level_idex + 1],LINE3(2));
        clcd_print(water_level_arr[0],LINE4(2));
    }
    else if(water_level_idex == 4)
    {
        clcd_print(water_level_arr[water_level_idex],LINE2(2));
        clcd_print(water_level_arr[0],LINE3(2));
        clcd_print(water_level_arr[1],LINE4(2));
    }
}

void run_program(unsigned char key)
{
    door_status_check();
    static int total_time, time;
        if(reset_mode == START_PROGRAM_RESET)
        {
            clear_screen();
            clcd_print("PROG: ",LINE1(0));
            clcd_print(washing_prog[program_no],LINE1(6));
            clcd_print("TIME:",LINE2(0));
            //mm:ss
            //min--> 12
            clcd_putch((min/10)+'0',LINE2(6));
            clcd_putch((min%10)+'0',LINE2(7));
            clcd_putch(':',LINE2(8));
            clcd_putch((sec/10)+'0',LINE2(9));
            clcd_putch((sec%10)+'0',LINE2(10));
            clcd_print("(MM:SS)",LINE3(5));
            __delay_ms(2000);//2sec
            
            //display screen
            clear_screen();
            clcd_print("Function -",LINE1(0));
            clcd_print("TIME",LINE2(0));
            clcd_print("4-START  6-PAUSE", LINE4(0));
            
            //sec= 60 , min =0
            time = total_time = (min*60+sec);
            wash_time = (int) total_time *0.46;
            rinse_time =(int) total_time *0.12;
            spin_time = total_time - wash_time - rinse_time;

            //start the timer
            TMR2ON = ON;
            //start the fan
            FAN = ON;
            DC_MOTOR = ON;
        }
    if(key == SW6)//PAUSE THE MACHINE
    {
        TMR2ON = OFF; //TURN OFF TIMER
        FAN = OFF;
        DC_MOTOR = OFF;
        operation_mode = PAUSE;
    }
    
    total_time = (min*60+sec);
    if(program_no <= 7)
    {
    if(total_time >= (time - wash_time)) 
        clcd_print("WASH  ",LINE1(11));

    else if(total_time >= (time - wash_time - rinse_time))
        clcd_print("RINSE",LINE1(11));
  
    else
        clcd_print("SPIN   ",LINE1(11));
    }
    else if(program_no == 8)  //rinse+dry
    {
        if(total_time >= (time - (0.40 * time)) )
          clcd_print("RINSE",LINE1(11));  
  
        else
           clcd_print("SPIN  ",LINE1(11));
    } 
    else if(program_no == 9) //dry only
        clcd_print("RINSE",LINE1(11)); //aqua store

    else
        clcd_print("WASH   ",LINE1(11));
    
    clcd_putch((min/10)+'0',LINE2(6));
    clcd_putch((min%10)+'0',LINE2(7));
    clcd_putch(':',LINE2(8));
    clcd_putch((sec/10)+'0',LINE2(9));
    clcd_putch((sec%10)+'0',LINE2(10));
            
    if(sec == 0 && min == 0)
    {
        //STOP THE TIMER
        TMR2ON = OFF;
        //TURN OFF FAN
        FAN =OFF;
        DC_MOTOR = OFF;
        //TURN ON BUZZER
         BUZZER = ON;
                
        clear_screen();
        clcd_print(" DONE WASHING!!", LINE2(0));
        clcd_print(" Remove Clothes", LINE3(0));
        __delay_ms(2000);
        BUZZER = OFF;
                
        operation_mode = WASHING_PROGRAM_DISPLAY;
        reset_mode = WASHING_PROGRAM_DISPLAY_RESET;
        clear_screen();
    }
}

void set_time_for_program(void)
{
    switch(program_no)
    {
        case 0:
            switch(water_level_idex)
            {
                case 0: 
                case 1: sec = 33; min = 0; break;
                case 2: sec = 41; min = 0; break;
                case 3: sec = 45; min = 0; break;
                case 4: sec = 45; min = 0; break;
            }
            break;
        case 1:
            switch(water_level_idex)
            {
                case 0: 
                case 1: sec = 43; min = 0; break;
                case 2: sec = 50; min = 0; break;
                case 3: sec = 57; min = 0; break;
                case 4: sec = 57; min = 0; break;
            }
            break;
        case 2:
            switch(water_level_idex)
            {
                case 0: 
                case 1: sec = 20; min = 0; break;
                case 2: sec = 26; min = 0; break;
                case 3: sec = 31; min = 0; break;
                case 4: sec = 31; min = 0; break;
            }
            break;
        case 3:
            sec = 16; min = 1; break;
        case 4:
            sec = 36; min = 1; break;
        case 5:
            switch(water_level_idex)
            {
                case 0: 
                case 1: sec = 20; min = 0; break;
                case 2: sec = 31; min = 0; break;
                case 3: sec = 36; min = 0; break;
                case 4: sec = 36; min = 0; break;
            }
            break;
        case 6:
            sec = 29; min = 0; break;
        case 7:
            switch(water_level_idex)
            {
                case 0: 
                case 1: sec = 46; min = 0; break;
                case 2: sec = 53; min = 0; break;
                case 3: sec = 0;  min = 1; break;
                case 4: sec = 0;  min = 1; break;
            }
            break;
        case 8:
            switch(water_level_idex)
            {
                case 0: 
                case 1: sec = 20; min = 0; break;
                case 2: sec = 20; min = 0; break;
                case 3: sec = 20; min = 0; break;
                case 4: sec = 20; min = 0; break;
            }
            break;
        case 9:
            sec = 6; min = 0; break;
        case 10:
            switch(water_level_idex)
            {
                case 0: 
                case 1: sec = 16; min = 0; break;
                case 2: sec = 21; min = 0; break;
                case 3: sec = 26; min = 0; break;
                case 4: sec = 26; min = 0; break;
            }
            break;
        case 11:
            sec = 50; min = 0; break;
    }
}

void door_status_check(void)
{
    if(RB0 == 0) //DOOR OPEN CONDITION
    {
        //TURN ON BUZZER
        BUZZER = ON;
        FAN = OFF;
        DC_MOTOR = OFF;
        TMR2ON = OFF;
        
        clear_screen();
        clcd_print("DOOR : OPEN", LINE1(0));
        clcd_print("CLOSE THE DOOR", LINE3(0));
        
        // wait still door is closed
        while(RB0 == 0 );
  
        clear_screen();
        //CONTINUE THE WORK
        FAN = ON;
        DC_MOTOR = ON;
        TMR2ON = ON;
        
        BUZZER = OFF;
        clcd_print("FUNCTION -", LINE1(0));
        clcd_print("TIME", LINE2(0));
        clcd_print("4-START  6-PAUSE", LINE4(0));
    }
}