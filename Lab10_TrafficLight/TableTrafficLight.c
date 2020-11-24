// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

#define DELAY_1MS 13333

// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

void PortBEF_Init(void);
void Delay(unsigned long);
void SysTick_Init(void);

// ***** 3. Subroutines Section *****

// Linked data structure
struct State {
  unsigned long OutTraffic; 
	unsigned long OutWalk;
  unsigned long Time;  
  unsigned long Next[8];}; 
typedef const struct State STyp;
#define goW    0
#define waitW  1
#define goN    2
#define waitN  3
#define walk   4
#define hurry1 5
#define hurry2 6
#define hurry3 7
#define hurry4 8
#define allRed 9
STyp FSM[10]={
	{0x0C, 0x02, 2000, {goW,   goW,   waitW, waitW, waitW, waitW, waitW, waitW}},  // go west
	{0x14, 0x02, 1000,  {goN,   goN,   goN,   goN,   walk,  walk,  goN,   goN}},    // wait west
	{0x21, 0x02, 2000, {goN,   waitN, goN,   waitN, waitN, waitN, waitN, waitN}},  // go north
	{0x22, 0x02, 1000,  {goW,   goW,   goW,   goW,   walk,  walk,  walk,  walk}},   // wait north
	{0x24, 0x08, 2000, {walk,  hurry1,hurry1,hurry1,hurry1,hurry1,hurry1,hurry1}}, // walk
	{0x24, 0x02, 500,  {hurry2,hurry2,hurry2,hurry2,hurry2,hurry2,hurry2,hurry2}}, // hurry1
	{0x24, 0x00, 500,  {hurry3,hurry3,hurry3,hurry3,hurry3,hurry3,hurry3,hurry3}}, // hurry2
	{0x24, 0x02, 500,  {hurry4,hurry4,hurry4,hurry4,hurry4,hurry4,hurry4,hurry4}}, // hurry3
	{0x24, 0x00, 500,  {allRed,goW,   goN,   goW,   allRed,goW,   goN,   goW}},    // hurry4
	{0x24, 0x02, 1000,  {allRed,goW,   goN,   goW,   walk,  goW,   goN,   goW}}     // all red
};
unsigned long currentState;  // index to the current state 
unsigned long input; 

int main(void){ 
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210,ScopeOff); // activate grader and set system clock to 80 MHz
	PortBEF_Init();
	SysTick_Init();
  
  EnableInterrupts();
	
	currentState = goW;
  while(1){
    // set traffic lights - PB
		GPIO_PORTB_DATA_R = FSM[currentState].OutTraffic;
		// set pedestrian lights - PF
		GPIO_PORTF_DATA_R = FSM[currentState].OutWalk;
		// wait
		Delay(FSM[currentState].Time);
		// read sensors - PE
		input = GPIO_PORTE_DATA_R;
		// go to next state
		currentState = FSM[currentState].Next[input];
  }
}

void PortBEF_Init(void)
{
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x32;           // 1) activate clock for Port B,E,F
  delay = SYSCTL_RCGC2_R;           // 2) no need to unlock
	
	GPIO_PORTF_AMSEL_R &= ~0x0A;      // 3) disable analog on PF3,1
  GPIO_PORTF_PCTL_R &= ~0x0000F0F0; // 4) enable regular GPIO
  GPIO_PORTF_DIR_R |= 0x0A;         // 5) outputs on PF3,1
  GPIO_PORTF_AFSEL_R &= ~0x0A;      // 6) regular function on PF3,1
  GPIO_PORTF_DEN_R |= 0x0A;         // 7) enable digital on PF3,1
	
	GPIO_PORTE_AMSEL_R &= ~0x07;      // 3) disable analog function on PE2-0
	GPIO_PORTE_PCTL_R &= ~0x00000FFF; // 4) enable regular GPIO
	GPIO_PORTE_DIR_R &= ~0x07;        // 5) inputs on PE2-0
	GPIO_PORTE_AFSEL_R &= ~0x07;      // 6) regular function on PE2-0
	GPIO_PORTE_DEN_R |= 0x07;         // 7) enable digital on PE2-0
	
	GPIO_PORTB_AMSEL_R &= ~0x3F;      // 3) disable analog function on PB5-0
	GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; // 4) enable regular GPIO
  GPIO_PORTB_DIR_R |= 0x3F;         // 5) outputs on PB5-0
  GPIO_PORTB_AFSEL_R &= ~0x3F;      // 6) regular function on PB5-0
  GPIO_PORTB_DEN_R |= 0x3F;         // 7) enable digital on PB5-0
}

// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void)
{
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = 0x00FFFFFF;        // maximum reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it             
  NVIC_ST_CTRL_R = 0x00000005;          // enable SysTick with core clock
}

void Delay(unsigned long msec)
{
	unsigned long i = msec * DELAY_1MS;
	while(i > 0)
		i--;
}
