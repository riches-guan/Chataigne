
#pragma once

class TriggerLayerPanel :
	public SequenceLayerPanel
{
public:
	TriggerLayerPanel(TriggerLayer * layer);
	~TriggerLayerPanel();
	
	TriggerLayer * triggerLayer;
	std::unique_ptr<TriggerButtonUI> lockAllBT;
	std::unique_ptr<TriggerButtonUI> unlockAllBT;

	void resizedInternalContent(Rectangle<int> &r) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TriggerLayerPanel)
};