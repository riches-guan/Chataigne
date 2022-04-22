/*
  ==============================================================================

    StreamingCommand.h
    Created: 27 Jan 2018 12:22:20pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class StreamingCommand :
	public BaseCommand
{
public:
	StreamingCommand(StreamingModule * _module, CommandContext context, var params, Multiplex* multiplex = nullptr);
	virtual ~StreamingCommand();

	StreamingModule * streamingModule;
};