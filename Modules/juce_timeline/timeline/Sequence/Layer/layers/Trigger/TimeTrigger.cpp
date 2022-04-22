
TimeTrigger::TimeTrigger(StringRef name) :
	BaseItem(name)
{
	itemDataType = "TimeTrigger"; 

	showWarningInUI = true;

	time = addFloatParameter(L"时间", "触发动作的时间。", 0, 0);//Time  //Time at which the action is triggered
	
	time->defaultUI = FloatParameter::TIME;
	flagY = addFloatParameter(L"Y标识", L"触发器标志的位置。", 0,0,1);//Flag  //Position of the trigger's flag
	isTriggered = addBoolParameter(L"是否触发", L"这个时间触发器在播放过程中已经触发了吗？", false);//Is Triggered  //Is this Time Trigger already triggered during this playing ?
	
	color = addColorParameter(L"颜色", L"触发器的颜色", BG_COLOR.brighter(.2f));//Color  //Color of the trigger

	isTriggered->hideInEditor = true;
	isTriggered->isSavable = false;
}

TimeTrigger::~TimeTrigger()
{

}


void TimeTrigger::setMovePositionReferenceInternal()
{
	movePositionReference = Point<float>(time->floatValue(), flagY->floatValue());
}

void TimeTrigger::setPosition(Point<float> targetPosition)
{
	time->setValue(targetPosition.x);
	flagY->setValue(targetPosition.y);
}

Point<float> TimeTrigger::getPosition()
{
	return Point<float>(time->floatValue(), flagY->floatValue());
}

void TimeTrigger::addUndoableMoveAction(Array<UndoableAction*>& actions)
{
	actions.add(time->setUndoableValue(movePositionReference.x, time->floatValue(), true));
	actions.add(flagY->setUndoableValue(movePositionReference.y, flagY->floatValue(), true));

}

void TimeTrigger::trigger()
{
	if (!enabled->boolValue()) return;
	isTriggered->setValue(true);
	triggerInternal();
}