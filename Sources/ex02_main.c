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

void main(void)
{
  int counter = 0;

  // Exercise 2:
  char* string = "Bitte schreib mich Rueckwaerts";
  
  // Exercise 2a:
  char* pointerString = "Ich bin ein String, der als char* deklariert wurde!";
  char[] arrayString = "Ich bin ein String, der als char[] deklariert wurde!";

  /***** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init(); /********************************************************/
  /***** End of Processor Expert internal initialization. **********************/
  

  // ----------------------------------------------------
  // print string backwards by writing it on the stack
  asm{
	  move.l string, a1		// a1 is the string
	  move.w counter, d3	// d3 is the character counter
	  
	  loop2:
	  	  move.b (a1)+, d0	// move next character to d0
	  	  add.l #1, d3		// add 1 to the counter
	  	  tst.b d0			// check if d0 is zero to see if we are at the end of the string
	  	  beq end_loop2		// if we are at the end, we end the loop
	  	  move.b d0, -(sp)	// else we push the next character on the stack
	  	  bra loop2			// next iteration
	  end_loop2:
	  
	  loop3:
	  	  move.b -(a1), d0	// move the next character to d0 (now starting from the end of the string)
	  	  sub.l #1, d3		// subtract 1 from the counter
	  	  tst.l d3			// check if the counter is 0
	  	  beq end_loop3		// if it is, we end the loop
	  	  jsr TERM_Write	// else we write the character
	  	  adda.l #1, sp		// add 1 to the stack to pop the last written character
	  	  bra loop3			// next iteration
	  end_loop3:
  }
  
  // ----------------------------------------------------
  // Optimized version: Print string backwards only using the stack for the character we currently want to write
  asm {
	  move.l (string), a1	// a1 is the string
	  move.w #0, d3			// d3 is the counter
	  
	  count_loop:
	  	  adda.l #1, a1		// d0 is the current character
	  	  add.l #1, d3		// add 1 to the counter
	  	  tst.b a1 			// check if d0 is zero to see if we are at the end of the string
	  	  beq end_count_loop// if we are at the end, we end the loop 
	  	  bra loop2			// next iteration
	  end_count_loop:
	  
	  write_loop:
	  	  sub.l #1, d3		// subtract 1 from the counter
	  	  tst.l d3			// check if the counter is 0
	  	  beq end_write_loop// if so, we end the loop
	  	  move.b -(a1), sp	// else we push the next character to the stack
	  	  jsr TERM_Write	// print the character on tos
	  	  adda.l #1, sp		// pop the character from the stack
	  	  bra write_loop	// next iteration
	  end_write_loop
  }
  
  
  
  // -------------------------------------------
  // Exercise 2a
  
  // -------------------------------------------
  // print string that has been defined as char*
  asm {
	  move.l pointerString, a1
	  
	  pointer_loop:
	  	  tst.b (a1)
	  	  beq end_pointer_loop
	  	  move.b (a1), -(sp)
	  	  jsr TERM_Write
	  	  adda.l #1, sp
	  	  adda.l #1, (a1)
	  	  bra pointer_loop
	  end_pointer_loop:
  }
  
  // -------------------------------------------
  // print string that has been defined as char[]
  asm {
	  move.l arrayString, a1
	  
	  array_loop:
	  	  tst.b a1
	  	  beq end_array_loop
	  	  move.b a1, -(sp)
	  	  jsr TERM_Write
	  	  adda.l #1, sp
	  	  adda.l #1, a1
	  	  bra pointer_loop
	  end_array_loop:
  }

  

// Als Ende-Behandlung nachfolgend ein einfacher Leerlauf-Prozess 
// **************************************************************
 
  for(;;){
	  counter++;
  }
 
}  /* END main */

