
#pragma once

class ColorMappingLayerTimeline :
    public MappingLayerTimeline
{
public:
    ColorMappingLayerTimeline(ColorMappingLayer* layer);
    ~ColorMappingLayerTimeline();

    ColorMappingLayer* cml;
    GradientColorManagerUI* colorManagerUI;

    virtual void updateContent() override;
    virtual void updateMiniModeUI() override;
    
    virtual void addSelectableComponentsAndInspectables(Array<Component*>& selectables, Array<Inspectable*>& inspectables) override;
};