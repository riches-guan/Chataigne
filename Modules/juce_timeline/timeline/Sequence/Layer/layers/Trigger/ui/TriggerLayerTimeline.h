
#pragma once

class TriggerLayerTimeline :
	public SequenceLayerTimeline
{
public:
	TriggerLayerTimeline(TriggerLayer * layer);
	~TriggerLayerTimeline();

	std::unique_ptr<TimeTriggerManagerUI> ttmui;

	void resized() override;
	virtual void updateContent() override;

	virtual void updateMiniModeUI() override;
	
	virtual void addSelectableComponentsAndInspectables(Array<Component*>& selectables, Array<Inspectable*>& inspectables) override;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TriggerLayerTimeline)

};