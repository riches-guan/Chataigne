﻿
MappingLayerPanel::MappingLayerPanel(MappingLayer * layer) :
	SequenceLayerPanel(layer),
	mappingLayer(layer)
{

	if (mappingLayer->mapping->om.outParams.size() > 0 && mappingLayer->mapping->om.outParams[0][0] != nullptr)
	{
		mappingOutputUI.reset(dynamic_cast<ControllableEditor*>(mappingLayer->mapping->om.outParams[0][0]->getEditor(false)));
		mappingOutputUI->setShowLabel(false);
		if (mappingOutputUI != nullptr) addAndMakeVisible(mappingOutputUI.get());
	}

	resized();
}

MappingLayerPanel::~MappingLayerPanel()
{
}

void MappingLayerPanel::resizedInternalHeader(Rectangle<int>& r)
{
	SequenceLayerPanel::resizedInternalHeader(r);

	if (item->miniMode->boolValue())
	{
		mappingOutputUI->setBounds(r);
	}
}

void MappingLayerPanel::resizedInternalContent(Rectangle<int>& r)
{
	Rectangle<int> cr = r.reduced(2, 0);

	if (!item->miniMode->boolValue())
	{
		if (mappingOutputUI != nullptr)
		{
			mappingOutputUI->setBounds(cr.removeFromTop(16));
			cr.removeFromTop(2);
		}

		resizedInternalPanelContent(cr);
	}
	
}

void MappingLayerPanel::addContextMenuItems(PopupMenu& p)
{
	SequenceLayerPanel::addContextMenuItems(p);
	p.addItem(100, L"将烘焙值导出到剪贴板");
	//Export baked values to clipboard
	p.addItem(101, L"将烘焙值导出到剪贴板（仅数据）");
	//Export baked values to clipboard (data only)

	p.addSeparator();

	p.addItem(102, L"复制过滤器");//Copy filters
	p.addItem(103, L"粘贴过滤器");//Paste filters

	p.addItem(104, L"复制输出");//Copy outputs
	p.addItem(105, L"粘贴输出");//Paste outputs
}

void MappingLayerPanel::handleContextMenuResult(int result)
{
	SequenceLayerPanel::handleContextMenuResult(result);
	switch (result)
	{
	case 100: mappingLayer->exportBakedValues(false); break;
	case 101: mappingLayer->exportBakedValues(true); break;

	case 102: SystemClipboard::copyTextToClipboard(JSON::toString(mappingLayer->mapping->fm.getJSONData())); break;
	case 103: mappingLayer->mapping->fm.loadJSONData(JSON::fromString(SystemClipboard::getTextFromClipboard())); break;
	case 104: SystemClipboard::copyTextToClipboard(JSON::toString(mappingLayer->mapping->om.getJSONData())); break;
	case 105: mappingLayer->mapping->om.loadJSONData(JSON::fromString(SystemClipboard::getTextFromClipboard())); break;
	}
}
