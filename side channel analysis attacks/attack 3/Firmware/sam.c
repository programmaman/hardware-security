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

// DO NOT CHANGE THIS FILE //

#include "hal.h"

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include "simpleserial.h"

uint8_t do_SAM64(uint8_t*, uint8_t);
uint8_t do_SAM32(uint8_t*, uint8_t);

uint8_t do_SAM64(uint8_t* e, uint8_t len)
{
    volatile uint64_t r   = 0;
    volatile uint64_t b   = 0x42;
    volatile uint64_t exp = 0;
    volatile uint64_t m   = 0x17;
    volatile uint8_t  exp_bin[64];
    volatile uint8_t  i = 0, n = 0;
    
    // convert intput bytes array 'e' to int 'exp'
    for (i = 8; i > 0; i--)
    {
        exp <<= 8;
        exp |= e[i-1];
    }
    
    // init exponent (exp) binary array
    for(i = 0; i < 64; i++)
    {
        exp_bin[i] = 0x00;
    }
    
    // convert 'exp' to binary array
    while(exp > 0)
    {
        exp_bin[n] = exp & 0x1;
        exp >>= 1;
        n++;
    }

    trigger_high();
    
    /*****************
     * SAM Algorithm *
     *****************/
    r = b;
    i = n-1;
    while (i > 0)
    {
        r = (r * r) % m;
        if(exp_bin[--i])
        {
            r = (r * b) % m; // long operation here
        }
    }
    
    trigger_low();
    
    for(i = 0; i < 8; i++)
    {
        e[i] = (uint8_t)(r & 0xFF);
        r = r >> 8;
    } 
    
    simpleserial_put('r', 16, e);
    
    return 0x00;
}

uint8_t do_SAM32(uint8_t* e, uint8_t len)
{
    volatile uint32_t r   = 0;
    volatile uint32_t b   = 0x42;
    volatile uint32_t exp = 0;
    volatile uint32_t m   = 0x17;
    volatile uint8_t  exp_bin[32];
    volatile uint8_t  i = 0, n = 0;
    
    // convert intput bytes array 'e' to int 'exp'
    for (i = 4; i > 0; i--)
    {
        exp <<= 8;
        exp |= e[i-1];
    }
    
    // init exponent (exp) binary array
    for(i = 0; i < 32; i++)
    {
        exp_bin[i] = 0x00;
    }
    
    // convert 'exp' to binary array 'exp_bin'
    while(exp > 0)
    {
        exp_bin[n] = exp & 0x1;
        exp >>= 1;
        n++;
    }

    trigger_high();
    
    /*****************
     * SAM Algorithm *
     *****************/
    r = b;
    i = n-1;
    while (i > 0)
    {
        r = (r * r) % m;
        if(exp_bin[--i])
        {
            r = (r * b) % m; // long operation here
        }
    }
    
    trigger_low();
    
    
    /*
    for(i = 0; i < 4; i++)
    {
        e[i] = (uint8_t)(r & 0xFF);
        r = r >> 8;
    } 
    
    simpleserial_put('r', 16, e);
    */
    return 0x00;
}

int main(void)
{
    platform_init();
    init_uart();
    trigger_setup();

    simpleserial_init();

    simpleserial_addcmd('e', 16, do_SAM64);
    simpleserial_addcmd('f', 16, do_SAM32);
    
    while(1)
        simpleserial_get();
}
