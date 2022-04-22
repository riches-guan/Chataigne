
#pragma once

class Mapping2DLayerPanel :
    public AutomationMappingLayerPanel
{
public:
    Mapping2DLayerPanel(Mapping2DLayer* layer);
    ~Mapping2DLayerPanel();

    Mapping2DLayer* mappingLayer;

    std::unique_ptr<BoolToggleUI> keySyncUI;
    Curve2DUI curveUI;

    void resizedInternalPanelContent(Rectangle<int>& r) override;
};