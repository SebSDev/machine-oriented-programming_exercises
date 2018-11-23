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
#include "stdio.h"
#include "string.h"

void exOne();
void exTwo();
void exTwoA();
void exThree();
void exFour();
void exFive();
void exSix();
void exSeven();
void exSevenA();

void * mymemcopy ( void *, const void *, size_t);



// - Bitte darauf achten, dass am Coldfire-Serial Port ein  
//   Terminal (Putty o.�.) mit 19200kBaud angeschlossen ist.
// - Als Target muss   <projektname>_RAM_OSBDM   ausgew�hlt werden.

void main(void)
{
	int counter = 0;
	
	/***** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
	PE_low_level_init(); /********************************************************/
	/***** End of Processor Expert internal initialization. **********************/
	
  
	// call the exercise you want to run here:
	exSevenA();
	

	// Als Ende-Behandlung nachfolgend ein einfacher Leerlauf-Prozess 
	// **************************************************************
 
	for(;;)
	{
		counter++;
	}
 
}  /* END main */


void exOne()
{
	char* string = "maus";
	
	/***** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
	PE_low_level_init(); /********************************************************/
	/***** End of Processor Expert internal initialization. **********************/
	
	asm{
	  
		  move.l string, a1		// move the string (address) to a1 register
		  
		  loop: 
			  move.b (a1)+, d0	// move the next character to the d0 register
			  tst.b d0			// check if the character is 0 to see if we are at the end of the string
			  beq end_loop		// if so, end the loop
			  move.b d0, -(sp)  // else we push the character to the stack
			  jsr TERM_Write	// and print it out
			  adda.l #1, sp		// pop the character from tos
			  bra loop			// next iteration
		  end_loop:
	}
}

// writing a string backwards
void exTwo()
{
	char* string = "Bitte schreib mich Rueckwaerts"; // length 30
	
	/***** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
	PE_low_level_init(); /********************************************************/
	/***** End of Processor Expert internal initialization. **********************/

	
	asm{
		move.l (string), a1
		move.l (string), a2 // a2 remembers where the string started
		suba.l #1, a2 // we sub one from this address because when a1 is at the starting point of the string, 
					  // we still want to print this character
		
		// move the address of a1 to the end of the string
		count_loop:
			tst.b (a1)
			beq end_count_loop
			adda.l #1, a1
			bra count_loop
		end_count_loop:
		
		// sub 1 from a1 because it currently stands at the \0
		suba.l #1, a1
		
		// now we write characters until we are at the start of the string-1 (a1==a2)
		write_loop:
		
			cmp.l a1, a2
			beq end_write_loop
			
			move.b (a1), -(sp)
			jsr TERM_Write
			adda.l #1, sp
			
			suba.l #1, a1
			bra write_loop
			
		end_write_loop:
	}
}

void exTwoA()
{
	char* pointerString = "Ich bin ein String, der als char* deklariert wurde!";
	char arrayString[] = "Ich bin ein String, der als char[] deklariert wurde!";
	
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
		  adda.l #1, a1
		  bra pointer_loop
	  end_pointer_loop:
	}
	
	TERM_WriteString("    ");
	
	// -------------------------------------------
	// print string that has been defined as char[]
	asm {
	  lea arrayString, a1
	  
	  array_loop:
		  tst.b (a1)
		  beq end_array_loop
		  move.b (a1), -(sp)
		  jsr TERM_Write
		  adda.l #1, sp
		  adda.l #1, a1
		  bra array_loop
	  end_array_loop:
	}
	
	TERM_WriteString("    ");
}

void exThree()
{
	int number = 53839; // expected hex output: D24F

	/***** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
	PE_low_level_init(); /********************************************************/
	/***** End of Processor Expert internal initialization. **********************/
	
	// -------------------------------------------
	// Exercise 3
	// print the hex code of an integer value
	// -------------------------------------------
	asm{
	  
	  move.l #16, d5 // d2 is the left shift amount, it gets increased by 4 each loop iteration
	  move.l #28, d4// d4 is right shift amount
	  
	  loop:
		  move.w number, d1 // d1 is the number we want to print the hex code from
		  
		  cmp.l #32, d5 // if we push right by 32 the register is empty so we are at the end
		  beq end_loop // end the loop in this case
		  
		  lsl d5, d1
		  lsr d4, d1
		  
		  // if the number is 9 or lower, we have to add 48 for the correct ascii code
		  // if the number is above 9, we have to add 65 for the correct ascii code
		  cmp #9, d1
		  bgt elseteil
		  addi.l #48, d1
		  bra if_ende
		  elseteil:
		  addi.l #55, d1
		  if_ende:
		  
		  // print the character by pushing it to tos and calling the writing subroutine
		  move.b d1, -(sp)
		  jsr TERM_Write
		  adda.l #1, sp // increase the stackpointer to clear the space of the printed character
		  
		  addq.l #4, d5 // in the next iteration we shift to the left 4 bits more to remove the last used value
		  
		  bra loop
		  
	  end_loop:
	  
	}
	
	TERM_WriteString(" ");
}

void exFour()
{
	int number = 53839; // expected hex output: D24F

	asm{
		bra start
		
		UP_Hexout:
			link a6, #-4
			movem.l d4-d5/d1, (sp)
			
			
			// Aufgabe 3 Programm
			move.l #16, d5 // d2 is the left shift amount, it gets increased by 4 each loop iteration
			move.l #28, d4// d4 is right shift amount
			
			loop:
				move.w 8(a6), d1 // d1 is the number we want to print the hex code from
				
				cmp.l #32, d5 // if we push right by 32 the register is empty so we are at the end
				beq end_loop  // end the loop in this case
				
				lsl d5, d1
				lsr d4, d1
				
				// if the number is 9 or lower, we have to add 48 for the correct ascii code
				// if the number is above 9, we have to add 65 for the correct ascii code
				cmp #9, d1
				bgt elseteil
				addi.l #48, d1
				bra if_ende
				elseteil:
				addi.l #55, d1
				if_ende:
				
				// print the character by pushing it to tos and calling the writing subroutine
				move.b d1, -(sp)
				jsr TERM_Write
				adda.l #1, sp // increase the stackpointer to clear the space of the printed character
				
				addq.l #4, d5 // in the next iteration we shift to the left 4 bits more to remove the last used value
				
				bra loop
			  
			end_loop:
			// ende Aufgabe 3 Programm
			
			
			movem.l (sp), d4-d5/d1
			unlk a6
			rts
			
		
		start: 
			move.w number, -(sp)	// push param on tos
			jsr UP_Hexout			// jump to subroutine
			adda.l #2, sp			// pop param of tos		
	}
}

void exFive()
{
	char* source = "gartenzaun";
	char* dest = "              ";
	
	mymemcopy(dest, source, 10);
	
	printf("%s \n", dest);
}

void* mymemcopy(void* destination, const void* source, size_t num)
{
	
	asm{ naked					// alternatively asm void* ...
		link a6, #0 			// setting framepointer
		suba.l #12, sp 			// reserving space for three saved registers (3*4 byte)
		movem.l a0-a1/d1, (sp) 	// saving registers
		
		move.l 16(a6), d1 		// d0 is num
		move.l 12(a6), a0 		// a0 is source
		move.l 8(a6), a1		// a1 is destination
		
		loop:
			tst.l d1			// check if we still have to copy bytes
			beq endloop
			
			move.b (a0)+, (a1)+	//copy next byte
			
			subq.l #1, d1
			
			bra loop
			
		endloop:
		
		move.l 8(a6), d0		// moving the result to d0 (result register)
		
		movem.l (sp), a0-a1/d1 	// moving back the saved registers
		adda.l #12, sp
		unlk a6
		rts
	}
	
}

void exSix()
{
	char* pre = "before the exception ";
	char* ex = "-- exception handler -- ";
	char* post = "after the exception ";
	
	
	/***** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
	PE_low_level_init(); /********************************************************/
	/***** End of Processor Expert internal initialization. **********************/
	
	asm{
		bra start // branching to the entry point
		
		exception:
		
			// writing the string "-- exception handler --"
			move.l ex, a1 // ex is the address of our string
			move.l a1, -(sp)
			jsr TERM_WriteString
			adda.l #4, sp
			
			addq.l #2, 4(a7)// adding 2 to the address in 'Stackpointer + 4'
							// 'Stackpointer + 4' holds the information of where to jump when calling rte (stacked program counter)
							// by adding 2 we will jump right after the exception was thrown.
							// else we would just call the exception again and we would be in an infinite loop
			rte	// return from exception
			
		start:
		
			// writing the string "before the exception"
			move.l pre, a1
			move.l a1, -(sp)
			jsr TERM_WriteString
			adda.l #4, sp
			
			lea exception, a0			// saving the address of the exception handler in a0
			move.l a0, 0x20000000+0x10	// saving the address of the exception handler in the address 0x10
										// which holds the information of where to jump when an exception is thrown
			dc.w 0x4AFC		// throwing an exception. 0x4AFC is the hex code for the ILLEGAL command
			
			// writing the string "after the exception"
			move.l post, a1
			move.l a1, -(sp)
			jsr TERM_WriteString
			adda.l #4, sp
			
		ende:

		}
		
		TERM_WriteString("Ende des Programms");
}

void exSeven()
{
	char* string = "Betriebssystem";
	int userStack[15]; 
	
	asm{
		bra userstart
		betriebssystem:
			
			// writing the string "Betriebssystem"
			move.l usp, a3
			move.l (a3), -(sp)
			jsr TERM_WriteString
			adda.l #4, sp
			
			rte
			
		userstart:
		
			lea betriebssystem, a1;
			move.l a1, 0x20000000+0x84
			
			lea userStack, a2
			adda.l #15, a2
			move.l a2, usp
			
			// ENTERING USER MODE
			move.w sr, d0
			bclr #13, d0
			move.w d0, sr
			
			move.l string, -(sp)
			trap #1
			adda.l #4, sp
	}
}

void exSevenA()
{
	asm{
			bra start
		myBitRev:
			move.l #0, d2 	// d2 is right shift amount
			move.l #0, d3	// d3 is for saving the result temporarily
							// d4 is used for extracting the current bit
			move.l #31, d5	// d5 is to always shift 31 to the right
			
			revloop:
				cmp.l #32, d2
				beq endrevloop
				
				move.l d0, d4	// moving the value to reverse in d4
				lsl.l d2, d4	// shift it left by d2, which is zero at the start and gets increased every loop round
				lsr.l d5, d4	// shift it to the right be 31
				lsl.l d2, d4
				
				or d4, d3		// adding the extracted bit to the result register
				
				move.l #0, d4
				subq.l #1, d1
				addq.l #1, d2
				
				bra revloop
			endrevloop:
			
			move.l d3, d0 	// saving the result in d0 
			rts
			
		start:
			move.l #0xFFFCC000, d0  // in binary: 		11111111111111001100000000000000
								   // result should be:	00000000000000110011111111111111  (33FFF)
			jsr myBitRev
			
			moveq.l #1, d3
		
	}
}


