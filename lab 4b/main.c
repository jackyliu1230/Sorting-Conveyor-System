/*
 * final project.c
 *
 * Created: 2022-03-24 1:59:24 PM
* Author : mech458
# NAME 1: Gucheng, Wang, V00942654
# NAME 2: Cheuk Ki, Liu, V00937822
		

 */ 

#include <avr/interrupt.h>
#include <stdlib.h>
#include <avr/io.h>
#include <string.h>
#include "lcd.h"
#include "LinkedQueue.h"

// define the global variables that can be used in every function  ==========

volatile unsigned int ADC_result; //char

// 0b0000/INA/INB/ENA/ENB

#define MOTOR_BRKVCC 0b00001111

#define MOTOR_CW  0b00001011

#define MOTOR_CCW 0b00000111

#define MOTOR_BRKG 0b00000011

#define STEP1 0b00110101 //0b00110000
#define STEP2 0b00110110
#define STEP3 0b00101110
#define STEP4 0b00101101
// #define STEP1 0b00110000; //0b00110000
// #define STEP2 0b00000110;
// #define STEP3 0b00101000;
// #define STEP4 0b00000101;


int stepnumber;
int m;

// Global variable for checking the state of the motor and the kill ISR

volatile unsigned int state_Flag=1 ; //0 for CCW 1 for CW

volatile unsigned int kill_Flag=0 ; // not kill

volatile unsigned int interruptflag = 0;
volatile int max;
volatile int min=1200;

volatile unsigned int ADC_result_flag;
volatile unsigned int PRFlag;
volatile int count;
volatile int tim3_count;
volatile int curPos=0;
//volatile double curPos;
volatile int futPos;
volatile int curStep;
volatile int disp_ADC_flag;
volatile int del_Q_flag;
volatile int ramp_flag;
volatile int rotate_check=0;
//Global variables to display the sorted objects and unsorted objects
volatile int B_Sorted;
volatile int B_Total;
volatile int W_Sorted;
volatile int W_Total;
volatile int A_Sorted;
volatile int A_Total;
volatile int S_Sorted;
volatile int S_Total;
volatile int store0=1;
volatile int current;
volatile int store2;
volatile int store3;
volatile int array[100];
volatile int arrays[]={STEP1,STEP2,STEP3,STEP4};

volatile int array100[]={26,25,24,22,19,16,13,10,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,10,13,16,19,22,24,25,26};
volatile int count2 = 0;
volatile int count3 = 0;
volatile int count4 = 0;
//Global variables for the linked list
 link *head;			/* The ptr to the head of the queue */
 link *tail;			/* The ptr to the tail of the queue */
 link *newLink;		/* A ptr to a link aggregate data type (struct) */
 link *rtnLink;		/* same as the above */
 element eTest;		/* A variable to hold the aggregate data type known as element */

void mStepper(int step, int flag, int tim);
void Hall();
void Left_Turn();
void Right_Turn();
void Turn_Around();

void Rota_CW();
void Rota_CCW();
void mTimer(int count);

void pwmDEF();


void main()
{

CLKPR=0x80;

CLKPR=0x01; //Required to set CPU Clock to 8MHz


TCCR1B|= _BV(CS11);




cli();  // disable all of the interrupt ==========================

// config the external interrupt ======================================

    
	//Enable INT2, INT3, INT4, INT5
	EIMSK |= (_BV(INT2) | _BV(INT3) | _BV(INT4) | _BV(INT5));
	
	//Rising edge interrupt for INT2 (OR); Falling edge interrupt for INT3 (EX), INT4 (Pause-Resume Button), INT5 (Ramp-Down Button)
	EICRA |= (_BV(ISC21) | _BV(ISC20) | _BV(ISC31));
	EICRB |= (_BV(ISC41) | _BV(ISC40)| _BV(ISC51)) ;
	
// config ADC =========================================================
	//enable ADC; enabale ADC interrupt; set a pre-scaler of 32 for ADC (ADC got a frequency of 250kHz)
	ADCSRA |= (_BV(ADEN) | _BV(ADIE) );
	
	//The ADC is right-adjusted; ADC1 is configured
	ADMUX |= (_BV(REFS0) | _BV(MUX0));


// sets the Global Enable for all interrupts ==========================

sei();



// initialize the ADC, start one conversion at the beginning ==========

   	InitLCD(LS_BLINK|LS_ULINE);

    	
	DDRC = 0xFF; // set the PORTC as output to display the ADC result ==================

	DDRE = 0x00; //initialize the port E as the input
	DDRD = 0x00;
	DDRL = 0xFF;

	//ADCSRA |= _BV(ADSC);

	DDRB = 0xFF; //initialize the port B
	DDRA = 0b00111111; //0xFF
    initLink(&rtnLink);
	rtnLink = NULL;
	newLink = NULL;

	pwmDEF();
    Hall();
    curStep=1;
	curPos=0;
	
// 	Rota_CW(190);
// 	mTimer(1000);
// 	Rota_CCW(190);
// 	mTimer(1000);

	
	




   

    PORTB = MOTOR_CCW;
        
		while(1){
		 
	         if ((PIND&0x04)==0 && min < 1200)
	         {
		         ADC_result_flag = 1;
				 //array[count2]=min;
				
	         }
	               
	               
               
           
		 if (ADC_result_flag){
			 //min=array[0];
			       //Set on the flag to display the result on LCD
			       if (min<= 300){
					   array[count2]=1;//AL
					   count2++;

				       A_Total = A_Total + 1;
					   A_Sorted=A_Total-1;
			       }
			       else if ( (400 <= min) && (min <= 800))
			       {
				       array[count2]=3;//S
					   count2++;
				       S_Total = S_Total + 1;
					    S_Sorted=S_Total-1;
					   
			       }
			       else if( (870 <= min) && (min <= 963))
			       {
				       array[count2]=2;// W
					   count2++;
				       W_Total = W_Total + 1;
					    W_Sorted=W_Total-1;
			       }
			       else if( (964 < min) && (min <= 1000))
			       {
				       array[count2]=0;//B
					   count2++;
				       B_Total = B_Total + 1;
					    B_Sorted=B_Total-1;
			       }
				  // LCDClear();
				   //LCDWriteIntXY(0,0,array[0],4);
				   //LCDWriteIntXY(6,0,array[1],4);
				   //LCDWriteIntXY(0,1,array[2],4);
				  // LCDWriteIntXY(6,1,array[3],4);
			       LCDWriteIntXY(11,0,min,4); 
				   LCDWriteIntXY(11,1,count4,4);  
				   //mTimer(1000);
			 ADC_result_flag = 0;
			 min = 1200;
	
		 }
		 
            
			if (del_Q_flag == 1){
				del_Q_flag = 0;
				if(count2==8){
					count2=0;
				}
				count3++;
				if(count3==8){
					count3=0;
				}
				
				
			}
			 	if (ramp_flag){
					
					 TCCR3B = 0x00;
					 TIMSK3 = 0x00;
				 	cli();
				 	while(1){
						 ramp_flag = 0;
					 }
			 	}
			 	
			 
		
			//LCDWriteIntXY(6,0,min,4);       
			
			

		}


}   // end main


/****************************************************************************
INT2 (OR sensor)
*****************************************************************************/
ISR(INT2_vect){
	//PORTL = 0x0F;
	PORTL = 0b11000000;
	
	if ((PIND&0x04)==0x04)
	{
		ADCSRA |= _BV(ADSC);	
	}
	
	

}
/****************************************************************************
Bad ISR
*****************************************************************************/
ISR(BADISR_vect){
	LCDWriteString("BADCALLED");
	while(1);

}


/* 
INT3 is EX sensor
*/

ISR(INT3_vect)
{
       
		del_Q_flag = 1;
		PORTL = 0b00110000;
        PORTB = MOTOR_BRKVCC;
        mTimer(20);
		futPos = array[count3];
		
		
		rotate_check = futPos - curPos;
		curPos = futPos;
		 if ((rotate_check == 2) || (rotate_check == -2))
		 {
			 Rota_CW(100);
			 
			 
		 }
		 else if ((rotate_check == 1) || (rotate_check == -3))
		 {
			 Rota_CW(50);
			
		 }
		 else if ((rotate_check == -1) || (rotate_check == 3))
		 {
			 Rota_CCW(50);

		 }		LCDClear();
 		LCDWriteIntXY(0,0,array[0],4);
		LCDWriteIntXY(6,0,array[1],4);
		LCDWriteIntXY(0,1,array[2],4);
		LCDWriteIntXY(6,1,array[3],4);
		
	
		 /*eTest = firstValue(&head);
		 futPos = eTest.itemCode;
		 rotate_check = futPos - curPos;
		 
         if ((rotate_check == 2) || (rotate_check == -2))
         {
             Rota_CW(100);
			 rotate_check = 0;
         }
         else if ((rotate_check == 1) || (rotate_check == -3))
         {
             Rota_CW(50);
			 rotate_check = 0;
         }
         else if ((rotate_check == -1) || (rotate_check == 3))
         {
             Rota_CCW(50);
			 rotate_check = 0;
         }        			dequeue(&head, &tail, &rtnLink);
          curPos = futPos;*/
		 if(array[count3]==0){
			 B_Sorted++;
		 }
		 if(array[count3]==2){
			 W_Sorted++;
		 }
		 if(array[count3]==3){
			 S_Sorted++;
		 }
		 if(array[count3]==1){
			 A_Sorted++;
		 }
		mTimer(150);
		PORTB = MOTOR_CCW;

}



// sensor switch:  Active HIGH starts AD converstion =======

// the interrupt will be trigured if the ADC is done ======================== 

ISR(ADC_vect)  {
   if ((PIND&0x04)==0x04)
   {
       ADCSRA |= _BV(ADSC);
	   
   }
   if(ADC <= min)
   {
       min = ADC;
   }
 

  
}
ISR(INT4_vect)

	{
		if((PINE&0x10)==0x10){// push

			mTimer(10);
			if(state_Flag ){

				state_Flag = 0;
				mTimer(20);
				PORTB = MOTOR_BRKVCC;
				mTimer(10);
				 LCDClear();
				 
				 LCDWriteStringXY(0, 0, "Bl:");
				 LCDWriteIntXY(3,0,B_Sorted, 2);
				 LCDWriteIntXY(5,0,B_Total-B_Sorted,2);	
				 LCDWriteString(" ");
				 
				 LCDWriteString("Al:");
				 LCDWriteIntXY(11,0,A_Sorted, 2);
				 LCDWriteIntXY(13,0,A_Total-A_Sorted,2);	
				 
				 LCDWriteStringXY(0, 1, "Wh:");
				 LCDWriteIntXY(3,1,W_Sorted, 2);
				 LCDWriteIntXY(5,1,W_Total-W_Sorted,2);				
				 LCDWriteString(" ");
				 
				 LCDWriteString("St:");
				 LCDWriteIntXY(11,1,S_Sorted, 2);
				 LCDWriteIntXY(13,1,S_Total-S_Sorted,2);
				
				
			}
			else {
				
				state_Flag = 1;
				mTimer(20);
				PORTB = MOTOR_CCW;
				mTimer(10);
				
			}
			while((PINE&0x10)==0x10){} // push
			mTimer(20);
		}
		
		

}
ISR(INT5_vect){
    //Initialize a second timer to count 5 seconds
    //Set the pre-scaler of timer 3 to 256
    TCCR3B |= _BV(CS32)|_BV(CS30);
	
    //Set the timer 3 mode to CTC for comparing OCRA
    TCCR3B |= _BV(WGM32);
	//enable timer interrupt
	TIMSK3|= _BV(OCIE3A);
    //Set the output time to 5sec, count for every 1 second.
    OCR3A = 0x989E;
    //Set the counter to 0 and enable the interrupt
    TCNT3 = 0x0000;
   



    
}
ISR(TIMER3_COMPA_vect){
   
    
        PORTB = MOTOR_BRKVCC; 
        
        LCDClear();
        
        LCDWriteStringXY(0, 0, "Bl:");
        LCDWriteInt(B_Sorted, 3);
        LCDWriteString("   ");
        
        LCDWriteString("Al:");
        LCDWriteInt(A_Sorted, 3);
        
        LCDWriteStringXY(0, 1, "Wh:");
        LCDWriteInt(W_Sorted, 3);
        LCDWriteString("   ");
        
        LCDWriteString("St:");
        LCDWriteInt(S_Sorted, 3);
        
        ramp_flag = 1;
        PORTB = MOTOR_BRKVCC;
    
}

	
void Rota_CW(int turndegree){
	int delay = 0;
	for (int i = 0; i < turndegree; i++){
		stepnumber++;
		if(stepnumber == 5){
			stepnumber=1;
		}
		switch (stepnumber)
		{
			case 1:
			PORTA = STEP1;
			mTimer(array100[delay]);
			delay++;
			break;
			case 2:
			PORTA = STEP2;
			mTimer(array100[delay]);
			delay++;
			break;
			case 3:
			PORTA = STEP3;
			mTimer(array100[delay]);
			delay++;
			break;
			case 4:
			PORTA = STEP4;
			mTimer(array100[delay]);
			delay++;
			break;
		}
		
		
	}
}

/*
To do: counterclockwise rotate when stepnumber less than equal to 0, it must start from 4, otherwise, it start from 4 to 1.
*/
void Rota_CCW(int turndegree){
	int delay = 0;
	for (int i = 0; i <turndegree; i++){
		stepnumber--;
		if(stepnumber <= 0){
			stepnumber = 4;
		}
		
		switch (stepnumber)
		{
			case 1:
			PORTA = STEP1;
			mTimer(array100[delay]);
			delay++;
			break;
			case 2:
			PORTA = STEP2;
			mTimer(array100[delay]);
			delay++;
			break;
			case 3:
			PORTA = STEP3;
			mTimer(array100[delay]);
			delay++;
			break;
			case 4:
			PORTA = STEP4;
			mTimer(array100[delay]);
			delay++;
			break;
		}
		}
	}


void mStepper(int step, int flag, int tim){
    step = step%200;
    //For the initialization of stepper motor;
    //step = step + 4;
    if(flag == 1){
        for(int c = 0; c < step; c++){
            
            if(curStep == 1)
            {
                //PORTA = 0b00110000;
                PORTA = STEP1;
                curStep = 2;
                mTimer(tim);
            }
            else if(curStep == 2)
            {
                //PORTA = 0b00000110;
                PORTA = STEP3;
                curStep = 3;
                mTimer(tim);
            }
            else if(curStep == 3)
            {
                //PORTA = 0b00101000;
                PORTA = STEP4;
                curStep = 4; 
                mTimer(tim);
            }
            else if(curStep == 4)
            {
                //PORTA = 0b00000101;
                PORTA = STEP1;
                curStep = 1;
                mTimer(tim);
            }
        }
}
    else{
        //Flow control to make sure stepper motor ticks the "step" number of steps.
        for(int c = 0; c < step; c++){

            //A switch statement to make sure the stepper initiating corresponding steps in correct order
            //Different motors might have different initialization pattern:
            //For the stepper motor in the kit: "0x08, 0x04, 0x02, 0x01" for clockwise.
            //For the stepper motor in the lab: "" for clockwise.
            
            if (curStep== 2)
            {
                //PORTA = 0b00110000;
                PORTA = STEP1;
                curStep = 1;
                mTimer(tim);
            }
            else if(curStep == 3)
            {
                //PORTA = 0b00000110;
                PORTA = STEP2;
                curStep = 2;
                mTimer(tim);
            }
            else if(curStep == 4)
            {
                //PORTA = 0b00101000;
                PORTA = STEP3;
                curStep = 3;
                mTimer(tim);
            }
            else if(curStep == 1)
            {
                //PORTA = 0b00000101;
                PORTA = STEP4;
                curStep = 4;
                mTimer(tim);
            }
        }
    }
}

// void Hall(){
//     while ((PINA&0x80) == 0x80)
//     {
//         mStepper(1,1,20);
//     }
//     mStepper(4, 0, 20);
//     curPos = 0;
//     curStep = 1;
// }

/**************************************************************************************/
/***************************** SUBROUTINES ********************************************/
/**************************************************************************************/


/**************************************************************************************
* DESC: initializes the linked queue to 'NULL' status
* INPUT: the head and tail pointers by reference
*/

void setup(link **h,link **t){
    *h = NULL;		/* Point the head to NOTHING (NULL) */
    *t = NULL;		/* Point the tail to NOTHING (NULL) */
    return;
    }/*setup*/




    /**************************************************************************************
    * DESC: This initializes a link and returns the pointer to the new link or NULL if error
    * INPUT: the head and tail pointers by reference
    */
 void initLink(link **nl){
//link *l;
 *nl = malloc(sizeof(link));
 (*nl)->next = NULL;
 return;
}/*initLink*/




/****************************************************************************************
*  DESC: Accepts as input a new link by reference, and assigns the head and tail
*  of the queue accordingly
*  INPUT: the head and tail pointers, and a pointer to the new link that was created
will put an item at the tail of the queue */
void enqueue(link **h, link **t, link **nL){

if (*t != NULL){
/* Not an empty queue */
 (*t)->next = *nL;
 *t = *nL; //(*t)->next;
}/*if*/
else{
    /* It's an empty Queue */
    //(*h)->next = *nL;
    //should be this
 *h = *nL;
 *t = *nL;
 }/* else */
return;
}
/*enqueue*/




/**************************************************************************************






//**************************************************************************************
* DESC: Peeks at the first element in the list
* INPUT: The head pointer
* RETURNS: The element contained within the queue
*/
/* This simply allows you to peek at the head element of the queue and returns a NULL pointer if empty */
element firstValue(link **h){
    return((*h)->e);
    }/*firstValue*/





/**************************************************************************************
* DESC: deallocates (frees) all the memory consumed by the Queue
* INPUT: the pointers to the head and the tail
*/
/* This clears the queue */
void clearQueue(link **h, link **t){

    link *temp;

    while (*h != NULL){
        temp = *h;
        *h=(*h)->next;
        free(temp);
        }/*while*/
                                            
        /* Last but not least set the tail to NULL */
        *t = NULL;

        return;
        }/*clearQueue*/





/**************************************************************************************
* DESC: Checks to see whether the queue is empty or not
* INPUT: The head pointer
* RETURNS: 1:if the queue is empty, and 0:if the queue is NOT empty
*/
/* Check to see if the queue is empty */
char isEmpty(link **h){
    /* ENTER YOUR CODE HERE */
    return(*h == NULL);
    }/*isEmpty*/





/**************************************************************************************
* DESC: Obtains the number of links in the queue
* INPUT: The head and tail pointer
* RETURNS: An integer with the number of links in the queue
*/
/* returns the size of the queue*/
int size(link **h, link **t){
                                                    
    link 	*temp;			/* will store the link while traversing the queue */
    int 	numElements;

    numElements = 0;

    temp = *h;			/* point to the first item in the list */

    while(temp != NULL){
        numElements++;
        temp = temp->next;
        }/*while*/
                                                        
        return(numElements);
    }

        
  



void mTimer(int count){

	int i;

	i =0; /* initializes loop counter */

/* Set the Wavefom Generation mode bit description to Clear Timer on Compare Math mode (CTC) only */

	TCCR1B |=_BV(WGM12);

/* set WGM bíts to 0100, see page 145 */

/*Note WGN is spread over two register. */

	OCR1A= 0x03E8; /* Set Output Compare Register for 1000 cycles =1ms */

	TCNT1 = 0x0000; /* Sets initial value of Timer Counter to 0x0000 */

//TIMSK1=TIMSK1 |0b00000010; /* Enable the output compare interrupt enable */

	TIFR1 |= _BV(OCF1A); /*clear the timer interrupt flag and begin new timing */

/* If the following statement is confusing, please ask for clarification! */

/* Poll the timer to determine when the timer has reached 0x03E8 */

	while(i<count){

		if((TIFR1 &0x02)==0x02){

		TIFR1 |=_BV(OCF1A); /* clear interrupt flag by writing a ONE to the bit */

		i++;/*increment loop number*/

		}

	}

return;

}

void pwmDEF(){
	//Set the timer to fast PWM mode; pg 126
	TCCR0A |= _BV(WGM00) | _BV(WGM01);
	//Clear on a compare match; Set output compare A when the time reaches TOP; pg 126
	TCCR0A |= _BV(COM0A1);
	
	//Set the frequency for timer 0 for PWM with a pre-scaler of 1024
	TCCR0B |= _BV(CS01) | _BV(CS00);
	//Set the duty cycle for PWM; 50%, x/255
	OCR0A = 0x55; //SPEED
}
void Hall(){
    while ((PINA&0x80) == 0x80)
    {
        stepnumber++;
        if(stepnumber == 5){
            stepnumber=1;
        }
        switch (stepnumber)
        {
            case 1:
            PORTA = STEP1;
            mTimer(20);
            break;
            case 2:
            PORTA = STEP2;
            mTimer(20);
            break;
            case 3:
            PORTA = STEP3;
            mTimer(20);
            break;
            case 4:
            PORTA = STEP4;
            mTimer(20);
            break;
        }
    }
    curPos = 0;
}