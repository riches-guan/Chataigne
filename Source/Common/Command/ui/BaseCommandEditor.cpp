/*
  ==============================================================================

    BaseCommandEditor.cpp
    Created: 21 Dec 2020 11:09:27pm
    Author:  bkupe

  ==============================================================================
*/

BaseCommandContainerEditor::BaseCommandContainerEditor(BaseCommand* command, ControllableContainer * container, bool isRoot) :
    ParamLinkContainerEditor(command, isRoot, false),
    baseCommand(command)
{
    resetAndBuild(); //force here to use the overriden getEditorUI function
}

BaseCommandContainerEditor::~BaseCommandContainerEditor()
{
}

InspectableEditor* BaseCommandContainerEditor::getEditorUIForContainer(ControllableContainer* cc)
{
    if (Script* s = dynamic_cast<Script*>(cc)) return GenericControllableContainerEditor::getEditorUIForContainer(cc);
    else if(cc == baseCommand->customValuesManager.get()) return GenericControllableContainerEditor::getEditorUIForContainer(cc);

    return ParamLinkContainerEditor::getEditorUIForContainer(cc);
}
