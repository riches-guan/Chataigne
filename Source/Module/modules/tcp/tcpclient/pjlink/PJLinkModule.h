/*
  ==============================================================================

    PJLinkModule.h
    Created: 5 Jan 2018 3:41:58pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#pragma once

class PJLinkModule :
	public TCPClientModule
{
public:
	PJLinkModule();
	~PJLinkModule() {}

	StringParameter* password;

	EnumParameter * powerStatus;
	BoolParameter * shutterVideoStatus;
	BoolParameter * shutterAudioStatus;

	String passBytes;
	float timeAtConnect;

	virtual void sendMessageInternal(const String& message, var) override;

	CommandDefinition * getBasePJCommand(const String &menu, const String &commandName, const String &command, CommandContext context = CommandContext::ACTION);

	static PJLinkModule * create() { return new PJLinkModule(); }
	virtual String getDefaultTypeString() const override { return "PJLink"; }

	void initThread() override;
	void runInternal() override;

	void processDataLineInternal(const String &message) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PJLinkModule)
};