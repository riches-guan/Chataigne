
AutomationMappingLayerTimeline::AutomationMappingLayerTimeline(AutomationMappingLayer* layer) :
	MappingLayerTimeline(layer, new AutomationUI(layer->automation)),
	aml(layer)
{
	automationUI = ((AutomationUI*)mainComponent.get());
	needle.timeBarColor = aml->recorder.arm->boolValue() ? Colours::red : needle.defaultTimeBarColor;

	std::function<void(Array<float>*, AutomationKey*)> func = std::bind(&AutomationMappingLayer::getSequenceSnapTimesForAutomation, layer, std::placeholders::_1, std::placeholders::_2);;
	automationUI->getSnapTimesFunc = func;
	autoDrawContourWhenSelected = false;
	//autoSelectWithChildRespect = false;

	updateMiniModeUI();
}

AutomationMappingLayerTimeline::~AutomationMappingLayerTimeline()
{
}

void AutomationMappingLayerTimeline::updateContent()
{
	MappingLayerTimeline::updateContent();
	automationUI->setViewRange(item->sequence->viewStartTime->floatValue(), item->sequence->viewEndTime->floatValue());
}

void AutomationMappingLayerTimeline::setSeekManipulationMode(bool isManipulating)
{
	MappingLayerTimeline::setSeekManipulationMode(isManipulating);
	automationUI->setPreviewMode(isManipulating || aml->miniMode->boolValue());
}

void AutomationMappingLayerTimeline::updateMiniModeUI()
{
	MappingLayerTimeline::updateMiniModeUI();
	automationUI->setPreviewMode(aml->miniMode->boolValue());
}

void AutomationMappingLayerTimeline::controllableFeedbackUpdateInternal(Controllable* c)
{
	MappingLayerTimeline::controllableFeedbackUpdateInternal(c);

	if (c == aml->recorder.arm)
	{
		needle.timeBarColor = aml->recorder.arm->boolValue() ? RED_COLOR : needle.defaultTimeBarColor;
		repaint();
	}
}

void AutomationMappingLayerTimeline::inspectableSelectionChanged(Inspectable* i)
{
	// automationUI->setViewMode(mappingLayer->isSelected ? AutomationUI::EDIT : AutomationUI::VIEW);
}

void AutomationMappingLayerTimeline::addSelectableComponentsAndInspectables(Array<Component*>& selectables, Array<Inspectable*>& inspectables)
{
	if (automationUI == nullptr) return;
	automationUI->addSelectableComponentsAndInspectables(selectables, inspectables);
}
