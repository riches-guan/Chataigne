/*
  ==============================================================================

	SequenceAudioCommand.cpp
	Created: 27 Sep 2020 10:57:00am
	Author:  bkupe

  ==============================================================================
*/

#include "TimeMachine/ChataigneSequenceManager.h"

SequenceAudioCommand::SequenceAudioCommand(SequenceModule* _module, CommandContext context, var params, Multiplex * multiplex) :
	BaseCommand(_module, context, params, multiplex),
	sequenceModule(_module),
	value(nullptr)
{
	actionType = (ActionType)(int)params.getProperty("type", SET_VOLUME);

	target = addTargetParameter("Target", "Target for the command");
	target->targetType = TargetParameter::CONTAINER;

	target->customGetTargetContainerFunc = &ChataigneSequenceManager::showMenuAndGetAudioLayerStatic;

	time = addFloatParameter("Time", "Target time to set", 0);
	time->defaultUI = FloatParameter::TIME;

	automation = new Automation();
	automation->isSelectable = false;
	automation->length->setValue(1);
	automation->addKey(0, 0, false);
	automation->items[0]->easingType->setValueWithData(Easing::BEZIER);
	automation->addKey(1, 1, false);
	automation->selectItemWhenCreated = false;
	automation->hideEditorHeader = true;
	addChildControllableContainer(automation, true);

	switch (actionType)
	{
	case SET_VOLUME:
		value = addFloatParameter("Volume", "The target volume to set", 1);
		break;
	}

	if (value != nullptr) linkParamToMappingIndex(value, 0);

	stopAtFinish = addBoolParameter("Stop at Finish", "If enabled, will stop the sequence when finished", false);
}

SequenceAudioCommand::~SequenceAudioCommand()
{
}

void SequenceAudioCommand::triggerInternal(int multiplexIndex)
{
	switch (actionType)
	{
	case SET_VOLUME:
	{
		if (AudioLayer* al = getLinkedTargetContainerAs<AudioLayer>(target, multiplexIndex))
		{
			al->setVolume((float)getLinkedValue(value, multiplexIndex), (float)getLinkedValue(time, multiplexIndex), automation, (int)getLinkedValue(stopAtFinish,multiplexIndex));
		}
	}
	break;
	}

}

void SequenceAudioCommand::loadJSONDataInternal(var data)
{
	if (Engine::mainEngine->isLoadingFile)
	{
		Engine::mainEngine->addEngineListener(this);
		dataToLoad = data;
	}
	else BaseCommand::loadJSONDataInternal(data);
}

void SequenceAudioCommand::endLoadFile()
{
	//reset data we want to reload
	if (target != nullptr) target->setValue("", true);

	loadJSONData(dataToLoad);
	dataToLoad = var();

	Engine::mainEngine->removeEngineListener(this);
}

BaseCommand* SequenceAudioCommand::create(ControllableContainer* module, CommandContext context, var params, Multiplex * multiplex)
{
	return new SequenceAudioCommand((SequenceModule*)module, context, params, multiplex);
}
