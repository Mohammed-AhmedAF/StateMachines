#include "Macros.h"
#include "STD_TYPES.h"
#include "GPIO_interface.h"
#include "NVIC_interface.h"
#include "PWM_interface.h"
#include "SYSCNTRL_interface.h"

typedef enum
{
	STATE_RED,
	STATE_BLUE,
	STATE_GREEN
}state_t;

static state_t CurrentState;
static state_t PrevState;

void vidInitStateMachine(void);
void vidRunEntryAction(state_t state);
void vidProcessSwitch(void);

void vidProcessSwitch(void)
{
	PrevState = CurrentState;
	if (CurrentState == STATE_RED)
	{
		if (GPIO_u8GetInterruptStatus(GPIO_PORTF,GPIO_PIN0) == STD_HIGH)
		{
			GPIO_vidClearInterrupt(GPIO_PORTF,GPIO_PIN0);

			CurrentState = STATE_BLUE;
		}
		else
		{
			CurrentState = STATE_GREEN;
			GPIO_vidClearInterrupt(GPIO_PORTF,GPIO_PIN4);

		}
	}
	else if (CurrentState == STATE_BLUE)
	{
	
		if (GPIO_u8GetInterruptStatus(GPIO_PORTF,GPIO_PIN0) == STD_HIGH)
		{
			GPIO_vidClearInterrupt(GPIO_PORTF,GPIO_PIN0);
			CurrentState = STATE_GREEN;
		}
		else
		{
			GPIO_vidClearInterrupt(GPIO_PORTF,GPIO_PIN4);
			CurrentState = STATE_RED;

		}
	}
	else if (CurrentState == STATE_GREEN)
	{
		if (GPIO_u8GetInterruptStatus(GPIO_PORTF,GPIO_PIN0) == STD_HIGH)
		{
			GPIO_vidClearInterrupt(GPIO_PORTF,GPIO_PIN0);
			CurrentState = STATE_RED;
		}
		else
		{
			GPIO_vidClearInterrupt(GPIO_PORTF,GPIO_PIN4);
			CurrentState = STATE_BLUE;

		}		
	}
	if (CurrentState != PrevState)
	{
		vidRunEntryAction(CurrentState);
	}
}

void vidRunEntryAction(state_t state)
{
	switch(state)
	{
		case STATE_RED:
		GPIO_vidSetPinValue(GPIO_PORTF,GPIO_LED_RED,STD_HIGH);
		GPIO_vidSetPinValue(GPIO_PORTF,GPIO_LED_BLUE,STD_LOW);
		GPIO_vidSetPinValue(GPIO_PORTF,GPIO_LED_GREEN,STD_LOW);
		break;
		case STATE_BLUE:
		GPIO_vidSetPinValue(GPIO_PORTF,GPIO_LED_RED,STD_LOW);
		GPIO_vidSetPinValue(GPIO_PORTF,GPIO_LED_BLUE,STD_HIGH);
		GPIO_vidSetPinValue(GPIO_PORTF,GPIO_LED_GREEN,STD_LOW);
		break;
		case STATE_GREEN:
		GPIO_vidSetPinValue(GPIO_PORTF,GPIO_LED_RED,STD_LOW);
		GPIO_vidSetPinValue(GPIO_PORTF,GPIO_LED_BLUE,STD_LOW);
		GPIO_vidSetPinValue(GPIO_PORTF,GPIO_LED_GREEN,STD_HIGH);
		break;
	}
}

void vidInitStateMachine(void)
{
	
	CurrentState = STATE_RED;
	vidRunEntryAction(CurrentState);
}

int main(void)
{
	/*Enabling clock to used peripherals*/
	SYSCNTRL_vidEnableGPIOClock(SYSCNTRL_GPIO_PORTF);
	
	/*GPIO LEDs configuration*/
	GPIO_vidSetPinDirection(GPIO_PORTF,GPIO_LED_BLUE,GPIO_OUTPUT);
	GPIO_vidSetPinDigEnable(GPIO_PORTF,GPIO_LED_BLUE,GPIO_DEN_SET);
	
	GPIO_vidSetPinDirection(GPIO_PORTF,GPIO_LED_GREEN,GPIO_OUTPUT);
	GPIO_vidSetPinDigEnable(GPIO_PORTF,GPIO_LED_GREEN,GPIO_DEN_SET);
	
	GPIO_vidSetPinDirection(GPIO_PORTF,GPIO_LED_RED,GPIO_OUTPUT);
	GPIO_vidSetPinDigEnable(GPIO_PORTF,GPIO_LED_RED,GPIO_DEN_SET);
	
	/*GPIO Switch configuration*/
	GPIO_vidUnlock(GPIO_PORTF);
	GPIO_vidCommit(GPIO_PORTF,GPIO_PIN0);
	GPIO_vidCommit(GPIO_PORTF,GPIO_PIN4);
	
	GPIOConfig_t strctGPIOConfig_SW;
	strctGPIOConfig_SW.u8DigEnable = GPIO_DEN_SET;
	strctGPIOConfig_SW.u8Direction = GPIO_INPUT;
	strctGPIOConfig_SW.u8Port = GPIO_PORTF;
	strctGPIOConfig_SW.u8Pin = GPIO_PIN0;
	strctGPIOConfig_SW.u8PullUpRes = GPIO_PUR_ENABLED;
	strctGPIOConfig_SW.u8AlternateFunc = GPIO_ALTERFUNC_UNSET;
	
	GPIO_vidConfigurePin(&strctGPIOConfig_SW);
	strctGPIOConfig_SW.u8Pin = GPIO_PIN4;
	GPIO_vidConfigurePin(&strctGPIOConfig_SW);

	/*External interrupt configuration*/
	ExtInterruptConfig_t strctExtConfig_SW;
	strctExtConfig_SW.ptrFunc = vidProcessSwitch;
	strctExtConfig_SW.u8BothEdges = GPIO_INTERRUPT_EVENTCONTROLLED;
	strctExtConfig_SW.u8PullResistance = GPIO_PUR_ENABLED;
	strctExtConfig_SW.u8InterruptEvent = GPIO_EVENT_FALLINGEDGE;
	strctExtConfig_SW.u8InterruptSense = GPIO_SENSE_EDGE;
	
	GPIO_vidConfigInterrupt(GPIO_PORTF,GPIO_PIN0,&strctExtConfig_SW);
	GPIO_vidConfigInterrupt(GPIO_PORTF,GPIO_PIN4,&strctExtConfig_SW);

	/*Starting the state machine*/
	vidInitStateMachine();

	/*Enabling NVIC*/
	NVIC_vidSetInterrupt(NVIC_GPIOF);
	
	//__enable_irq();
	
	while(1);
}