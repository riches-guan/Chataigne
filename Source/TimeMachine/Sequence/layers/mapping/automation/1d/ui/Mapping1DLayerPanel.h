
#pragma once

class Mapping1DLayerPanel :
    public AutomationMappingLayerPanel
{
public:
    Mapping1DLayerPanel(Mapping1DLayer* layer);
    virtual ~Mapping1DLayerPanel();

    Mapping1DLayer* ml;
    std::unique_ptr<ControllableEditor> valueRangeUI;
    std::unique_ptr<ControllableEditor> rangeRemapUI;

    void resizedInternalPanelContent(Rectangle<int>& r) override;
};