/*
  ==============================================================================

    AudioModuleHardwareEditor.cpp
    Created: 4 Jan 2018 1:45:32pm
    Author:  Ben

  ==============================================================================
*/

AudioModuleHardwareEditor::AudioModuleHardwareEditor(AudioModuleHardwareSettings * hs, bool isRoot) :
	GenericControllableContainerEditor(hs,isRoot),
	hs(hs),
	selector(*hs->am, 0, 16, 0, 16, false, false, false, false)
{
	selector.setSize(100, 300);
	selector.setVisible(!container->editorIsCollapsed);
	addAndMakeVisible(selector);
	if(selector.isVisible()) setSize(100,selector.getBottom() + headerHeight + headerGap);
}

AudioModuleHardwareEditor::~AudioModuleHardwareEditor()
{
}

void AudioModuleHardwareEditor::setCollapsed(bool value, bool force, bool animate, bool doNotRebuild)
{
	GenericControllableContainerEditor::setCollapsed(value, force, animate, doNotRebuild);
	selector.setVisible(!container->editorIsCollapsed);
}

void AudioModuleHardwareEditor::resizedInternalContent(Rectangle<int>& r)
{
	selector.setBounds(r.withHeight(selector.getHeight()));
	r.setY(selector.getHeight());
	r.setHeight(0);
}
