/*
  ==============================================================================

	Conductor.cpp
	Created: 5 Oct 2021 9:35:28pm
	Author:  bkupe

  ==============================================================================
*/

Conductor::Conductor(var params, Multiplex* multiplex) :
	Action(params, multiplex),
	currentCue(nullptr),
	processorManager("Processors", multiplex)
{
	saveAndLoadRecursiveData = true;
	type = CONDUCTOR;

	setNiceName("Conductor");
	nextCueIndex = addIntParameter("Next Cue", "The Index of the action that will be triggered when", 1, 1);

	currentCueName = addStringParameter("Current Cue Name", "The name of the last triggered Cue.", "");
	currentCueName->setControllableFeedbackOnly(true);
	nextCueName = addStringParameter("Next Cue Name", "The name of the cue that will be trigger if triggering the conductor", "");
	nextCueName->setControllableFeedbackOnly(true);

	currentCueIndex = addIntParameter("Current Cue", "The Index of the last action triggered", 1, 1);
	currentCueIndex->setControllableFeedbackOnly(true);
	cueTriggerSetCurrent = addBoolParameter("Cue Trigger Set Current", "If checked, triggering a cue manually sets it current and sets cueIndex to next cue", true);
	triggerConductorConsequencesOnDirect = addBoolParameter("Cue Trigger Consequences", "If checked, when directly launching a cue will also trigger this conductor's consequences.", true);

	loop = addBoolParameter("Loop", "If checked, when finishing the cue list, this will set the cue index to 0 again", false);

	processorManager.factory.defs.clear();
	processorManager.factory.defs.add(MultiplexTargetDefinition<Processor>::createDef<ConductorCue>("", "Cue", multiplex));
	processorManager.hideInEditor = true;
	processorManager.addBaseManagerListener(this);
	addChildControllableContainer(&processorManager);
}

Conductor::~Conductor()
{
}

void Conductor::onContainerParameterChangedInternal(Parameter* p)
{
	Action::onContainerParameterChangedInternal(p);
	if (p == nextCueIndex) updateNextCue();
}

void Conductor::updateDisables(bool force)
{
	Action::updateDisables(force);
	processorManager.setForceDisabled(!enabled->boolValue() || forceDisabled);
}

void Conductor::itemAdded(Processor* p)
{
	if (ConductorCue* c = dynamic_cast<ConductorCue*>(p)) c->addActionListener(this);
	if (!isCurrentlyLoadingData && !isClearing) nextCueIndex->setRange(1, jmax(processorManager.items.size() + 1, 2));
	updateIndices();
}

void Conductor::itemsAdded(Array<Processor*> pList)
{
	for (auto& p : pList)
	{
		if (ConductorCue* c = dynamic_cast<ConductorCue*>(p)) c->addActionListener(this);
	}

	if (!isCurrentlyLoadingData && !isClearing) nextCueIndex->setRange(1, jmax(processorManager.items.size() + 1, 2));
	updateIndices();
}

void Conductor::itemRemoved(Processor* p)
{
	if (ConductorCue* c = dynamic_cast<ConductorCue*>(p))
	{
		if (c == currentCue) currentCue = nullptr;
		c->removeActionListener(this);
	}

	if (!isCurrentlyLoadingData && !isClearing) nextCueIndex->setRange(1, jmax(processorManager.items.size() + 1, 2));
	updateIndices();
}

void Conductor::itemsRemoved(Array<Processor*> items)
{
	for (auto& i : items)
	{
		if (ConductorCue* c = dynamic_cast<ConductorCue*>(i))
		{
			if (c == currentCue) currentCue = nullptr;
			c->removeActionListener(this);
		}
	}

	if (!isCurrentlyLoadingData && !isClearing) nextCueIndex->setRange(1, jmax(processorManager.items.size() + 1, 2));
	updateIndices();
}

void Conductor::itemsReordered()
{
	updateIndices();
}

void Conductor::actionTriggered(Action* a, bool triggerTrue, int multiplexIndex)
{
	if (ConductorCue* c = dynamic_cast<ConductorCue*>(a))
	{
		if (cueTriggerSetCurrent->boolValue()) triggerCue(c, false);
	}
}

void Conductor::triggerCue(ConductorCue* cue, bool triggeredFromConductor)
{
	if (currentCue != nullptr)
	{
		if (currentCue != nullptr) currentCue->setIsCurrent(false);
	}

	currentCue = cue;
	currentCue->setIsCurrent(true);
	currentCueName->setValue(cue->niceName);
	nextCueIndex->setValue(getValidIndexAfter(processorManager.items.indexOf(currentCue) + 1));
	updateNextCue();

	
	if (!triggeredFromConductor)
	{
		if (triggerConductorConsequencesOnDirect->boolValue()) csmOn->triggerAll();
	}
	else
	{
		currentCue->triggerOn->trigger();
	}
}

void Conductor::updateIndices()
{
	if (isCurrentlyLoadingData) return;

	for (int i = 0; i < processorManager.items.size(); i++)
	{
		if (ConductorCue* c = dynamic_cast<ConductorCue*>(processorManager.items[i]))
		{
			c->setIndex(i + 1);
		}
	}

	updateNextCue();
}

void Conductor::triggerConsequences(bool triggerTrue, int multiplexIndex)
{
	if (!enabled->boolValue() || forceDisabled) return;

	if (triggerTrue)
	{
		int itemIndex = nextCueIndex->intValue() - 1;
		if (itemIndex < processorManager.items.size())
		{
			if (Action* a = dynamic_cast<Action*>(processorManager.items[itemIndex]))
			{
				if (ConductorCue* cue = dynamic_cast<ConductorCue*>(a)) triggerCue(cue, true);
			}

			nextCueIndex->setValue(getValidIndexAfter(itemIndex + 1));
		}
	}

	if (triggerConductorConsequencesOnDirect->boolValue()) notifyActionTriggered(triggerTrue, multiplexIndex); //only notify but do not make Action class trigger consequences here
	else Action::triggerConsequences(triggerTrue, multiplexIndex);
}

int Conductor::getValidIndexAfter(int index)
{
	for (int i = index + 1; i <= processorManager.items.size(); i++)
	{
		if (processorManager.items[i - 1]->enabled->boolValue()) return i;
	}

	if (loop->boolValue() && index > 0) return getValidIndexAfter(0);
	return processorManager.items.size();
}

void Conductor::updateNextCue()
{
	int nextIndex = nextCueIndex->intValue();

	String nextName = "";
	for (int i = 0; i < processorManager.items.size(); i++)
	{
		if (ConductorCue* cue = dynamic_cast<ConductorCue*>(processorManager.items[i]))
		{
			bool isNext = (i + 1) == nextIndex;
			cue->setIsNext(isNext);
			if (isNext) nextName = cue->niceName;
		}
	}

	nextCueName->setValue(nextName);
}

void Conductor::afterLoadJSONDataInternal()
{
	Action::afterLoadJSONDataInternal();
	nextCueIndex->setRange(1, jmax(processorManager.items.size() + 1, 2));
	updateIndices();
}

ProcessorUI* Conductor::getUI()
{
	return new ConductorUI(this);
}
