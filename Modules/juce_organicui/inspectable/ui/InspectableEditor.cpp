/*
  ==============================================================================

	CustomEditor.cpp
	Created: 9 May 2016 6:42:18pm
	Author:  bkupe

  ==============================================================================
*/

InspectableEditor::InspectableEditor(Array<Inspectable*> inspectables, bool _isRoot) :
	inspectables(Inspectable::getWeakArray(inspectables)),
	inspectable(inspectables[0]),
	parentInspector(nullptr),
	fitToContent(false),
	isRoot(_isRoot),
	isInsideInspectorBounds(false)
{
	addComponentListener(this);
}

InspectableEditor::InspectableEditor(Inspectable * _inspectable, bool _isRoot) :
	InspectableEditor(Array<Inspectable *>({ _inspectable }), _isRoot)
{
}


InspectableEditor::~InspectableEditor()
{
}

void InspectableEditor::componentMovedOrResized(Component& c, bool wasMoved, bool wasResized)
{
	if (&c == this) updateVisibility();
}

void InspectableEditor::updateVisibility()
{
	if (parentInspector == nullptr) return;
	//if (inspectable.wasObjectDeleted()) return;

	juce::Rectangle<int> r = parentInspector->getLocalArea(this, getLocalBounds()); //getLocalArea(parentInspector, parentInspector->getLocalBounds());
	juce::Rectangle<int> ir = parentInspector->getLocalBounds().getIntersection(r);

	isInsideInspectorBounds = !ir.isEmpty();
	bool shouldBeVisible = isInsideInspectorBounds;

	//Too hacky ? parent should handle children visibility
	//if (GenericControllableContainerEditor* pe = dynamic_cast<GenericControllableContainerEditor*>(getParentComponent()))
	//{
	//	if (GenericControllableContainerEditor* gce = dynamic_cast<GenericControllableContainerEditor*>(pe->getParentComponent()))
	//	{
	//		if (!pe->isRoot && !pe->container.wasObjectDeleted() && pe->container->editorIsCollapsed && gce->childEditors.contains(pe)) shouldBeVisible = false;
	//	}
	//}

	setVisible(shouldBeVisible);

	Array<Component*> children = getChildren();
	for (auto& c : children)
	{
		InspectableEditor* ed = dynamic_cast<InspectableEditor*>(c);
		if (ed != nullptr) ed->updateVisibility();
	}
}

void InspectableEditor::parentHierarchyChanged()
{
	parentInspector = findParentComponentOfClass<Inspector>();
	updateVisibility();

}


//GENERIC COMPONENT EDITOR

GenericComponentEditor::GenericComponentEditor(WeakReference<Inspectable> i, Component* c, bool isRoot) :
	InspectableEditor(i, isRoot),
	child(c)
{
	addAndMakeVisible(c);
	setSize(getWidth(), c->getHeight());
}


GenericComponentEditor::~GenericComponentEditor()
{
}

void GenericComponentEditor::resized()
{
	child->setBounds(getLocalBounds());
}

void GenericComponentEditor::childBoundsChanged(Component* c)
{
	setSize(getWidth(), child->getHeight());
}
