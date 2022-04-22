
AudioLayerClipManager::AudioLayerClipManager(AudioLayer * layer) :
	LayerBlockManager(layer, "Clips"), //
	audioLayer(layer)
{
	itemDataType = "AudioClip";
	blocksCanOverlap = false;

	isSelectable = false;
}

AudioLayerClipManager::~AudioLayerClipManager()
{
}

LayerBlock* AudioLayerClipManager::createItem()
{
	return audioLayer->createAudioClip();
}
