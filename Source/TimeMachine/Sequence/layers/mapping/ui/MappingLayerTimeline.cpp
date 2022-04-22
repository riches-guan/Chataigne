
MappingLayerTimeline::MappingLayerTimeline(MappingLayer * layer, Component * component) :
	SequenceLayerTimeline(layer),
	mappingLayer(layer)
{
	setInterceptsMouseClicks(true, true);
	
	mainComponent.reset(component);
	addAndMakeVisible(mainComponent.get());

	needle.toFront(false);
	updateContent();
}

MappingLayerTimeline::~MappingLayerTimeline()
{
}

void MappingLayerTimeline::resized()
{
	mainComponent->setBounds(getLocalBounds());
}