/*
 * Calculating_PI_YoussefHorriche.c
 *
* Created: 12.03.2021
* Author : YoussefHorriche
*/

#include <stdio.h>
#include <avr/io.h>				//Ein- und Ausgänge
#include <avr/interrupt.h>		//Interupt
#include <math.h>
#include <stdlib.h>

#include "avr_compiler.h"
#include "pmic_driver.h"
#include "TC_driver.h"
#include "clksys_driver.h"
#include "sleepConfig.h"
#include "port_driver.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include "stack_macros.h"

#include "mem_check.h"

#include "init.h"
#include "utils.h"
#include "errorHandler.h"
#include "NHD0420Driver.h"
#include "buttonhandler.h"
#include "avr_f64.h"


char Pistring[20];
float64_t Pi=0;
uint64_t counter=0;
int zeitlauft=0;
int zeit=0;


#define SEED time(NULL)

#define StartStop	( 1 << 0 )
#define Reset		( 1 << 1 )
#define Status		( 1 << 2 )


#define BIT_4	( 1 << 4 )
#define BIT_8	( 1 << 8 )
#define BIT_10	( 1 << 10 )

#define LeibnizAlgo 0
#define MCarlo 1

//#define BUTTON1SHORT    0x01
//#define BUTTON2SHORT    0x02
//#define BUTTON3SHORT    0x04
//#define BUTTON4SHORT    0x08
//#define BUTTON1LONG     0x10
//#define BUTTON2LONG     0x20
//#define BUTTON3LONG     0x40
//#define BUTTON4LONG     0x80
#define BUTTONCLEARAll    0xFF

typedef enum
{
    Idle,
    LeibnizCalc,
    MCarloCalc
} eControlStates;

extern void vApplicationIdleHook( void );
void vInitHeartbeatCounter(void);
void vInterfaceTask(void *pvParameters);
void vButtonTask(void *pvParameters);
void vLeibnizTask(void *pvParameters);
void vMonteCarloTask(void *pvParameters);

TaskHandle_t xInterfaceTaskHandle;
TaskHandle_t xButtonTaskHandle;
TaskHandle_t xLeibnizTaskHandle;
TaskHandle_t xMonteCarloTaskHandle;

EventGroupHandle_t xInterfaceeventgrop;
EventGroupHandle_t xButtoneventgroup;
EventGroupHandle_t xLeibnizeventgroup;
EventGroupHandle_t xMonteCarloeventgroup;

eControlStates eControlStateMachine = Idle;
 
 
void vApplicationIdleHook( void )
{
	
}

int main(void)
{	
	
	//resetReason_t reason = getResetReason();
	xInterfaceeventgrop = xEventGroupCreate();
	xButtoneventgroup = xEventGroupCreate();
	xLeibnizeventgroup = xEventGroupCreate();
	xMonteCarloeventgroup = xEventGroupCreate();
	
	vInitClock();
	vInitDisplay();
	initButtons();
	vInitHeartbeatCounter();
	
	xTaskCreate( vInterfaceTask, (const char *) "Interface", configMINIMAL_STACK_SIZE+500, NULL, 2, &xInterfaceTaskHandle);
	xTaskCreate( vButtonTask, (const char *) "Button", configMINIMAL_STACK_SIZE+200, NULL, 2, &xButtonTaskHandle);
	xTaskCreate( vLeibnizTask, (const char *) "Leibniz", configMINIMAL_STACK_SIZE+200, NULL, 1, &xLeibnizTaskHandle);
	xTaskCreate( vMonteCarloTask, (const char *) "MonteCarlo", configMINIMAL_STACK_SIZE+200, NULL, 1, &xMonteCarloTaskHandle);
	
	vDisplayClear();
	vTaskStartScheduler();
	
	return 0;
	
}

void vInitHeartbeatCounter(void)
{
	/* Initializes the Heartbeat timer counter */
	TCC1.CTRLA |= (0b0101) << TC1_CLKSEL_gp;            // CLKdiv = 64 -> fTimer = 500kHz
	TCC1.INTCTRLA |= 1 << TC1_OVFINTLVL_gp;             // Overflow interrupt enable at low-level priority
	TCC1.PER = 499;                                    // TOP after 500 counts -> Tp = 1/(500kHz) * 5000 = 10ms
	PMIC.CTRL |= 1 << PMIC_LOLVLEN_bp;                  // enable low-level interrupt
}

void vInterfaceTask(void *pvParameters) {
	(void) pvParameters;
	float64_t floatpii=0;
	float64_t float0=0;
	float64_t float00001=0;
	int8_t hilf1=0;
	int8_t hilf2=0;
	int8_t hilf3=0;
	int8_t hilf4=0;
	
	for(;;) 
	{
	updateButtons();
	
		floatpii=f_sd(3.141595);
		float0=f_sd(0);
		float00001=f_sd(0.000005);
		hilf1=f_compare(f_sub(floatpii, Pi),float0);
		hilf2=f_compare(float00001,f_sub(floatpii, Pi));
		hilf3=f_compare(f_sub(Pi,floatpii),float0);
		hilf4=f_compare(float00001,f_sub(Pi,floatpii));
		if((hilf3>0&&hilf4>0)||(hilf1>0&&hilf2>0)){
			zeitlauft=0;
		}
		char* tempResultString = f_to_string(Pi, 16, 16);		
		sprintf(Pistring, "%s", tempResultString);				
		vDisplayClear();										
		vDisplayWriteStringAtPos(0,0,"Pi_Leibniz");				
		vDisplayWriteStringAtPos(1,0,"%s", Pistring);				
		vDisplayWriteStringAtPos(3,0,"Zeit:%ds", tempResultString);	
		
		vTaskDelay(pdMS_TO_TICKS(500));
		//vTaskDelay(500 / portTICK_RATE_MS);		
	}
}

void vButtonTask(void *pvParameters) {
	(void) pvParameters;
	
	while (1) {
		updateButtons();
		
		if(getButtonPress(BUTTON1)== LONG_PRESSED)				
		{
			zeitlauft=1;										
			xEventGroupSetBits(xLeibnizeventgroup, StartStop);			
		}
		if(getButtonPress(BUTTON1)== SHORT_PRESSED)				
		{
			zeitlauft=0;										
			xEventGroupClearBits(xLeibnizeventgroup, StartStop);
		}		
  		if(getButtonPress(BUTTON2)== LONG_PRESSED)
  		{
  			zeitlauft=1;
  			xEventGroupSetBits(xMonteCarloeventgroup, StartStop);
  		}
  		if(getButtonPress(BUTTON2)== SHORT_PRESSED)
  		{
  			zeitlauft=0;
  			xEventGroupClearBits(xMonteCarloeventgroup, StartStop);			
  		}
		
		if(getButtonPress(BUTTON4)== SHORT_PRESSED)				
		{
			xEventGroupSetBits(xLeibnizeventgroup, Reset);			
		}
 		if(getButtonPress(BUTTON4)== SHORT_PRESSED)				
 		{
 			xEventGroupSetBits(xMonteCarloeventgroup, Reset);			
 		}
		
		vTaskDelay(10 / portTICK_RATE_MS);
	}
	
}

void vLeibnizTask(void *pvParameters) {
	(void) pvParameters;
	float64_t PiFloat1=0;
	float64_t PiFloat2=0;
	float64_t PiFloat3=0;
	float64_t PiFloat4=0;
	float64_t PiFloat5=0;
	float64_t PiFloat6=0;
	float64_t PiFloat7=0;
	for(;;)
	{

		if((xEventGroupGetBits(xLeibnizeventgroup)&1)==1)
		{
			if(counter%2)
			{
				PiFloat1=f_mult(PiFloat2,f_sd(2));
				PiFloat3=f_add(PiFloat1,f_sd(1));
				PiFloat4=f_div(f_sd(4.0),PiFloat3);
				Pi=f_sub(Pi, PiFloat4);
				PiFloat2=f_add(PiFloat2, f_sd(1));
				counter++;
			}
			else
			{
				PiFloat5=f_mult(PiFloat2,f_sd(2));
				PiFloat6=f_add(PiFloat5,f_sd(1));
				PiFloat7=f_div(f_sd(4.0),PiFloat6);
				Pi=f_add(Pi, PiFloat7);
				PiFloat2=f_add(PiFloat2, f_sd(1));
				counter++;
			}
			xEventGroupSetBits(xLeibnizeventgroup, Status);
		}
		else
		{
			xEventGroupClearBits(xLeibnizeventgroup, Status);

		}
		if((xEventGroupGetBits(xLeibnizeventgroup)&2)==2)
		{
			counter=0;
			PiFloat1=0;
			PiFloat2=0;
			PiFloat3=0;
			PiFloat4=0;
			PiFloat5=0;
			PiFloat6=0;
			PiFloat7=0;
			Pi=0;
			zeit=0;
			xEventGroupClearBits(xLeibnizeventgroup, Reset);
			
	
		}
	}
}

void vMonteCarloTask(void *pvParameters) {
	(void) pvParameters;

	srand( SEED );
	int i, count, n;
	double x,y,z,pi;
	double qn(int n, int max);
	counter = 0;
	
	while(1)
	{
	if((xEventGroupGetBits(xMonteCarloeventgroup)&1)==1)
		
		for(i = 0; i < n; ++i) 
		{

			x = (double)rand() / RAND_MAX;
			y = (double)rand() / RAND_MAX;
			z = x * x + y * y;
			if( z <= 1 ) count++;
		}

		pi = (double) count / n * 4;	
		double qn(int n, int max)
		{
			if (n==0)
			return 1.+1./qn(1,max);
			else if (n < max)
			return 2.+(2.*n+1.)*(2.*n+1.)/qn(n+1,max);
			else
			return 2.+2.*sqrt(n*n+n+1);
			double pi;
			{
			if (n==0) 
			
			pi = 4.0/qn(0,n);
			
			xEventGroupSetBits(xMonteCarloeventgroup, Status);
		}
			xEventGroupClearBits(xMonteCarloeventgroup, Status);
			
		
		}
	 }
 }
 
 
