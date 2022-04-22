
#pragma once

class AutomationMappingLayerTimeline :
    public MappingLayerTimeline
{
public:
    AutomationMappingLayerTimeline(AutomationMappingLayer* layer);
    ~AutomationMappingLayerTimeline();

    AutomationMappingLayer* aml;
    AutomationUI* automationUI;

    virtual void updateContent() override;

    virtual void setSeekManipulationMode(bool isManipulating) override;
    virtual void updateMiniModeUI() override;

    virtual void controllableFeedbackUpdateInternal(Controllable* c) override;

    virtual void inspectableSelectionChanged(Inspectable* i) override;

    virtual void addSelectableComponentsAndInspectables(Array<Component*>& selectables, Array<Inspectable*>& inspectables) override;

};