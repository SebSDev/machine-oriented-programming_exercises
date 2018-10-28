/* ###################################################################
**     Filename    : main.c
**     Project     : test8
**     Processor   : MCF52259CAG80
**     Version     : Driver 01.00
**     Compiler    : CodeWarrior MCF C Compiler
**     Date/Time   : 2014-10-11, 15:20, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.00
** @brief
**         Main module.
**         This module contains user's application code.
*/                  
/* MODULE main */ 
 
  #pragma compact_abi 
 
#include "UART0.h"
#include "support_common.h"  // include peripheral declarations and more; 
#include "uart_support.h"    // universal asynchronous receiver transmitter,
                             // (d.h. die serielle Schnittstelle)
#include "terminal_wrapper.h"



// - Bitte darauf achten, dass am Coldfire-Serial Port ein  
//   Terminal (Putty o.ä.) mit 19200kBaud angeschlossen ist.
// - Als Target muss   <projektname>_RAM_OSBDM   ausgewählt werden.

void print_as_hex(int value)
{
	asm{
		prolog:
			sub.l #12, sp
			movem.l d1/d2/d4, (sp)
			move.l sp, a6 // moving framepointer
	}
	asm{
		  move.w #16, d2 // d2 is the left shift amount, it gets increased by 4 each loop iteration
		  move.w #12, d4// d4 is right shift amount, it gets decreased by 4 each loop iteration
		  
		  loop:
			  move.w number, d1 // d1 is the number we want to print the hex code from
			  
			  cmp.l #32, d2 // if we push right by 32 the register is empty so we are at the end
			  beq end_loop // end the loop in this case
			  
			  lsl d2, d1
			  lsr d4, d1
			  
			  // if the number is 9 or lower, we have to add 48 for the correct ascii code
			  // if the number is above 9, we have to add 65 for the correct ascii code
			  cmp #9, d1
			  bgt elseteil
			  addi.l #48, d1
			  bra if_ende
			  elseteil:
			  addi.l #65, d1
			  if_ende:
			  
			  // print the character by pushing it to tos and calling the writing subroutine
			  move.b d1, -(sp)
			  jsr TERM_Write
			  adda.l #1, sp // increase the stackpointer to clear the space of the printed character
			  
			  addq.l #4, d2 // in the next iteration we shift to the left 4 bits more to remove the last used value
			  subq.l #4, d4 // in the next iteration we shift to the right 4 bits less, so we get the next value 
			  
			  bra loop
			  
		  end_loop:
	}
}


void main(void)
{
  int number = 53839; // expected hex output: F9B3

  /***** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init(); /********************************************************/
  /***** End of Processor Expert internal initialization. **********************/

 
  // -------------------------------------------
  // Exercise 4
  // calling the code from exercise 3 as a subroutine
  // -------------------------------------------
  // Sack when calling a subroutine:
  // SP: Return address
  //     Param1
  //     Param2
  //     ...
  asm{
	  move.w number, -(sp)
	  jsr print_as_hex
	  adda.l #1, sp
  }


// Als Ende-Behandlung nachfolgend ein einfacher Leerlauf-Prozess 
// **************************************************************
 
  for(;;){
	  counter++;
  }
 
}  /* END main */

