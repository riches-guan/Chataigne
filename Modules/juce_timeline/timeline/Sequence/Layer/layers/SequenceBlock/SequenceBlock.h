
#pragma once

class SequenceManager;

class SequenceBlock :
	public LayerBlock
{
public:
	SequenceBlock(SequenceManager * manager);
	virtual ~SequenceBlock();

	TargetParameter * target;

	Sequence* targetSequence;
	WeakReference<Inspectable> sequenceRef;

	void setTargetSequence(Sequence* sequence);
	Sequence* getTargetSequence();


	void onContainerParameterChangedInternal(Parameter *) override;
	String getTypeString() const override { return "SequenceBlock"; }
};


