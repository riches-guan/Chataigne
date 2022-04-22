/*
  ==============================================================================

    ResolumeBaseCommandEditor.cpp
    Created: 19 Feb 2017 1:39:03pm
    Author:  Ben

  ==============================================================================
*/

ResolumeBaseCommandEditor::ResolumeBaseCommandEditor(ResolumeBaseCommand * _command, bool isRoot) :
	BaseCommandContainerEditor(_command, _command,isRoot),
	resolumeCommand(_command)
{

}

ResolumeBaseCommandEditor::~ResolumeBaseCommandEditor()
{

}

void ResolumeBaseCommandEditor::controllableFeedbackUpdate(Controllable * c)
{
	if (c == resolumeCommand->levelParam)
	{
		resetAndBuild();
		resized();
	}
}
