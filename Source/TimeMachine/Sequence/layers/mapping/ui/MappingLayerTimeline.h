
#pragma once

class MappingLayerTimeline :
	public SequenceLayerTimeline
{
public:
	MappingLayerTimeline(MappingLayer * layer, Component * mainComponent);
	~MappingLayerTimeline();

	MappingLayer * mappingLayer;

	std::unique_ptr<Component> mainComponent;

	void resized() override;
};