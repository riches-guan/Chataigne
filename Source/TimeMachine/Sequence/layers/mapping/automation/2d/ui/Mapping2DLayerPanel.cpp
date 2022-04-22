

Mapping2DLayerPanel::Mapping2DLayerPanel(Mapping2DLayer* layer) :
    AutomationMappingLayerPanel(layer),
    mappingLayer(layer),
    curveUI(&layer->curve)
{
    keySyncUI.reset(mappingLayer->curve.keySyncMode->createToggle());
    addAndMakeVisible(keySyncUI.get());
    addAndMakeVisible(&curveUI);

    contentComponents.add(keySyncUI.get());
    contentComponents.add(&curveUI);
}

Mapping2DLayerPanel::~Mapping2DLayerPanel()
{
}

void Mapping2DLayerPanel::resizedInternalPanelContent(Rectangle<int>& r)
{
    AutomationMappingLayerPanel::resizedInternalPanelContent(r);
    r.removeFromTop(2);
    keySyncUI->setBounds(r.removeFromTop(16).reduced(4, 0));
    r.removeFromTop(2);
    curveUI.setBounds(r);
}
