//=============================================================================
// irRemote.cpp
//	The implementation of the infra-red remote controller module of 
//		the Iot package. (INU Object Toolkit for the Internet of Things)
//	Created: 2015-01-14 오후 5:49:47
//	Author: kkim@inu.ac.kr / SDTLAB INU (Incheon National University)
//=============================================================================

#include "irRemote.h"

iotIrDecoder	*iotIrReceiver::iDecoder = (iotIrDecoder *) NULL;
iotTimer		*iotIrReceiver::iTimer = (iotTimer *) NULL;

iotTimer		*iotIrTransmitter::iTimer = (iotTimer *) NULL;

//-----------------------------------------------------------------------------
//	iotIrRcReceiver::iotIrRcReceiver -
//	iotIrRcReceiver::~iotIrRcReceiver -
//-----------------------------------------------------------------------------
iotIrReceiver::iotIrReceiver(iotTimer::iotTimerId aTimerId) {
	if(iDecoder) return;
	iDecoder = new iotIrDecoder;

	if(iotIrTransmitter::iTimer) iTimer = iotIrTransmitter::iTimer;
	else
		iTimer = new iotTimer(aTimerId, iotTimer::iotcPsd1,
			F_CPU * iotcIrUSecPerTick / 1000000);
}
iotIrReceiver::~iotIrReceiver() {
	delete iDecoder; iDecoder = (iotIrDecoder *) NULL;
	if(!iotIrTransmitter::iTimer) delete iTimer;
	iTimer = (iotTimer *) NULL;
}

//-----------------------------------------------------------------------------
//	iotIrRcTransmitter::iotIrRcTransmitter -
//	iotIrRcTransmitter::~iotIrTransmitter -
//-----------------------------------------------------------------------------
iotIrTransmitter::iotIrTransmitter(iotTimer::iotTimerId aTimerId) {
	if(iotIrReceiver::iTimer) iTimer = iotIrReceiver::iTimer;
	else
		iTimer = new iotTimer(aTimerId, iotTimer::iotcPsd1,
			F_CPU * iotcIrUSecPerTick / 1000000);
}
iotIrTransmitter::~iotIrTransmitter() {
	if(!iotIrReceiver::iTimer) delete iTimer;
	iTimer = (iotTimer *) NULL;
}