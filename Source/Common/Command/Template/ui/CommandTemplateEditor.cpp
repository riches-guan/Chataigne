/*
  ==============================================================================

    CommandTemplateEditor.cpp
    Created: 31 May 2018 11:29:07am
    Author:  Ben

  ==============================================================================
*/

CommandTemplateEditor::CommandTemplateEditor(CommandTemplate * t, bool isRoot) :
	BaseItemEditor(t, isRoot)
{
	triggerUI.reset(t->triggerTrigger->createButtonUI());
	addAndMakeVisible(triggerUI.get());
}

CommandTemplateEditor::~CommandTemplateEditor()
{
}

void CommandTemplateEditor::resizedInternalHeaderItemInternal(Rectangle<int>& r)
{
	BaseItemEditor::resizedInternalHeaderItemInternal(r);
	triggerUI->setBounds(r.removeFromRight(60).reduced(2));
	r.removeFromRight(2);
}
