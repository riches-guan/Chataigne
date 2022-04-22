﻿
LayerBlockManagerUI::LayerBlockManagerUI(SequenceLayerTimeline * timeline, LayerBlockManager * manager) :
	BaseManagerUI(L"块管理", manager, false),//Block Manager
	timeline(timeline),
	miniMode(false)
{
	noItemText = L"要向该层添加块，请双击此处。";//To add a block to this layer, double-click here
	addItemText = L"添加块";//Add Block
	animateItemOnAdd = false;
	transparentBG = true;

	bringToFrontOnSelect = false;

	addItemBT->setVisible(false);
}

LayerBlockManagerUI::~LayerBlockManagerUI()
{
}

void LayerBlockManagerUI::resized()
{
	updateContent();
}

void LayerBlockManagerUI::updateContent()
{
	for (auto &cui : itemsUI) placeBlockUI(cui);
}

void LayerBlockManagerUI::setMiniMode(bool value)
{
	if (miniMode == value) return;
	miniMode = value;

	for (auto& i : itemsUI) i->setInterceptsMouseClicks(!miniMode, !miniMode);
}

LayerBlockUI * LayerBlockManagerUI::createUIForItem(LayerBlock * block)
{
	LayerBlockUI * b =  new LayerBlockUI(block);
	b->setInterceptsMouseClicks(!miniMode, !miniMode);
	return b;
}

void LayerBlockManagerUI::placeBlockUI(LayerBlockUI * cui)
{
	int tx = timeline->getXForTime(cui->item->time->floatValue());
	int tx2 = timeline->getXForTime(cui->item->time->floatValue() + cui->item->getTotalLength());

	cui->setViewRange(timeline->item->sequence->viewStartTime->floatValue() - cui->item->time->floatValue(), timeline->item->sequence->viewEndTime->floatValue() - cui->item->time->floatValue());
	cui->setBounds(tx, 0, tx2 - tx, getHeight());

	updateItemVisibility(cui);
}

void LayerBlockManagerUI::mouseDoubleClick(const MouseEvent & e)
{
	if(!e.mods.isCommandDown() && !e.mods.isShiftDown() && manager->userCanAddItemsManually && !miniMode) manager->addBlockAt(timeline->getTimeForX(getMouseXYRelative().x));
}

void LayerBlockManagerUI::addItemFromMenu(bool isFromAddButton, Point<int> mouseDownPos)
{
	if (!manager->userCanAddItemsManually || miniMode) return;
	if (isFromAddButton) return;
	manager->addBlockAt(timeline->getTimeForX(mouseDownPos.x));
}

void LayerBlockManagerUI::addItemUIInternal(LayerBlockUI * cui)
{
	cui->addBlockUIListener(this);
	placeBlockUI(cui);
}

void LayerBlockManagerUI::removeItemUIInternal(LayerBlockUI * cui)
{
	cui->removeBlockUIListener(this);
}

void LayerBlockManagerUI::blockUITimeChanged(LayerBlockUI * cui)
{
	placeBlockUI(cui);
}

void LayerBlockManagerUI::blockUIMouseDown(LayerBlockUI* cui, const MouseEvent& e)
{
	snapTimes.clear();
	timeline->item->sequence->getSnapTimes(&snapTimes);
}

void LayerBlockManagerUI::blockUIDragged(LayerBlockUI * cui, const MouseEvent& e)
{
	if (miniMode) return;

	float targetOffsetTime = timeline->getTimeForX(e.getOffsetFromDragStart().x, false);
	//float targetTime = cui->timeAtMouseDown + targetOffsetTime;

	if (e.mods.isShiftDown() || timeline->item->sequence->autoSnap->boolValue())
	{
		float tTime = cui->item->movePositionReference.x + targetOffsetTime;
		float clipLength = cui->item->getTotalLength();
		float tEndTime = tTime + clipLength;
		float diff = INT32_MAX;
		float targetTime = tTime;
		for (auto& t : snapTimes)
		{
			float d = fabsf(tTime - t);
			if (d < diff)
			{
				diff = d;
				targetTime = t;
			}

			float de = fabsf(tEndTime - t);
			if (de < diff)
			{
				diff = de;
				targetTime = t - clipLength;
			}
		}

		targetOffsetTime = targetTime - cui->item->movePositionReference.x;
	}

	cui->item->movePosition(Point<float>(targetOffsetTime, 0), true);

	
	cui->setViewRange(timeline->item->sequence->viewStartTime->floatValue() - cui->item->time->floatValue(), timeline->item->sequence->viewEndTime->floatValue() - cui->item->time->floatValue());
	cui->resized(); //force resize because changing time will not resize it, just move it
}

void LayerBlockManagerUI::blockUIStartDragged(LayerBlockUI * cui, const MouseEvent & e)
{
	if (miniMode) return;
	
	float timeDiff = timeline->getTimeForX(e.getOffsetFromDragStart().x, false);
	float targetTime = cui->item->movePositionReference.x +timeDiff;

	if (timeline->item->sequence->autoSnap->boolValue())
	{
		targetTime = timeline->item->sequence->getClosestSnapTimeFor(snapTimes, targetTime);
	}


	int itemIndex = manager->items.indexOf(cui->item);
	float minTime = (itemIndex > 0 && !manager->blocksCanOverlap) ? manager->items[itemIndex - 1]->getEndTime() : 0;
	targetTime = jmax<float>(minTime, targetTime);

	manager->placeBlockAt(cui->item, targetTime);

	float realTimeDiff = cui->item->time->floatValue() - cui->item->movePositionReference.x;
	float targetCoreLength = cui->coreLengthAtMouseDown - realTimeDiff;

	cui->item->setCoreLength(targetCoreLength,e.mods.isShiftDown(), !e.mods.isAltDown());
	cui->setViewRange(timeline->item->sequence->viewStartTime->floatValue() - cui->item->time->floatValue(), timeline->item->sequence->viewEndTime->floatValue() - cui->item->time->floatValue());
}

void LayerBlockManagerUI::blockUICoreDragged(LayerBlockUI * cui, const MouseEvent & e)
{
	if (miniMode) return;
	
	if (e.mods.isCommandDown())
	{
		blockUILoopDragged(cui, e);
		return;
	}

	float targetCoreLength = cui->coreLengthAtMouseDown + timeline->getTimeForX(e.getOffsetFromDragStart().x, false);

	if (timeline->item->sequence->autoSnap->boolValue())
	{
		float targetTime = cui->item->time->floatValue() + targetCoreLength;
		targetTime = timeline->item->sequence->getClosestSnapTimeFor(snapTimes, targetTime);
		targetCoreLength = targetTime - cui->item->time->floatValue();
	}


	if (!manager->blocksCanOverlap)
	{
		int itemIndex = manager->items.indexOf(cui->item);
		if (itemIndex < manager->items.size() - 1)
		{
			float maxCoreLength = manager->items[itemIndex + 1]->time->floatValue() - cui->item->loopLength->floatValue() - cui->item->time->floatValue();
			targetCoreLength = jmin<float>(maxCoreLength, targetCoreLength);
		}
	}

	cui->item->setCoreLength(targetCoreLength, e.mods.isShiftDown(), e.mods.isAltDown());
	cui->setViewRange(timeline->item->sequence->viewStartTime->floatValue() - cui->item->time->floatValue(), timeline->item->sequence->viewEndTime->floatValue() - cui->item->time->floatValue());
}

void LayerBlockManagerUI::blockUILoopDragged(LayerBlockUI * cui, const MouseEvent & e)
{
	float targetLoopLength = cui->loopLengthAtMouseDown + timeline->getTimeForX(e.getOffsetFromDragStart().x, false);
	
	if (!manager->blocksCanOverlap)
	{
		int itemIndex = manager->items.indexOf(cui->item);
		if (itemIndex < manager->items.size() - 1)
		{
			float maxLoopLength = manager->items[itemIndex + 1]->time->floatValue() - cui->item->coreLength->floatValue() - cui->item->time->floatValue();
			targetLoopLength = jmin<float>(maxLoopLength, targetLoopLength);
		}
	}
	
	if (e.mods.isShiftDown() || timeline->item->sequence->autoSnap->boolValue())
	{
		float targetTime = cui->item->getCoreEndTime() + targetLoopLength;
		targetTime = timeline->item->sequence->getClosestSnapTimeFor(snapTimes, targetTime);
		targetLoopLength = targetTime - cui->item->getCoreEndTime();
	}
	
	cui->item->setLoopLength(targetLoopLength);
	cui->setViewRange(timeline->item->sequence->viewStartTime->floatValue() - cui->item->time->floatValue(), timeline->item->sequence->viewEndTime->floatValue() - cui->item->time->floatValue());
}

void LayerBlockManagerUI::blockUINeedsReorder()
{
	manager->reorderItems();
}

void LayerBlockManagerUI::updateItemVisibility(LayerBlockUI* cui)
{
	float start = cui->item->time->floatValue();
	float end = cui->item->getEndTime();
	bool isOutside = end < timeline->item->sequence->viewStartTime->floatValue() || start > timeline->item->sequence->viewEndTime->floatValue();
	cui->setVisible(!isOutside);
}
