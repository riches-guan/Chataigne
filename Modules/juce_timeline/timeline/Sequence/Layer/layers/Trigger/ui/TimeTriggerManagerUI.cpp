
TimeTriggerManagerUI::TimeTriggerManagerUI(TriggerLayerTimeline * _timeline, TimeTriggerManager * manager) :
	BaseManagerUI(L"��������", manager, false),//Triggers
	timeline(_timeline),
	miniMode(false)
{
	addItemText = L"��Ӵ�����";//Add Trigger
	animateItemOnAdd = false;
	transparentBG = true;

	addItemBT->setVisible(false);

	noItemText = L"Ҫ��Ӵ���������˫���˴���";//To add triggers, double click here

	manager->selectionManager->addSelectionListener(this);

	resizeOnChildBoundsChanged = false;
	addExistingItems();
}

TimeTriggerManagerUI::~TimeTriggerManagerUI()
{
	manager->selectionManager->removeSelectionListener(this);
	if (InspectableSelector::getInstanceWithoutCreating()) InspectableSelector::getInstance()->removeSelectorListener(this);
}

void TimeTriggerManagerUI::setMiniMode(bool value)
{
	if (miniMode == value) return;
	miniMode = value;

	for (auto& i : itemsUI) i->setInterceptsMouseClicks(!miniMode, !miniMode);
}

void TimeTriggerManagerUI::resized()
{
	updateContent();
}

void TimeTriggerManagerUI::updateContent()
{
	for (auto &ttui : itemsUI)
	{
		placeTimeTriggerUI(ttui);
		if(ttui->item->isSelected) ttui->toFront(true);
		else ttui->toBack();
	}
}

void TimeTriggerManagerUI::placeTimeTriggerUI(TimeTriggerUI * ttui)
{
	int tx = timeline->getXForTime(ttui->item->time->floatValue());
	
	float ttuiWidthTime = timeline->getTimeForX(ttui->getWidth(),false);

	float viewEnd = timeline->item->sequence->viewEndTime->floatValue();
	float totalTime = timeline->item->sequence->totalTime->floatValue();
	
	float ttuiTime = totalTime - ttuiWidthTime;

	if (viewEnd >= ttuiTime && ttuiTime < totalTime)
	{
		float rel = jmap<float>(viewEnd, ttuiTime, totalTime, 0, 1);
		int minTx = getWidth() - ttui->getWidth() * rel;
		int safeTx = jmin(tx, minTx); 
		ttui->flagXOffset = tx - safeTx;
		tx = safeTx;
	} else
	{
		ttui->flagXOffset = 0;
	}

	ttui->setBounds(tx, 0, ttui->getWidth(), getHeight());
}

void TimeTriggerManagerUI::mouseDown(const MouseEvent & e)
{
	BaseManagerUI::mouseDown(e);

	if (e.eventComponent == this)
	{
		if (e.mods.isLeftButtonDown())
		{
			if (e.mods.isAltDown())
			{
				if (manager->managerFactory == nullptr || manager->managerFactory->defs.size() == 1)
				{
					float time = timeline->getTimeForX(getMouseXYRelative().x);
					manager->addTriggerAt(time, getMouseXYRelative().y * 1.f / getHeight());
				}
			}
		}
	}
}

void TimeTriggerManagerUI::mouseDoubleClick(const MouseEvent & e)
{
	if (miniMode) return;
	if (manager->managerFactory != nullptr && manager->managerFactory->defs.size() > 1) return;

	float time = timeline->getTimeForX(getMouseXYRelative().x);
	manager->addTriggerAt(time, getMouseXYRelative().y*1.f / getHeight());
}

void TimeTriggerManagerUI::addItemFromMenu(bool isFromAddButton, Point<int> mouseDownPos)
{
	if (isFromAddButton || miniMode) return;

	float time = timeline->getTimeForX(mouseDownPos.x);
	manager->addTriggerAt(time, mouseDownPos.y*1.f / getHeight());
}

void TimeTriggerManagerUI::addItemFromMenu(TimeTrigger* t, bool, Point<int> mouseDownPos)
{
	float time = timeline->getTimeForX(mouseDownPos.x);
	t->time->setValue(time);
	t->flagY->setValue(mouseDownPos.y * 1.f / getHeight());
	manager->addItem(t);
}

void TimeTriggerManagerUI::addItemUIInternal(TimeTriggerUI * ttui)
{
	ttui->addTriggerUIListener(this);
}

void TimeTriggerManagerUI::removeItemUIInternal(TimeTriggerUI * ttui)
{
	ttui->removeTriggerUIListener(this);
}

void TimeTriggerManagerUI::timeTriggerMouseDown(TimeTriggerUI* ttui, const MouseEvent& e)
{
	snapTimes.clear();
	if (getSnapTimesFunc != nullptr) getSnapTimesFunc(&snapTimes);
}

void TimeTriggerManagerUI::timeTriggerDragged(TimeTriggerUI * ttui, const MouseEvent & e)
{
	if (miniMode) return;

	float diffTime = timeline->getTimeForX(e.getOffsetFromDragStart().x, false);
	//if (e.mods.isShiftDown()) diffTime = timeline->item->sequence->cueManager->getNearestCueForTime(ttui->timeAtMouseDown + diffTime) - ttui->timeAtMouseDown;

	if (e.mods.isAltDown())
	{
		ttui->item->scalePosition(Point<float>(diffTime, 0), true);
	}
	else
	{
		Point<float> offset(diffTime, e.mods.isShiftDown()?0:e.getDistanceFromDragStartY()*1.0f / (getHeight() -20)); //-20 is for subtracting flag height

		if (e.mods.isShiftDown() || timeline->item->sequence->autoSnap->boolValue())
		{
			float targetTime = timeline->item->sequence->getClosestSnapTimeFor(snapTimes, ttui->item->movePositionReference.x + offset.x);
			offset.x = targetTime - ttui->item->movePositionReference.x;
		}

		ttui->item->movePosition(offset, true);
	}
}

void TimeTriggerManagerUI::timeTriggerTimeChanged(TimeTriggerUI * ttui)
{
	placeTimeTriggerUI(ttui);
}

void TimeTriggerManagerUI::selectionEnded(Array<Component*> selectedComponents)
{
	if(InspectableSelector::getInstanceWithoutCreating()) InspectableSelector::getInstance()->removeSelectorListener(this);
	if (selectedComponents.size() == 0) timeline->item->selectThis();
}
