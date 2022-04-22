
#pragma once

class SequenceBlockLayerTimeline;

class SequenceBlockManagerUI :
	public LayerBlockManagerUI
{
public:
	SequenceBlockManagerUI(SequenceBlockLayerTimeline * timeline, SequenceBlockManager * manager);
	~SequenceBlockManagerUI();

	virtual LayerBlockUI* createUIForItem(LayerBlock* item) override;
};