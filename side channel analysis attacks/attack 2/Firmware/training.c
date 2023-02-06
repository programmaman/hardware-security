/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2012-2017 NewAE Technology Inc.
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "hal.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "simpleserial.h"

// Delays the system for 2ms
static void delay_2_ms(void)
{
    for (volatile unsigned int i=0; i < 0xfff; i++ );
}

// Prints string
void my_print(char *c)
{
  do {
    putch(*c);
  } while (*++c);
}


// PASSWORD CHECKER V1 //
uint8_t password_checker_v1(uint8_t * test_password, uint8_t len){
    char real_password[] = "USFCSE";
    int password_wrong = 0;
    
    trigger_high(); /* START POWER COLLECTION */
    
    for (int i = 0; i < sizeof(real_password) - 1; i++)
    {
        if (real_password[i] != test_password[i])
        {
            password_wrong = 1;
            break;
        }
    }
    
    if (password_wrong)
        my_print("WRONG PASSWORD");
    else
        my_print("ACCESS GRANTED");

    trigger_low(); /* STOP POWER COLLECTION */
    return 0x00;
}

// PASSWORD CHECKER V2 //
uint8_t password_checker_v2(uint8_t * test_password, uint8_t len){
    char real_password[] = "USFCSE";
    int password_wrong = 0;
    
    trigger_high(); /* START POWER COLLECTION */
    
    for (uint8_t i = 0; i < sizeof(real_password) - 1; i++)
    {
        if (real_password[i] != test_password[i])
        {
            password_wrong = 1;
            break;
        }
    }
    
    if (password_wrong)
    {
        int wait = rand() % 12345;
        for(volatile int delay = 0; delay < wait; delay++);
        my_print("WRONG PASSWORD");   
    }
    else
        my_print("ACCESS GRANTED");
    
    trigger_low(); /* STOP POWER COLLECTION */
    return 0x00;
}


uint8_t password_checker_v3(uint8_t * test_password, uint8_t len){
    
    char real_password[] = "USFCSE";
    int password_wrong = 0;
    int xor_d = 0;
    int pass_xor = 0;
    
    trigger_high(); /* START POWER COLLECTION */
    

        
   for (int i = 0; i < sizeof(real_password) - 1; i++)
   {
       pass_xor = real_password[i] ^ test_password[i];
       xor_d = xor_d ^ pass_xor;
   }
    
    password_wrong = xor_d;
    
    if (password_wrong)
        my_print("WRONG PASSWORD");
    else
        my_print("ACCESS GRANTED");

    trigger_low(); /* STOP POWER COLLECTION */
    return 0x00;

}



/* Main Function:
 * - Sets up the CW's firmware,
 * - Prints the start-up text,
 * - and setup the commands (cmd) for the password_checker_v1 and v2.
 */
int main(void)
{
    platform_init();
    init_uart();
    trigger_setup();
    
    //Print some fancy-sounding stuff so that attackers
    //will get scared and leave us alone
    my_print("*****Safe-o-matic 3000 Booting...\n");
    my_print("Aligning bits........[DONE]\n");
    delay_2_ms();
    my_print("Checking Cesium RNG..[DONE]\n");
    delay_2_ms();
    my_print("Masquerading flash...[DONE]\n");
    delay_2_ms();
    my_print("Decrypting database..[DONE]\n");
    delay_2_ms();
    my_print("\n\n");
    //Give them one last warning
    my_print("WARNING: UNAUTHORIZED ACCESS WILL BE PUNISHED\n");
    //Get password
    my_print("Please enter password to continue: ");
    
    trigger_low();
    
    simpleserial_init();
    simpleserial_addcmd('a', 16, password_checker_v1);
    simpleserial_addcmd('b', 16, password_checker_v2);
    simpleserial_addcmd('c', 16, password_checker_v3);
    
    while(1)
        simpleserial_get();
}