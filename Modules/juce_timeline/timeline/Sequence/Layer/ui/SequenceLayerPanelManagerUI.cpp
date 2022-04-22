
SequenceLayerPanelManagerUI::SequenceLayerPanelManagerUI(SequenceLayerManager * _manager) :
	BaseManagerUI<SequenceLayerManager, SequenceLayer, SequenceLayerPanel>("SequenceLayers", _manager)
{
	viewport.setScrollBarsShown(false, false, true, false);
	addExistingItems();
}

SequenceLayerPanelManagerUI::~SequenceLayerPanelManagerUI()
{
}



SequenceLayerPanel * SequenceLayerPanelManagerUI::createUIForItem(SequenceLayer * layer)
{
	return layer->getPanel();
}

void SequenceLayerPanelManagerUI::addItemFromMenu(SequenceLayer* item, bool fromAddButton, Point<int> pos)
{
    BaseManagerUI::addItemFromMenu(item, fromAddButton, pos);
    item->addDefaultContent();
}
