﻿
#pragma once

class TimeTrigger :
	public BaseItem
{
public:
	TimeTrigger(StringRef name = "Trigger");//
	virtual ~TimeTrigger();

	FloatParameter * time;
	BoolParameter * isTriggered;
	ColorParameter* color;

	//ui
	FloatParameter * flagY;

	void setMovePositionReferenceInternal() override;
	void setPosition(Point<float> targetTime) override;
	Point<float> getPosition() override;

	void addUndoableMoveAction(Array<UndoableAction *> &actions) override;

	virtual void trigger();
	virtual void triggerInternal() {}
};