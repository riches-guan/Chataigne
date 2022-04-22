
#pragma once

class AudioLayer;

class AudioLayerClipManager :
	public LayerBlockManager
{
public:
	AudioLayerClipManager(AudioLayer * layer);
	~AudioLayerClipManager();

	AudioLayer* audioLayer;
	LayerBlock* createItem() override;
	
};
