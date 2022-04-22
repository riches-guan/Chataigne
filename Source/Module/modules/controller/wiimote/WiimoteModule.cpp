/*
  ==============================================================================

	WiimoteModule.cpp
	Created: 26 Dec 2016 6:20:01pm
	Author:  Ben

  ==============================================================================
*/


WiimoteModule::WiimoteModule(const String & name) :
	Module(name),
    irCC("IR"),
    device(nullptr)
{
	setupIOConfiguration(true, false);

	deviceID = moduleParams.addIntParameter("Device Select", "Select the ID of the device, depending on how many wiimotes are connected (max 4)", 1, 1, 4);
	connectTrigger = moduleParams.addTrigger("Refresh Wiimotes", "Try to reconnect the wiimotes");
	smoothing = moduleParams.addFloatParameter("Smoothing", "Set the orientation smoothing for the wiimote orientation algorithm", .5f, 0, 1);

	connected = valuesCC.addBoolParameter("Connected", "Wiimote is connected ?", false);

	nunchuckConnected = valuesCC.addBoolParameter("Nunchuck", "Nunchuck is connected ? ", false);
	motionPlusConnected = valuesCC.addBoolParameter("MotionPlus", "MotionPlus is connected ? ", false);
	batteryLevel = valuesCC.addFloatParameter("Battery", "Battery level of the wiimote", 0, 0, 1);
	pitch = valuesCC.addFloatParameter("Pitch", "Pitch of the wiimote", 0, -1, 1);
	roll = valuesCC.addFloatParameter("Roll", "Roll of the wiimote", 0, -1, 1);
	yaw = valuesCC.addFloatParameter("Yaw", "Yaw of the wiimote.\nMotion Plus must be connected !", 0, -1, 1);

	nunchuckX = valuesCC.addFloatParameter("Nunchuck X", "X axis of the nunchuck if plugged",0,-1,1);
	nunchuckY = valuesCC.addFloatParameter("Nunchuck Y", "Y axis of the nunchuck if plugged",0,-1,1);

	accelX = valuesCC.addFloatParameter("Accel X", "Raw values of accelerometer",0,-1,1);
	accelY = valuesCC.addFloatParameter("Accel Y", "Raw values of accelerometer", 0, -1, 1);
	accelZ = valuesCC.addFloatParameter("Accel Z", "Raw values of accelerometer", 0, -1, 1);
	rawAccelX = valuesCC.addFloatParameter("Raw Accel X", "Raw values of accelerometer", 0, 0,300);
	rawAccelY = valuesCC.addFloatParameter("Raw Accel Y", "Raw values of accelerometer", 0, 0,300);
	rawAccelZ = valuesCC.addFloatParameter("Raw Accel Z", "Raw values of accelerometer", 0, 0,300);

	

	
	BoolParameter * bt_a = valuesCC.addBoolParameter("Button A", "Button A", false);
	BoolParameter * bt_b = valuesCC.addBoolParameter("Button B", "Button B", false);
	BoolParameter * bt_c = valuesCC.addBoolParameter("Button C", "Button C", false);
	BoolParameter * bt_z = valuesCC.addBoolParameter("Button Z", "Button Z", false);
	BoolParameter * bt_1 = valuesCC.addBoolParameter("Button 1", "Button 1", false);
	BoolParameter * bt_2 = valuesCC.addBoolParameter("Button 2", "Button 2", false);
	BoolParameter * bt_h = valuesCC.addBoolParameter("Button Home", "Button Home", false);
	BoolParameter * bt_mi = valuesCC.addBoolParameter("Button -", "Button -", false);
	bt_mi->setCustomShortName("buttonMinus");
	BoolParameter * bt_pl = valuesCC.addBoolParameter("Button +", "Button +", false);
	bt_pl->setCustomShortName("buttonPlus"); //avoid autoShortName to mix up + and - as they're both converted to _ (needs fixing?)
	BoolParameter * bt_up = valuesCC.addBoolParameter("Button Up", "Button Up", false);
	BoolParameter * bt_do = valuesCC.addBoolParameter("Button Down", "Button Down", false);
	BoolParameter * bt_le = valuesCC.addBoolParameter("Button Left", "Button Left", false);
	BoolParameter * bt_ri = valuesCC.addBoolParameter("Button Right", "Button Right", false);


	buttons.addArray({ bt_2,bt_1,bt_b,bt_a,bt_mi,bt_c,bt_z,bt_h,bt_le,bt_ri,bt_do,bt_up,bt_pl });

	irNumPoints = irCC.addIntParameter("Num Points", "Number of detected Infrared sources by the wiimote", 0, 0, 4);
	irDistance = irCC.addFloatParameter("Point Distance", "Distance detected by the first 2 points", 0, 0);
	irPos = irCC.addPoint3DParameter("Position", "Estimated 3D position from the detected points");
	for (int i = 0; i < 4; i++)
	{
		Point2DParameter* p2d = irCC.addPoint2DParameter("Point " + String(i + 1), "Normalized -1/1 position of this point");
		p2d->setBounds(-1, -1, 1, 1);
		irPoints.add(p2d);
	}

	valuesCC.addChildControllableContainer(&irCC);

	for (auto &c : valuesCC.controllables) c->isControllableFeedbackOnly = true;

	connectionFeedbackRef = connected;

	WiimoteManager::getInstance()->addListener(this);
}

WiimoteModule::~WiimoteModule()
{
	if (WiimoteManager::getInstanceWithoutCreating() != nullptr) WiimoteManager::getInstance()->removeListener(this);
	if (device != nullptr)
	{
		device->removeListener(this);
		device = nullptr;
	}
}

void WiimoteModule::deviceConnected(Wiimote * w)
{
	if (device != nullptr || w == device) return;

	device = w;
	w->addListener(this);
	connected->setValue(true);
	smoothing->setValue(device->smoothing);
}

void WiimoteModule::deviceDisconnected(Wiimote * w)
{
	if (w != device) return;
	device->removeListener(this);
	device = nullptr;
	connected->setValue(false);
}

void WiimoteModule::wiimoteButtonPressed(Wiimote * w, Wiimote::WiimoteButton b)
{
	if (w != device) return;
	buttons[(int)b]->setValue(true);
	inActivityTrigger->trigger();
}

void WiimoteModule::wiimoteButtonReleased(Wiimote * w, Wiimote::WiimoteButton b)
{
	if (w != device) return;
	buttons[(int)b]->setValue(false);
	inActivityTrigger->trigger();
}

void WiimoteModule::wiimoteOrientationUpdated(Wiimote * w)
{
	if (w != device) return;

	pitch->setValue(device->pitch);
	roll->setValue(device->roll);
	yaw->setValue(device->yaw);
}

void WiimoteModule::wiimoteAccelUpdated(Wiimote * w)
{
	if (w != device) return;
	accelX->setValue(device->accelX);
	accelY->setValue(device->accelY);
	accelZ->setValue(device->accelZ);
	rawAccelX->setValue(device->rawAccelX);
	rawAccelY->setValue(device->rawAccelY);
	rawAccelZ->setValue(device->rawAccelZ);
}

void WiimoteModule::wiimoteJoystickUpdated(Wiimote * w)
{
	if (w != device) return;
	nunchuckX->setValue(device->joystickX);
	nunchuckY->setValue(device->joystickY);
}

void WiimoteModule::wiimoteIRPointsUpdated(Wiimote* w)
{
	if (w != device) return;
	irNumPoints->setValue(device->irNumDots);
	irDistance->setValue(device->irDistance);
	irPos->setVector(device->irPos);
	for (int i = 0; i < device->irNumDots; i++) irPoints[i]->setPoint(device->irPoints[i]);
}

void WiimoteModule::wiimoteBatteryLevelChanged(Wiimote * w)
{
	if (w != device) return;
	batteryLevel->setValue(device->batteryLevel);
}

void WiimoteModule::wiimoteNunchuckPlugged(Wiimote *w)
{
	if (w != device) return;
	nunchuckConnected->setValue(true);
}

void WiimoteModule::wiimoteNunchuckUnplugged(Wiimote *w)
{
	if (w != device) return;
	nunchuckConnected->setValue(false);
}

void WiimoteModule::wiimoteMotionPlusPlugged(Wiimote *w)
{
	if (w != device) return;
	motionPlusConnected->setValue(true);
}

void WiimoteModule::wiimoteMotionPlusUnplugged(Wiimote *w)
{
	if (w != device) return;
	motionPlusConnected->setValue(false);
}

void WiimoteModule::onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable * c)
{
	Module::onControllableFeedbackUpdateInternal(cc, c);

	if (c == smoothing)
	{
		if (device != nullptr) device->setSmoothing(1 - (smoothing->floatValue()*.9f) + .1f); //force minimum .1f
	}else if (c == connectTrigger)
	{
		WiimoteManager::getInstance()->numReconnectTries = 0;
		WiimoteManager::getInstance()->reinitWiimotes = true;
	}
}
