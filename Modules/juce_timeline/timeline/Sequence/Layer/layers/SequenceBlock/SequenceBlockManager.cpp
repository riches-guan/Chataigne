
SequenceBlockManager::SequenceBlockManager(SequenceBlockLayer * layer, SequenceManager * refManager) :
	LayerBlockManager(layer, "Blocks"),//
	refManager(refManager),
	sequenceBlockLayer(layer)
{
	jassert(refManager != nullptr);

	itemDataType = "SequenceBlock";
	blocksCanOverlap = false;

	isSelectable = false;
}

SequenceBlockManager::~SequenceBlockManager()
{
}

LayerBlock* SequenceBlockManager::createItem()
{
	return new SequenceBlock(refManager);
}
