/*
  ==============================================================================

    InspectableSelector.cpp
    Created: 25 Mar 2017 4:13:37pm
    Author:  Ben

  ==============================================================================
*/


juce_ImplementSingleton(InspectableSelector)

InspectableSelector::InspectableSelector() :
	selectablesParent(nullptr),
	currentSelectionManager(nullptr),
	clearSelectionAtEnd(false)
{
	setRepaintsOnMouseActivity(true);
}

InspectableSelector::~InspectableSelector()
{

}

void InspectableSelector::startSelection(Component * parent, Array<Component *> _selectables, InspectableSelectionManager * manager, bool clearSelection)
{
	Array<Inspectable *> arr;
	for (auto &s : selectables)
	{
		InspectableContent * ic = dynamic_cast<InspectableContent *>(s);
		arr.add(ic->inspectable);
	}

	startSelection(parent, _selectables, arr, manager, clearSelection);
}

void InspectableSelector::startSelection(Component * parent, Array<Component*> _selectables, Array<Inspectable*> relatedInspectables, InspectableSelectionManager * manager, bool clearSelection)
{
	jassert(_selectables.size() == relatedInspectables.size());
	
	currentSelectionManager = manager != nullptr ? manager : InspectableSelectionManager::mainSelectionManager;
	
	if (_selectables.size() == 0)
	{
		selectablesParent = nullptr;
		selectables.clear();
		inspectables.clear();
		return;
	}

	selectablesParent = parent;
	selectables = _selectables;
	inspectables = relatedInspectables;

	clearSelectionAtEnd = clearSelection;

	selectablesParent->addAndMakeVisible(this);
	selectablesParent->addMouseListener(this, false);
	setBounds(selectablesParent->getLocalBounds());
	selectionBounds.setSize(0, 0);

}


void InspectableSelector::endSelection(bool confirmSelection)
{
	if (selectablesParent == nullptr || selectables.size() == 0) return;

	selectablesParent->removeChildComponent(this);
	selectablesParent->removeMouseListener(this);

	if (selectionBounds.getWidth() == 0 || selectionBounds.getHeight() == 0) return; //No selection attempt, just click.
	
	Array<Inspectable *> selection;

	Array<Component *> selectedComponents;
	int numSelectables = selectables.size();
	for (int i = 0; i < numSelectables; ++i) if (inspectables[i]->isPreselected) selectedComponents.add(selectables[i]);

	for (auto& s : inspectables)
	{
		if (confirmSelection)
		{
			if (s->isPreselected) selection.add(s);
		}
		else
		{
			s->setPreselected(false);
		}
	}

	if (confirmSelection) currentSelectionManager->selectInspectables(selection, clearSelectionAtEnd);

	listeners.call(&SelectorListener::selectionEnded, selectedComponents);

}

void InspectableSelector::paint(Graphics& g)
{
	if (selectionBounds.getWidth() == 0 || selectionBounds.getHeight() == 0) return;
	g.setColour(PRESELECT_COLOR.withAlpha(.2f));
	g.fillRect(selectionBounds);
	g.setColour(PRESELECT_COLOR.darker(.3f).withAlpha(.7f));
	g.drawRect(selectionBounds);
}

void InspectableSelector::mouseDrag(const MouseEvent& e)
{
	Point<int> topLeft = Point<int>(jmin(e.getMouseDownX(), e.getPosition().x), jmin(e.getMouseDownY(), e.getPosition().y));
	Point<int> bottomRight = Point<int>(jmax(e.getMouseDownX(), e.getPosition().x), jmax(e.getMouseDownY(), e.getPosition().y));

	selectionBounds.setBounds(topLeft.x, topLeft.y, bottomRight.x - topLeft.x, bottomRight.y - topLeft.y);
	repaint();

	if (e.eventComponent == selectablesParent)
	{
		int numSelectables = selectables.size();
		for (int i = 0; i < numSelectables; ++i)
		{
			juce::Rectangle<int> icp = selectablesParent->getLocalArea(selectables[i], selectables[i]->getLocalBounds());
			if (selectionBounds.intersects(icp))
			{
				inspectables[i]->setPreselected(true);
			}
			else
			{
				inspectables[i]->setPreselected(false);
			}
		}
	}
	
}

void InspectableSelector::mouseUp(const MouseEvent &)
{
	endSelection();
}
