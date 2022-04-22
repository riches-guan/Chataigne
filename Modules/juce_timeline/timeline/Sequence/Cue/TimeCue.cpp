
TimeCue::TimeCue(const float & _time, TimeCueManager * manager) :
	BaseItem("Cue")
{
	//nameParam->hideInEditor = false;
	helpID = "TimeCue";

	time = addFloatParameter(L"时间", L"cue时间", 0, 0);//Time  //Cue Time
	time->setValue(_time, true, true);
	time->defaultUI = FloatParameter::TIME;

	cueAction = addEnumParameter(L"在cue点上激活", L"这可用于在时间线到达提示时添加额外动作。暂停：将在提示处暂停，跳转：将跳转到另一个提示，允许轻松的循环输入输出行为。");
	//Action on Cue
	//This can be used to add an extra action when the timeline hits the cue. Pause will pause at the cue, Loop Jump will jump to another cue, allowing for easy loop in-out behaviour.
	cueAction->addOption(L"什么也不做", NOTHING)->addOption(L"暂停", PAUSE)->addOption(L"循环跳转", LOOP_JUMP);//Nothing  //Pause  //Loop Jump

	loopCue = addTargetParameter(L"循环跳转到cue", L"如果Cue动作设置为循环跳转，这将决定跳转到哪个提示。", manager, false);
	//Loop Jump Cue
	//If Cue Action is set to Loop Jump, this will decide which Cue to jump to
	loopCue->targetType = TargetParameter::CONTAINER;
	loopCue->maxDefaultSearchLevel = 0;
	loopCue->showParentNameInEditor = false;

	isUILocked->hideInEditor = false;

}

TimeCue::~TimeCue()
{
}

void TimeCue::onContainerParameterChangedInternal(Parameter* p)
{
	if (p == cueAction)
	{
		loopCue->setEnabled(cueAction->getValueDataAsEnum<CueAction>() == LOOP_JUMP);
	}
	else if (p == loopCue)
	{
		if (loopCue->targetContainer == this)
		{
			NLOGWARNING(niceName, L"自动引用这个循环跳跃的提示，你想创造时空突破吗？？");//Auto referencing this cue for Loop Jump, are you looking to create a spacetime breach ??
		}
	}
}

Sequence * TimeCue::getSequence()
{
	if (parentContainer == nullptr) return nullptr;
	return dynamic_cast<Sequence*>(parentContainer->parentContainer.get());
}
