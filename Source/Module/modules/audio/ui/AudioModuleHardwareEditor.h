/*
  ==============================================================================

    AudioModuleHardwareEditor.h
    Created: 4 Jan 2018 1:45:32pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class AudioModuleHardwareEditor :
	public GenericControllableContainerEditor
{
public:
	AudioModuleHardwareEditor(AudioModuleHardwareSettings * hs, bool isRoot);
	~AudioModuleHardwareEditor();

	AudioModuleHardwareSettings * hs;
	AudioDeviceSelectorComponent selector;

	void setCollapsed(bool value, bool force = false, bool animate = true, bool doNotRebuild = false) override;
	void resizedInternalContent(Rectangle<int> &r) override;
};