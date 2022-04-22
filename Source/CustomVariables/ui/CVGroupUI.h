/*
  ==============================================================================

    CVGroupUI.h
    Created: 22 Feb 2018 3:42:48pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
class CVGroupUI :
	public BaseItemUI<CVGroup>
{
public:
	CVGroupUI(CVGroup* item);
	~CVGroupUI();

	std::unique_ptr<FloatSliderUI> presetProgressionUI;

	void resizedInternalHeader(Rectangle<int> &r) override;
	void controllableFeedbackUpdateInternal(Controllable* c) override;
};


class CVGroupVariablesEditor:
	public GenericManagerEditor<GenericControllableItem>
{
public:
	CVGroupVariablesEditor(GenericControllableManager * manager, bool isRoot);
	~CVGroupVariablesEditor();

	void addPopupMenuItems(PopupMenu* p) override;
	void handleMenuSelectedID(int result) override;
};

