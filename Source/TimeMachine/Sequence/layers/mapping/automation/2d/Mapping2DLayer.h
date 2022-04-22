
#pragma once

class Mapping2DLayer :
	public AutomationMappingLayer
{
public:
	Mapping2DLayer(Sequence* s, var params);
	~Mapping2DLayer();

	Curve2D curve;

	void addDefaultContent() override;

	virtual var getValueAtPosition(float position) override;

	virtual void stopRecorderAndAddKeys() override;

	SequenceLayerPanel* getPanel() override;
	SequenceLayerTimeline* getTimelineUI() override;

	virtual String getTypeString() const override { return getTypeStringStatic(); }
	static const String getTypeStringStatic() { return "Mapping 2D"; }

	static Mapping2DLayer* create(Sequence* sequence, var params) { return new Mapping2DLayer(sequence, params); }

};