/*
 ==============================================================================

 FloatParameter.h
 Created: 8 Mar 2016 1:22:10pm
 Author:  bkupe

 ==============================================================================
 */

#pragma once

class FloatSliderUI;
class FloatStepperUI;
class FloatParameterLabelUI;
class TimeLabel;

class FloatParameter : public Parameter
{
public:
	FloatParameter(const String& niceName, const String& description, const float& initialValue, const float& minValue = (float)INT32_MIN, const float& maxValue = (float)INT32_MAX, bool enabled = true);
	virtual ~FloatParameter() {}

	float unitSteps;

	enum UIType { NONE, SLIDER, STEPPER, LABEL, TIME };
	UIType defaultUI;
	UIType customUI;

	FloatSliderUI* createSlider(Array<Parameter*> parameters = {});
	FloatStepperUI* createStepper(Array<Parameter *> parameters = {});
	FloatParameterLabelUI* createLabelParameter(Array<Parameter *> parameters = {});
	TimeLabel* createTimeLabelParameter(Array<Parameter *> parameters = {});

	virtual ControllableUI* createDefaultUI(Array<Controllable*> controllables = {}) override;

	bool checkValueIsTheSame(var oldValue, var newValue) override;

	virtual void setValueInternal(var& value) override;

	virtual bool hasRange() override;

	virtual var getLerpValueTo(var targetValue, float weight) override;
	virtual void setWeightedValue(Array<var> values, Array<float> weights) override;
	virtual float getStepSnappedValueFor(float originalValue);

	void setControlAutomation() override;

	virtual String stringValue() override { return String(floatValue()); }

	void setAttribute(String name, var val) override;
	virtual StringArray getValidAttributes() const override;

	var getJSONDataInternal() override;
	void loadJSONDataInternal(var data) override;

	static FloatParameter* create() { return new FloatParameter("New Float Parameter", "", 0); }
	virtual String getTypeString() const override { return getTypeStringStatic(); }
	static String getTypeStringStatic() { return "Float"; }

	virtual var getCroppedValue(var originalValue) override;
};
