
TriggerLayer::TriggerLayer(Sequence * _sequence, StringRef name, var params) :
	SequenceLayer(_sequence, String(name)),
	ttm(nullptr)
{
	helpID = "TriggerLayer";
	//color->setColor(ACTION_COLOR.darker());

	lockAll = addTrigger(L"全部锁定", L"锁定此层中的所有现有触发器。");//Lock All  //Lock all existing triggers in this layer
	unlockAll = addTrigger(L"全部解锁", L"解锁此层中的所有现有触发器。");//Unlock All  //Unlock all existing triggers in this layer
	triggerWhenSeeking = addBoolParameter(L"请求时触发", L"如果选中此选项，则当播放一个序列并向前跳时，它将触发中间的所有触发器。", true); //Trigger when seeking   //If checked, this when a sequence is playing and you jump on it forward, it will trigger all the triggers inbetween.
	//Trigger when seeking
}

TriggerLayer::~TriggerLayer()
{
}


void TriggerLayer::setManager(TimeTriggerManager * _ttm)
{
	ttm.reset(_ttm);
	if (ttm != nullptr) addChildControllableContainer(ttm.get());
}

Array<Inspectable*> TriggerLayer::selectAllItemsBetweenInternal(float start, float end)
{
	Array<Inspectable*> result;
	result.addArray(ttm->getTriggersInTimespan(start, end, true));
	return result;
}

Array<UndoableAction*> TriggerLayer::getRemoveAllItemsBetweenInternal(float start, float end)
{
	return ttm->getRemoveItemsUndoableAction(ttm->getTriggersInTimespan(start, end, true));
}

Array<UndoableAction*> TriggerLayer::getInsertTimespanInternal(float start, float length)
{
	return ttm->getMoveKeysBy(start, length);
}

Array<UndoableAction*> TriggerLayer::getRemoveTimespanInternal(float start, float end)
{
	return ttm->getRemoveTimespan(start, end);
}

bool TriggerLayer::paste()
{
	if (ttm == nullptr) return false;
	if (!ttm->addItemsFromClipboard(false).isEmpty()) return true;
	return SequenceLayer::paste();
}

void TriggerLayer::onContainerTriggerTriggered(Trigger * t)
{
	SequenceLayer::onContainerTriggerTriggered(t);
	
	if (ttm == nullptr) return;

	if (t == lockAll) for (auto &i : ttm->items) i->isUILocked->setValue(true);
	else if (t == unlockAll) for (auto &i : ttm->items) i->isUILocked->setValue(false);
}

void TriggerLayer::getSnapTimes(Array<float>* arrayToFill)
{
	if (ttm == nullptr) return;
	for (auto& i : ttm->items) arrayToFill->addIfNotAlreadyThere(i->time->floatValue());
}

void TriggerLayer::getSequenceSnapTimesForManager(Array<float>* arrayToFill)
{
	sequence->getSnapTimes(arrayToFill);
}

var TriggerLayer::getJSONData()
{
	var data = SequenceLayer::getJSONData();
	if(ttm != nullptr) data.getDynamicObject()->setProperty(ttm->shortName, ttm->getJSONData());
	return data;
}

void TriggerLayer::loadJSONDataInternal(var data)
{
	SequenceLayer::loadJSONDataInternal(data);
	if(ttm != nullptr) ttm->loadJSONData(data.getProperty(ttm->shortName, var()));
}

SequenceLayerPanel * TriggerLayer::getPanel()
{
	return new TriggerLayerPanel(this);
}

SequenceLayerTimeline * TriggerLayer::getTimelineUI()
{
	return new TriggerLayerTimeline(this);
}
