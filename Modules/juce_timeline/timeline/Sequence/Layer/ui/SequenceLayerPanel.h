
#pragma once

class SequenceLayerPanel :
	public BaseItemUI<SequenceLayer>
{
public:
	SequenceLayerPanel(SequenceLayer *);
	virtual ~SequenceLayerPanel();

	std::unique_ptr<BoolToggleUI> lockUI;
	std::unique_ptr<BoolToggleUI> miniModeUI;

	std::unique_ptr<ColorParameterUI> colorUI;
	void paintOverChildren(Graphics &g) override;
	void resized() override;
	void resizedInternalHeader(Rectangle<int> &r) override;
	
	void controllableFeedbackUpdateInternal(Controllable *) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SequenceLayerPanel)
};