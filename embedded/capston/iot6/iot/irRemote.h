//=============================================================================
// irRemote.h
//	The header of the infra-red remote controller module of the Iot package.
//		(INU Object Toolkit for the Internet of Things)
//	Created: 2015-01-14 오후 3:24:48
//	Author: kkim@inu.ac.kr / SDTLAB INU (Incheon National University)
//=============================================================================
#ifndef IRREMOTE_H_
#define IRREMOTE_H_

#include "iot.h"

// minimum gap between transmissions.
#define		iotcIrGap				5000

#define		iotcIrUSecPerTick		50
#define		iotcIrGapTicks			(iotcIrGap / iotcIrUSecPerTick)

// percent tolerance in measurements
#define		iotcIrTolerance			25

#define		iotcIrLowerTolerance	(1.0 - iotcIrTolerance / 100.)
#define		iotcIrUpperTolerance	(1.0 + iotcIrTolerance / 100.)

#define		iotmIrTicksLow(us) \
(int) (((us) * iotcIrLowerTolerance / iotcIrUSecPerTick))
#define		iotmIrTicksHigh(us)	\
(int) (((us) * iotcIrUpperTolerance / iotcIrUSecPerTick + 1))

// marks tend to be 100us too long, and spaces 100us too short when received
// due to sensor lag.
#define		iotcIrMarkExcess		100
//-----------------------------------------------------------------------------
//	iotIrRcDecoder -
//-----------------------------------------------------------------------------
typedef enum {
	iotcUnknown, iotcNec, iotcSony, iotcRc5,
	iotcRc6, iotcDish, iotcSharp, iotcPanasonic,
	iotcJvc, iotcSanyo, iotcMitsubishi,
	iotcSamsung, iotcLg
} iotIrVendor;

// pulse parms are *50-100 for the mark and *50+100 for the space.
// first mark is the one after the long gap

// pulse parameters in usec
#define		iotcNecHdrMark				9000
#define		iotcNecHdrSpace				4500
#define		iotcNecBitMark				560
#define		iotcNecOneSpace				1600
#define		iotcNecZeroSpace			560
#define		iotcNecRptSpace				2250

#define		iotcSonyHdrMark				2400
#define		iotcSonyHdrSpace			600
#define		iotcSonyOneMark				1200
#define		iotcSonyZeroMark			600
#define		iotcSonyRptLength			45000
// usually ssee 713 - not using ticks as get number wrap around
#define		iotcSonyDoubleSpaceUSecs	500

// SA 8650B
#define		iotcSanyoHdrMark			3500
#define		iotcSanyoHdrSpace			950
#define		iotcSanyoOneMark			2400
#define		iotcSanyoZeroMark			700
// usually ssee 713 - not using ticks as get number wrap around
#define		iotcSanyoDoubleSpaceUSecs	800
#define		iotcSanyoRptLength			45000

// Mitsubishi RM 75501
// 14200 7 41 7 42 7 42 7 17 7 17 7 18 7 41 7 18 7 17 7 17 7 18 7 41 8 17 7
// 17 7 18 7 17 7
/*
#define		iotcMitsubishiHdrMark		250		// seen range 3500
*/
#define		iotcMitsubishiHdrSpace		350		//  7 * 50 + 100
#define		iotcMitsubishiOneMark		1950	// 41 * 50 - 100
#define		iotcMitsubishiZeroMark		750		// 17 * 50 - 100
/*
// usually ssee 713 - not using ticks as get number wrap around
#define		iotcMitsubishiDoubleSpaceUSecs	800
#define		iotcMitsubishiRptLength			45000
*/

#define		iotcRc5T1					889
#define		iotcRc5RptLength			46000

#define		iotcRc6HdrMark				2666
#define		iotcRc6HdrSpace				889
#define		iotcRc6T1					444
#define		iotcRc6RptLength			46000

#define		iotcSharpBitMark			245
#define		iotcSharpOneSpace			1805
#define		iotcSharpZeroSpace			795
#define		iotcSharpGap				600000
#define		iotcSharpToggleMask			0x3FF
#define		iotcSharpRptSpace			3000

#define		iotcDishHdrMark				400
#define		iotcDishHdrSpace			6100
#define		iotcDishBitMark				400
#define		iotcDishOneSpace			1700
#define		iotcDishZeroSpace			2800
#define		iotcDishRptSpace			6200
#define		iotcDishTopBit				0x8000

#define		iotcPanasonicHdrMark		3502
#define		iotcPanasonicHdrSpace		1750
#define		iotcPanasonicBitMark		502
#define		iotcPanasonicOneSpace		1244
#define		iotcPanasonicZeroSpace		400

#define		iotcJvcHdrMark				8000
#define		iotcJvcHdrSpace				4000
#define		iotcJvcBitMark				600
#define		iotcJvcOneSpace				1600
#define		iotcJvcZeroSpace			550
#define		iotcJvcRptLength			60000

#define		iotcLgHdrMark				8000
#define		iotcLgHdrSpace				4000
#define		iotcLgBitMark				600
#define		iotcLgOneSpace				1600
#define		iotcLgZeroSpace				550
#define		iotcLgRptLength				60000

#define		iotcSamsungHdrMark			5000
#define		iotcSamsungHdrSpace			5000
#define		iotcSamsungBitMark			560
#define		iotcSamsungOneSpace			1600
#define		iotcSamsungZeroSpace		560
#define		iotcSamsungRptSpace			2250

#define		iotcSharpBits				15
#define		iotcDishBits				16

#define		iotcNecBits					32
#define		iotcSonyBits				12
#define		iotcSanyoBits				12
#define		iotcMitsubishiBits			16
#define		iotcMinRc5Samples			11
#define		iotcMinRc6Samples			1
#define		iotcPanasonicBits			48
#define		iotcJvcBits					16
#define		iotcLgBits					28
#define		iotcSamsungBits				32

#define		iotcTopBit					0x80000000

// decoded value for NEC/Samsung when a repeat code is received
#define		iotcRepeat					0xffffffff

typedef enum {
	iotcIrStateIdle, iotcIrStateMark, iotcIrStateSpace, iotcIrStateStop
} iotIrState;

enum {
	iotcIrMark = 0,
	iotcIrSpace = 1
};

#define		iotcIrMaxIntervals	100

class iocIrReceiver;

class iotIrDecoder {
	public:
		iotIrVendor
		decode() {
			if((iState == iotcIrStateStop) &&
			   (decodeNec() || // decodeSony() || decodeSanyo() ||
//				decodeMitsubishi() || decodeRc5() || decodeRc6() ||
//				decodePanasonic() || decodeJVC() ||
				decodeLg() || decodeSamsung()))
				return(iVendor);
			return (iotcUnknown);
		}
		iotIrVendor		getVendor()	{ return(iVendor); }
		unsigned long	getValue()	{ return(iValue); }
		int				getBits()	{ return(inBits); }

		void
		dumpIntervals() {
			printf("%d:", iVendor);
			for(int i = 0; i < inIntervals; i++)
				printf(" %d", iInterval[i]);
			printf("\n\r");
		}

	private:
		iotIrVendor					iVendor;
		volatile	iotIrState		iState;

		// intervals in .5 us ticks.
		volatile uint8_t		inIntervals;
		volatile unsigned int	iInterval[iotcIrMaxIntervals];

		volatile unsigned int	iTimer;
//		unsigned int			iDebug;
		union {
			unsigned int	iPanasonic;
			unsigned int	iSharp;
		} iAddress;
		unsigned long	iValue;		// decoded value
		int				inBits;		// number of bits in decoded value

		bool
		matchMark(int aMeasuredTicks, int aDesiredUs) {
			return(
				aMeasuredTicks
					>= iotmIrTicksLow(aDesiredUs + iotcIrMarkExcess) &&
				aMeasuredTicks
					<= iotmIrTicksHigh(aDesiredUs + iotcIrMarkExcess)
			);
		}

		bool
		matchSpace(int aMeasuredTicks, int aDesiredUs) {
			return(
				aMeasuredTicks
					>= iotmIrTicksLow(aDesiredUs - iotcIrMarkExcess) &&
				aMeasuredTicks
					<= iotmIrTicksHigh(aDesiredUs - iotcIrMarkExcess)
			);
		}

		bool
		decodeNec() {
			long	data = 0;
			int		offset = 1;		// skip first space
			// initial mark
			if(!matchMark(iInterval[offset], iotcNecHdrMark))
				return(false);
			offset++;
			// check for repeat
			if(inIntervals == 4 &&
				matchSpace(iInterval[offset], iotcNecRptSpace) &&
				matchMark(iInterval[offset + 1], iotcNecBitMark)) {
				inBits = 0;
				iValue = iotcRepeat;
				iVendor = iotcNec;
				return true;
			}
			if(inIntervals < 2 * iotcNecBits + 4) return false;
			// initial space
			if(!matchSpace(iInterval[offset], iotcNecHdrSpace))
				return false;
			offset++;
			for(int i = 0; i < iotcNecBits; i++) {
				if(!matchMark(iInterval[offset], iotcNecBitMark))
					return false;
				offset++;
				if(matchSpace(iInterval[offset], iotcNecOneSpace))
					data = (data << 1) | 1;
				else if(matchSpace(
					iInterval[offset], iotcNecZeroSpace)) data <<= 1;
				else return false;
				offset++;
			}
			// success
			inBits = iotcNecBits;
			iValue = data;
			iVendor = iotcNec;
			return true;
		}

		bool
		decodeLg() {
			long	data = 0;
			int		offset = 1;		// skip first space
			
			if (!matchMark(iInterval[offset], iotcLgHdrMark))
			// initial mark
				return false;
			offset++;
			if (inIntervals < 2 * iotcLgBits + 1) return false;
			// initial space
			if (!matchSpace(iInterval[offset], iotcLgHdrSpace))
				return false;
			offset++;
			for (int i = 0; i < iotcLgBits; i++) {
				if (!matchMark(iInterval[offset], iotcLgBitMark))
					return false;
				offset++;
				if (matchSpace(iInterval[offset], iotcLgOneSpace))
					data = (data << 1) | 1;
				else if (matchSpace(
					iInterval[offset], iotcLgZeroSpace))
					data <<= 1;
				else return false;
				offset++;
			}
			// stop bit
			if (!matchMark(iInterval[offset], iotcLgBitMark))
				return false;
			// success
			inBits = iotcLgBits;
			iValue = data;
			iVendor = iotcLg;
			return true;
		}

		bool
		decodeSamsung() {
			long	data = 0;
			int		offset = 1;		// skip first space
			
			// initial mark
			if (!matchMark(iInterval[offset], iotcSamsungHdrMark))
				return false;
			offset++;
			// check for repeat
			if(inIntervals == 4 &&
				matchSpace(iInterval[offset], iotcSamsungRptSpace) &&
				matchMark(
					iInterval[offset + 1], iotcSamsungBitMark)) {
				inBits = 0;
				iValue = iotcRepeat;
				iVendor = iotcSamsung;
				return true;
			}
			if (inIntervals < 2 * iotcSamsungBits + 4) return false;
			// initial space
			if (!matchSpace(iInterval[offset], iotcSamsungHdrSpace))
				return false;
			offset++;
			for (int i = 0; i < iotcSamsungBits; i++) {
				if (!matchMark(
					iInterval[offset], iotcSamsungBitMark))
					return false;
				offset++;
				if (matchSpace(
					iInterval[offset], iotcSamsungOneSpace))
					data = (data << 1) | 1;
				else if (matchSpace(
					iInterval[offset], iotcSamsungZeroSpace))
					data <<= 1;
				else return false;
				offset++;
			}
			// stop bit
			if (!matchMark(iInterval[offset], iotcLgBitMark))
				return false;
			// success
			inBits = iotcSamsungBits;
			iValue = data;
			iVendor = iotcSamsung;
			return true;
		}

	friend class iotIrReceiver;
};

class iotIrTransmitter;

//-----------------------------------------------------------------------------
//	iotIrReceiver -
//-----------------------------------------------------------------------------
class iotIrReceiver {
	public:
		iotIrReceiver(iotTimer::iotTimerId aTimerId);
		~iotIrReceiver();

		void
		enable() {
			iTimer->configure(iotTimer::iotcPsd1,
				F_CPU * iotcIrUSecPerTick / 1000000);

			iDecoder->iVendor = iotcUnknown;
			iDecoder->iState = iotcIrStateIdle;
			iDecoder->inIntervals = 0;
			iDecoder->iTimer = 0;

			// set pin direction to input.
			if(iTimer->getId() == iotTimer::iotcTimer1) DDRB &= ~(1 << PB4);
			else DDRE &= ~(1 << PE2);	// iotTimer::iotcTimer3

			iTimer->enableInterrupt(iotIrReceiver::stateMachine);
		}

		iotIrDecoder *
		getDecoder() {
			if(iDecoder->iState == iotcIrStateStop)
				return(iDecoder);
			return ((iotIrDecoder *) NULL);
		}

		static void
		stateMachine() {
			uint8_t	 data;
			// cf.) PB5(OC1A), PE3(OC3A) are used for IrLEDs.
			if(iTimer->getId() == iotTimer::iotcTimer1) {
				data = (PINB & (1 << PB4)) != 0;
				if(data) PORTB |= 0x08;
				else PORTB &= ~0x08;
			} else {			// iotTimer::iotcTimer3
				data = (PINE & (1 << PE2)) != 0;
				if(data) PORTE |= 0x80;
				else PORTE &= ~0x80;
			}
			iDecoder->iTimer++;
			if(iDecoder->inIntervals >= iotcIrMaxIntervals)
				iDecoder->iState = iotcIrStateStop;
			switch(iDecoder->iState) {
				case iotcIrStateIdle:	// in the middle of a gap
					if(data == iotcIrMark) {
						if(iDecoder->iTimer < iotcIrGapTicks) {
							// not big enough to be a gap.
							iDecoder->iTimer = 0;
						} else {
							// gap just ended, record duration and start
							// recording transmission
							iDecoder->inIntervals = 0;
							iDecoder->iInterval[iDecoder->inIntervals++]
								= iDecoder->iTimer;
							iDecoder->iTimer = 0;
							iDecoder->iState = iotcIrStateMark;
						}
					}
					break;
				case iotcIrStateMark:	// timing Mark
					if(data == iotcIrSpace) {
						iDecoder->iInterval[iDecoder->inIntervals++]
							= iDecoder->iTimer;
						iDecoder->iTimer = 0;
						iDecoder->iState = iotcIrStateSpace;
					}
					break;
				case iotcIrStateSpace:	// timing Space
					if(data == iotcIrMark) {	// Space just ended
						iDecoder->iInterval[iDecoder->inIntervals++]
							= iDecoder->iTimer;
						iDecoder->iTimer = 0;
						iDecoder->iState = iotcIrStateMark;
					} else {	// Space
						if(iDecoder->iTimer > iotcIrGapTicks) {
							// big Space, indicates gap between codes.
							// mark current code as ready for processing.
							// switch to Stop. don't reset iTimer;
							// keep counting space width.
							iDecoder->iState = iotcIrStateStop;
						}
					}
					break;
				case iotcIrStateStop:	// waiting, measuring gap
					if(data == iotcIrMark) {	// reset gap iTimer
						iDecoder->iTimer = 0;
					}
					break;
			}
		}

		static iotIrDecoder		*iDecoder;
		static iotTimer			*iTimer;
};

//-----------------------------------------------------------------------------
//	iotIrTransmitter -
//-----------------------------------------------------------------------------
class iotIrTransmitter {
	public:
		iotIrTransmitter(iotTimer::iotTimerId aTimerId);
		~iotIrTransmitter();

		void
		sendNec(unsigned long aData, int anBits) {
			enable(38);
			mark(iotcNecHdrMark);
			space(iotcNecHdrSpace);
			for(int i = 0; i < anBits; i++) {
				if(aData & iotcTopBit) {
					mark(iotcNecBitMark); space(iotcNecOneSpace);
				} else {
					mark(iotcNecBitMark); space(iotcNecZeroSpace);
				}
				aData <<= 1;
			}
			mark(iotcNecBitMark);
			space(0);
		}

		void
		sendSamsung(unsigned long aData, int anBits) {
			enable(38);
			mark(iotcSamsungHdrMark);
			space(iotcSamsungHdrSpace);
			for(int i = 0; i < anBits; i++) {
				if(aData & iotcTopBit) {
					mark(iotcSamsungBitMark); space(iotcSamsungOneSpace);
				} else {
					mark(iotcSamsungBitMark); space(iotcSamsungZeroSpace);
				}
				aData <<= 1;
			}
			mark(iotcSamsungBitMark);
			space(0);
		}
		
		

		static iotTimer		*iTimer;

	private:
		void
		delayMicroseconds(unsigned int auSec) {
			// for the 16 MHz clock
			// for a one-microsecond delay, simply return. the overhead of
			// the function call yields a delay of approximately 1 1/8 us.
			if(--auSec == 0) return;

			// the following loop takes a quarter of a microsecond (4 cycles)
			// per iteration, so execute it four times for each microsecond of
			// delay requested.
			auSec <<= 2;

			// account for the time taken in the preceding commands.
			auSec -= 2;

			// busy wait
			__asm__ __volatile__ (
				"1: sbiw %0,1" "\n\t"					// 2 cycles
				"brne 1b" : "=w" (auSec) : "0" (auSec)	// 2 cycles
			);
		}

		void
		mark(unsigned int auSec) {
			// sends an IR mark for the specified number of microseconds.
			// the mark output is modulated at the PWM frequency.
			iTimer->enablePWMOut();
			if(auSec > 0) delayMicroseconds(auSec);
		}

		void
		space(unsigned int auSec) {
			// sends an IR mark for the specified number of microseconds.
			// the mark output is modulated at the PWM frequency.
			iTimer->disablePWMOut();
			if(auSec > 0) delayMicroseconds(auSec);
		}

		void
		enable(int aKHz) {
			iTimer->disableInterrupt();
			if(iTimer->getId() == iotTimer::iotcTimer1) {
				DDRB |= (1 << PB5);		// output
				// when not sending PWM, we want it low
				PORTB &= ~(1 << PB5);
			} else if(iTimer->getId() == iotTimer::iotcTimer3) {
				DDRE |= (1 << PE3);		// output
				// when not sending PWM, we want it low
				PORTE &= ~(1 << PE3);
			}
			iTimer->configureOneThirdDuty(aKHz);
		}
};

#endif /* IRREMOTE_H_ */