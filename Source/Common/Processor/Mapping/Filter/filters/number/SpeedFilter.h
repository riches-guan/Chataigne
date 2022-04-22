/*
  ==============================================================================

    SpeedFilter.h
    Created: 2 Nov 2021 3:11:00pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class SpeedFilter :
	public TimeFilter
{
public:
	SpeedFilter(var params, Multiplex* multiplex);
	~SpeedFilter();

	HashMap<Parameter*, var> prevValueMap;
	HashMap<Parameter*, var> speedMap;
	const int smoothCount = 10;

	void setupParametersInternal(int multiplexIndex, bool rangeOnly) override;
	Parameter* setupSingleParameterInternal(Parameter* source, int multiplexIndex, bool rangeOnly) override;
	virtual ProcessResult processSingleParameterTimeInternal(Parameter* source, Parameter* out, int multiplexIndex, double deltaTime) override;

	String getTypeString() const override { return "Speed"; }
};
