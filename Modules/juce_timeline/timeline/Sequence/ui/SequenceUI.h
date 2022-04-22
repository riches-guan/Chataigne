
#pragma once

class SequenceUI :
	public BaseItemUI<Sequence>,
	public Sequence::AsyncListener
{
public:
	SequenceUI(Sequence * output);
	virtual ~SequenceUI();

	std::unique_ptr<FloatSliderUI> timeUI;
	std::unique_ptr<TriggerImageUI> togglePlayUI;
	std::unique_ptr<TriggerImageUI> stopUI;
	std::unique_ptr<ColorParameterUI> colorUI;

	void resizedInternalHeader(Rectangle<int> &r) override;
	void resizedInternalContent(Rectangle<int> &r) override;

	void controllableFeedbackUpdateInternal(Controllable *) override;

	void newMessage(const Sequence::SequenceEvent &e) override;

	JUCE_DECLARE_NON_COPYABLE(SequenceUI)
};