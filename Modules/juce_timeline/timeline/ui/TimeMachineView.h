
#pragma once


class TimeMachineView :
	public ShapeShifterContentComponent,
	public InspectableSelectionManager::Listener,
	public Inspectable::InspectableListener
{
public:
	
	TimeMachineView(const String &contentName);
	~TimeMachineView();

	std::unique_ptr<SequenceEditorView> editor;

	bool autoSelectOnSequenceSelected;
	void paint(Graphics &g) override;
	void resized() override;

	virtual void setSequence(Sequence * sequence);
	virtual SequenceEditorView * createEditorForSequence(Sequence * sequence);

	virtual void inspectablesSelectionChanged() override;
	void inspectableDestroyed(Inspectable * i) override;


	static TimeMachineView * create(const String &contentName) { return new TimeMachineView(contentName); }


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeMachineView)
};


