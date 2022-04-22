/*
  ==============================================================================

    ReaperTimeCommand.h
    Created: 26 Jan 2017 2:42:39pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class ReaperTimeCommand :
	public OSCCommand
{
public:
	ReaperTimeCommand(ReaperModule* _module, CommandContext context, var params, Multiplex * multiplex = nullptr);
	~ReaperTimeCommand();

	ReaperModule* reaperModule;
	BoolParameter* stopTimePlay;
	FloatParameter* timeParam;

	void triggerInternal(int multiplexIndex) override;

	static ReaperTimeCommand* create(ControllableContainer* module, CommandContext context, var params, Multiplex * multiplex) { return new ReaperTimeCommand((ReaperModule*)module, context, params, multiplex); }

};