//=============================================================================
// iot.h
//	The header of the iot package.
//		(INU Object Toolkit for the Internet of Things)
//	Created: 2015-01-09 오후 2:33:04
//	Author: kkim@inu.ac.kr / SDTLAB INU (Incheon National University)
//============================================================================= 

#ifndef IOT_H_
#define IOT_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

//-----------------------------------------------------------------------------
//	cplusplus -
//-----------------------------------------------------------------------------

//	This is applicable if using virtual inheritance.
__extension__ typedef int __guard __attribute__((mode (__DI__)));
 
extern "C" int __cxa_guard_acquire(__guard *);
extern "C" void __cxa_guard_release (__guard *);
extern "C" void __cxa_guard_abort (__guard *);
 
//	This is applicable if using pure virtual inheritance.
extern "C" void __cxa_pure_virtual(void); 

//	Operators required for C++
void *operator new(size_t size);
void operator delete(void *size);

//-----------------------------------------------------------------------------
//	INU Object Toolkit C++ Version -
//-----------------------------------------------------------------------------
#define		F_CPU							16000000UL

#define		iotmAtomic(x)	{ char _sreg = SREG; cli(); x; SREG = _sreg; }

#define		min(a,b)	((a)<(b)?(a):(b))
#define		max(a,b)	((a)>(b)?(a):(b))

//-----------------------------------------------------------------------------
//	iotPin -
//-----------------------------------------------------------------------------
#define		iotcNumPinsPerPorts		8
class iotPin {
	public:
		// iotPortId: PA, PB, ...
		typedef enum {
			iotcPA, iotcPB, iotcPC, iotcPD,
			iotcPE, iotcPF, iotcPG, iotcNumPorts
		} iotPortId;
		// iotPinId: PA0, PA1, ..., PB0, PB2, ...
		typedef enum {
			iotcPA0, iotcPA1, iotcPA2, iotcPA3,
			iotcPA4, iotcPA5, iotcPA6, iotcPA7,
			iotcPB0, iotcPB1, iotcPB2, iotcPB3,
			iotcPB4, iotcPB5, iotcPB6, iotcPB7,
			iotcPC0, iotcPC1, iotcPC2, iotcPC3,
			iotcPC4, iotcPC5, iotcPC6, iotcPC7,
			iotcPD0, iotcPD1, iotcPD2, iotcPD3,
			iotcPD4, iotcPD5, iotcPD6, iotcPD7,
			iotcPE0, iotcPE1, iotcPE2, iotcPE3,
			iotcPE4, iotcPE5, iotcPE6, iotcPE7,
			iotcPF0, iotcPF1, iotcPF2, iotcPF3,
			iotcPF4, iotcPF5, iotcPF6, iotcPF7,
			iotcPG0, iotcPG1, iotcPG2, iotcPG3,
			iotcPG4, iotcNumPins
		} iotPinId;
		
		static volatile uint8_t *getpPort(iotPortId id);
		static volatile uint8_t *getpPin(iotPortId id);
		static volatile uint8_t *getpDDR(iotPortId id);
		
		static volatile uint8_t *getpPort(iotPinId id);
		static volatile uint8_t *getpPin(iotPinId id);
		static volatile uint8_t *getpDDR(iotPinId id);
		
		static inline bool		isValid(iotPortId id) {
			return (id >= 0 && id < iotcNumPorts);
		}
		static inline bool		isValid(iotPinId id) {
			return (id >= 0 && id < iotcNumPins);
		}
		static inline uint8_t	getPinMask(iotPinId pin) {
			return 1 << (pin % iotcNumPinsPerPorts);		//iotcNumPinsPerPorts = 8;0~7
		}

		// PINA, PINB, ... // PORTA, PORTB, ... // DDRA, DDRB, ...
		static volatile uint8_t	*pPin[7];
		static volatile uint8_t	*pPort[7];
		static volatile uint8_t	*pDDR[7];
};

//-----------------------------------------------------------------------------
//	iotTimer -
//-----------------------------------------------------------------------------
class iotTimer {
	public:
		// iotTimerId: Timer0, TImer1, ...
		typedef enum {
			iotcTimerNone, iotcTimer0, iotcTimer1, iotcTimer2, iotcTimer3
		} iotTimerId;
			// iotTimerPsd: values of pre-scaler divisors (1, 8, 32, ...)
		typedef enum {		// 32 & 128 only for 8-bit counters 0 & 2.
			iotcPsdNo, iotcPsd1, iotcPsd8, iotcPsd32, iotcPsd64, iotcPsd128,
			iotcPsd256, iotcPsd1024
		} iotTimerPsd;
		iotTimer(iotTimerId aId, iotTimerPsd aPsd, unsigned int aCount) {
			iId = iotcTimerNone;
			int		psd = 0;
			if(aId == iotcTimer0) {
				switch(aPsd) {
					case iotcPsdNo: psd = 0; break;
					case iotcPsd1: psd = 1; break;
					case iotcPsd8: psd = 2; break;
					case iotcPsd32: psd = 3; break;
					case iotcPsd64: psd = 4; break;
					case iotcPsd128: psd = 5; break;
					case iotcPsd256: psd = 6; break;
					case iotcPsd1024: psd = 7; break;
					default: return;
				}
			} else { // iotcTimer1, 2, | 3
				switch(aPsd) {
					case iotcPsdNo: psd = 0; break;
					case iotcPsd1: psd = 1; break;
					case iotcPsd8: psd = 2; break;
					case iotcPsd64: psd = 3; break;
					case iotcPsd256: psd = 4; break;
					case iotcPsd1024: psd = 5; break;
					default: return;
				}
			}
			switch(aId) {
				case iotcTimer0:
					if((TCCR0 & 0x07) != 0) return;
					TCNT0 = 0; OCR0 = aCount & 0xff; 
					TCCR0 = (TCCR0 & ~(0x07)) | (1 << WGM01) | psd;
					break;
				case iotcTimer1:
					if((TCCR1B & 0x07) != 0) return;
					TCNT1 = 0; OCR1A = aCount; TCCR1A = 0;
					TCCR1B = (TCCR1B & ~(0x07)) | (1 << WGM12) | psd;
					break;
				case iotcTimer2:
					if((TCCR2 & 0x07) != 0) return;
					TCNT2 = 0; OCR2 = aCount & 0xff;
					TCCR2 = (TCCR2 & ~(0x07)) | (1 << WGM21) | psd;
					break;
				case iotcTimer3:
					if((TCCR3B & 0x07) != 0) return;
					TCNT3 = 0; OCR3A = aCount; TCCR3A = 0;
					TCCR3B = (TCCR3B & ~(0x07)) | (1 << WGM32) | psd;
					break;
				default: return;
			}
			iId = aId;
//			printf("Timer%d created.\n\r", iId - 1);
		}
		~iotTimer() {
			switch(iId) {
				case iotcTimer0: TCCR0 &= ~0x07; TIMSK &= ~(1 << OCIE0);
					iCallback0 = (void (*)(void)) NULL; break;
				case iotcTimer1: TCCR1B &= ~0x07; TIMSK &= ~(1 << OCIE1A);
					iCallback1 = (void (*)(void)) NULL; break;
				case iotcTimer2: TCCR2 &= ~0x07; TIMSK &= ~(1 << OCIE2);
					iCallback2 = (void (*)(void)) NULL; break;
				case iotcTimer3: TCCR3B &= ~0x07; ETIMSK &= ~(1 << OCIE3A);
					iCallback3 = (void (*)(void)) NULL; break;
				default: break;
			}
			iId = iotcTimerNone;
		}
		iotTimerId getId()		{ return(iId); }
		bool isValid()	{ return(iId != iotcTimerNone); }
		
		static void
		delay(int cnt) {	// about cnt * 15 usec
			for(volatile int i = 0; i < cnt; i++) {
				for(volatile int j = 0; j < 6; j++) ;
			}
		}
		void
		configure(iotTimerPsd aPsd, unsigned int aCount) {
			unsigned char	psd = 0;
			if(iId == iotcTimer0) {
				switch(aPsd) {
					case iotcPsdNo: psd = 0; break;
					case iotcPsd1: psd = 1; break;
					case iotcPsd8: psd = 2; break;
					case iotcPsd32: psd = 3; break;
					case iotcPsd64: psd = 4; break;
					case iotcPsd128: psd = 5; break;
					case iotcPsd256: psd = 6; break;
					case iotcPsd1024: psd = 7; break;
					default: return;
				}
			} else { // iotcTimer1, 2, | 3
				switch(aPsd) {
					case iotcPsdNo: psd = 0; break;
					case iotcPsd1: psd = 1; break;
					case iotcPsd8: psd = 2; break;
					case iotcPsd64: psd = 3; break;
					case iotcPsd256: psd = 4; break;
					case iotcPsd1024: psd = 5; break;
					default: return;
				}
			}
			switch(iId) {
				case iotcTimer0:
					TCNT0 = 0; OCR0 = aCount & 0xff;
					TCCR0 = (1 << WGM01) | psd;
					break;
				case iotcTimer1:
					TCCR1A = 0; TCCR1B = 0; TCNT1 = 0; OCR1A = aCount;
					TCCR1B = (1 << WGM12) | psd;
					break;
				case iotcTimer2:
					TCNT2 = 0; OCR2 = aCount & 0xff;
					TCCR2 = (1 << WGM21) | psd;
					break;
				case iotcTimer3:
					TCCR3A = 0; TCCR3B = 0; TCNT3 = 0; OCR3A = aCount;
					TCCR3B = (1 << WGM32) | psd;
					break;
				default: return;
			}
		}
		void
		enableInterrupt(void (* aCallback) (void)) {
			switch(iId) {
				case iotcTimer0:
					iCallback0 = aCallback; TIMSK |= (1 << OCIE0); break;
				case iotcTimer1:
					iCallback1 = aCallback; TIMSK |= (1 << OCIE1A); break;
				case iotcTimer2:
					iCallback2 = aCallback; TIMSK |= (1 << OCIE2); break;
				case iotcTimer3:
					iCallback3 = aCallback; ETIMSK |= (1 << OCIE3A); break;
				default: break;
			}
		}
		void
		disableInterrupt() {
			switch(iId) {
				case iotcTimer0: TIMSK &= ~(1 << OCIE0); break;
				case iotcTimer1: TIMSK &= ~(1 << OCIE1A); break;
				case iotcTimer2: TIMSK &= ~(1 << OCIE2); break;
				case iotcTimer3: ETIMSK &= ~(1 << OCIE3A); break;
				default: break;
			}
		}
		void
		configureOneThirdDuty(int aKHz) {
			// WGMn3:0 = 10. PWM, Phase Correct.
			// the counter repeatedly counts from bottom to top, 
			// and then from top to bottom.
			// OCnA is cleared on the compare match while up counting,
			// and set on the compare match while down counting.
			// TOP = ICRn, Update OCRnA at TOP.
			switch(iId) {
				case iotcTimer1: {
					const uint16_t	pwmVal = F_CPU / 2000 / aKHz;
					ICR1 = pwmVal; OCR1A = pwmVal / 3;
					TCCR1A = (1 << WGM11); TCCR1B = (1 << WGM13) | (1 << CS10);
					break;
				}
				case iotcTimer3: {
					const uint16_t	pwmVal = F_CPU / 2000 / aKHz;
					ICR3 = pwmVal; OCR3A = pwmVal / 3;
					TCCR3A = (1 << WGM31); TCCR3B = (1 << WGM33) | (1 << CS30);
					break;
				}
				default: break;
			}
		}
		void
		enablePWMOut() {
			// COMnA1:0 = 2 with WGMn3:0 = 10.
			// clear OCnA on compare match when up-counting.
			// set OCnA on compare match when down-counting.
			switch(iId) {
				case iotcTimer0: TCCR0 |= (1 << COM01); break;
				case iotcTimer1: TCCR1A |= (1 << COM1A1); break;
				case iotcTimer2: TCCR2 |= (1 << COM21); break;
				case iotcTimer3: TCCR3A |= (1 << COM3A1); break;
				default: break;
			}
		}
		void
		disablePWMOut() {
			// COMnA1:0 = 0 with WGM3:0 = 10.
			// OCnA disconnected.
			switch(iId) {
				case iotcTimer0: TCCR0 &= ~(1 << COM01); break;
				case iotcTimer1: TCCR1A &= ~(1 << COM1A1); break;
				case iotcTimer2: TCCR2 &= ~(1 << COM21); break;
				case iotcTimer3: TCCR3A &= ~(1 << COM3A1); break;
				default: break;
			}
		}
		static bool
		isAvailable(int aId) {
			switch(aId) {
				case iotcTimer0: if((TCCR0 & 0x07) == 0) return(true);
				case iotcTimer1: if((TCCR1B & 0x07) == 0) return(true);
				case iotcTimer2: if((TCCR2 & 0x07) == 0) return(true);
				case iotcTimer3: if((TCCR3B & 0x07) == 0) return(true);
				default: break;
			}
			return(false);
		}

	private:
		iotTimerId		iId;

	public:
		static	void	(* iCallback0) (void);
		static	void	(* iCallback1) (void);
		static	void	(* iCallback2) (void);
		static	void	(* iCallback3) (void);
};

//-----------------------------------------------------------------------------
//	iotAnalog -
//-----------------------------------------------------------------------------
class iotAnalog {
	public:
		static void
		init() {
			ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
		}
		static uint16_t
		read(iotPin::iotPinId pin) {
			if((pin < iotPin::iotcPF0) || (pin > iotPin::iotcPF7)) return 0;
			// 0x40: REFS1:0 = 01 = AVCC with external capacitor at AREF pin.
			// ADLAR == 0: Right Adjusted Result
			ADMUX  = 0x40 | (pin - iotPin::iotcPF0);
			ADCSRA |= (1 << ADSC);
			while(!(ADCSRA & (1 << ADIF))) ;
			ADCSRA |= (1 << ADIF);
			return ADC;
		}
};

//-----------------------------------------------------------------------------
//	iotCharLcd -
//-----------------------------------------------------------------------------
#define		iotcCharLcdControl			PORTG
#define		iotcCharLcdData				PORTA

#define		iotmCharLcdEnOn		(iotcCharLcdControl = (iControl |= 0x10))
#define		iotmCharLcdEnOff	(iotcCharLcdControl = (iControl &= 0x0f))
#define		iotmCharLcdRwOn		(iotcCharLcdControl = (iControl |= 0x08))
#define		iotmCharLcdRwOff	(iotcCharLcdControl = (iControl &= 0x17))
#define		iotmCharLcdRsOn		(iotcCharLcdControl = (iControl |= 0x04))
#define		iotmCharLcdRsOff	(iotcCharLcdControl = (iControl &= 0x1b))

#define		iotcCharLcdQueueSize		100
#define		iotcCharLcdColumns			16

class iotCharLcd {
	public:
		class iotCharLcdOut {
			public:
				unsigned char	x, y;
				char			c;
		};

		iotCharLcd() {
			iHead = iTail = iX = iY = 0;
			
			// DDRs to output
			*(&iotcCharLcdControl - 1) = 0x1c;
			*(&iotcCharLcdData - 1) = 0xff;
			
			writeControl(0x38);	// 8-bit data, 2 lines
			writeControl(0x0e);	// display on, cursor on
			writeControl(0x06);	// increment address for the next data.
			writeControl(0x01);	// cursor clear.
			iotTimer::delay(100);
			writeControl(0x02);	// cursor home.
			iotTimer::delay(100);
		}
		~iotCharLcd()	{ }
			
		void
		print(unsigned char y, unsigned char x, const char *s) {
			while(*s) print(y, x++, *s++);
		}
		void
		print(unsigned char y, unsigned char x, char c) {
			if((iHead + 1) % iotcCharLcdQueueSize == iTail)
				return;	// buffer full

			volatile iotCharLcdOut	*out = iQueue + iHead;
			out->x = x; out->y = y; out->c = c;

			iotmAtomic(iHead = (iHead + 1) % iotcCharLcdQueueSize);
		}
		void
		processOne() {
			if(iTail == iHead) return;		// buffer empty.

			volatile iotCharLcdOut	*out = iQueue + iTail;
			if((out->x != iX) || (out->y != iY))
				goTo(out->x, out->y);
			iX = (out->x + 1) % iotcCharLcdColumns;
			iY = out->y;
			writeData(out->c);

			iotmAtomic(iTail = (iTail + 1) % iotcCharLcdQueueSize);
		}

	private:
		volatile iotCharLcdOut	iQueue[iotcCharLcdQueueSize];
		volatile unsigned char	iHead, iTail;
		
		volatile unsigned char	iX, iY;
		char			iControl;

		void
		goTo(unsigned char aX, unsigned char aY) {
			if(aY == 1) {
				writeControl(0x80 + aX - 1); iotTimer::delay(1);	
			} else if (aY == 2) {
				writeControl(0xc0 + aX - 1); iotTimer::delay(1);
			}
		}
		void
		writeControl(char value) {
			iotmCharLcdEnOff; iotTimer::delay(1);
			iotmCharLcdRsOff; iotTimer::delay(1);
			iotmCharLcdRwOff; iotTimer::delay(1);
			iotmCharLcdEnOn;  iotTimer::delay(1);
			iotcCharLcdData = value; iotTimer::delay(1);
			iotmCharLcdEnOff; iotTimer::delay(1);
		}
		void
		writeData(char value) {
			iotmCharLcdEnOff; iotTimer::delay(1);
			iotmCharLcdRsOn;  iotTimer::delay(1);
			iotmCharLcdRwOff; iotTimer::delay(1);
			iotmCharLcdEnOn;  iotTimer::delay(1);
			iotcCharLcdData = value; iotTimer::delay(1);
			iotmCharLcdEnOff; iotTimer::delay(1);
		}
};

//-----------------------------------------------------------------------------
//	iotUart -
//-----------------------------------------------------------------------------
#define		iotcUartBuffSize		128

#define		iotmBaudRate(rate)		(F_CPU / 8 / (rate) - 1)
#define		iotmBaudRateHigh(rate)	((iotmBaudRate(rate) >> 8) & 0xff)
#define		iotmBaudRateLow(rate)	(iotmBaudRate(rate) & 0xff)

class iotUart {
	public:
		// iotUartId: Uart0 & Uart1
		typedef enum {
			iotcUartNone, iotcUart0, iotcUart1
		} iotUartId;
		iotUart(iotUartId aId, long aBaudRate, void (* arxCallback) (void)) {
			iId = iotcUartNone;
			if(aId == iotcUart0) {
				if(itx0Buff) return;
				itx0Head = itx0Tail = irx0Head = irx0Tail = 0;
				itx0Buff = (char *) malloc(iotcUartBuffSize);
				irx0Buff = (char *) malloc(iotcUartBuffSize);
				
				UCSR0B = 0x00; UCSR0A = 0x02; UCSR0C = 0x06;
				
				UBRR0L = iotmBaudRateLow(aBaudRate);
				UBRR0H = iotmBaudRateHigh(aBaudRate);

				irx0Callback = arxCallback;
				UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
			} else if(aId == iotcUart1) {
				if(itx1Buff) return;
				itx1Head = itx1Tail = irx1Head = irx1Tail = 0;
				itx1Buff = (char *) malloc(iotcUartBuffSize);
				irx1Buff = (char *) malloc(iotcUartBuffSize);
				
				UCSR1B = 0x00; UCSR1A = 0x02; UCSR1C = 0x06;
				
				UBRR1L = iotmBaudRateLow(aBaudRate);
				UBRR1H = iotmBaudRateHigh(aBaudRate);
				
				irx1Callback = arxCallback;
				UCSR1B = (1 << RXCIE1) | (1 << RXEN1) | (1 << TXEN1);
			} else return;
			iId = aId;
		}
		~iotUart() {
			if(iId == iotcUart0) {
				UCSR0B = 0x00;
				free(itx0Buff);
				free(irx0Buff);
				itx0Buff = irx0Buff = (char *) NULL;
			} else if(iId == iotcUart1) {
				UCSR1B = 0x00;
				free(itx1Buff);
				free(irx1Buff);
				itx1Buff = irx1Buff = (char *) NULL;
			}
		}
		bool isValid()	{ return(iId != iotcUartNone); }

		void
		processOne(void) {
			if(irx0Tail != irx0Head) {	// buffer not empty
				if(irx0Callback) (* irx0Callback)();
			}
			if(irx1Tail != irx1Head) {	// buffer not empty
				if(irx1Callback) (* irx1Callback)();
			}
			if((itx0Tail != itx0Head) && (UCSR0A & (1 << UDRE0))) {
				// buffer is not empty but data register is empty.
				UDR0 = itx0Buff[itx0Tail];
				iotmAtomic(itx0Tail = (itx0Tail + 1) % iotcUartBuffSize);
			}
			if((itx1Tail != itx1Head) && (UCSR1A & (1 << UDRE1))) {
				// buffer is not empty but data register is empty.
				UDR1 = itx1Buff[itx1Tail];
				iotmAtomic(itx1Tail = (itx1Tail + 1) % iotcUartBuffSize);
			}
		}
		int
		putChar(char c) {
			if(iId == iotcUart0) {
				if(itx0Head == itx0Tail) {
					// buffer empty
					if(UCSR0A & (1 << UDRE)) {		// register empty
						UDR0 = c;
						return(c);	
					}
				} else if((itx0Head + 1) % iotcUartBuffSize == itx0Tail) {
					// buffer full
					// wait until the data register becomes empty.
					while(!(UCSR0A & (1 << UDRE))) ;
					UDR0 = itx0Buff[itx0Tail];
					iotmAtomic(itx0Tail = (itx0Tail + 1) % iotcUartBuffSize);
				}
				itx0Buff[itx0Head] = c;
				iotmAtomic(itx0Head = (itx0Head + 1) % iotcUartBuffSize);
			} else if(iId == iotcUart1) {
				if(itx1Head == itx1Tail) {
					// buffer empty
					if(UCSR1A & (1 << UDRE)) {		// register empty
						UDR1 = c;
						return(c);
					}
				} else if((itx1Head + 1) % iotcUartBuffSize == itx1Tail) {
					// buffer full
					// wait until the data register becomes empty.
					while(!(UCSR1A & (1 << UDRE))) ;
					UDR1 = itx1Buff[itx1Tail];

					iotmAtomic(itx1Tail = (itx1Tail + 1) % iotcUartBuffSize);
				}
				itx1Buff[itx1Head] = c;
				iotmAtomic(itx1Head = (itx1Head + 1) % iotcUartBuffSize);
			} else return 0;
			return(c);
		}
		int
		peekChar(void) {
			if(iId == iotcUart0) {
				if(irx0Tail != irx0Head) return(irx0Buff[irx0Tail]);
			} else if(iId == iotcUart1) {
				if(irx1Tail != irx1Head) return(irx1Buff[irx1Tail]);
			}
			return(-1);
		}
		int
		getChar(void) {
			char	c;
			if(iId == iotcUart0) {
				if(irx0Tail == irx0Head) {	// buffer empty
					// wait until the data is received.
					while((UCSR0A & (1 << RXC0)) == 0) ;
					return(UDR0);
				}
				c = irx0Buff[irx0Tail];
				iotmAtomic(irx0Tail = (irx0Tail + 1) % iotcUartBuffSize);
			} else if(iId == iotcUart1) {
				if(irx1Tail == irx1Head) {	// buffer empty
					// wait until the data is received.
					while((UCSR1A & (1 << RXC1)) == 0) ;
					return(UDR1);
				}
				c = irx1Buff[irx1Tail];
				iotmAtomic(irx1Tail = (irx1Tail + 1) % iotcUartBuffSize);
			} else return(-1);
			return(c);
		}

	private:
		iotUartId	iId;

	public:
		static volatile unsigned char	itx0Head, itx0Tail, irx0Head, irx0Tail;
		static char						*itx0Buff, *irx0Buff;
		static volatile unsigned char	itx1Head, itx1Tail, irx1Head, irx1Tail;
		static char						*itx1Buff, *irx1Buff;
		
		static	void	(* irx0Callback) (void);
		static	void	(* irx1Callback) (void);
};

//-----------------------------------------------------------------------------
//	Terminal -
//		command line interface.
//		non-blocking i/o functions getLine() for read and printf() for write.
//		scanf() may block - not recommended for use.
//		a command line queue is used to handle the commands issued by
//		synchronous functions such as parseLine() functions and their callees.
//		the last line in the queue is left unused intentionally. since the
//		queue buffer is wrapped around, the last line may be the one currently
//		being executed, and should not be overwritten by a new one, yet.
//-----------------------------------------------------------------------------
#define		iotcMaxLine				50
#define		iotcCmdQueueSize		4

class iotTerminal {
	public:
		static void
		init(iotUart::iotUartId id, long baud = 115200) {
			iUart = new iotUart(id, baud, receiveLine);
			fdevopen(putChar, getChar);
		}
		static void
		destroy() {
			delete iUart;
			iUart = (iotUart *) NULL;
		}
		static char	*
		getLine() {
			char	*s;
			if(iTail != iHead) {	// queue not empty.
				s = iQueue[iTail];
				iTail = (iTail + 1) % iotcCmdQueueSize;
			} else if((s = iLineToParse)) iLineToParse = (char *) NULL;
			return s;
		}
		static bool
		enqueueLine(char *s) {
			if((iHead + 1) % iotcCmdQueueSize == iTail) return false;	// full
			strcpy(iQueue[iHead], s);
			iHead = (iHead + 1) % iotcCmdQueueSize;
			return true;
		}
		static void
		processOne() {
			if(iUart) iUart->processOne();
		}
		static int
		putChar(char c, FILE *f)
		{
			if(iUart) return(iUart->putChar(c));
			return(-1);
		}
		static int
		getChar(FILE *f)
		{
			if(iUart) return(iUart->getChar());
			return(-1);
		}
		static void
		receiveLine() {
			if(!iUart) return;
			
			int		c;
			while(iUart->peekChar() > -1) {
				c = iUart->getChar();
				if(c == '\n') {
					iLine[iCurChar] = '\0';
					iLineToParse = iLine;
					iLine = (iLine == iLine1) ? iLine2 : iLine1;
					iCurChar = 0;
				} else if(iCurChar < iotcMaxLine - 2) {
					if(c != '\r') iLine[iCurChar++] = c;
				} else iCurChar = 0;	// line overflow - thrown away.
			}
		}
		
	private:
		static iotUart		*iUart;
		static char			iLine1[iotcMaxLine], iLine2[iotcMaxLine];
		static char			*iLine, *iLineToParse;
		static int			iCurChar;
		static uint8_t		iHead, iTail;
		static char			iQueue[iotcCmdQueueSize][iotcMaxLine];
};

//-----------------------------------------------------------------------------
//	TimeOut -
//-----------------------------------------------------------------------------
class iotTimeOut {
	public:
		static void
		init(iotTimer::iotTimerId id) {
			if(!iTimer)
				iTimer = new iotTimer(id, iotTimer::iotcPsdNo, 125);
		}
		static void
		destroy() {
			if(iTimer) {
				delete iTimer;
				iTimer = (iotTimer *) NULL;
			}
		}
		static void
		remove() {
			if(iTimer) {
				iTimer->disableInterrupt();
				iTimer->configure(iotTimer::iotcPsdNo, 125);
			}
		}
		static void
		tick8msec() {
			if(++in8mSecs == iDeadLine) {
				remove();
				(* iCallback)();
			}
		}
		static void
		add(unsigned int deadline, void (* callback)()) {
			if(iTimer) {
				iTimer->configure(iotTimer::iotcPsd1024, 125); // 8 msec
				iTimer->enableInterrupt(tick8msec);
			}
			iDeadLine = deadline;
			iCallback = callback;
			in8mSecs = 0;
		}
	private:
		static iotTimer			*iTimer;
		static unsigned int		in8mSecs;	// # of 8 mili-seconds
		static unsigned int		iDeadLine;
		static void				(* iCallback) ();
};

//-----------------------------------------------------------------------------
//	iotDht -
//-----------------------------------------------------------------------------
#define iotcDhtOk					0
#define iotcDhtErrorCheckSum		-1
#define iotcDhtErrorTimeOut			-2
#define iotcDhtErrorConnect			-3
#define iotcDhtErrorAckL			-4
#define iotcDhtErrorAckH			-5

// 1200 = 18 msec / 15 usec, 67 = 1 msec / 15 usec
#define iotcDhtDht11WakeUp			1200
#define iotcDhtDhtWakeUp			67

#define iotcDhtDht11LeadingZeros	1
#define iotcDhtDhtLeadingZeros		6

// max timeout is 100 usec.
// For a 16 Mhz proc 100 usec is 1600 clock cycles
// loops using iotcDhtTimeout use at least 4 clock cycli
// so 100 us takes max 400 loops
// so by dividing F_CPU by 40000 we "fail" as fast as possible
#define iotcDhtTimeOut				400 // (F_CPU/40000)

#define	iotcDhtMaxTrigger			3

class iotDht {
	public:
		iotDht(iotPin::iotPinId pin) {
			ipPort = iotPin::getpPort(pin);
			ipPin = iotPin::getpPin(pin);
			ipDDR = iotPin::getpDDR(pin);
			iPinMask = iotPin::getPinMask(pin);
			for(iotDhtTrigger *t = iTrigger;
				t < iTrigger + iotcDhtMaxTrigger; t++) t->iType = iotcDhtNone;
		}
		int8_t read11() {
			// READ VALUES
			int8_t result = readSensor(
				iotcDhtDht11WakeUp, iotcDhtDht11LeadingZeros);
			if(result != iotcDhtOk) return result;

			// these bits are always zero, masking them reduces errors.
			iBytes[0] &= 0x7F;
			iBytes[2] &= 0x7F;

			// CONVERT AND STORE
			iHumidity    = iBytes[0];  // iBytes[1] == 0;
			iTemperature = iBytes[2];  // iBytes[3] == 0;

			// TEST CHECKSUM
			// iBytes[1] && iBytes[3] both 0
			uint8_t sum = iBytes[0] + iBytes[2];
			if (iBytes[4] != sum) return iotcDhtErrorCheckSum;
			return result;
		}
		int8_t read() {
			// READ VALUES
			int8_t result = readSensor(
				iotcDhtDhtWakeUp, iotcDhtDhtLeadingZeros);

			// these bits are always zero, masking them reduces errors.
			iBytes[0] &= 0x03;
			iBytes[2] &= 0x83;

			// CONVERT AND STORE
			iHumidity = word(iBytes[0], iBytes[1]) * 0.1;
			iTemperature = word(iBytes[2] & 0x7F, iBytes[3]) * 0.1;
			if (iBytes[2] & 0x80)	// negative temperature
				iTemperature = -iTemperature;
			// TEST CHECKSUM
			uint8_t sum = iBytes[0] + iBytes[1] + iBytes[2] + iBytes[3];
			if (iBytes[4] != sum) return iotcDhtErrorCheckSum;
			return result;
		}

		inline int8_t read21() { return read(); };
		inline int8_t read22() { return read(); };
		inline int8_t read33() { return read(); };
		inline int8_t read44() { return read(); };

		static inline uint16_t	word(uint8_t h, uint8_t l) {
			return (h << 8) | l;
		}

		typedef enum {
			iotcDhtNone, iotcDhtTemperature, iotcDhtHumidity
		} iotDhtTriggerValue;
		typedef enum {
			iotcDhtLessThan, iotcDhtGreaterThan
		} iotDhtTriggerCondition;

		bool
		parseLine(char *s) {
			if(strncmp(s, "dht", 3) == 0) {
				iotDhtTriggerValue		type;
				iotDhtTriggerCondition	rel;
				
				int		n;
				bool	err;
				double	val;
				
				s += 3; while(isspace(*s)) s++;
				if(strncmp(s, "get", 3) == 0) {
					if((n = read()) == iotcDhtOk) {
						pullTriggers();
						char	str[10];
						dtostrf(iTemperature, 5, 1, str);
						printf("dht %s", str);
						dtostrf(iHumidity, 5, 1, str);
						printf(" %s\n", str);
					} else printf("dht error %d\n", n);
					return true;
				} else if(strncmp(s, "free", 4) == 0) {
					s += 4; while(isspace(*s)) s++;
					n = atoi(s);
					if((n = freeTrigger(n)) < 0) printf("dht free: error\n");
					else printf("dht free %d\n", n);
					return true;
				}
				if(tolower(*s) == 't') type = iotcDhtTemperature;
				else if(tolower(*s) == 'h') type = iotcDhtHumidity;
				else return true;							// error
				s++; while(isspace(*s)) s++;
				if(*s == '<') rel = iotcDhtLessThan;
				else if(*s == '>') rel = iotcDhtGreaterThan;
				else goto syntaxError;
				val = getFloat(&s, &err);
				if(err) goto syntaxError;
				while(isspace(*s)) s++;
				if((n = allocTrigger()) < 0) goto syntaxError;
				iTrigger[n].iType = type;
				iTrigger[n].iCondition = rel;
				iTrigger[n].iValue = val;
				strcpy(iTrigger[n].iLine, s);
				printf("dht trigger %d\n", n);
				return true;
			syntaxError:
				printf("dht: syntax error\n");
				return true;
			}
			return false;
		}

		double iHumidity;
		double iTemperature;

	private:
		volatile uint8_t	*ipPin, *ipPort, *ipDDR;
		uint8_t				iPinMask;
		uint8_t				iBytes[5];  // buffer to receive data

		int8_t readSensor(
			uint16_t wakeupDelay, uint8_t leadingZeroBits) {
			// INITIALIZE BUFFER VARIABLES
			uint8_t	mask = 0x80;
			uint8_t idx = 0, data = 0;
			uint8_t state = 0, pstate = 0;
			uint16_t zeroLoop = iotcDhtTimeOut;
			uint16_t delta = 0;

			leadingZeroBits = 40 - leadingZeroBits; // reverse counting...

			// REQUEST SAMPLE
			*ipDDR |= iPinMask;		// OUTPUT
			*ipPort &= ~iPinMask;	// T-be: LOW
			iotTimer::delay(wakeupDelay);
			*ipPort |= iPinMask;	// T-go: HIGH
			*ipDDR &= ~iPinMask;	// INPUT

			uint16_t loopCount = iotcDhtTimeOut * 2;  // 200uSec max
			while ((*ipPin & iPinMask) != 0 ) {
				if (--loopCount == 0) return iotcDhtErrorConnect;
			}
			// GET ACKNOWLEDGE or TIMEOUT
			loopCount = iotcDhtTimeOut;
			while ((*ipPin & iPinMask) == 0 ) {	// T-rel
				if (--loopCount == 0) return iotcDhtErrorAckL;
			}
			loopCount = iotcDhtTimeOut;
			while ((*ipPin & iPinMask) != 0 ) {	// T-reh
				if (--loopCount == 0) return iotcDhtErrorAckH;
			}
			loopCount = iotcDhtTimeOut;
			// READ THE OUTPUT - 40 BITS => 5 BYTES
			for (uint8_t i = 40; i != 0; ) {
				// WAIT FOR FALLING EDGE
				state = (*ipPin & iPinMask);
				if (state == 0 && pstate != 0) {
					if (i > leadingZeroBits) {
						// DHT22 first 6 bits are all zero !! DHT11 only 1
						zeroLoop = min(zeroLoop, loopCount);
						delta = (iotcDhtTimeOut - zeroLoop) / 4;
					} else if(loopCount <= (zeroLoop - delta)) // long -> one
						data |= mask;
					mask >>= 1;
					if (mask == 0) {	// next byte
						mask = 0x80;
						iBytes[idx] = data; idx++; data = 0;
					}
					// next bit
					--i;

					// reset timeout flag
					loopCount = iotcDhtTimeOut;
				}
				pstate = state;
				// Check timeout
				if (--loopCount == 0) return iotcDhtErrorTimeOut;
			}
			*ipDDR |= iPinMask;		// OUTPUT
			*ipPort |= iPinMask;	// HIGH

			return iotcDhtOk;
		}
		static double
		getFloat(char **pS, bool *err) {
			int8_t	sign = 1;
			double	val;
			char	*s = *pS;

			while(isspace(*s)) s++;
			if(*s == '-') { sign = -1; s++; }
			else if(*s == '+') s++;
			while(isspace(*s)) s++;
			if(!isdigit(*s)) { *err = true; return 0.0; }
			val = *s++ - '0';
			while(isdigit(*s)) val = 10.0 * val + *s++ - '0';
			if(*s == '.') {
				double scale = 1.0;
				s++;
				while(isdigit(*s)) {
					scale /= 10.0;
					val += (*s++ - '0') * scale;
				}
			}
			*pS = s; *err = false;
			return sign * val;
		}

		class iotDhtTrigger {
			public:
				iotDhtTriggerValue		iType;
				iotDhtTriggerCondition	iCondition;

				double	iValue;				// value to be compared.
				char	iLine[iotcMaxLine];	// line to be executed.
		};
		static iotDhtTrigger	iTrigger[iotcDhtMaxTrigger];
		
		static int8_t
		allocTrigger() {
			int8_t	i;
			for(i = 0; i < iotcDhtMaxTrigger; i++) {
				if(iTrigger[i].iType == iotcDhtNone) return i;
			}
			return -1;
		}
		void
		pullTriggers() {
			iotDhtTrigger	*t;
			double			val;
			for(t = iTrigger; t < iTrigger + iotcDhtMaxTrigger; t++) {
				if(t->iType == iotcDhtNone) continue;
				if(t->iType == iotcDhtTemperature) val = iTemperature;
				else if(t->iType == iotcDhtHumidity) val = iHumidity;
				else continue;
				if(((t->iCondition == iotcDhtLessThan) && (val < t->iValue)) ||
				   ((t->iCondition == iotcDhtGreaterThan) && (val > t->iValue)))
					if(!iotTerminal::enqueueLine(t->iLine)) break;	// full
			}
		}
		static int
		freeTrigger(int8_t n) {
			if((n < 0) || (n >= iotcDhtMaxTrigger)) return -1;
			iTrigger[n].iType = iotcDhtNone;
			return n;
		}
};

//-----------------------------------------------------------------------------
//	iotServo -
//		- Servo Motor Driver
//		- Interrupts:   10kHz = 10000 Hz = 16MHz / 64 / 25 = 250000 / (5 * 5)
//			-> the minimum possible interrupt frequency is selected.
//			-> remember that the interrupts are one of the most valuable
//			   resources. try not to use high interrupt frequencies.
//		- period: 20 msec:		 200 ticks = 20.0 * 10
//		- 1.0 msec (-90 degrees): 10 ticks =  1.0 * 10 ->  5 ticks
//		- 1.5 msec   (0 degree):  15 tikcs =  1.5 * 10 -> 14 ticks
//		- 2.0 msec  (90 degrees): 20 ticks =  2.0 * 10 -> 23 ticks
//			-> experimentally adjusted by shifting one tick
//			   to the negative side, and scaling.
//			-> use in the range of -90 <= angle <= 90 with step = 10.
//			-> unstable at angles out of the range.
//-----------------------------------------------------------------------------
class iotServo {
	public:
		// period, zero, delta
		enum {
			iotcServoPeriod = 200,
			iotcServoTicksAtZero = 14,
			iotcServoDeltaDegree = 10
		};

		static void
		tick10kHz() {
			if(++in10kCycles == iAngle) {
				*ipPort &= ~iPinMask;
			} else if(in10kCycles == iotcServoPeriod) {
				*ipPort = iPinMask;
				in10kCycles = 0;
			}
		}
		static void
		init(iotPin::iotPinId pin) {
			ipPort = iotPin::getpPort(pin);
			iPinMask = iotPin::getPinMask(pin);
			*iotPin::getpDDR(pin) |= iPinMask;

			iTimer
				= new iotTimer(iotTimer::iotcTimer0, iotTimer::iotcPsd64, 25);
			in10kCycles = 0;
			*ipPort |= iPinMask;
			iTimer->enableInterrupt(tick10kHz);
		}
		static void
		setAngle(int angle) {	// in degree
			int		delta;
			int		sign;
		
			if(angle < 0) {
				delta = (-angle + iotcServoDeltaDegree / 2)
					/ iotcServoDeltaDegree;
				sign = -1;
			} else {
				delta = (angle + iotcServoDeltaDegree / 2)
					/ iotcServoDeltaDegree;
				sign = 1;
			}
			if(delta > 9) delta = 9;

			iAngle = iotcServoTicksAtZero + sign * delta;
		}
		static int
		getAngle(void) {	// in degree
			return (iAngle - iotcServoTicksAtZero) * iotcServoDeltaDegree;
		}
		static bool
		parseLine(char *s) {
			if(strncmp(s, "servo", 5) == 0) {
				s += 5; while(isspace(*s)) s++;
				if(strncmp(s, "get", 3) != 0) {
					int		val = atoi(s);
					setAngle(val);
				}
				printf("servo %d\n", getAngle());
				return true;
			}
			return false;
		}

	private:
		static iotTimer			*iTimer;
		static int				in10kCycles;
		static int				iAngle;
		static volatile uint8_t	*ipPort;
		static uint8_t			iPinMask;
};

//-----------------------------------------------------------------------------
//	Finger Print Sensor
//-----------------------------------------------------------------------------
class iotFpSensor {
	public:
		// fpAckState: Success, Fail, ...
		typedef enum {
			fpcAckSuccess = 0,		fpcAckFail = 1,		fpcAckParityError = 2,
			fpcAckPrevilege = 3,	fpcAckFull = 4,		fpcAckNoUser = 5,
			fpcAckUserExists = 6,	fpcAckFpExists = 7,	fpcAckTimeOut = 8,
			fpcAckBusy = 9,			fpcAckUnknown = 10
		} fpAckState;
		// fpAckUser: All, Guest, Normal, UserMaster
		typedef enum {
			fpcAckUserAll, fpcAckUserGuest, fpcAckUserNormal, fpcAckUserMaster
		} fpAckUser;
		// fpPacket: Head, Tail, Size
		typedef enum {
			fpcPktHead = 0xf5, fpcPktTail = 0xf5, fpcPktSize = 8
		} fpPacket;
		// fpCommand: Add, Del, Users, Match, Level, ...
		typedef enum {
			fpcCmdNone = 0x00,
			fpcCmdAdd1 = 0x01,   fpcCmdAdd2 = 0x02,     fpcCmdAdd3 = 0x03,
			fpcCmdDel = 0x04,    fpcCmdDelAll = 0x05,   fpcCmdUserCount = 0x09,
			fpcCmdMatch = 0x0b,  fpcCmdMatchAny = 0x0c, fpcCmdCmpLevel = 0x28,
			fpcCmdLpMode = 0x2c, fpcCmdTimeOut = 0x2e
		} fpCommand;
		// fpState: Idle, Busy, Parse
		typedef enum {
			fpcStateIdle, fpcStateBusy, fpcStateParse
		} fpState;

		// Blocking:
		//     UserCount, CmpLevel, TimeOut: 10, Add: 200, Del: 50, Match: 150
		//     Multiplier: 120,000, T0 = 120,000 * 15 usec / 6 = 0.3 sec
		// Non-Blocking:
		//     Ticks: 8msec, Multiplier: 0.3 sec / 8 msec = 37.5
		typedef enum {
			fpcDelayScale = 38,
			fpcDelayQuery = fpcDelayScale * 10,
			fpcDelayAdd = fpcDelayScale * 200,
			fpcDelayDelete = fpcDelayScale * 50,
			fpcDelayMatch = fpcDelayScale * 150
		} fpDelay;

		static fpState		getCurState()	{ return iCurState; }
		static fpCommand	getCurCommand()	{ return iCurCommand; }
		
		static void
		init(iotUart::iotUartId uid, iotTimer::iotTimerId tid) {
			if(!iUart) {
				iUart = new iotUart(uid, 19200, receivePacket);
				iotTimeOut::init(tid);
			}
			iCurState = fpcStateIdle;
			iCurCommand = fpcCmdNone;
			iRxPktPtr = 0;
		}
		static void
		processOne() {
			if(iUart) iUart->processOne();
			parsePacket();
		}
		static void
		detroy() {
			delete iUart;
			iUart = (iotUart *) NULL;
		}
		static void
		receivePacket() {
			if(!iUart) return;
			uint8_t		c;
			while(iUart->peekChar() > -1) {
				c = iUart->getChar();
				// skip unexpected/timed-out acknowledges.
				if((iCurState == fpcStateParse) || (iCurCommand == fpcCmdNone))
					continue;
				// skip to a head: head and tail have the same code.
				if((iRxPktPtr == 0) && (c != fpcPktHead)) continue;
				// skip this head, too since the previous head was actually a tail.
				if((iRxPktPtr == 1) && (c == fpcPktHead)) continue;
				iRxPacket[iRxPktPtr++] = c;
				if(iRxPktPtr == fpcPktSize) {
					if(c == fpcPktTail) {
						iotTimeOut::remove(); iCurState = fpcStateParse;
					}
					iRxPktPtr = 0;
				}
			}
		}
		static void
		timeOutCallback() {
			iCurCommand = fpcCmdNone;
			iCurState = fpcStateIdle;
			iRxPktPtr = 0;
			printf("fp error %d\n", fpcAckTimeOut);
		}
		static void
		sendPacket(fpCommand cmd, uint8_t arg1, uint8_t arg2, uint8_t arg3) {
			uint8_t			checkSum, i;
			unsigned int	delay;

			iCurCommand = cmd;
			checkSum =  iTxPacket[1] = iCurCommand;
			checkSum ^= iTxPacket[2] = arg1;
			checkSum ^= iTxPacket[3] = arg2;
			checkSum ^= iTxPacket[4] = arg3;
			iTxPacket[6] = checkSum;
			for(i = 0; i < fpcPktSize; i++) iUart->putChar(iTxPacket[i]);
			iCurState = fpcStateBusy;
		
			switch(iCurCommand) {
				case fpcCmdMatch:
				case fpcCmdMatchAny:	delay = fpcDelayMatch; break;
				case fpcCmdAdd1:
				case fpcCmdAdd2:
				case fpcCmdAdd3:		delay = fpcDelayAdd; break;
				case fpcCmdDel:
				case fpcCmdDelAll:		delay = fpcDelayDelete; break;
				case fpcCmdUserCount:
				case fpcCmdTimeOut:
				case fpcCmdLpMode:
				default:				delay = fpcDelayQuery; break;
			}
			iotTimeOut::add(delay, timeOutCallback);
		}

		static void
		parsePacket() {
			if(iCurState != fpcStateParse) return;
//			assert(fpRxPktPtr == fpcPacketSize);

			uint8_t	error = 0;
			if(iRxPacket[6] !=
				(iRxPacket[1] ^ iRxPacket[2] ^ iRxPacket[3] ^ iRxPacket[4]))
				error = 2;
			else error = iRxPacket[4];
			if((error > fpcAckSuccess) && (error != fpcAckPrevilege)) {
				printf("fp error; %d\n", error); iCurState = fpcStateIdle; return;
			}

			iRxPktPtr = 0;
			switch(iCurCommand) {
				case fpcCmdMatchAny:
					printf("fp match %d\n", (iRxPacket[2] << 8) | iRxPacket[3]);
					break;
				case fpcCmdMatch:
					printf("fp match %d\n", (iTxPacket[2] << 8) | iTxPacket[3]);
					break;
				case fpcCmdAdd1:
					sendPacket(fpcCmdAdd2,
						iTxPacket[2], iTxPacket[3], fpcAckPrevilege); return;
				case fpcCmdAdd2:
					sendPacket(fpcCmdAdd3,
						iTxPacket[2], iTxPacket[3], fpcAckPrevilege); return;
				case fpcCmdAdd3:
					printf("fp add %d\n", (iTxPacket[2] << 8) | iTxPacket[3]);
					break;
				case fpcCmdDel:
					printf("fp delete %d\n", (iTxPacket[2] << 8) | iTxPacket[3]);
					break;
				case fpcCmdDelAll:
					printf("fp delete all\n"); break;
				case fpcCmdUserCount:
					printf("fp users %d\n", (iRxPacket[2] << 8) | iRxPacket[3]);
					break;
				case fpcCmdCmpLevel:
					printf("fp level %d\n", iRxPacket[3]); break;
				case fpcCmdTimeOut:
					printf("fp timeout %d\n", iRxPacket[3]); break;
				case fpcCmdLpMode:
					printf("fp sleep\n"); break;
				default: break;
			}
			iCurCommand = fpcCmdNone;
			iCurState = fpcStateIdle;
		}
		static bool
		parseLine(char *s) {
			if(strncmp(s, "fp", 2) == 0) {
				fpCommand	cmd = fpcCmdNone;
				uint16_t	val = 0;
				uint8_t		arg3 = 0;
				
				if(iCurState != fpcStateIdle) {
					printf("fp %d\n", fpcAckBusy); return true;
				}
				s += 2; while(isspace(*s)) s++;
				if(strncmp(s, "timeout", 7) == 0) {
					s += 7; cmd = fpcCmdTimeOut;
					while(isspace(*s)) s++;
					if(strncmp(s, "get", 3) == 0) arg3 = 1;
					else val = atoi(s);
				} else if(strncmp(s, "level", 5) == 0) {
					s += 5; cmd = fpcCmdCmpLevel;
					while(isspace(*s)) s++;
					if(strncmp(s, "get", 3) == 0) arg3 = 1;
					else val = atoi(s);
				} else if(strncmp(s, "users", 5) == 0) {
					s += 5; cmd = fpcCmdUserCount;
				} else if(strncmp(s, "sleep", 5) == 0) {
					s += 5; cmd = fpcCmdLpMode;
				} else if(strncmp(s, "add", 3) == 0) {
					s += 3; cmd = fpcCmdAdd1;
					while(isspace(*s)) s++; val = atoi(s);
					arg3 = fpcAckPrevilege;
				} else if(strncmp(s, "delete", 6) == 0) {
					s += 6; while(isspace(*s)) s++;
					if(strcmp(s, "all") == 0) cmd = fpcCmdDelAll;
					else { cmd = fpcCmdDel; val = atoi(s); }
				} else if(strncmp(s, "match", 5) == 0) {
					s += 5; while(isspace(*s)) s++;
					if(strcmp(s, "any") == 0) cmd = fpcCmdMatchAny;
					else { cmd = fpcCmdMatch; val = atoi(s); }
				} else {
					printf("fp error %d\n", fpcAckUnknown); return true;
				}
				sendPacket(cmd, (val >> 8) & 0xff, val & 0xff, arg3);
				return true;
			}
			return false;
		}

	private:
		// T0: time to scan and process a finger print image (0.2~0.3 sec)
		// Actual TimeOut = T0 * response(fpcCmdTimeOut);
		// commands: timeout t/get, level l/get, users, sleep
		//     add k, delete k/all, match u/any
		static iotUart		*iUart;
		static fpState		iCurState;
		static fpCommand	iCurCommand;

		static uint8_t		iRxPktPtr;
		static uint8_t		iRxPacket[fpcPktSize];
		static uint8_t		iTxPacket[fpcPktSize];
};

//-----------------------------------------------------------------------------
//	Clock
//-----------------------------------------------------------------------------
#define			iotcMaxSchedules	5

class iotClock {
	public:
		static void
		init(iotTimer::iotTimerId tid, long sec, iotCharLcd *clcd, uint8_t row)
		{
			iSeconds = sec; iRow = row;
			iTimer = new iotTimer(tid, iotTimer::iotcPsd1024, 15625);
			iTimer->enableInterrupt(update);
			if(!iCharLcd) {
				if(clcd) iCharLcd = clcd;
				else { iCharLcd = new iotCharLcd; iCharLcdCreated = true; }
			}
			iCharLcd->print(iRow, 1, iMonDyHrMnSc);
			for(iotSchedule *s = iSchedule;
				s < iSchedule + iotcMaxSchedules; s++) s->iTime = 0;
		}
		static void
		destroy() {
			delete iTimer;
			if(iCharLcdCreated) delete iCharLcd;
			iTimer = (iotTimer *) NULL;
			iCharLcd = (iotCharLcd *) NULL;
		}

		static void			setTime(long int sec)	{ iSeconds = sec; }
		static char			*getMonDyHrMnSc()		{ return iMonDyHrMnSc; }
		static iotCharLcd	*getCharLcd()			{ return iCharLcd; }
		static char			lcdmode;

		static void
		processOne() {
			if(!iFlush) return;
			iFlush = 0;

			int days;
			if((days = iSeconds / 86400L) != iYearDays) {
				iYearDays = days;
				toDate(iMonDyHrMnSc, days);
				if(iCharLcd && lcdmode) iCharLcd->print(iRow, 1, iMonDyHrMnSc);
				else iCharLcd->print(iRow, 1, "            ");
			}
			toTime(iMonDyHrMnSc + 7, iSeconds);
			if(iCharLcd && lcdmode) iCharLcd->print(iRow, 8, iMonDyHrMnSc + 7);
			else iCharLcd->print(1, 0, "            ");
			executeSchedules();
		}
		static void
		toDate(char *s, int days) {
			int		m;
			for(m = 0; days > iMonthDays[m]; m++)
				days -= iMonthDays[m];
			s[0] = iMonth[m][0];
			s[1] = iMonth[m][1];
			s[2] = iMonth[m][2];
			s[3] = '-'; days++;
			s[4] = '0' + days / 10;
			s[5] = '0' + days % 10;
			s[6] = ' ';
			s[7] = '\0';
		}
		static void
		toTime(char *s, long secs) {
			secs %= 86400L;
			s[0] = '0' + secs / 36000L;
			s[1] = '0' + (secs % 36000L) / 3600;
			s[2] = s[5] = ':';
			s[3] = '0' + (secs % 3600) / 600;
			s[4] = '0' + (secs % 600) / 60;
			s[6] = '0' + (secs % 60) / 10;
			s[7] = '0' + (secs % 10);
			s[8] = '\0';
		}
		static long
		toSeconds(char **ps) {
			long	secs;
			char	*s;

			s = *ps;
			if(isalpha(*s)) {
				int		m, days = 0;
				for(m = 0; m < 12; m++) {
					if(strncasecmp(s, iMonth[m], 3) == 0) break;
					days += iMonthDays[m];
				}
				if((m == 12) || s[3] != '-' || !isdigit(s[4]) || !isdigit(s[5]))
					return -1;
				secs = 86400L * (days + 10 * (s[4] - '0') + s[5] - '0' - 1);
				s += 7;
			} else if(*s == '+') {
				secs = iSeconds; s++;
			} else if(*s == '-') {
				secs = iSeconds - iSeconds % 86400L; s++;
			} else secs = 0;
			
			if(!isdigit(s[0])) return -1;
			if(isdigit(s[1]) && s[2] == ':') {
				if( !isdigit(s[3]) || !isdigit(s[4]) || (s[5] != ':') ||
					!isdigit(s[6]) || !isdigit(s[7]) ) return -1;
				secs += 36000L * (s[0] - '0');
				secs += 3600 * (s[1] - '0');
				secs += 600 * (s[3] - '0');
				secs += 60 * (s[4] - '0');
				secs += 10 * (s[6] - '0');
				secs += (s[7] - '0');
				*ps = s + 8;
			} else {
				long	n = 0;
				while(isdigit(*s)) n = 10 * n + *s++ - '0';
				secs += n;
				*ps = s;
			}
			return secs;
		}
		static bool
		parseLine(char *s) {
			long	secs;
			if(strncmp(s, "time", 4) == 0) {
				s += 4; while(isspace(*s)) s++;
				if((secs = toSeconds(&s)) >= 0) iSeconds = secs;
				printf("time %ld\n", iSeconds);
				return true;
			} else if(strncmp(s, "at", 2) == 0) {
				int8_t	n;
				s += 2; while(isspace(*s)) s++;
				if( ((secs = toSeconds(&s)) >= 0) &&
					((n = allocSchedule()) >= 0) ) {
					while(isspace(*s)) s++;
					iotSchedule	*sch = iSchedule + n;
					sch->iTime = secs;
					if((secs = toSeconds(&s)) >= 0) {
						sch->iPeriod = secs;
						while(isspace(*s)) s++;
					} else sch->iPeriod = 0;
					strcpy(sch->iLine, s);
					printf("schedule %d: %ld %ld %s\n",
						n, sch->iTime, sch->iPeriod, sch->iLine);
				} else printf("at: error\n");
				return true;
			} else if(strncmp(s, "cancel", 6) == 0) {
				int8_t	n;
				s += 6; while(isspace(*s)) s++;
				n = atoi(s);
				if((n = cancelSchedule(n)) >= 0) printf("cancel %d\n", n);
				else printf("cancel: error\n");
				return true;
			}
			return false;
		}

	private:
		static iotTimer		*iTimer;
		static long int		iSeconds;	// 14472000 for Jun-17 10:00:00
		static char			iMonDyHrMnSc[16];
		static iotCharLcd	*iCharLcd;
		static uint8_t		iRow;
		static char			iFlush;
		static int			iYearDays;
		static const char	*iMonth[12];
		static char			iMonthDays[12];
		static bool			iCharLcdCreated;
		
		static void
		update() {
			iSeconds++;
			if(iSeconds >= 31536000L) iSeconds = 0;
			iFlush = 1;
		}
		
		class iotSchedule {
			public:
				long	iTime;		// the expiration time in second.
									// 0 for invalid schedules.
				long	iPeriod;	// for periodic schedules in second.
									// 0 for aperiodic schedules.
				char	iLine[iotcMaxLine];
									// line to be executed.
		};
		static iotSchedule	iSchedule[iotcMaxSchedules];
		
		static int8_t
		allocSchedule() {
			int8_t	i;
			for(i = 0; i < iotcMaxSchedules; i++) {
				if(iSchedule[i].iTime == 0) return i;
			}
			return -1;
		}
		static void
		executeSchedules() {
			iotSchedule	*s;
			for(s = iSchedule; s < iSchedule + iotcMaxSchedules; s++) {
				if((s->iTime == 0) || (s->iTime > iSeconds)) continue;
				if(!iotTerminal::enqueueLine(s->iLine)) break;	// full
				if(s->iPeriod > 0) s->iTime += s->iPeriod;
				else s->iTime = 0;
			}
		}
		static int
		cancelSchedule(int8_t n) {
			if((n < 0) || (n >= iotcMaxSchedules)) return -1;
			iSchedule[n].iTime = 0;
			return n;
		}
};

//-----------------------------------------------------------------------------
//	Dust Sensor
//-----------------------------------------------------------------------------
#define		iotcDsBufSize		10

#define		iotcDsCovRatio		0.2		// um/m3/mV
#define		iotcSystemVoltage	5000	// mV
#define		iotcNoDustVoltage	400		// mV

class iotDustSensor {
	public:
		static void
		init(iotPin::iotPinId iled, iotPin::iotPinId aout) {
			ipiLed = iotPin::getpPort(iled);
			iiLedPin = iotPin::getPinMask(iled);
			*iotPin::getpDDR(iled) |= iiLedPin;
			*ipiLed &= ~iiLedPin;
			iaOut = aout;
			iCount = -1;
			iotAnalog::init();
		}
		static double
		read() {
			uint16_t	adcOut;
			double		voltage;

			*ipiLed |= iiLedPin;	// iled high
			iotTimer::delay(20);	// 280 usec (approximately 20 x 15 usec)
			adcOut = iotAnalog::read(iaOut);
			*ipiLed &= ~iiLedPin;	// iled low

			adcOut = filter(adcOut);
			voltage = (iotcSystemVoltage / 1024.0) * adcOut * 11;
			return (voltage > iotcNoDustVoltage
				 ? (voltage - iotcNoDustVoltage) * iotcDsCovRatio : 0.0);
		}
		static bool
		parseLine(char *s) {
			if(strncmp(s, "dust", 4) == 0) {
				char	str[10];
				dtostrf(read(), 5, 1, str);
				printf("dust %s\n", str);
				return true;
			}
			return false;
		}

	private:
		static int
		filter(int n) {
			if(iCount < 0) {
				for(int i = 0; i < iotcDsBufSize; i++) iBuf[i] = n;
				iSum = iotcDsBufSize * n;
				iCount = 0;
				return n;
			} else {
				iSum -= iBuf[iCount]; iSum += n;
				iBuf[iCount] = n;
				iCount = (iCount + 1) % iotcDsBufSize;
				n = iSum / iotcDsBufSize;
				return n;
			}
		}
		static volatile uint8_t	*ipiLed;
		static iotPin::iotPinId	iaOut;
		static uint8_t			iiLedPin;
		static int8_t			iCount;
		static uint16_t			iBuf[iotcDsBufSize], iSum;
};


//-----------------------------------------------------------------------------
//	LED
//-----------------------------------------------------------------------------
class iotLed {
	public:
	static void
	init(iotPin::iotPinId pin) {
		ipPort = iotPin::getpPort(pin);
		iPinMask = iotPin::getPinMask(pin);
		*ipPort |= ~iPinMask;
	}
	static bool
	parseLine(char *s) {
		if(strncmp(s, "led", 3) == 0) {
			s += 3; while(isspace(*s)) s++;
			if(strncmp(s, "on", 2) == 0) {
				*ipPort |= iPinMask;
				//printf("led on \n");
			}
			else if(strncmp(s, "off", 3) == 0) {
				*ipPort &= ~iPinMask; // off
				//printf("led off \n");
			}
			
			return true;
		}
		return false;
	}
	private:
	static volatile uint8_t		*ipPort;
	static uint8_t				iPinMask;

	
};
class iotSubLed {
public:
static void
init(iotPin::iotPinId pin) {
	ipPort = iotPin::getpPort(pin);
	iPinMask = iotPin::getPinMask(pin);
	*ipPort |= ~iPinMask;
}
static bool
parseLine(char *s) {
	if(strncmp(s, "subled", 6) == 0) {
		s += 6; while(isspace(*s)) s++;
		if(strncmp(s, "on", 2) == 0) {
			*ipPort |= iPinMask;
			//printf("subled on \n");
		}
		else if(strncmp(s, "off", 3) == 0) {
			*ipPort &= ~iPinMask; // off
			//printf("subled off \n");
		}
		
		return true;
	}
	return false;
}
private:
static volatile uint8_t		*ipPort;
static uint8_t				iPinMask;


};

#endif /* IOT_H_ */