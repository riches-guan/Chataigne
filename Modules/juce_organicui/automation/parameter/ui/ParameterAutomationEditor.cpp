

ParameterAutomationEditor::ParameterAutomationEditor(ParameterAutomation * ppa) :
	ppa(ppa)
{
	automationEditor.reset(ppa->getContentEditor(false));
	addAndMakeVisible(automationEditor.get());

	if (ppa->mode != nullptr)
	{
		modeUI.reset(ppa->mode->createDefaultUI());
		lengthUI.reset(ppa->lengthParamRef->createDefaultUI());
		addAndMakeVisible(modeUI.get());
		addAndMakeVisible(lengthUI.get());
	}

	if (AutomationEditor * ae = dynamic_cast<AutomationEditor *>(automationEditor.get()))
	{
		ae->automationUI->autoAdaptViewRange = true;
	}
	
	setSize(100, automationEditor->getHeight());
}

ParameterAutomationEditor::~ParameterAutomationEditor()
{

}

void ParameterAutomationEditor::resized()
{
	Rectangle<int> r = getLocalBounds();
	
	if (modeUI != nullptr)
	{
		Rectangle<int> f = r.removeFromBottom(14);
		modeUI->setBounds(f.removeFromLeft(80));
		f.removeFromLeft(2);
		lengthUI->setBounds(f);

	}
	
	r.removeFromBottom(2);
	automationEditor->setBounds(r);
	
	
}
