
LayerBlock::LayerBlock(StringRef name) :
	BaseItem(name, true, false)
{
	editorIsCollapsed = true;

	time = addFloatParameter(L"开始时间", L"剪辑开始的时间。", 0, 0);//Start Time  //Time of the start of the clip
	time->defaultUI = FloatParameter::TIME;

	coreLength = addFloatParameter(L"长度", L"剪辑核心的长度，无循环（秒）。", 10, .1f);//Length  //Length of the clip's core, without looping (in seconds)
	coreLength->defaultUI = FloatParameter::TIME;
	loopLength = addFloatParameter(L"循环长度", L"核心后的剪辑循环长度", 0, 0);//Loop Length  //Length of the clip's oop, after the core
	loopLength->defaultUI = FloatParameter::TIME;

	isActive = addBoolParameter(L"是否活动", L"这是一个反馈，以了解块当前是否在时间轴中处于活动状态。", false);//Is Active  //This is a feedback to know if block is currently active in the timeline
	isActive->setControllableFeedbackOnly(true);

	isUILocked->hideInEditor = false;

}
LayerBlock::~LayerBlock()
{
}

float LayerBlock::getTotalLength()
{
	return coreLength->floatValue() + loopLength->floatValue();
}

float LayerBlock::getCoreEndTime()
{
	return time->floatValue() + coreLength->floatValue();
}

float LayerBlock::getEndTime()
{
	return time->floatValue() + getTotalLength();
}

bool LayerBlock::isInRange(float _time)
{
	return _time >= time->floatValue() && _time < getEndTime();
}

void LayerBlock::setMovePositionReferenceInternal()
{
	movePositionReference.setX(time->floatValue());
}

void LayerBlock::setPosition(Point<float> targetPosition)
{
	blockListeners.call(&BlockListener::askForPlaceBlockTime, this, targetPosition.x);
}

Point<float> LayerBlock::getPosition()
{
	return Point<float>(time->floatValue(),0);
}

void LayerBlock::addUndoableMoveAction(Array<UndoableAction *> &actions)
{
	actions.add(time->setUndoableValue(movePositionReference.x, time->floatValue(), true));
}


void LayerBlock::setCoreLength(float newLength, bool /*stretch*/, bool /*stickToCoreEnd*/)
{
	coreLength->setValue(newLength);
}

void LayerBlock::setLoopLength(float newLength)
{
	loopLength->setValue(newLength);
}

void LayerBlock::setStartTime(float newStart, bool keepCoreEnd, bool stickToCoreEnd)
{
	float timeDiff = newStart - time->floatValue();
	time->setValue(newStart);
	if (keepCoreEnd) setCoreLength(coreLength->floatValue() - timeDiff, false, stickToCoreEnd);
}

double LayerBlock::getRelativeTime(double t, bool timeIsAbsolute, bool noLoop)
{
	if (timeIsAbsolute) t -= time->floatValue();
	if (t == coreLength->floatValue() || noLoop) return t; //avoid getting the start value when asking for the end value
	return fmod(t, coreLength->floatValue());
}
