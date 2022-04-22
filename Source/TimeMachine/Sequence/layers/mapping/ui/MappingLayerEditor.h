
#pragma once

class MappingLayerEditor :
	public BaseItemEditor
{
public:
	MappingLayerEditor(MappingLayer * layer, bool isRoot);
	~MappingLayerEditor();

	MappingLayer * mappingLayer;
	std::unique_ptr<InspectableEditor> mappingEditor;

	void resizedInternalContent(Rectangle<int> &r) override;
};