/*
  ==============================================================================

    DMXCommand.h
    Created: 6 Apr 2017 10:22:35pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class DMXCommand :
	public BaseCommand
{
public:
	enum DMXAction { SET_VALUE, SET_RANGE, COLOR, SET_VALUE_16BIT, BLACK_OUT, SET_ALL, SET_CUSTOM };

	DMXCommand(DMXModule * _module, CommandContext context, var params, Multiplex* multiplex = nullptr);
	~DMXCommand();

	DMXAction dmxAction;
	DMXModule * dmxModule;

	EnumParameter * byteOrder;

	IntParameter * channel;
	IntParameter * channel2;
	IntParameter * value;
	ColorParameter * colorParam;

	int remapTarget;
	BoolParameter * remap01To255;

	void setValue(var value, int multiplexIndex) override;
	void triggerInternal(int multiplexIndex) override;


	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

	void itemAdded(CustomValuesCommandArgument* a) override;

	static BaseCommand * create(ControllableContainer * module, CommandContext context, var params, Multiplex * multiplex) { return new DMXCommand((DMXModule *)module, context, params, multiplex); }

	
};