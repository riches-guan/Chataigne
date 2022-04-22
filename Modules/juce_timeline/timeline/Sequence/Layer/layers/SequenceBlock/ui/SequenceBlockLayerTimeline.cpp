
SequenceBlockLayerTimeline::SequenceBlockLayerTimeline(SequenceBlockLayer * layer) :
	SequenceLayerTimeline(layer),
	sequenceBlockLayer(layer)
{
	bmUI.reset(new SequenceBlockManagerUI(this, &layer->blockManager));
	addAndMakeVisible(bmUI.get());

	updateMiniModeUI();

	needle.toFront(false);
	updateContent();
}

SequenceBlockLayerTimeline::~SequenceBlockLayerTimeline()
{

}

void SequenceBlockLayerTimeline::resized()
{
	bmUI->setBounds(getLocalBounds());
}

void SequenceBlockLayerTimeline::updateContent()
{
	bmUI->updateContent();
}

void SequenceBlockLayerTimeline::updateMiniModeUI()
{
	bmUI->setMiniMode(item->miniMode->boolValue());
}

void SequenceBlockLayerTimeline::addSelectableComponentsAndInspectables(Array<Component*>& selectables, Array<Inspectable*>& inspectables)
{
	if (bmUI == nullptr) return;
	bmUI->addSelectableComponentsAndInspectables(selectables, inspectables);
}
