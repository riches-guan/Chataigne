
TriggerLayerPanel::TriggerLayerPanel(TriggerLayer * layer) :
	SequenceLayerPanel(layer),
	triggerLayer(layer)
{
	lockAllBT.reset(layer->lockAll->createButtonUI());
	unlockAllBT.reset(layer->unlockAll->createButtonUI());
	addAndMakeVisible(lockAllBT.get());
	addAndMakeVisible(unlockAllBT.get());

	contentComponents.add(lockAllBT.get());
	contentComponents.add(unlockAllBT.get());


}

TriggerLayerPanel::~TriggerLayerPanel()
{
}

void TriggerLayerPanel::resizedInternalContent(Rectangle<int>& r)
{
	SequenceLayerPanel::resizedInternalContent(r);

	Rectangle<int> btr = r.removeFromTop(12);

	lockAllBT->setBounds(btr.removeFromLeft(60));
	btr.removeFromLeft(10);
	unlockAllBT->setBounds(btr.removeFromLeft(60));
}
