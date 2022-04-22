/*
  ==============================================================================

    ShapeShifter.cpp
    Created: 2 May 2016 6:33:16pm
    Author:  bkupe

  ==============================================================================
*/

ShapeShifter::ShapeShifter(Type _type) :
parentContainer(nullptr),
shifterType(_type),
	preferredWidth(300),
    preferredHeight(300)
{
}

ShapeShifter::~ShapeShifter()
{
}

void ShapeShifter::setPreferredWidth(int newWidth)
{
	preferredWidth = jmax<int>(150, newWidth);
}

void ShapeShifter::setPreferredHeight(int newHeight)
{
	preferredHeight = jmax<int>(50, newHeight);
}

int ShapeShifter::getPreferredWidth()
{
	return preferredWidth;
}

int ShapeShifter::getPreferredHeight()
{
	return preferredHeight;
}

bool ShapeShifter::isDetached()
{
	return parentContainer == nullptr;
}

void ShapeShifter::setParentContainer(ShapeShifterContainer * _parent)
{
	if (_parent == parentContainer) return;
	parentContainer = _parent;
}

bool ShapeShifter::isFlexible()
{
	return false;
}

var ShapeShifter::getCurrentLayout()
{
	var layout(new DynamicObject());
	
	//set to actual values before saving so when loading, will get exactly the same
	preferredWidth = getWidth();
	preferredHeight = getHeight();

	layout.getDynamicObject()->setProperty("type", (int)shifterType);
	layout.getDynamicObject()->setProperty("width", preferredWidth);
	layout.getDynamicObject()->setProperty("height", preferredHeight);
	return layout;
}

void ShapeShifter::loadLayout(var layout)
{
	setPreferredWidth(layout.getDynamicObject()->getProperty("width"));
	setPreferredHeight(layout.getDynamicObject()->getProperty("height"));

	loadLayoutInternal(layout);
}
