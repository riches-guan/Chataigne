
SequenceLayer::SequenceLayer(Sequence * _sequence, const String &name) :
	BaseItem(name,true),
	sequence(_sequence)
{
	jassert(sequence != nullptr);

	itemDataType = "SequenceLayer";

	uiHeight = addIntParameter(L"UI高度", L"编辑器中图层的高度。", 60);//Height   //Height of the layer in the editor
	uiHeight->hideInEditor = true;

	color = new ColorParameter(L"图层颜色", L"图层的颜色。", BG_COLOR.brighter(.2f));//Layer Color  //Color of the layer
	color->hideInEditor = true;
	addParameter(color);

	sequence = _sequence;
	sequence->addSequenceListener(this);

}

SequenceLayer::~SequenceLayer()
{
	if (!sequence->isClearing)
		sequence->removeSequenceListener(this);
}

Array<Inspectable*> SequenceLayer::selectAllItemsBetween(float start, float end)
{
	if (isUILocked->boolValue() || !enabled->boolValue()) return Array<Inspectable *>();
	return selectAllItemsBetweenInternal(start, end);
}

Array<Inspectable*> SequenceLayer::selectAllItemsBetweenInternal(float start, float end)
{
	return Array<Inspectable*>();
}

Array<UndoableAction*> SequenceLayer::getRemoveAllItemsBetween(float start, float end)
{
	if (isUILocked->boolValue() || !enabled->boolValue()) return Array<UndoableAction*>();
	return getRemoveAllItemsBetweenInternal(start, end);
}

Array<UndoableAction*> SequenceLayer::getRemoveAllItemsBetweenInternal(float start, float end)
{
	return Array<UndoableAction*>();
}

Array<UndoableAction*> SequenceLayer::getInsertTimespan(float start, float length)
{
	if (isUILocked->boolValue() || !enabled->boolValue()) return Array<UndoableAction*>();
	return getInsertTimespanInternal(start, length);
}

Array<UndoableAction*> SequenceLayer::getInsertTimespanInternal(float start, float length)
{
	return Array<UndoableAction*>();
}

Array<UndoableAction*> SequenceLayer::getRemoveTimespan(float start, float end)
{
	if (isUILocked->boolValue() || !enabled->boolValue()) return Array<UndoableAction*>();
	return getRemoveTimespanInternal(start, end);
}

Array<UndoableAction*> SequenceLayer::getRemoveTimespanInternal(float start, float end)
{
	return Array<UndoableAction*>();
}

SequenceLayerPanel * SequenceLayer::getPanel()
{
	return new SequenceLayerPanel(this);
}

SequenceLayerTimeline * SequenceLayer::getTimelineUI()
{
	return new SequenceLayerTimeline(this);
}

//InspectableEditor * SequenceLayer::getEditor(bool isRoot)
//{
//	return new BaseItemEditor(this, isRoot);
//}

