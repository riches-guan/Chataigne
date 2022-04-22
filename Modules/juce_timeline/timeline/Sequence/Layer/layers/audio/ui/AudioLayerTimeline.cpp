
AudioLayerTimeline::AudioLayerTimeline(AudioLayer * layer) :
	SequenceLayerTimeline(layer),
	audioLayer(layer)
{
	bgColor = AUDIO_COLOR.withSaturation(.2f).darker(1);

	cmMUI.reset(new AudioLayerClipManagerUI(this, &layer->clipManager));
	addAndMakeVisible(cmMUI.get());

	updateMiniModeUI();

	needle.toFront(false);
	updateContent();
}

AudioLayerTimeline::~AudioLayerTimeline()
{

}

void AudioLayerTimeline::resized()
{
	cmMUI->setBounds(getLocalBounds());
}

void AudioLayerTimeline::updateContent()
{
	cmMUI->updateContent();
}

void AudioLayerTimeline::updateMiniModeUI()
{
	cmMUI->setMiniMode(item->miniMode->boolValue());
}

void AudioLayerTimeline::addSelectableComponentsAndInspectables(Array<Component*>& selectables, Array<Inspectable*>& inspectables)
{
	if (cmMUI == nullptr) return;
	cmMUI->addSelectableComponentsAndInspectables(selectables, inspectables);
}
