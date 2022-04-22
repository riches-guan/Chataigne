/*
  ==============================================================================

	SerialModule.cpp
	Created: 15 Mar 2017 10:17:22am
	Author:  Ben-Portable

  ==============================================================================
*/

SerialModule::SerialModule(const String& name) :
	StreamingModule(name),
	port(nullptr)
{
	portParam = new SerialDeviceParameter("Port", "Serial Port to connect", true);
	moduleParams.addParameter(portParam);
	baudRate = moduleParams.addIntParameter("Baud Rate", "The connection speed. Common values are 9600, 57600, 115200", 115200, 9600, 1000000);
	portParam->openBaudRate = baudRate->intValue();

	isConnected = moduleParams.addBoolParameter("Is Connected", "This is checked if a serial port is connected.", false);
	isConnected->setControllableFeedbackOnly(true);
	isConnected->isSavable = false;
	connectionFeedbackRef = isConnected;

	SerialManager::getInstance()->addSerialManagerListener(this);

}

SerialModule::~SerialModule()
{
	if (SerialManager::getInstanceWithoutCreating() != nullptr)
	{
		SerialManager::getInstance()->removeSerialManagerListener(this);
	}

	setCurrentPort(nullptr);
}

bool SerialModule::setPortStatus(bool status) 
{
	if (port == nullptr) 
	{
		isConnected->setValue(false);
		return false;
	}

	bool shouldOpen = status && enabled->boolValue();

	if (shouldOpen)  //We want to open the port, it's not already opened and the module is enabled
	{
		port->setMode(streamingType->getValueDataAsEnum<SerialDevice::PortMode>()); //always set mode, port might be already open with default mode
		port->setBaudRate(baudRate->intValue());
		setupPortInternal();
		if (port->isOpen()) port->close();
		port->open(baudRate->intValue());
		if (!port->isOpen())
		{
			NLOGERROR(niceName, "Could not open port : " << port->info->port);
			port = nullptr; //Avoid crash if SerialPort is busy
		}
		else
		{
			NLOG(niceName, "Port " << port->info->port << " opened, have fun.");
		}
	}
	else  //We want to close the port or the module is disabled, and it's actually opened
	{
		NLOG(niceName, "Port closed : " << port->info->port);
		if (port->isOpen()) port->close();
	}

	isConnected->setValue(port->isOpen());
	return port->isOpen();
}

void SerialModule::setCurrentPort(SerialDevice* _port)
{
	if (port == _port) return;

	setPortStatus(false);

	if (port != nullptr)
	{
		portClosedInternal();
		port->removeSerialDeviceListener(this);
	}

	port = _port;

	if (port != nullptr)
	{
		port->addSerialDeviceListener(this);
		setPortStatus(true);
		lastOpenedPortID = port->info->deviceID;
		portOpenedInternal();
	}
	
	serialModuleListeners.call(&SerialModuleListener::currentPortChanged);
}

void SerialModule::onContainerParameterChangedInternal(Parameter* p)
{
	StreamingModule::onContainerParameterChangedInternal(p);
	if (p == enabled)
	{
		if (port != nullptr)
		{
			setPortStatus(true);

			NLOG(niceName, "Module is " << (enabled->boolValue() ? "enabled" : "disabled") << ", " << (enabled->boolValue() ? "opening" : "closing serial port"));
		}
	}
}

void SerialModule::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c)
{
	StreamingModule::onControllableFeedbackUpdateInternal(cc, c);

	if (c == baudRate)
	{
		portParam->openBaudRate = baudRate->intValue();
		if (port != nullptr && port->isOpen())
		{
			SerialDevice* d = portParam->getDevice();
			if (d != nullptr) d->setBaudRate(portParam->openBaudRate);
		}

	}
	if (c == portParam)
	{
		SerialDevice* newDevice = portParam->getDevice();
		SerialDevice* prevPort = port;
		setCurrentPort(newDevice);

		if (port == nullptr && prevPort != nullptr)
		{
			DBG("Manually set no ghost port");
			lastOpenedPortID = ""; //forces no ghosting when user chose to manually disable port
		}
	}if (c == streamingType)
	{
		if (port != nullptr) port->setMode((SerialDevice::PortMode)(int)streamingType->getValueData());
	}
}
bool SerialModule::isReadyToSend()
{
	if (port == nullptr)
	{
		NLOGWARNING(niceName, "You must set a device before sending data.");
		return false;
	}

	return true;
}
void SerialModule::sendMessageInternal(const String& message, var)
{
	if (port == nullptr) return;
	port->writeString(message);
}

void SerialModule::sendBytesInternal(Array<uint8> data, var)
{
	if (port == nullptr) return;
	port->writeBytes(data);
}


void SerialModule::portOpened(SerialDevice*)
{
	serialModuleListeners.call(&SerialModuleListener::portOpened);
}

void SerialModule::portClosed(SerialDevice*)
{
	portClosedInternal();
	serialModuleListeners.call(&SerialModuleListener::portClosed);
}

void SerialModule::portRemoved(SerialDevice*)
{
	setCurrentPort(nullptr);
}

void SerialModule::serialDataReceived(const var& data)
{
	switch (port->mode)
	{

	case SerialDevice::LINES:
		processDataLine(data.toString());
		break;

	case SerialDevice::DATA255:
	case SerialDevice::RAW:
	case SerialDevice::COBS:
		if (data.isBinaryData() && data.getBinaryData() != nullptr)
		{
			Array<uint8> bytes((const uint8_t*)data.getBinaryData()->getData(), (int)data.getBinaryData()->getSize());
			processDataBytes(bytes);
		}
		else
		{
			NLOGWARNING(niceName, "Wrong data type detected, skipping");
		}
		break;

	}
}

var SerialModule::getJSONData()
{
	var data = StreamingModule::getJSONData();
	data.getDynamicObject()->setProperty("portID", lastOpenedPortID);
	return data;
}

void SerialModule::loadJSONDataInternal(var data)
{
	StreamingModule::loadJSONDataInternal(data);
	lastOpenedPortID = data.getProperty("portID", "");
}

void SerialModule::setupModuleFromJSONData(var data)
{
	if (data.hasProperty("vidFilter")) portParam->vidFilter = data.getDynamicObject()->getProperty("vidFilter").toString().getHexValue32();
	if (data.hasProperty("pidFilter")) portParam->pidFilter = data.getDynamicObject()->getProperty("pidFilter").toString().getHexValue32();

	StreamingModule::setupModuleFromJSONData(data);
}

void SerialModule::portAdded(SerialDeviceInfo* info)
{
	if (port == nullptr && lastOpenedPortID == info->deviceID)
	{
		setCurrentPort(SerialManager::getInstance()->getPort(info));
	}
}

void SerialModule::portRemoved(SerialDeviceInfo* p)
{
	if (port != nullptr && port->info == p) setCurrentPort(nullptr);
}