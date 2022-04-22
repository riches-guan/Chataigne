
#pragma once

class Mapping2DTimeline :
	public AutomationMappingLayerTimeline
{
public:
	Mapping2DTimeline(Mapping2DLayer* layer);
	~Mapping2DTimeline() {}

	Mapping2DLayer* mappingLayer;

	void paint(Graphics& g) override;

	void controllableFeedbackUpdateInternal(Controllable* c) override;
};