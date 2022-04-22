
SequenceBlock::SequenceBlock(SequenceManager * manager) :
	LayerBlock(getTypeString()),
	targetSequence(nullptr)
{
	jassert(manager != nullptr);

	target = addTargetParameter(L"序列", L"要控制的目标序列", manager);//Sequence  //The target Sequence to control
	target->targetType = TargetParameter::CONTAINER;
	target->maxDefaultSearchLevel = 0;
}

SequenceBlock::~SequenceBlock()
{
	setTargetSequence(nullptr);

}

void SequenceBlock::setTargetSequence(Sequence* sequence)
{
	if (sequence == targetSequence) return;


	targetSequence = sequence;
	sequenceRef = sequence;

	if (targetSequence != nullptr)
	{
		if (!isCurrentlyLoadingData && !coreLength->isOverriden) setCoreLength(targetSequence->totalTime->floatValue(), false);
	}
}

Sequence* SequenceBlock::getTargetSequence()
{
	return !sequenceRef.wasObjectDeleted() ? targetSequence : nullptr;
}

void SequenceBlock::onContainerParameterChangedInternal(Parameter* p)
{
	if (p == target) setTargetSequence(((Sequence*)target->targetContainer.get()));
}
