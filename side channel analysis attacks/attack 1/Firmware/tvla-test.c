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

#include "simpleserial.h"

uint8_t read_and_send(uint8_t* pt, uint8_t len)
{
	/**********************************
	* Start trace collection*/
	trigger_high();

	//16 hex bytes held in 'pt' were sent
	//from the computer. Store your response
	//back into 'pt', which will send 16 bytes
	//back to computer. Can ignore of course if
	//not needed

    //end trace collection
	trigger_low();
    
	/* Send result *
	********************************/
	simpleserial_put('r', 16, pt);
	return 0x00;
}



uint8_t get_key(uint8_t* k, uint8_t len)
{
	// Load key here
	return 0x00;
}

uint8_t get_pt(uint8_t* pt, uint8_t len)
{
	/**********************************
	* Start user-specific code here. */
	trigger_high();

	//16 hex bytes held in 'pt' were sent
	//from the computer. Store your response
	//back into 'pt', which will send 16 bytes
	//back to computer. Can ignore of course if
	//not needed

	trigger_low();
	/* End user-specific code here. *
	********************************/
	simpleserial_put('r', 16, pt);
	return 0x00;
}

uint8_t tvla_test_A(uint8_t* pt, uint8_t len)
{
 //init
    int i = 0;
    int product = 1;
    uint8_t result[16] = {0};
    
    //start trace collection
    trigger_high();
    
    //multiply all 16 bytes
    for(i = 0; i < len; i++)
    {
        product = product * pt[i];
    }
    
    //end trace
    trigger_low();
    
    //separate sum by bytes
    for(i = 15; i >= 0; i--)
    {
        result[i] = product & 0xFF;        /*gets 8LSB from sum*/
        product = product >> 8;                /*right shift the value in sum by 8 bits*/
    }
            
    //send result
	simpleserial_put('r', 16, result);
	return 0x00;
}

uint8_t tvla_test_B(uint8_t* pt, uint8_t len)
{
 //init
    int i = 0;
    int xor = 0;
    uint8_t result[16] = {0};
    
    //start trace collection
    trigger_high();
    
    //sum all 16 bytes
    for(i = 0; i < len; i++)
    {
        xor = xor ^ pt[i];
    }
    
    //end trace
    trigger_low();
    
    //separate sum by bytes
    for(i = 15; i >= 0; i--)
    {
        result[i] = xor & 0xFF;        /*gets 8LSB from sum*/
        xor = xor >> 8;                /*right shift the value in sum by 8 bits*/
    }
            
    //send result
	simpleserial_put('r', 16, result);
	return 0x00;
}

uint8_t tvla_test_C(uint8_t* pt, uint8_t len)
{
 //init
    int i = 0;
    int product = 1;
    int prod_mod = 0;
    uint8_t result[16] = {0};
    //start trace collection
    trigger_high();
    //multiply all 16 bytes
    for(i = 0; i < len; i++)
    {
        product = product * pt[i];
        prod_mod = product % 251;
    }
    //end trace
    trigger_low();
    //separate sum by bytes
    for(i = 15; i >= 0; i--)
    {
        result[i] = prod_mod & 0xFF;        /*gets 8LSB from sum*/
        prod_mod = prod_mod >> 8;                /*right shift the value in sum by 8 bits*/
    }
    //send result
    simpleserial_put('r', 16, result);
    return 0x00;
}

#if SS_VER == SS_VER_2_1
uint8_t aes(uint8_t cmd, uint8_t scmd, uint8_t len, uint8_t *buf)
{
    uint8_t req_len = 0;
    uint8_t err = 0;

    if (scmd & 0x02) {
        req_len += 16;
        if (req_len > len) {
            return SS_ERR_LEN;
        }
        err = get_key(buf + req_len - 16, 16);
        if (err)
            return err;
    }
    if (scmd & 0x01) {
        req_len += 16;
        if (req_len > len) {
            return SS_ERR_LEN;
        }
        err = get_pt(buf + req_len - 16, 16);
        if (err)
            return err;
    }

    if (len != req_len) {
        return SS_ERR_LEN;
    }

    return 0x00;

}
#endif

int main(void)
{
    platform_init();
	init_uart();
	trigger_setup();

 	/* Uncomment this to get a HELLO message for debug */
	/*
	putch('h');
	putch('e');
	putch('l');
	putch('l');
	putch('o');
	putch('\n');
	*/

	simpleserial_init();
#if SS_VER != SS_VER_2_1
	simpleserial_addcmd('p', 16, get_pt);
	simpleserial_addcmd('k', 16, get_key);
    simpleserial_addcmd('r', 16, read_and_send);

    
    simpleserial_addcmd('a', 16, tvla_test_A);
    simpleserial_addcmd('b', 16, tvla_test_B);
    simpleserial_addcmd('c', 16, tvla_test_C);
#else
    simpleserial_addcmd(0x01, 16, aes);

#endif
	while(1)
		simpleserial_get();
}
