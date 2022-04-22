/*
  ==============================================================================

    AutomationMultiKeyTransformer.h
    Created: 25 Mar 2017 4:14:45pm
    Author:  Ben

  ==============================================================================
*/

#pragma once  
class AutomationUI;

class AutomationMultiKeyTransformer :
	public Component
{
public:
	AutomationMultiKeyTransformer(AutomationUI * aui, Array<AutomationKeyUI *> keysUI);
	~AutomationMultiKeyTransformer();

	AutomationUI * aui;
	Array<AutomationKeyUI *> keysUI;
	Array<Point<float>> keysRelativePositions;
	Array<Point<float>> keysTimesAndValuesPositions;

	ResizableBorderComponent resizer;
	
	Rectangle<int> keyBounds;
	Point<int> posAtMouseDown;

	void updateBoundsFromKeys();
	void updateKeysFromBounds();

	void parentHierarchyChanged() override;

	void resized() override;
	void paint(Graphics &g) override;

	void mouseDown(const MouseEvent &e) override;
	void mouseDrag(const MouseEvent &e) override;
	void mouseUp(const MouseEvent &e) override;
};

