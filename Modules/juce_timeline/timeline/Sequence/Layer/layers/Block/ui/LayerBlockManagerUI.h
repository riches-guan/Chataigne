
#pragma once

class LayerTimeline;

class LayerBlockManagerUI :
	public BaseManagerUI<LayerBlockManager, LayerBlock, LayerBlockUI>,
	public LayerBlockUI::BlockUIListener
{
public:
	LayerBlockManagerUI(SequenceLayerTimeline * timeline, LayerBlockManager * manager);
	~LayerBlockManagerUI();

	SequenceLayerTimeline * timeline;

	bool miniMode;
	Array<float> snapTimes;

	virtual void resized() override;
	virtual void updateContent();

	virtual void setMiniMode(bool value);

	virtual LayerBlockUI * createUIForItem(LayerBlock * block) override; 

	virtual void placeBlockUI(LayerBlockUI * cui);
	virtual void mouseDoubleClick(const MouseEvent &e) override;

	virtual void addItemFromMenu(bool, Point<int> mouseDownPos) override;
	virtual void addItemUIInternal(LayerBlockUI * cui) override;
	virtual void removeItemUIInternal(LayerBlockUI * cui) override;

	virtual void blockUITimeChanged(LayerBlockUI * cui) override;
	virtual void blockUIMouseDown(LayerBlockUI* cui, const MouseEvent&) override;
	virtual void blockUIDragged(LayerBlockUI * cui, const MouseEvent &) override;
	virtual void blockUIStartDragged(LayerBlockUI * cui, const MouseEvent &) override;
	virtual void blockUICoreDragged(LayerBlockUI * cui, const MouseEvent &) override;
	virtual void blockUILoopDragged(LayerBlockUI * cui, const MouseEvent &) override;
	virtual void blockUINeedsReorder() override;

	virtual void updateItemVisibility(LayerBlockUI* cui) override;

};

