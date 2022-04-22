
TimeMachineView::TimeMachineView(const String &contentName) :
	ShapeShifterContentComponent(contentName),
	autoSelectOnSequenceSelected(true)
{
	contentIsFlexible = true;
	InspectableSelectionManager::mainSelectionManager->addSelectionListener(this);

	Sequence * s = InspectableSelectionManager::mainSelectionManager->getInspectableAs<Sequence>();
	if (s != nullptr) setSequence(s);

	helpID = "SequenceEditor";

}

TimeMachineView::~TimeMachineView()
{
	if(InspectableSelectionManager::mainSelectionManager) InspectableSelectionManager::mainSelectionManager->removeSelectionListener(this);
	setSequence(nullptr);
}

void TimeMachineView::paint(Graphics & g)
{
	g.fillAll(BG_COLOR.darker(.3f));
	
	if (editor == nullptr)
	{
		g.setFont(20);
		g.setColour(BG_COLOR.brighter(.1f));
		g.drawFittedText(L"从序列面板创建一个序列并在此处编辑该序列。", getLocalBounds(), Justification::centred, 3);
		//Create a sequence from the Sequences Panel to edit this sequence here.
	}
}

void TimeMachineView::resized()
{
	if (editor != nullptr)
	{
		editor->setBounds(getLocalBounds());
	}
}

void TimeMachineView::setSequence(Sequence * sequence)
{
	if (editor != nullptr && editor->sequence == sequence) return;

	if (editor != nullptr)
	{
		editor->sequence->removeInspectableListener(this);
		removeChildComponent(editor.get());
		editor = nullptr;
		
	}

	if (sequence != nullptr)
	{
		editor.reset(createEditorForSequence(sequence));
		editor->sequence->addInspectableListener(this);
		addAndMakeVisible(editor.get());
	}
	
	resized();
}

SequenceEditorView * TimeMachineView::createEditorForSequence(Sequence * sequence)
{
	return new SequenceEditorView(sequence);
}

void TimeMachineView::inspectablesSelectionChanged()
{
	if (!autoSelectOnSequenceSelected) return;
	if (InspectableSelectionManager::mainSelectionManager->isEmpty()) return;
	
	Sequence * s = InspectableSelectionManager::mainSelectionManager->getInspectableAs<Sequence>();
	if (s != nullptr) setSequence(s);
}

void TimeMachineView::inspectableDestroyed(Inspectable * i)
{
	if (i == editor->sequence) setSequence(nullptr);
}
