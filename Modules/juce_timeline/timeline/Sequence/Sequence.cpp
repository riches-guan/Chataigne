
Sequence::Sequence() :
	BaseItem(L"序列", true),//Sequence
	Thread("Sequence"),
	currentManager(nullptr),
	hiResAudioTime(0),
	sampleRate(44100),
	isSeeking(false),
	//timeAtSetTime(0),
	millisAtSetTime(0),
	prevMillis(0),
    targetTime(0),
    isBeingEdited(false),
	sequenceNotifier(10)
{
	itemDataType = "Sequence";
	helpID = "Sequence";

	isPlaying = addBoolParameter(L"是否播放", L"正在播放这个序列吗？", false);//Is Playing  //Is the sequence playing ?
	isPlaying->setControllableFeedbackOnly(true);
	//isPlaying->isSavable = false;
	//isPlaying->hideInEditor = true;

	playTrigger = addTrigger(L"播放", L"播放序列。");//Play  //Play the sequence
	stopTrigger = addTrigger(L"停止", L"停止序列并将当前时间设置为0。");//Stop  //Stops the sequence and set the current time at 0.
	finishTrigger = addTrigger(L"完成", L"当序列自然到达终点时，不再循环。");//Finish  //When the sequence reached naturally its end, and there is no loop
	finishTrigger->hideInEditor = true;
	pauseTrigger = addTrigger(L"暂停", L"暂时停止播放序列并保持当前时间不变。");//Pause  //Pause the sequence and keep the current time as is.
	togglePlayTrigger = addTrigger(L"切换播放", L"根据序列设置在播放/暂停或播放/停止之间切换。");//TogglePlay  //Toggle between play/pause or play/stop depending on sequence settings

	float initTotalTime = 60; //default to 60 seconds, may be in general preferences later

	startAtLoad = addBoolParameter(L"在加载时播放", L"如果选中，序列将在加载文件后立即开始播放。", false);//Play at Load  //If selected, the sequence will start playing just after loading the file

	includeCurrentTimeInSave = addBoolParameter(L"保存当前时间", L"如果选中，这将保存并加载文件中的当前时间；否则，文件加载时，当前时间将重置为0。", false);
	//Save Current Time
	//If checked, this will save and load the current time in the file. Otherwise the current time will be reset to 0 on file load.

	currentTime = addFloatParameter(L"当前时间", L"此序列的当前时间位置。", 0, 0, initTotalTime);//Current Time  //Current position in time of this sequence
	currentTime->defaultUI = FloatParameter::TIME;
	currentTime->isSavable = false;


	totalTime = addFloatParameter(L"累计时间", L"此序列的总时间，以秒为单位。", initTotalTime, minSequenceTime);//Total Time  //Total time of this sequence, in seconds
	totalTime->defaultUI = FloatParameter::TIME;

	playSpeed = addFloatParameter(L"播放速度", L"播放速度系数，1为正常速度，2为双速，0.5为半速。", 1);
	//Play Speed
	//Playing speed factor, 1 is normal speed, 2 is double speed and 0.5 is half speed
	prevSpeed = playSpeed->floatValue();

	fps = addIntParameter("FPS", L"每秒帧数。定义每秒计算序列的次数，该值越高，计算的优先级越高。此设置还设置每秒从自动层发送的消息数。", 50, 1, 500);
	//Frame Per Second.Defines the number of times per seconds the sequence is evaluated, the higher the value is, the more previse the calculation will be.This setting also sets how many messages per seconds are sent from layer with automations.
	loopParam = addBoolParameter(L"循环", L"播放时序列是否从开始到结束再次播放。", false);
	//Loop
	//Whether the sequence plays again from the start when reached the end while playing

	bpmPreview = addFloatParameter(L"BPM预览", L"在时间线中预览BPM。", 120, 1, 999, false);//Preview  //BPM for previewing in the timeline.
	bpmPreview->canBeDisabledByUser = true;
	beatsPerBar = addIntParameter(L"每小节拍数", L"每小节的节拍数，仅用于在UI中预览。", 4, 1, 32, false);
	//Beats Per Bar
	//Number of beats in a bar. Only for preview in UI

	autoSnap = addBoolParameter(L"自动捕捉", L"如果选中，移动项目时将自动捕捉", false);
	//Auto Snap
	//If checked, this will automatically snap when moving items

	currentTime->unitSteps = fps->intValue();
	totalTime->unitSteps = fps->intValue();

	prevCue = addTrigger(L"前一个CUE点", L"跳转到前一个CUE点，如果前一个CUE点距离前一秒不到，跳转到前一个CUE点。");
	//Prev Cue
	//Jump to previous cue, if previous cue is less than 1 sec before, jump to the one before that.
	nextCue = addTrigger(L"下一个CUE点", L"跳转到下一个CUE点。");
	//Next Cue
	//Jump to the next cue

	viewStartTime = addFloatParameter(L"视图开始时间", L"视图的开始时间。", 0, 0, initTotalTime - minSequenceTime);//View start time  //Start time of the view
	viewEndTime = addFloatParameter(L"视图结束时间", L"视图的结束时间", initTotalTime, minSequenceTime, initTotalTime);//View end time //End time of the view
	viewFollowTime = addBoolParameter(L"视图跟踪时间", L"如果选中，这将自动跟随当前时间，因此光标位于时间线的中心。", false);
	//View follow time
	//If checked, this will automatically follow the current time so the cursor is at the center of the timeline.

	color = addColorParameter(L"颜色", L"UI中序列的颜色", BG_COLOR.brighter(.1f));//Color  //The color of the sequence in the UI

	layerManager.reset(new SequenceLayerManager(this));
	addChildControllableContainer(layerManager.get());

	cueManager.reset(new TimeCueManager());
	addChildControllableContainer(cueManager.get());
	cueManager->hideInEditor = true;

	listUISize->setValue(30);
	listUISize->isSavable = false;
}

Sequence::~Sequence()
{
	clearItem();
}

void Sequence::clearItem()
{
	BaseItem::clearItem();

	setAudioDeviceManager(nullptr);

	stopThread(500);

	//if(!Engine::mainEngine->isClearing) stopTrigger->trigger();
	if (Engine::mainEngine != nullptr) Engine::mainEngine->removeEngineListener(this);
}

void Sequence::setCurrentTime(float time, bool forceOverPlaying, bool seekMode)
{
	time = jlimit<float>(0, totalTime->floatValue(), time);

	if (isPlaying->boolValue() && !forceOverPlaying) return;

	isSeeking = seekMode;

	millisAtSetTime = Time::getMillisecondCounterHiRes();
	//timeAtSetTime = time;
	if (seekMode || forceOverPlaying) targetTime = time;

	if (timeIsDrivenByAudio())
	{
		hiResAudioTime = time;
		if (!isPlaying->boolValue() || isSeeking) currentTime->setValue(time);
	}
	else
	{
		currentTime->setValue(time);
	}

	isSeeking = false;
}

int Sequence::getFrameForTime(float time, bool forceDirection, bool forcePrev)
{
	float f = time * fps->floatValue();
	return forceDirection ? (forcePrev ? floorf(f) : ceilf(f)) : round(f);
}

double Sequence::getTimeForFrame(float frame)
{
	return frame * 1.0 / fps->floatValue();
}

double Sequence::getNextFrameTimeForTime(float time)
{
	return getTimeForFrame(getFrameForTime(time, true, false));
}

double Sequence::getPrevFrameTimeForTime(float time)
{
	return getTimeForFrame(getFrameForTime(time, true, true));
}

void Sequence::setBeingEdited(bool value)
{
	if (isBeingEdited == value) return;
	isBeingEdited = value;
	sequenceListeners.call(&SequenceListener::sequenceEditingStateChanged, this);
	sequenceNotifier.addMessage(new SequenceEvent(SequenceEvent::EDITING_STATE_CHANGED, this));
}

void Sequence::selectAllItemsBetween(float start, float end)
{
	Array<Inspectable*> selection;
	for (auto& l : layerManager->items) selection.addArray(l->selectAllItemsBetween(start, end));
	InspectableSelectionManager::mainSelectionManager->selectInspectables(selection);
}

void Sequence::removeAllItemsBetween(float start, float end)
{
	Array<UndoableAction*> actions;
	for (auto& l : layerManager->items)  actions.addArray(l->getRemoveAllItemsBetween(start, end));
	UndoMaster::getInstance()->performActions("Remove items between timespan", actions);
}

void Sequence::removeTimespan(float start, float end)
{
	Array<UndoableAction*> actions;
	for (auto& l : layerManager->items)  actions.addArray(l->getRemoveTimespan(start, end));
	actions.addArray(cueManager->getRemoveTimespan(start, end));
	actions.add(totalTime->setUndoableValue(totalTime->floatValue(), totalTime->floatValue() - (end - start), true));
	UndoMaster::getInstance()->performActions("Remove timespan", actions);
}

void Sequence::insertTimespan(float start, float length)
{
	Array<UndoableAction*> actions;
	actions.add(totalTime->setUndoableValue(totalTime->floatValue(), totalTime->floatValue() + length, true));
	for (auto& l : layerManager->items)  actions.addArray(l->getInsertTimespan(start, length));
	actions.addArray(cueManager->getInsertTimespan(start, length));
	UndoMaster::getInstance()->performActions("Insert timespan", actions);
}

void Sequence::getSnapTimes(Array<float>* arrayToFill, float start, float end, const Array<float>& excludeValues)
{
	if (end == -1) end = totalTime->floatValue();
	
	for (auto& i : layerManager->items) i->getSnapTimes(arrayToFill);
	
	cueManager->getSnapTimes(arrayToFill);
	arrayToFill->addIfNotAlreadyThere(currentTime->floatValue());

	if (bpmPreview->enabled)
	{
		float step = 60.0 / bpmPreview->floatValue();
		for (float i = start; i < end; i += step) arrayToFill->addIfNotAlreadyThere(i);
	}

	arrayToFill->removeValuesIn(excludeValues);
	if(start > 0 || end < totalTime->floatValue()) arrayToFill->removeIf([start, end](float v) {return v >= start && v <= end; });
}

float Sequence::getClosestSnapTimeFor(Array<float> snapTimes, float time)
{
	float result = time;
	float diff = INT32_MAX;
	for (auto& t : snapTimes)
	{
		float d = fabsf(time - t);
		if (d < diff)
		{
			diff = d;
			result = t;
		}
	}
	return result;
}

bool Sequence::paste()
{
	Array<SequenceLayer*> p = layerManager->addItemsFromClipboard(false);
	if (p.isEmpty()) return BaseItem::paste();
	return true;
}

void Sequence::setAudioDeviceManager(AudioDeviceManager* manager)
{
	if (currentManager == manager) return;

	if (currentManager != nullptr) currentManager->removeAudioCallback(this);

	currentManager = manager;

	if (currentManager != nullptr) currentManager->addAudioCallback(this);


	//resync values between audio/non-audio driving variables
	hiResAudioTime = (double)currentTime->floatValue();

	sequenceListeners.call(&SequenceListener::sequenceMasterAudioModuleChanged, this);
}

void Sequence::updateSampleRate()
{
	AudioDeviceManager::AudioDeviceSetup s;
	if (currentManager != nullptr) currentManager->getAudioDeviceSetup(s);
	if (s.sampleRate != 0) sampleRate = s.sampleRate;
}

bool Sequence::timeIsDrivenByAudio()
{
	return currentManager != nullptr;
}

var Sequence::getJSONData()
{
	var data = BaseItem::getJSONData();
	var layerData = layerManager->getJSONData();
	if (!layerData.isVoid()) data.getDynamicObject()->setProperty(layerManager->shortName, layerData);
	var cueData = cueManager->getJSONData();
	if (!cueData.isVoid()) data.getDynamicObject()->setProperty(cueManager->shortName, cueData);
	if (isBeingEdited) data.getDynamicObject()->setProperty("editing", true);
	return data;
}

void Sequence::loadJSONDataInternal(var data)
{
	BaseItem::loadJSONDataInternal(data);
	layerManager->loadJSONData(data.getProperty(layerManager->shortName, var()));
	cueManager->loadJSONData(data.getProperty(cueManager->shortName, var()));
	isBeingEdited = data.getProperty("editing", false);

	if (Engine::mainEngine->isLoadingFile)
	{
		Engine::mainEngine->addEngineListener(this);
	}

}

void Sequence::onContainerParameterChangedInternal(Parameter* p)
{
	if (p == enabled)
	{

	}
	else if (p == currentTime)
	{
		if (isPlaying->boolValue() && !isSeeking)
		{
			float minTime = jmin<float>(prevTime, currentTime->floatValue());
			float maxTime = jmax<float>(prevTime, currentTime->floatValue());
			bool playingForward = playSpeed->floatValue() > 0;
			Array<TimeCue*> cues = cueManager->getCuesInTimespan(minTime, maxTime, !playingForward, playingForward);
			for (auto& cue : cues)
			{
				if (!cue->enabled->boolValue()) continue;

				TimeCue::CueAction a = cue->cueAction->getValueDataAsEnum<TimeCue::CueAction>();
				if (a == TimeCue::NOTHING) continue;
				else if (a == TimeCue::PAUSE)
				{
					pauseTrigger->trigger();
					prevTime = currentTime->floatValue();
					currentTime->setValue(cue->time->floatValue());
					return;
				}
				else if (a == TimeCue::LOOP_JUMP)
				{
					if (TimeCue* tc = dynamic_cast<TimeCue*>(cue->loopCue->targetContainer.get()))
					{
						if (tc != cue) setCurrentTime(tc->time->floatValue(), true, true);
					}
				}
			}
		}

		if ((!isPlaying->boolValue() || isSeeking) && timeIsDrivenByAudio()) hiResAudioTime = currentTime->floatValue();
		else if (getCurrentThreadId() != getThreadId())
		{
			millisAtSetTime = Time::getMillisecondCounterHiRes();
			//timeAtSetTime = timeIsDrivenByAudio() ? hiResAudioTime : currentTime->floatValue();
		}

		sequenceListeners.call(&SequenceListener::sequenceCurrentTimeChanged, this, (float)prevTime, isPlaying->boolValue() && !isSeeking);
		prevTime = currentTime->floatValue();
	}
	else if (p == totalTime)
	{
		float minViewTime = jmax(minSequenceTime, totalTime->floatValue() / 100.f); //small hack to avoid UI hang when zooming too much

		currentTime->setRange(0, totalTime->floatValue());
		viewStartTime->setRange(0, totalTime->floatValue() - minViewTime);
		viewEndTime->setRange(viewStartTime->floatValue() + minViewTime, totalTime->floatValue());
		sequenceListeners.call(&SequenceListener::sequenceTotalTimeChanged, this);
	}
	else if (p == isPlaying)
	{
		signalThreadShouldExit();
		notify();
		if (getCurrentThreadId() != getThreadId())
		{
			waitForThreadToExit(300);
		}

		if (isPlaying->boolValue())
		{
			if (currentTime->floatValue() >= totalTime->floatValue()) currentTime->setValue(0); //if reached the end when hit play, go to 0

			prevTime = currentTime->floatValue();
			if (!isThreadRunning()) startThread();
		}

		sequenceListeners.call(&SequenceListener::sequencePlayStateChanged, this);
		sequenceNotifier.addMessage(new SequenceEvent(SequenceEvent::PLAY_STATE_CHANGED, this));
	}
	else if (p == playSpeed)
	{
		if ((prevSpeed < 0 && playSpeed->floatValue() > 0) || (prevSpeed > 0 && playSpeed->floatValue() < 0))
		{
			sequenceListeners.call(&SequenceListener::sequencePlayDirectionChanged, this);
		}

		prevSpeed = playSpeed->floatValue();
		sequenceListeners.call(&SequenceListener::sequencePlaySpeedChanged, this);
	}
	else if (p == viewStartTime)
	{
		float minViewTime = jmax(minSequenceTime, totalTime->floatValue() / 100.f); //small hack to avoid UI hang when zooming too much
		viewEndTime->setRange(viewStartTime->floatValue() + minViewTime, totalTime->floatValue()); //Should be a range value
	}
	else if (p == fps)
	{
		currentTime->unitSteps = fps->intValue();
		totalTime->unitSteps = fps->intValue();
		totalTime->setValue(totalTime->floatValue()); //force update
		currentTime->setValue(currentTime->floatValue()); //force update
	}
	else if (p == includeCurrentTimeInSave)
	{
		currentTime->isSavable = includeCurrentTimeInSave->boolValue();
	}
}

void Sequence::onContainerTriggerTriggered(Trigger* t)
{
	if (t == playTrigger)
	{
		isPlaying->setValue(true);
	}
	else if (t == stopTrigger)
	{
		isPlaying->setValue(false);
		setCurrentTime(0, true, true);
	}
	else if (t == pauseTrigger)
	{
		isPlaying->setValue(false);
	}
	else if (t == finishTrigger)
	{
		isPlaying->setValue(false);
	}
	else if (t == togglePlayTrigger)
	{
		if (isPlaying->boolValue()) pauseTrigger->trigger();
		else playTrigger->trigger();

	}
	else if (t == prevCue)
	{
		setCurrentTime(cueManager->getPrevCueForTime(currentTime->floatValue(), 1), true, true);
	}
	else if (t == nextCue)
	{
		setCurrentTime(cueManager->getNextCueForTime(currentTime->floatValue()), true, true);
	}
}

void Sequence::onControllableStateChanged(Controllable* c)
{
	BaseItem::onControllableStateChanged(c);
	if (c == bpmPreview)
	{
		beatsPerBar->setEnabled(bpmPreview->enabled);
	}
}

void Sequence::parameterControlModeChanged(Parameter* p)
{
	BaseItem::parameterControlModeChanged(p);
	if (p == currentTime)
	{
		includeCurrentTimeInSave->setValue(currentTime->controlMode != Parameter::MANUAL);
		includeCurrentTimeInSave->setEnabled(currentTime->controlMode == Parameter::MANUAL);
	}
}

void Sequence::run()
{
	millisAtSetTime = Time::getMillisecondCounterHiRes();
	//timeAtSetTime = timeIsDrivenByAudio() ? hiResAudioTime : currentTime->floatValue();
	prevMillis = Time::getMillisecondCounterHiRes();

	followViewRange = viewEndTime->floatValue() - viewStartTime->floatValue();

	targetTime = currentTime->floatValue();

	while (!threadShouldExit())
	{

		double millis = Time::getMillisecondCounterHiRes();
		//double millisSinceSetTime = millis - millisAtSetTime;
		double delta = millis - prevMillis;

		targetTime += (delta / 1000.0) * playSpeed->floatValue();

		prevMillis = millis;

		//double absoluteTargetTime = timeAtSetTime + (millisSinceSetTime / 1000.0) * playSpeed->floatValue();
		
		if (timeIsDrivenByAudio())
		{
			//DBG("Diff (ms): " << abs(hiResAudioTime - currentTime->floatValue()));
			//targetTime = hiResAudioTime;
		}

		//DBG(deltaMillis << " : " << (targetTime - currentTime->floatValue()));

		currentTime->setValue(targetTime);

		if (viewFollowTime->boolValue())
		{
			float targetStart = jmax(currentTime->floatValue() - followViewRange / 2, 0.f);
			float targetEnd = targetStart + followViewRange;
			if (targetEnd > totalTime->floatValue())
			{
				targetEnd = totalTime->floatValue();
				targetStart = targetEnd - followViewRange;
			}

			viewStartTime->setValue(viewStartTime->getLerpValueTo(targetStart, .3f));
			viewEndTime->setValue(viewEndTime->getLerpValueTo(targetEnd, .3f));
		}

		if (targetTime >= totalTime->floatValue())
		{
			if (loopParam->boolValue())
			{
				float offset = targetTime - totalTime->floatValue();
				sequenceListeners.call(&SequenceListener::sequenceLooped, this);
				//setCurrentTime(0); //to change in trigger layer to avoid doing that
				prevTime = 0;
				setCurrentTime(offset, true, true);
			}
			else finishTrigger->trigger();
		}

		double millisPerCycle = 1000.0 / fps->floatValue();
		double millisAfterProcess = Time::getMillisecondCounterHiRes();
		double relAbsMillis = millisAfterProcess - millisAtSetTime;
		double millisToWait = ceil(millisPerCycle - fmod(relAbsMillis, millisPerCycle));

		if (millisToWait >= 0) wait(millisToWait);
	}
}

void Sequence::endLoadFile()
{
	Engine::mainEngine->removeEngineListener(this);
	if (isBeingEdited) selectThis();

	if (startAtLoad->boolValue())
	{
		playTrigger->trigger();
	}
}


void Sequence::audioDeviceIOCallback(const float**, int, float** outputChannelData, int numOutputChannels, int numSamples)
{
	for (int i = 0; i < numOutputChannels; ++i) FloatVectorOperations::clear(outputChannelData[i], numSamples);

	if (isPlaying->boolValue()) hiResAudioTime += (numSamples / sampleRate) * playSpeed->floatValue();
}

void Sequence::audioDeviceAboutToStart(AudioIODevice*)
{
	updateSampleRate();
}

void Sequence::audioDeviceStopped()
{
}
