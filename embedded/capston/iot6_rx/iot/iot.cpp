//=============================================================================
// iot.cpp
//	The implementation the iot package.
//		(INU Object Toolkit for the Internet of Things)
//	Created: 2015-01-09 오후 2:33:04
//	Author: kkim@inu.ac.kr / SDTLAB INU (Incheon National University)
//=============================================================================

#include "iot.h"

//-----------------------------------------------------------------------------
//	cplusplus -
//-----------------------------------------------------------------------------
//	This is applicable if using virtual inheritance.
int __cxa_guard_acquire(__guard *g) { return !*(char *)(g); };
void __cxa_guard_release (__guard *g) { *(char *) g = 1; };
void __cxa_guard_abort (__guard *) { };

//	This is applicable if using pure virtual inheritance.
void __cxa_pure_virtual(void) {	};

//	Operators required for C++
void *operator new(size_t size) { return malloc(size); }
void operator delete(void* ptr) { free(ptr); }

//-----------------------------------------------------------------------------
//	iotPin -
//-----------------------------------------------------------------------------
volatile uint8_t *iotPin::pPin[]
	= { &PINA, &PINB, &PINC, &PIND, &PINE, &PINF, &PING };
volatile uint8_t *iotPin::pPort[]
	= { &PORTA, &PORTB, &PORTC, &PORTD, &PORTE, &PORTF, &PORTG };
volatile uint8_t *iotPin::pDDR[]
	= { &DDRA, &DDRB, &DDRC, &PORTD, &DDRE, &DDRF, &DDRG };
		
volatile uint8_t *
iotPin::getpPin(iotPin::iotPortId id)
{
	return isValid(id) ? pPin[id] : NULL;
}

volatile uint8_t *
iotPin::getpPort(iotPin::iotPortId id)
{
	return isValid(id) ? pPort[id] : NULL;
}

volatile uint8_t *
iotPin::getpDDR(iotPin::iotPortId id)
{
	return isValid(id) ? pDDR[id] : NULL;
}

volatile uint8_t *
iotPin::getpPin(iotPin::iotPinId id)
{
	return isValid(id) ? pPin[id / iotcNumPinsPerPorts] : NULL;
}

volatile uint8_t *
iotPin::getpPort(iotPin::iotPinId id)
{
	return isValid(id) ? pPort[id / iotcNumPinsPerPorts] : NULL;
}

volatile uint8_t *
iotPin::getpDDR(iotPin::iotPinId id)
{
	return isValid(id) ? pDDR[id / iotcNumPinsPerPorts] : NULL;
}

//-----------------------------------------------------------------------------
//	iotTimer -
//-----------------------------------------------------------------------------
void	(* iotTimer::iCallback0) (void) = (void (*) (void)) NULL;
void	(* iotTimer::iCallback1) (void) = (void (*) (void)) NULL;
void	(* iotTimer::iCallback2) (void) = (void (*) (void)) NULL;
void	(* iotTimer::iCallback3) (void) = (void (*) (void)) NULL;

ISR(TIMER0_COMP_vect)
{
	if(iotTimer::iCallback0) (* iotTimer::iCallback0)();
}

ISR(TIMER1_COMPA_vect)
{
	if(iotTimer::iCallback1) (* iotTimer::iCallback1)();
}

ISR(TIMER2_COMP_vect)
{
	if(iotTimer::iCallback2) (* iotTimer::iCallback2)();
}

ISR(TIMER3_COMPA_vect)
{
	if(iotTimer::iCallback3) (* iotTimer::iCallback3)();
}

//-----------------------------------------------------------------------------
//	iotUart -
//-----------------------------------------------------------------------------
volatile unsigned char		iotUart::itx0Head = 0, iotUart::itx0Tail = 0;
volatile unsigned char		iotUart::irx0Head = 0, iotUart::irx0Tail = 0;
char						*iotUart::itx0Buff = (char *) NULL;
char						*iotUart::irx0Buff = (char *) NULL;

volatile unsigned char		iotUart::itx1Head = 0, iotUart::itx1Tail = 0;
volatile unsigned char		iotUart::irx1Head = 0, iotUart::irx1Tail = 0;
char						*iotUart::itx1Buff = (char *) NULL;
char						*iotUart::irx1Buff = (char *) NULL;

void	(* iotUart::irx0Callback) (void) = (void (*) (void)) NULL;
void	(* iotUart::irx1Callback) (void) = (void (*) (void)) NULL;

ISR(USART0_RX_vect)
{
	if((iotUart::irx0Head + 1) % iotcUartBuffSize != iotUart::irx0Tail) {
		// buffer is not full.
		iotUart::irx0Buff[iotUart::irx0Head] = UDR0;
		iotUart::irx0Head = (iotUart::irx0Head + 1) % iotcUartBuffSize;
	}	// else buffer overflows! - chars are being lost.
}

ISR(USART1_RX_vect)
{
	if((iotUart::irx1Head + 1) % iotcUartBuffSize != iotUart::irx1Tail) {
		// buffer is not full.
		iotUart::irx1Buff[iotUart::irx1Head] = UDR1;
		iotUart::irx1Head = (iotUart::irx1Head + 1) % iotcUartBuffSize;
	}	// else buffer overflows! - chars are being lost.
}

//-----------------------------------------------------------------------------
//	iotTerminal -
//-----------------------------------------------------------------------------
iotUart		*iotTerminal::iUart = (iotUart *) NULL;
char		iotTerminal::iLine1[iotcMaxLine], iotTerminal::iLine2[iotcMaxLine];
char		*iotTerminal::iLine = iotTerminal::iLine1,
			*iotTerminal::iLineToParse = (char *) NULL;
int			iotTerminal::iCurChar = 0;
uint8_t		iotTerminal::iHead = 0, iotTerminal::iTail = 0;
char		iotTerminal::iQueue[iotcCmdQueueSize][iotcMaxLine];

//-----------------------------------------------------------------------------
//	iotDht -
//-----------------------------------------------------------------------------
iotDht::iotDhtTrigger	iotDht::iTrigger[iotcDhtMaxTrigger];

//-----------------------------------------------------------------------------
//	iotServo -
//-----------------------------------------------------------------------------
iotTimer			*iotServo::iTimer = (iotTimer *) NULL;
int					iotServo::in10kCycles = 0;
int					iotServo::iAngle = iotcServoTicksAtZero;
volatile uint8_t	*iotServo::ipPort = NULL;
uint8_t				iotServo::iPinMask = 0;

//-----------------------------------------------------------------------------
//	iotTimeOut -
//-----------------------------------------------------------------------------
iotTimer		*iotTimeOut::iTimer = (iotTimer *) NULL;
unsigned int	iotTimeOut::in8mSecs = 0;  // # of 8 mili-seconds
unsigned int	iotTimeOut::iDeadLine;
void			(* iotTimeOut::iCallback) ();

//-----------------------------------------------------------------------------
//	iotFpSensor -
//-----------------------------------------------------------------------------
// T0: time to scan and process a finger print image (0.2~0.3 sec)
// Actual TimeOut = T0 * response(fpcCmdTimeOut);
// commands: timeout t/get, level l/get, users, sleep
//     add k, delete k/all, match u/any
iotUart					*iotFpSensor::iUart = (iotUart *) NULL;
iotFpSensor::fpState	iotFpSensor::iCurState = fpcStateIdle;
iotFpSensor::fpCommand	iotFpSensor::iCurCommand = fpcCmdNone;

uint8_t		iotFpSensor::iRxPktPtr = 0;
uint8_t		iotFpSensor::iRxPacket[fpcPktSize];
uint8_t		iotFpSensor::iTxPacket[fpcPktSize]
				= { fpcPktHead, 0, 0, 0, 0, 0, 0, fpcPktTail };

//-----------------------------------------------------------------------------
//	iotClock -
//-----------------------------------------------------------------------------
iotTimer	*iotClock::iTimer = (iotTimer *) NULL;
long int	iotClock::iSeconds = 0;	// 14472000 for Jun-17 10:00:00
char		iotClock::iMonDyHrMnSc[16] = "Jan-01 00:00:00";
iotCharLcd	*iotClock::iCharLcd = (iotCharLcd *) NULL;
uint8_t		iotClock::iRow = 1;
char		iotClock::iFlush = 0;
int			iotClock::iYearDays = -1;
const char	*iotClock::iMonth[12] = {
				"Jan", "Feb", "Mar", "Apr", "May", "Jun",
				"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
			};
char		iotClock::iMonthDays[12] = {	// not good for leap years.
				31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
			};
bool		iotClock::iCharLcdCreated = false;
char		iotClock::lcdmode = 1 ;

iotClock::iotSchedule	iotClock::iSchedule[iotcMaxSchedules];

//-----------------------------------------------------------------------------
//	iotDustSensor -
//-----------------------------------------------------------------------------
volatile uint8_t	*iotDustSensor::ipiLed;
iotPin::iotPinId	iotDustSensor::iaOut;
uint8_t				iotDustSensor::iiLedPin;
int8_t				iotDustSensor::iCount = -1;
uint16_t			iotDustSensor::iBuf[iotcDsBufSize], iotDustSensor::iSum;


//-----------------------------------------------------------------------------
//	LED
//-----------------------------------------------------------------------------


volatile uint8_t	*iotLed::ipPort = NULL;
uint8_t				iotLed::iPinMask = 0;


//-----------------------------------------------------------------------------
//	SUBLED
//-----------------------------------------------------------------------------


volatile uint8_t	*iotSubLed::ipPort = NULL;
uint8_t				iotSubLed::iPinMask = 0;