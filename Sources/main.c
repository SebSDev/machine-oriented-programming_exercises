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

#include "UART0.h"
#include "support_common.h"  // include peripheral declarations and more; 
//#include "malloc_wrapper.h"

#include <stdlib.h>

// global vars for Exercise08
char     msginput[]  = "\r\nZahl eingeben (0 fuer Ende): "; 
char     msgfehler[] = "\nFehler bei malloc() !\n";
void     *anker = NULL;

// the exercises
void exOne();
void exTwo();
void exTwoA();
void exThree();
void exFour();
void exFive();
void exSix();
void exSeven();
void exSevenA();
void exEight();
void exNine();

void * mymemcopy ( void *, const void *, size_t);


#define IPS 0x40000000

#define PNQPAR IPS + 0x100068	
#define PTCPAR IPS + 0x10006F

#define DDRTC IPS + 0x100027 	// 8bit register
#define DDRNQ IPS + 0x100020

#define PORTTC IPS + 0x10000F
#define PORTNQ IPS + 0x100038


//!!!!! EXERCISE 9d !!!!!
/* Definitionen für MCF_GPIO Port NQ */
#define MNP_GPIO_PORTNQ      0x40100008
#define MNP_GPIO_DDRNQ       0x40100020
#define MNP_GPIO_SETNQ       0x40100038
#define MNP_GPIO_CLRNQ       0x40100050
#define MNP_GPIO_PNQPAR      0x40100068
#define MNP_GPIO_PORTNQ_NQ1  0x02
#define MNP_GPIO_PORTNQ_NQ5  0x20


/* Definitionen für MCF_GPIO Port TC */
#define MNP_GPIO_PORTTC      0x4010000F
#define MNP_GPIO_DDRTC       0x40100027
#define MNP_GPIO_SETTC       0x4010003F
#define MNP_GPIO_CLRTC       0x40100057
#define MNP_GPIO_PTCPAR      0x4010006F
#define MNP_GPIO_PORTTC_TC0  0x1
#define MNP_GPIO_PORTTC_TC1  0x2
#define MNP_GPIO_PORTTC_TC2  0x4
#define MNP_GPIO_PORTTC_TC3  0x8


/* Definitionen für MCF_Edge Port */
#define MNP_INTC0_IMRL       0x40000C0C  // Interrupt Mask Register 
#define MNP_EPORT_EPPAR      0x40130000  // Edge control register
#define MNP_EPORT_EPIER      0x40130003  // Edge Port Interrupt Enable Reg
#define MNP_EPORT_EPFR       0x40130006  // Edge Port Flag Register 
// !!!!! EX 9d END !!!!!


// - Bitte darauf achten, dass am Coldfire-Serial Port ein  
//   Terminal (Putty o.ä.) mit 19200kBaud angeschlossen ist.
// - Als Target muss   <projektname>_RAM_OSBDM   ausgewählt werden.

void main(void)
{
	int counter = 0;
	
	/***** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
	PE_low_level_init(); /********************************************************/
	/***** End of Processor Expert internal initialization. **********************/
	
	
	
    asm 
    {
         /* MCF52259RM.pdf
         - SW1 and SW2 verbunden mit PNQPAR5 and PNQPAR1 (Quad function pins!!)
         - LED's 1-4 verbunden mit DDRTC0-DDRTC3
         */
            
        /* LEDs als digitale Ausgabe konfigurieren ==========================        bekannt         */
         /* MCF52259RM.pdf
         - Port Data Direction auf output Funktion setzen 
           (15.6.2 Port Data Direction Registers (DDRn))
         - Pin Assignment auf GPIO Funktion setzen
           (15.6.5.1 Dual-Function Pin Assignment Registers)
        - Output Data Register zurücksetzen 
          (15.6.1 Port Output Data Registers (PORTn))
         */
         
         clr.b    MNP_GPIO_PTCPAR        //GPIO Funktion (=0)
         move.b   #0xf, d0               //output Funktion (=1)
         move.b   d0, MNP_GPIO_DDRTC
         clr.b    MNP_GPIO_CLRTC        //LEDS OFF, siehe Figure 15-3
         /* LEDs als digitale Ausgabe konfigurieren ==========================       bekannt (Ende) */

         
         
        /* Taster als digitale Eingabe konfigurieren  ============================= */
        /* MCF52259RM.pdf
         - Pin Assignment auf IRQ function (primary function) 
           (15.6.5.3 Port NQ Pin Assignment Register (PNQPAR))
         - Port Data Direction Löschen für input function 
           (15.6.2 Port Data Direction Registers (DDRn))
         - Output Data Register löschen 
           (15.6.1 Port Output Data Registers (PORTn))
        */
         
        
      // Achtung: Andere Funktion für NQ1 und NQ5 (primary function (=IRQ), 01 statt 00)
         move.w   MNP_GPIO_PNQPAR,d0    // NQ5 und NQ1 auf IRQ Funktion (01)
         and.l #0xF0F0, d0
         or.l  #0x0404, d0
         move.w   d0, MNP_GPIO_PNQPAR
      
         move.b   MNP_GPIO_DDRNQ, d0   //NQ5 and NQ1 löschen für input Funktion
         andi.l   #0xDD,d0
         move.b   d0, MNP_GPIO_DDRNQ
         
         move.b   MNP_GPIO_CLRNQ,d0    //NQ5+NQ1 löschen
         andi.l   #0xDD,d0 
         move.b   d0, MNP_GPIO_CLRNQ

      // Einhängen der Unterbrechungsantwortprogramme (interrupt service routines)
      //   (IRQ5=sw1 / IRQ1=sw2) 
        lea     int_handler_IRQ1, a1
        move.l 	a1, 0x40000000+0x104	// IRQ1 Vector
        lea     int_handler_IRQ5, a1
        move.l 	a1, 0x40000000+0x114	// IRQ5 Vector

         
      // Einstellen rising/falling edge detection, 
      // Einstellen auf falling edge active (MCF52259RM 17.4.1)
        move.w (MNP_EPORT_EPPAR), d1
        bset #3, d1
        bclr #2, d1
        bset #11, d1
        bclr #10, d1
        move.w d1, (MNP_EPORT_EPPAR)      
      
      // Enable EPORT Interrupts (MCF52259RM 17.4.3)              // ENABLE
         move.w   MNP_EPORT_EPIER, d1
         or.l  #0x00000022, d1       // 0x20=IRQ5, 0x2=IRQ1 
         move.w   d1, MNP_EPORT_EPIER
      
      // enable IRQ1+5  (MCF52259RM 16.3.2)                       // ÄUSSERE MASKE
         move.l   MNP_INTC0_IMRL, d1
         and.l 	  #(~0x00000022), d1    //  0x20=IRQ5, 0x2=IRQ1   //diese interrupts "dürfen durch"
         move.l   d1, MNP_INTC0_IMRL
      
      // Interrupts im Statusregister freigeben (CFPRM 1.5.1)     // INNERE MASKE
         move.w	sr,d1
		 andi.l  #~0x00000700,d1
		 move.w	d1,sr        
         
        
    loop:                       // Das ist unser Leerlaufprozess
      bra       loop  
      

    //////////////////////////////////////////////////////////////////////////////
    int_handler_IRQ5:      
        move.l   d0, -(sp)     // WICHTIG!  Alle benutzten Register retten
        move.l   d1, -(sp)
      
      LED1_ON:
      move.b (MNP_GPIO_PORTTC), d1
      or.l #0x1, d1
      move.b d1, (MNP_GPIO_PORTTC)// LED einschalten 

      // Interupt zurücksetzen (MCF52259 17.4.6)
      move.b   #0x20,d0                    // Schreiben von 1 löscht die Bits!
      move.b   d0, MNP_EPORT_EPFR
      
      move.l   (sp)+,d1      // WICHTIG!  Alle benutzten Register restaurieren
      move.l   (sp)+,d0  
      rte 
        
        
    //////////////////////////////////////////////////////////////////////////////    
    int_handler_IRQ1:      
        move.l   d0, -(sp) // has to be saved when interrupts happen
        move.l   d1, -(sp)
        
      LED1_OFF:  
      move.b (MNP_GPIO_PORTTC), d1
      bclr.b #0, d1
      move.b d1, (MNP_GPIO_PORTTC)         // LED ausschalten
         
      // Interupt zurücksetzen (MCF52259 17.4.6)
      move.b   #0x02,d0                    // Schreiben von 1 löscht die Bits! 
      move.b   d0, MNP_EPORT_EPFR
      
      move.l   (sp)+,d1
      move.l   (sp)+,d0  
      rte
    //////////////////////////////////////////////////////////////////////////////    


    }

	
	
	
	// call the exercise you want to run here:
	//exNine();
	
	

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
			  	  	  	  	  	// cmp.l #0, d0 would also work (cmp.b not because that's only in ISA_B)
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
	//TODO: check with local and global variables
	
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
			
		//TODO: save registers and restore them when we are done
		
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
			trap #1		//supervisor call
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

void exEight()
{
	

#if (CONSOLE_IO_SUPPORT || ENABLE_UART_SUPPORT)

	           
	asm {
		
		schleife:  
		
		// Zahl einlesen (Ende vereinfachend mit Wert 0)
		//----------------------------------------------
		lea msginput, a2
		move.l a2, -(sp)		// there is a bug here. this changes the value of "anker"
		jsr TERM_WriteString
		adda.l #4, sp
		
		jsr INOUT_ReadInt 	// getting an integer value from the terminal
							// the entered integer value is now in d0	
		clr.l d5			// clearing the register to prevent bugs
		move.w d0, d5		// saving the integer value in d2
		tst.w d0			// we end the input when the integer is 0
		beq ausgabe
		
		// Einzuhängendes Element aufbauen
		// -------------------------------
		// Tipp: Die Funktion malloc hat "register_abi", nicht "compact_abi". 
		// Das bedeutet, dass die Parameterversorgung nicht über den Stack erfolgt, 
		// sondern über Register, im vorliegenden Fall über Register D0.
		
		moveq.l #6, d0
		jsr malloc			// a0 is now the starting address of the reserved space
							// so a0 is the value and a0+2 is the pointer to the next value
		adda.l #4, sp
		
		move.w d5, (a0)		// moving the int value to the start of the reserved space (first two bytes)
		clr.l 2(a0)			// moving the address 0 to the last 4 bytes of our reserved space (0 indicates the end of the list)
		
		
		
		//          Ergebnis:
		//                  ___________ 
		//          a0 -->  |zahl|  0 | 
		//                  ¯¯¯¯¯¯¯¯¯¯¯    
		 
		
		// Organisation der Listeniteration
		//
		// a2 zeigt auf das aktuelle Element (oder 0 für Listenende)
		// a3 zeigt   a u f   d e n   Z e i g e r   im vorhergehenden Element
		//
		//                              a3         a2 
		//                                \         \
		//  anker:                         \         \
		//  ______     ___________     _____V_____    V___________     ___________
		//  |    | --> |zahl|next| --> |zahl|next| --> |zahl|next| --> |zahl|  0 |
		//  ¯¯¯¯¯¯     ¯¯¯¯¯¯¯¯¯¯¯     ¯¯¯¯¯¯¯¯¯¯¯     ¯¯¯¯¯¯¯¯¯¯¯     ¯¯¯¯¯¯¯¯¯¯¯
		//
		//    Funtioniert 
		//       + auch am Listenende (a3 zeigt auf 0, a2 ist 0)
		//       + auch bei Einfügen vor dem ersten Element 
		//                           (a3 zeigt auf Anker, a2 auf das erste Element
		//       + auch bei leerer Liste (a3 zeigt auf Anker, Anker und a2 sind 0)
		
		
		// suchen der Einfügestelle
		//-------------------------
		
		// Iterator-Paar initialisieren
		lea anker, a3
		move.l (a3), a2

		//TODO: sign extension beim einfügen von negativen werten
		naechstes:
		
		tst.l a2			// check if we are at the end of the list (address of next element is 0)
		beq gefunden		// and if so, we "found" our element
		
		// now we compare the value we want to add to the value of the next element
		// to find out if we reached the point where we want to add the new element
		move.w (a2), d4		// d4 is the next element value
		move.w (a0), d5		// d5 is the insert element value
		cmp.l d5, d4 		
		bgt gefunden 		// we go further into the list if the value to be added is bigger then the next element's
		
		move.l a2, a3		
		adda.l #2, a3		// adding two, so we are at the point where the address of our next element is stored
		move.l (a3), a2
		
		bra naechstes
		
		
		// Einfügestelle gefunden: Objekt einhängen
		//-----------------------------------------                
		gefunden:
			move.l a0, (a3)		// the prev element ((a3)) now points to our new element (a0)
			move.l a2, 2(a0)	// the new element now points at the "old next" element

		
		bra     schleife      	// next value
			  
		
		// gesamte Liste ausgeben
		//-----------------------
		ausgabe: 
			lea anker, a3		// set the starting point of the list
			move.l (a3), a2
		
		ausgabeLoop:
			move.w (a2), -(sp)	//write the value to the terminal
			jsr INOUT_WriteInt
			adda.l #2, sp
			
			move.l a2, a3		
			adda.l #2, a3		// adding two, so we are at the point where the address of our next element is stored
			move.l (a3), a2
			tst.l (a3)			// checking if we are at the end of the list (address 0)
			bne ausgabeLoop

		
		ende:
		
		move.l #0, d1
		move.l d1, (anker)
		bra schleife

	}

#endif


}

void exNine()
{
	asm{
		// initializing PNQPAR
		clr.l d0
		move.w (PNQPAR), d0
		bclr #2, d0
		bclr #3, d0
		bclr #10, d0
		bclr #11, d0
		move.w d0, (PNQPAR)
		
		// initializing PTCPAR
		clr.b (PTCPAR)
		
		// initializing DDRNQ
		clr.l d0
		move.b (DDRNQ), d0
		bclr #1, d0
		bclr #5, d0
		move.b d0, (DDRNQ)
		
		// initializing DDRTC
		clr.l d0
		move.b (DDRTC), d0
		bset #0, d0
		bset #1, d0
		bset #2, d0
		bset #3, d0
		move.b d0, (DDRTC)
		
		//----Binary counting with the LED's----
		moveq.l #0, d5 	// d5 is the current value to be displayed from 0 to 15
		
		clr.l d1
		clr.l d2
		
		loop:
			move.b (PORTNQ), d1 		// d1 holds the information if the switch is pressed or not
										// 0 is pressed, 1 is not pressed
			
			move.b d5, (PORTTC)			// set the LED's on/off
			
			btst #5, d1
			beq waitloop				// if the switch is pressed we enter the wait loop
			bra loop					// else we just display the same values again
		
			waitloop:
				move.b (PORTNQ), d1 
				btst #5, d1 			
				beq waitloop			// if the switch is still pushed we wait until its not pushed anymore
			waitloop_end:
			
			count:
				addq.l #1, d5			// increasing the counter
				cmp.l #15, d5			// if its above 15 we start at 0 again
				ble loop
				moveq.l #0, d5
				bra loop
			count_end:
			
		end_loop:
		
		//----Turning LED's off and on with each switch press----
		// turning all the leds off
		/*clr.l d0
		move.b d0, (PORTTC)
		
		clr.l d1
		clr.l d2
		
		loop:
			move.b (PORTNQ), d1 		// d1 holds the information if the switch is pressed or not
										// 0 is pressed, 1 is not pressed
			btst #5, d1
			beq waitloop				// if the switch is pressed we enter the wait loop
			bra loop
		
			waitloop:
				move.b (PORTNQ), d1 
				btst #5, d1 			
				beq waitloop			// if the switch is still pushed we wait until its not pushed anymore
			waitloop_end:
			
			change_led:
				move.b (PORTTC), d2		// d2 holds the LED state (on/off) information
				// change the LED's states
				bchg #0, d2
				bchg #1, d2
				bchg #2, d2
				bchg #3, d2
				move.b d2, (PORTTC)
				bra loop
			
		end_loop:*/
	}
}


