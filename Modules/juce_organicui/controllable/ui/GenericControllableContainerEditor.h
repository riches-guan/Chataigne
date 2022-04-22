/*
  ==============================================================================

    GenericControllableContainerEditor.h
    Created: 9 May 2016 6:41:59pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once


class GenericControllableContainerEditor :
	public InspectableEditor,
	public ContainerAsyncListener,
	public Button::Listener,
	public ChangeListener,
	public Label::Listener,
	public DragAndDropContainer
{
public:
	GenericControllableContainerEditor(Array<ControllableContainer *> containers, bool isRoot, bool buildAtCreation = true);
	virtual ~GenericControllableContainerEditor();

	
	int headerHeight;
	const int headerGap = 4;

	bool isRebuilding; //to avoid constant resizing when rebuilding/adding items
	
	bool prepareToAnimate;
	ComponentAnimator collapseAnimator;

	Colour contourColor;
	Label containerLabel;

	Array<ControllableContainer *> containers;
	WeakReference<ControllableContainer> container;
	OwnedArray<InspectableEditor> childEditors;

	std::unique_ptr<ImageButton> expandBT;
	std::unique_ptr<ImageButton> collapseBT;
	std::unique_ptr<ImageButton> addBT;
	std::unique_ptr<WarningTargetUI> warningUI;
	std::unique_ptr<ImageButton> removeBT;

	Component headerSpacer;

	bool dragAndDropEnabled;

	virtual void setCollapsed(bool value, bool force = false, bool animate = true, bool doNotRebuild = false);
	virtual void toggleCollapsedChildren();
	virtual void resetAndBuild();


	void paint(Graphics &g) override;
	void resized() override;
	virtual void resizedInternal(juce::Rectangle<int> &r);
	virtual void resizedInternalHeader(juce::Rectangle<int> &r);
	virtual void resizedInternalContent(juce::Rectangle<int> &r);

	virtual void clear();

	void mouseDown(const MouseEvent &e) override;
	void mouseDrag(const MouseEvent& e) override;

	virtual void setDragAndDropEnabled(bool value);


	virtual void showContextMenu();
	virtual void addPopupMenuItems(PopupMenu *) {} //for child classes
	virtual void handleMenuSelectedID(int) {} // to override


	juce::Rectangle<int> getHeaderBounds(); 
	juce::Rectangle<int> getContentBounds();
	
	bool canBeCollapsed();

	virtual InspectableEditor * getEditorUIForControllable(Controllable *c);
	virtual InspectableEditor * addControllableUI(Controllable * c, bool resize = false);
	virtual void removeControllableUI(Controllable *c, bool resize = false);

	virtual InspectableEditor * getEditorUIForContainer(ControllableContainer *cc);
	virtual InspectableEditor * addEditorUI(ControllableContainer * cc, bool resize = false);
	virtual void removeEditorUI(InspectableEditor * i, bool resize = false);
	
	virtual void showMenuAndAddControllable();

	InspectableEditor * getEditorForInspectable(Inspectable * i);
	
	virtual void buttonClicked(Button * b) override;
	virtual void labelTextChanged(Label * l) override;

	virtual void componentVisibilityChanged(Component &c) override;

	void newMessage(const ContainerAsyncEvent & p) override;
	virtual void controllableFeedbackUpdate(Controllable *) {};
	void childBoundsChanged(Component *) override;



	// Inherited via ChangeListener
	virtual void changeListenerCallback(ChangeBroadcaster * source) override;

	class  ContainerEditorListener
	{
	public:
		/** Destructor. */
		virtual ~ContainerEditorListener() {}
		virtual void containerRebuilt(GenericControllableContainerEditor *) {}
	};

	ListenerList<ContainerEditorListener> containerEditorListeners;
	void addContainerEditorListener(ContainerEditorListener* newListener) { containerEditorListeners.add(newListener); }
	void removeContainerEditorListener(ContainerEditorListener* listener) { containerEditorListeners.remove(listener); }
};


class EnablingControllableContainerEditor :
	public GenericControllableContainerEditor
{
public:
	EnablingControllableContainerEditor(Array<EnablingControllableContainer *> cc, bool isRoot, bool buildAtCreation = true);
	~EnablingControllableContainerEditor() {}

	Array<EnablingControllableContainer*> ioContainers;
	EnablingControllableContainer * ioContainer;
	std::unique_ptr<BoolToggleUI> enabledUI;

	virtual void resizedInternalHeader(juce::Rectangle<int> &r) override;
	virtual void controllableFeedbackUpdate(Controllable *) override;
};