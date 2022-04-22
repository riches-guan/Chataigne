
#pragma once

class SequenceBlockLayerTimeline :
	public SequenceLayerTimeline
{
public:
	SequenceBlockLayerTimeline(SequenceBlockLayer * layer);
	~SequenceBlockLayerTimeline();

	SequenceBlockLayer* sequenceBlockLayer;
	std::unique_ptr<SequenceBlockManagerUI> bmUI;

	void resized() override;
	void updateContent() override;
	virtual void updateMiniModeUI() override;

	virtual void addSelectableComponentsAndInspectables(Array<Component*>& selectables, Array<Inspectable*>& inspectables) override;
};