
#pragma once

class SequenceBlockUI :
	public LayerBlockUI
{
public:
	SequenceBlockUI(SequenceBlock * block);
	~SequenceBlockUI();

	SequenceBlock* block;

	void paint(Graphics &g) override;

	virtual void controllableFeedbackUpdateInternal(Controllable *) override;
};