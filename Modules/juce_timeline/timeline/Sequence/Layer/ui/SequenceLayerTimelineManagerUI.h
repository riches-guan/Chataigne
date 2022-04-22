
#pragma once

class SequenceLayerTimelineManagerUI :
	public BaseManagerUI<SequenceLayerManager, SequenceLayer, SequenceLayerTimeline>,
	public FileDragAndDropTarget
{
public:

	SequenceLayerTimelineManagerUI(SequenceLayerManager *_manager);
	~SequenceLayerTimelineManagerUI();

	SequenceLayerTimeline * createUIForItem(SequenceLayer * layer) override;

	void resized() override;

	void addSelectableComponentsAndInspectables(Array<Component*>& selectables, Array<Inspectable*>& inspectables) override;

	virtual bool isInterestedInFileDrag(const StringArray& files) override;
	virtual void filesDropped(const StringArray& files, int x, int y) override;
};