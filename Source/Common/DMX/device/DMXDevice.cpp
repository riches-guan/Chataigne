/*
  ==============================================================================

    DMXDevice.cpp
    Created: 7 Apr 2017 11:22:47am
    Author:  Ben

  ==============================================================================
*/

DMXDevice::DMXDevice(const String& name, Type _type, bool canReceive) :
	ControllableContainer(name),
	type(_type),
	enabled(true),
	isConnected(false),
	canReceive(canReceive),
	inputCC(nullptr),
	outputCC(nullptr)
{
	saveAndLoadRecursiveData = true;

	DMXManager::getInstance()->addDMXManagerListener(this);

	memset(dmxDataOut, 0, 512 * sizeof(uint8));
	memset(dmxDataIn, 0, 512 * sizeof(uint8));

	if (canReceive)
	{
		inputCC = new EnablingControllableContainer("Input");
		addChildControllableContainer(inputCC, true);
	}

	outputCC = new EnablingControllableContainer("Output");
	addChildControllableContainer(outputCC, true);
	alwaysSend = outputCC->addBoolParameter("Always Send", "If checked, the device will always send the stored values to the constant rate set by the target rate parameter.\nIf you experience some lags, try unchecking this option.", true);
	targetRate = outputCC->addIntParameter("Target send rate", "If fixed rate is checked, this is the frequency in Hz of the sending rate", 40, 1, 20000);
	
	if (alwaysSend->boolValue()) startTimer(1000/targetRate->intValue());
}

DMXDevice::~DMXDevice()
{
	if (DMXManager::getInstanceWithoutCreating() != nullptr) DMXManager::getInstance()->removeDMXManagerListener(this);
	stopTimer();
}

void DMXDevice::setConnected(bool value)
{
	if (isConnected == value) return;
	isConnected = value;

	if (isConnected)
	{
		dmxDeviceListeners.call(&DMXDeviceListener::dmxDeviceConnected);
	} else
	{
		dmxDeviceListeners.call(&DMXDeviceListener::dmxDeviceDisconnected);
	}
}

void DMXDevice::sendDMXValue(int channel, int value) //channel 1-512
{
	{
		ScopedLock lock(dmxLock);
		if (channel < 0 || channel > 512) return;
		dmxDataOut[channel - 1] = (uint8)value;
	}
	
	if (!alwaysSend->boolValue()) sendDMXValues();
}

void DMXDevice::sendDMXRange(int startChannel, Array<int> values)
{
	{
		ScopedLock lock(dmxLock);
		int numValues = values.size();
		for (int i = 0; i < numValues; ++i)
		{
			int channel = startChannel + i;
			if (channel < 0) continue;
			if (channel > 512) break;

			dmxDataOut[channel - 1] = (uint8)(values[i]);
		}
	}
	
	if (!alwaysSend->boolValue()) sendDMXValues();
	
}

void DMXDevice::setDMXValuesIn(int numChannels, uint8* values, int startChannel, const String& sourceName)
{
	for (int i = startChannel; i < numChannels && i < 512; ++i) dmxDataIn[i] = values[i];
	dmxDeviceListeners.call(&DMXDeviceListener::dmxDataInChanged, numChannels, values, sourceName);
}

void DMXDevice::sendDMXValues()
{
	if (!outputCC->enabled->boolValue()) return;

	ScopedLock lock(dmxLock);
	sendDMXValuesInternal();
}



void DMXDevice::clearDevice()
{
	stopTimer();
}

DMXDevice * DMXDevice::create(Type type)
{
	switch (type)
	{
	case OPENDMX:
		return new DMXOpenUSBDevice();
		break;

	case ENTTEC_DMXPRO:
		return new DMXEnttecProDevice();
		break;

	case ENTTEC_MK2:
		return new DMXEnttecProDevice(); //tmp but seems to work for 1 universe
		break;

	case ARTNET:
		return new DMXArtNetDevice();
		break;

	case SACN:
		return new DMXSACNDevice();
		break;

	default:
		DBG("Not handled");
		break;
	}

	return nullptr;
}

void DMXDevice::onControllableFeedbackUpdate(ControllableContainer* cc, Controllable* c)
{
	if (c == alwaysSend || c == targetRate)
	{
		if (c == alwaysSend) targetRate->setEnabled(alwaysSend->boolValue());
		
		if (alwaysSend->boolValue()) startTimer(1000/targetRate->intValue());
		else stopTimer();
	}
}

void DMXDevice::hiResTimerCallback()
{
	if (!enabled) return;
	if (!alwaysSend->boolValue()) return;

	sendDMXValues();
}
