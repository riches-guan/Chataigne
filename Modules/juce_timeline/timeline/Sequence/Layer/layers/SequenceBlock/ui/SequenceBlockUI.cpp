
SequenceBlockUI::SequenceBlockUI(SequenceBlock * _block) :
	LayerBlockUI(_block),
	block(_block)
{
	dragAndDropEnabled = false;
	bgColor = block->isActive->boolValue() ? BLUE_COLOR.brighter() : BG_COLOR.brighter(.1f);
}

SequenceBlockUI::~SequenceBlockUI()
{
}

void SequenceBlockUI::paint(Graphics& g)
{
	LayerBlockUI::paint(g);

	g.setColour(TEXT_COLOR);
	g.setFont(jmin(getHeight() - 5, 16));

	Sequence* s = block->getTargetSequence();
	g.drawFittedText(s != nullptr ? s->niceName : "No Sequence selected", getLocalBounds(), Justification::centred, 2);
}

void SequenceBlockUI::controllableFeedbackUpdateInternal(Controllable* c)
{
	LayerBlockUI::controllableFeedbackUpdateInternal(c);

	if (c == item->isActive)
	{
		bgColor = block->isActive->boolValue() ? BLUE_COLOR.brighter() : BG_COLOR.brighter(.1f);
		repaint();
	}
}