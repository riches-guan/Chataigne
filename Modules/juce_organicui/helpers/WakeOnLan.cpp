/*
  ==============================================================================

    WakeOnLan.h
    Created: 27 Oct 2016 2:09:50pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "WakeOnLan.h"

int WakeOnLan::wake(MACAddress * macAddress)
{
		
	uint8 ffBytes[6] = { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF };
	const uint8* macBytes = macAddress->getBytes();

	const int packetSize = 6 + 16 * 6;
	uint8 packet[packetSize];

	for (int i = 0; i < 6; ++i) packet[i] = ffBytes[i];

	for (int i = 0; i < 16; ++i)
	{
		for (int j = 0; j < 6; j++)
		{
			int index = 6 + i * 6 + j;
			packet[index] = macBytes[j];
		}
	}

	DatagramSocket s(true);
	s.bindToPort(0);
	return s.write(IPAddress::broadcast().toString(), 9, packet, packetSize);


	//LOG("Wake ON Lan sent to " + macAddress->toString());
}
