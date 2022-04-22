/*
  ==============================================================================

    InspectableSelector.h
    Created: 25 Mar 2017 4:13:37pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class InspectableSelector : 
	public Component
{
public:
	juce_DeclareSingleton(InspectableSelector, false);
	InspectableSelector();
	~InspectableSelector();

	Component * selectablesParent;

	Array<Component *> selectables;
	Array<Inspectable * > inspectables;

	 juce::Rectangle<int> selectionBounds;

	//memory of current Selection
	InspectableSelectionManager * currentSelectionManager;
	bool clearSelectionAtEnd;

	void startSelection(Component * parent, Array<Component *> selectables, InspectableSelectionManager * manager = nullptr, bool clearSelection = true);
	void startSelection(Component * parent, Array<Component *> selectables, Array<Inspectable *> relatedInspectables, InspectableSelectionManager * manager = nullptr, bool clearSelection = true); //If custom UI needed, use that function to provide different set of ui and inspectables (must be the same size !!)
	void endSelection(bool confirmSelection = true);

	void paint(Graphics &g) override;

	void mouseDrag(const MouseEvent &e) override;
	void mouseUp(const MouseEvent &e) override;

	
	class  SelectorListener
	{
	public:
		virtual ~SelectorListener() {}
		virtual void selectionEnded(Array<Component *> selectedComponents) {};
	};

	ListenerList<SelectorListener> listeners;
	void addSelectorListener(SelectorListener* newListener) { listeners.add(newListener); }
	void removeSelectorListener(SelectorListener* listener) { listeners.remove(listener); }
	
};