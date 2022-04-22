/*
  ==============================================================================

    InputDeviceHelpers.cpp
    Created: 17 Jun 2018 8:45:30pm
    Author:  Ben

  ==============================================================================
*/

GamepadParameterUI::GamepadParameterUI(Array<GamepadParameter *> parameters) :
	ParameterUI(Inspectable::getArrayAs<GamepadParameter, Parameter>(parameters)),
	gamepadParams(parameters),
	gamepadParam(parameters[0])
{
	chooser.setTextWhenNoChoicesAvailable("No gamepad connected");
	chooser.setTextWhenNothingSelected("Select a gamepad");
	chooser.addListener(this);

	chooser.setColour(chooser.backgroundColourId, BG_COLOR);
	chooser.setColour(chooser.outlineColourId, BG_COLOR.brighter(.2f));
	chooser.setColour(chooser.focusedOutlineColourId, BG_COLOR.brighter(.3f));
	chooser.setColour(chooser.textColourId, TEXT_COLOR);
	chooser.setColour(chooser.buttonColourId, TEXT_COLOR);
	chooser.setColour(chooser.arrowColourId, TEXT_COLOR.darker(.2f));

	addAndMakeVisible(&chooser);

	InputSystemManager::getInstance()->addAsyncInputListener(this);

	rebuild();
}

GamepadParameterUI::~GamepadParameterUI()
{
	if (InputSystemManager::getInstanceWithoutCreating() != nullptr) InputSystemManager::getInstance()->removeAsyncInputListener(this);
}

WeakReference<Gamepad> GamepadParameterUI::getGamepad()
{
	return chooser.getSelectedId() > 0 ? gamepads[chooser.getSelectedId() - 1] : nullptr;
}

void GamepadParameterUI::rebuild()
{
	WeakReference<Gamepad> selectedGamepad = getGamepad();

	chooser.clear(dontSendNotification);
	chooser.setTextWhenNothingSelected(gamepadParam->ghostName.isNotEmpty()?gamepadParam->ghostName+" disconnected":"Select a gamepad");
	chooser.addItem("Not connected", -2);

	if (InputSystemManager::getInstance()->gamepads.size() == 0) return;

	int id = 1;
	int idToSelect = -1;
	for (auto &g : InputSystemManager::getInstance()->gamepads)
	{
		gamepads.add(g);
		String gName = g->getName();
		chooser.addItem(gName, id);
		if (g == selectedGamepad || g == gamepadParam->gamepad) idToSelect = id;
		id++;
	}

	chooser.setSelectedId(idToSelect,dontSendNotification);
}

void GamepadParameterUI::resized()
{
	chooser.setBounds(getLocalBounds());
}

void GamepadParameterUI::valueChanged(const var & v)
{
	ParameterUI::valueChanged(v);
	chooser.setSelectedId(gamepads.indexOf(gamepadParam->gamepad) + 1);
}

void GamepadParameterUI::newMessage(const InputSystemManager::InputSystemEvent & e)
{
	switch (e.type)
	{
	case InputSystemManager::InputSystemEvent::GAMEPAD_ADDED:
	case InputSystemManager::InputSystemEvent::GAMEPAD_REMOVED:
		rebuild();
		break;

	default:
		break;
	}
}

void GamepadParameterUI::comboBoxChanged(ComboBox * c)
{
	if (!parameter.wasObjectDeleted())
	{
		if (chooser.getSelectedId() == -2)
		{
			gamepadParam->ghostID = SDL_JoystickGUID();
			gamepadParam->ghostName = "";
		}
		gamepadParam->setGamepad(getGamepad());
	}
}
