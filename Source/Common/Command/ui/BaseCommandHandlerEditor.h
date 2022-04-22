/*
  ==============================================================================

    BaseCommandHandlerEditor.h
    Created: 19 Jan 2017 6:41:22pm
    Author:  Ben

  ==============================================================================
*/
#pragma once

#include "JuceHeader.h"
#include "../CommandContext.h"
#include "CommandChooserUI.h"
#include "../BaseCommandHandlerListener.h"

class BaseCommandHandler;

class BaseCommandHandlerEditor :
	public BaseItemEditor,
	public CommandChooserUI::Listener,
	public BaseCommandHandlerAsyncListener
{
public:
	BaseCommandHandlerEditor(BaseCommandHandler *, bool isRoot);
	virtual ~BaseCommandHandlerEditor();

	BaseCommandHandler * handler;

	CommandChooserUI chooser;
	std::unique_ptr<TriggerButtonUI> triggerBT;

	void resizedInternalHeaderItemInternal(Rectangle<int> &r) override;

	void updateChooserLabel();

	void definitionChosen(CommandDefinition * d) override;
	void newMessage(const CommandHandlerEvent &e) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BaseCommandHandlerEditor)
};