// TuningFork.c Lab 12
// Runs on LM4F120/TM4C123
// Use SysTick interrupts to create a squarewave at 440Hz.  
// There is a positive logic switch connected to PA3, PB3, or PE3.
// There is an output on PA2, PB2, or PE2. The output is 
//   connected to headphones through a 1k resistor.
// The volume-limiting resistor can be any value from 680 to 2000 ohms
// The tone is initially off, when the switch goes from
// not touched to touched, the tone toggles on/off.
//                   |---------|               |---------|     
// Switch   ---------|         |---------------|         |------
//
//                    |-| |-| |-| |-| |-| |-| |-|
// Tone     ----------| |-| |-| |-| |-| |-| |-| |---------------
//
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015

 Copyright 2016 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */


#include "TExaS.h"
#include "..//tm4c123gh6pm.h"


// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode

int sound = 0, press=0;


// input from PA3, output from PA2, SysTick interrupts
void Sound_Init(void){ 
	//rising edge 
	SYSCTL_RCGC2_R |= 0x00000001; // (a) activate clock for port F
  GPIO_PORTA_DIR_R &= ~0x08;    // (c) make PF4 in (built-in button)
  GPIO_PORTA_AFSEL_R &= ~0x08;  //     disable alt funct on PF4
  GPIO_PORTA_DEN_R |= 0x08;     //     enable digital I/O on PF4   
  GPIO_PORTA_PCTL_R &= ~0x0000F000; // configure PF4 as GPIO
  GPIO_PORTA_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTA_PDR_R |= 0x08;     //     enable weak pull-up on PF4
  GPIO_PORTA_IS_R &= ~0x08;     // (d) PF4 is edge-sensitive
  GPIO_PORTA_IBE_R &= ~0x08;    //     PF4 is not both edges
  GPIO_PORTA_IEV_R &= ~0x00;    //     PF4 rising edge event
  GPIO_PORTA_ICR_R = 0x08;      // (e) clear flag4
  GPIO_PORTA_IM_R |= 0x08;      // (f) arm interrupt on PF4
  NVIC_PRI0_R = (NVIC_PRI0_R&0xFFFFFF00)|0x00000020; // (g) priority 1
  NVIC_EN0_R = 0x00000001;      // (h) enable interrupt 01 in NVIC
	
	//systick init
	NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = 90908;// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; // priority 2
                              // enable SysTick with core clock and interrupts
  NVIC_ST_CTRL_R = 0x07;
	
	//PA2 init
  GPIO_PORTA_DIR_R |= 0x04;   // make PF2 output (PF2 built-in LED)
  GPIO_PORTA_AFSEL_R &= ~0x04;// disable alt funct on PF2
  GPIO_PORTA_DEN_R |= 0x04;   // enable digital I/O on PF2
                              // configure PF2 as GPIO
  GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFFFFF0FF)+0x00000000;
}

// called at 880 Hz
void SysTick_Handler(void){
	if((GPIO_PORTA_DATA_R&0x08) == 0){
		press = 0;
	}
	else{
		if(press == 0){
			if(sound == 0){
				sound =1;
			}
			else{
				sound = 0;
			}
		}
		press =1;
	}
	if(sound == 1){
		GPIO_PORTA_DATA_R ^= 0x04;
	}
	else{
		GPIO_PORTA_DATA_R |= 0x00;
	}
}


int main(void){// activate grader and set system clock to 80 MHz
  TExaS_Init(SW_PIN_PA3, HEADPHONE_PIN_PA2,ScopeOn); 
  Sound_Init();    
  EnableInterrupts();   // enable after all initialization are done
  while(1){
    // main program is free to perform other tasks
    // do not use WaitForInterrupt() here, it may cause the TExaS to crash
  }
}
