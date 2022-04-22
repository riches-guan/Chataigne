
#pragma once

class SequenceTransportUI :
	public Component,
	public Sequence::AsyncListener
{
public:
	SequenceTransportUI(Sequence * _sequence);
	virtual ~SequenceTransportUI();

	Sequence * sequence;
	WeakReference<Inspectable> sequenceRef;

	TimeLabel timeLabel;
	TimeLabel timeStepLabel;
	TimeLabel totalTimeLabel;

	std::unique_ptr<TriggerImageUI> togglePlayUI;
	std::unique_ptr<TriggerImageUI> stopUI;
	std::unique_ptr<TriggerImageUI> prevCueUI;
	std::unique_ptr<TriggerImageUI> nextCueUI;
	std::unique_ptr<BoolToggleUI> loopUI;
	std::unique_ptr<BoolToggleUI> snapUI;
	
	void resized() override;

	void newMessage(const Sequence::SequenceEvent & e) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SequenceTransportUI)
};