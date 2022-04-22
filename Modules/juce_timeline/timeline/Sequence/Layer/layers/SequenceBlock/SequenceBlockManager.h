
#pragma once

class SequenceBlockLayer;
class SequenceManager;

class SequenceBlockManager :
	public LayerBlockManager
{
public:
	SequenceBlockManager(SequenceBlockLayer * layer, SequenceManager * refManager);
	~SequenceBlockManager();

	SequenceManager* refManager;

	SequenceBlockLayer * sequenceBlockLayer;
	LayerBlock* createItem() override;
	
};
