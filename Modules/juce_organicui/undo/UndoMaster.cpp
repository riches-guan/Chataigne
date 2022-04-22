/*
  ==============================================================================

  UndoMaster.cpp
  Created: 12 Apr 2017 8:10:30am
  Author:  Ben

  ==============================================================================
  */

juce_ImplementSingleton(UndoMaster);

UndoMaster::UndoMaster() :
	isPerforming(false)
{
}

UndoMaster::~UndoMaster() 
{
}

void UndoMaster::performAction(const String & name, UndoableAction * action)
{
	if (Engine::mainEngine != nullptr && Engine::mainEngine->isLoadingFile) return;
	isPerforming = true;
	beginNewTransaction(name);
	perform(action,name);
	isPerforming = false;
	if (Engine::mainEngine != nullptr) Engine::mainEngine->changed();
}

void UndoMaster::performActions(const String & name, Array<UndoableAction*> actions)
{
	if (Engine::mainEngine != nullptr && Engine::mainEngine->isLoadingFile) return;
	isPerforming = true;
	beginNewTransaction(name);
	for (auto &a : actions) perform(a,name);
	isPerforming = false;
	if (Engine::mainEngine != nullptr) Engine::mainEngine->changed();
}
