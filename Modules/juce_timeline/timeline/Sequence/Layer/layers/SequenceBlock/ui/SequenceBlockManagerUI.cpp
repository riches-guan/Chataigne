
SequenceBlockManagerUI::SequenceBlockManagerUI(SequenceBlockLayerTimeline * _timeline, SequenceBlockManager * manager) :
	LayerBlockManagerUI(_timeline, manager)
{
	addExistingItems();
}

SequenceBlockManagerUI::~SequenceBlockManagerUI()
{
}


LayerBlockUI* SequenceBlockManagerUI::createUIForItem(LayerBlock* item)
{
	SequenceBlockUI * ui =  new SequenceBlockUI(dynamic_cast<SequenceBlock *>(item));
	return ui;
}