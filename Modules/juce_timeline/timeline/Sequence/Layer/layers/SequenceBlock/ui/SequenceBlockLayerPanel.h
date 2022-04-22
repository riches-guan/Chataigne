
#pragma once

class SequenceBlockLayerPanel :
	public SequenceLayerPanel
{
public:
	SequenceBlockLayerPanel(SequenceBlockLayer * layer);
	~SequenceBlockLayerPanel();

	SequenceBlockLayer * sequenceBlockLayer;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SequenceBlockLayerPanel)
};