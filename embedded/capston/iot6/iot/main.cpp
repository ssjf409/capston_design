/*
 * main.cpp
 *
 * Created: 2015-01-13 오후 8:16:49
 *  Author: kkim
 */ 

#include "irRemote.h"
#include <string.h>
#include <ctype.h>


#define		CLOCK_TIMER			iotTimer::iotcTimer3
#define		TIMEOUT_TIMER		iotTimer::iotcTimer2
#define		SERVO_MOTOR_TIMER	iotTimer::iotcTimer0
#define		UART_TERMINAL		iotUart::iotcUart1
#define		UART_SENSOR			iotUart::iotcUart0

#define		IR_TIMER			iotTimer::iotcTimer1
#define		LED_PORT		PORTB
#define		EX_KEY			PINB

static iotCharLcd	*CharLcd = (iotCharLcd *) NULL;
static iotDht		*Dht = (iotDht *) NULL;


iotIrReceiver			*IrRecv;
iotIrTransmitter		*IrTrans;

iotIrDecoder			*dec;

#define			MaxKeys		5
#define			NumBits		32
int				NumKeys = 0;
int				CurKey = 0;
unsigned long	Key[MaxKeys] = { 0, 0, 0, 0, 0 };
	
#define		LED_LEFT	0
#define		LED_RIGHT	1

unsigned char	Num1kHzCycles = 0;
unsigned char	LedData = 1;
unsigned char	LedMode = LED_LEFT;

// DC Fan Motor
static iotTimer		*FanTimer = (iotTimer *) NULL;
static int			FanCount = 0;
static bool			FanState = 0;
static int			FanSpeed = 0;
static int			cMaxFanSpeed = 4;

int					lcdon = 0;



/*
//-----------------------------------------------------------------------------
//	led
//-----------------------------------------------------------------------------
void
updateLed() {
	Num1kHzCycles++;
	if(Num1kHzCycles == 200) {
		if(LedMode == LED_LEFT) {
			if(LedData == 0x04) LedData = 0x01;
			else LedData = LedData << 1;
		} else {
			if(LedData == 0x01) LedData = 0x04;
			else LedData = LedData >> 1;
		}
		LED_PORT = ~LedData;
		
		Num1kHzCycles = 0;
	}
}
*/

//-----------------------------------------------------------------------------
//	switch
//-----------------------------------------------------------------------------
/*
static void
inputKey()
{

	volatile char    key;

	key = ~EX_KEY & 0xc0;
	if(key == LastKey) return;
	LastKey = key;
	
	iotTimer::delay(200); //iotCharLcd
	
	switch(key) {
		case 0x40:
			if(Key[CurKey] == 0) printf("NoData"); // HhMmSs nonono
			else {
				IrTrans->sendSamsung(Key[CurKey], NumBits);
				IrRecv->enable();
			}
			CurKey = (CurKey + 1) % MaxKeys;
			break;
		case 0x80:
			if(LedMode == LED_LEFT) LedMode = LED_RIGHT;
			else LedMode = LED_LEFT;
			break;
		default: break;
	}

}
*/


// DC Fan Motor
static void
toggleFan()
{
	if(FanCount > 0)
	FanCount--;
	else {
		if(FanState) {
			FanState = false;
			FanCount = cMaxFanSpeed - FanSpeed;
			// set the motor pin (PORTE pin 0) to 0.
			PORTF &= ~0x01;
			} else {
			FanState = true;
			FanCount = FanSpeed;
			// set the motor pin to 1.
			PORTF |= 0x01;
		}
	}
}


// DC Fan Motor
static bool
parseFan(char *s)
{
	if(strncmp(s, "fan", 3) == 0) {
		int      n;
		s += 3; while(isspace(*s)) s++;
		n = atoi(s);
		if (n < 0) n = 0;
		else if (n > cMaxFanSpeed) n = cMaxFanSpeed;
		if(FanSpeed == 0) {
			if(n > 0) {
				FanTimer->configure(iotTimer::iotcPsd64, 250);
				FanTimer->enableInterrupt(toggleFan);
			}
		} else {
			if(n == 0) {
				FanTimer->configure(iotTimer::iotcPsdNo, 250);
				FanTimer->disableInterrupt();
			}
		}
		FanSpeed = n;
		FanCount = FanSpeed;
		FanState = false;
		return true;
	}
	return false;
}




//-----------------------------------------------------------------------------
//	parser
//-----------------------------------------------------------------------------
static void
parseLine(char *s) {
	CharLcd->print(2, 1, s);
	//if(iotFpSensor::parseLine(s)) return;
	if(Dht->parseLine(s)) return;
	if(iotServo::parseLine(s)) return;
	if(iotLed::parseLine(s)) return;
	if(iotSubLed::parseLine(s)) return;
	
	
	if(iotClock::parseLine(s)) return;
	//if(iotDustSensor::parseLine(s)) return;
	
	if(parseFan(s)) return;
	
	if(strncmp(s, "0x", 2) == 0) {
		cli();
		IrTrans->sendSamsung(strtoul(s,NULL,16), NumBits);
		IrRecv->enable();
		sei();
		
		//printf("number1\n");
		//switch로 나누기
		CharLcd->print(2, 1, "tx: tv   ");	// 추후에 수정
		return;
	}
	
	printf("unknown\n");
}

//-----------------------------------------------------------------------------
//	main -
//-----------------------------------------------------------------------------
int
main(void)
{
	cli();

	// initialize the clock: Jun-17 10:00:00 at row 1 of char LCD.
	iotClock::init(CLOCK_TIMER, 14472000L, (iotCharLcd *) NULL, 1);
	CharLcd = iotClock::getCharLcd();

	// initialize Terminal
	iotTerminal::init(UART_TERMINAL);

	// initialize Finger Print Sensor
	//iotFpSensor::init(UART_SENSOR, TIMEOUT_TIMER);	// PORTE

	// initialize Servo Motor
	iotServo::init(iotPin::iotcPE0);			// PORTE: VCC=R, GND=B, PWM=O //송시부에는 서보 모터 x 수신부만..
	
	
	
	// initialize Led
	iotLed::init(iotPin::iotcPD0);			// PORTD:  GND=B 0번 핀
	iotSubLed::init(iotPin::iotcPD1);			// PORTD:  GND=B 1번 핀
	
	
	// initialize DC Fan Motor - iotTimer::iotcTimer2
	// PORTF pin0 to OUTPUT
	FanTimer = new iotTimer(iotTimer::iotcTimer2, iotTimer::iotcPsdNo, 250);
	DDRF |= 0x01;
	PORTF |= 0x01;
	
	
	//iotTimer	timer0(iotTimer::iotcTimer0, iotTimer::iotcPsd64, 250);
	//timer0.enableInterrupt(updateLed);
	
	DDRB = 0x0f;
	
	DDRD = 0x0f; // portD 
	
	/*something more??*/
	
	// initialize infra-red receiver
	IrRecv = new iotIrReceiver(IR_TIMER);
	IrRecv->enable();
	// initialize infra-red transmitter
	IrTrans = new iotIrTransmitter(IR_TIMER);
	
	
	
	sei();

	// use PE0 & PE1 for RX0 & TX0, and PC0 for DHT11
	Dht = new iotDht(iotPin::iotcPC0);				// PORTC
	//iotDustSensor::init(iotPin::iotcPF1, iotPin::iotcPF0);	// PORTF
	int n = 0;
	char	*s;
	while(1) {
		
		iotClock::processOne();
		CharLcd->processOne();
		iotTerminal::processOne();
		//iotFpSensor::processOne();
		if((s = iotTerminal::getLine())) parseLine(s);

		// 추가한 부분
		
		
		
		
		if((dec = IrRecv->getDecoder())) {
			if(dec->decode() != iotcUnknown) {
				printf("%d: 0x%lx(%d)\n\r",
					dec->getVendor(), dec->getValue(), dec->getBits()); // 11: 0x001010(32)
				if(dec->getVendor() == iotcSamsung) { //&& (NumKeys < MaxKeys)
					/*
					if(dec->getValue() == 0xe0e020df) {
						
						//IrTrans->sendSamsung(0xe0e020df, 32);  // 받으면 자동으로 전송하도록 
						
						printf("number1\n"); // 명령어 처주면 될듯
						CharLcd->print(2, 1, "rx : number1");
					}*/
					
					switch(dec->getValue()) {
						case 0xe0e020df:
							CharLcd->print(2, 1, "rx : number1    ");
							break;
						case 0xe0e0a05f:
							CharLcd->print(2, 1, "rx : number2    ");
							break;
						case 0xe0e0609f:
							CharLcd->print(2, 1, "rx : number3    ");
							break;
						case 0xe0e010ef:
							CharLcd->print(2, 1, "rx : number4    ");
							break;
						case 0xe0e0906f:
							CharLcd->print(2, 1, "rx : number5    ");
							break;
						case 0xe0e050af:
							CharLcd->print(2, 1, "rx : number6    ");
							break;
						case 0xe0e030cf:
							CharLcd->print(2, 1, "rx : number7    ");
							break;
						case 0xe0e0b04f:
							CharLcd->print(2, 1, "rx : number8    ");
							break;
						case 0xe0e0708f:
							CharLcd->print(2, 1, "rx : number9    ");
							break;
						case 0xe0e08877:
							CharLcd->print(2, 1, "rx : number0    ");
							break;
						case 0xe0e0e01f:
							CharLcd->print(2, 1, "rx : vol up     ");
							break;
						case 0xe0e0d02f:
							CharLcd->print(2, 1, "rx : vol down   ");
							break;
						case 0xe0e048b7:
							CharLcd->print(2, 1, "rx : ch up      ");
							break;
						case 0xe0e008f7:
							CharLcd->print(2, 1, "rx : ch down    ");
							break;
							//testing
						case 0xe0e00000:	//fan 0
							CharLcd->print(2, 1, "fan turn off    ");
							n = 0;
							if (n < 0) n = 0;
							else if (n > cMaxFanSpeed) n = cMaxFanSpeed;
							if(FanSpeed == 0) {
								if(n > 0) {
									FanTimer->configure(iotTimer::iotcPsd64, 250);
									FanTimer->enableInterrupt(toggleFan);
								}
								} else {
								if(n == 0) {
									FanTimer->configure(iotTimer::iotcPsdNo, 250);
									FanTimer->disableInterrupt();
								}
							}
							FanSpeed = n;
							FanCount = FanSpeed;
							FanState = false;
							break;
						case 0xe0e00001:	//fan 1
							CharLcd->print(2, 1, "fan slow speed  ");
							n = 1;
							if (n < 0) n = 0;
							else if (n > cMaxFanSpeed) n = cMaxFanSpeed;
							if(FanSpeed == 0) {
								if(n > 0) {
									FanTimer->configure(iotTimer::iotcPsd64, 250);
									FanTimer->enableInterrupt(toggleFan);
								}
								} else {
								if(n == 0) {
									FanTimer->configure(iotTimer::iotcPsdNo, 250);
									FanTimer->disableInterrupt();
								}
							}
							FanSpeed = n;
							FanCount = FanSpeed;
							FanState = false;
							break;
						case 0xe0e00002:	//fan 1
							CharLcd->print(2, 1, "fan medium speed");
							n = 2;
							if (n < 0) n = 0;
							else if (n > cMaxFanSpeed) n = cMaxFanSpeed;
							if(FanSpeed == 0) {
								if(n > 0) {
									FanTimer->configure(iotTimer::iotcPsd64, 250);
									FanTimer->enableInterrupt(toggleFan);
								}
								} else {
								if(n == 0) {
									FanTimer->configure(iotTimer::iotcPsdNo, 250);
									FanTimer->disableInterrupt();
								}
							}
							FanSpeed = n;
							FanCount = FanSpeed;
							FanState = false;
							break;
							
						case 0xe0e00004:	//fan 1
							CharLcd->print(2, 1, "fan high speed  ");
							n = 4;
							if (n < 0) n = 0;
							else if (n > cMaxFanSpeed) n = cMaxFanSpeed;
							if(FanSpeed == 0) {
								if(n > 0) {
									FanTimer->configure(iotTimer::iotcPsd64, 250);
									FanTimer->enableInterrupt(toggleFan);
								}
								} else {
								if(n == 0) {
									FanTimer->configure(iotTimer::iotcPsdNo, 250);
									FanTimer->disableInterrupt();
								}
							}
							FanSpeed = n;
							FanCount = FanSpeed;
							FanState = false;
							break;
						case 0xe0e040bc:
							if(lcdon) {
								CharLcd->print(2, 1, "power off       ");
								lcdon = 0;
							} else {
								CharLcd->print(2, 1, "power on        ");
								lcdon = 1;
							}
							break;
							
							
							
					}
					/*
					int		i;
					for(i = 0; i < NumKeys; i++) {
						if(Key[i] == dec->getValue()) break;
					}
					if(i == NumKeys)
						Key[NumKeys++] = dec->getValue();
					*/
				}
			} else dec->dumpIntervals();
			IrRecv->enable();
		} 
	}
}