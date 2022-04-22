﻿
#pragma once

class SequenceLayerPanel;
class SequenceLayerTimeline;

class SequenceLayer :
	public BaseItem,
	public Sequence::SequenceListener
{
public:
	SequenceLayer(Sequence * _sequence = nullptr, const String &name = L"新建图层");//New Layer
	virtual ~SequenceLayer();

	Sequence * sequence;

	//UI
	IntParameter * uiHeight;
	ColorParameter * color;

	virtual void addDefaultContent() {}

	Array<Inspectable *> selectAllItemsBetween(float start, float end);
	virtual Array<Inspectable*> selectAllItemsBetweenInternal(float start, float end);
	Array<UndoableAction*> getRemoveAllItemsBetween(float start, float end);
	virtual Array<UndoableAction*> getRemoveAllItemsBetweenInternal(float start, float end);
	Array<UndoableAction*> getInsertTimespan(float start, float length);
	virtual Array<UndoableAction*> getInsertTimespanInternal(float start, float length);
	Array<UndoableAction*> getRemoveTimespan(float start, float end);
	virtual Array<UndoableAction*> getRemoveTimespanInternal(float start, float end);

	virtual void getSnapTimes(Array<float> * arrayToFill) {}

	//Factory
	virtual String getTypeString() const override { jassert(false); return ""; } //should be overriden

	//UI
	virtual SequenceLayerPanel * getPanel();
	virtual SequenceLayerTimeline * getTimelineUI();

	//InspectableEditor * getEditorInternal(bool isRoot, Array<Inspectable*> inspectables = Array<Inspectable*>()) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SequenceLayer)
};