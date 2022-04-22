#pragma once

class SequenceLayerTimeline :
	public BaseItemMinimalUI<SequenceLayer>,
	public Timer
{
public:
	SequenceLayerTimeline(SequenceLayer *);
	virtual ~SequenceLayerTimeline();

	bool shouldUpdateNeedle;
    bool seekManipulationMode;
	
	int getXForTime(float time);
	float getTimeForX(int tx, bool offsetStart = true);
    
    void updateNeedlePosition();
	virtual void updateContent() {} // to be overriden
    virtual void updateMiniModeUI() {} //to be overriden;

    virtual void mouseDown(const MouseEvent &e) override;

    virtual void addSelectableComponentsAndInspectables(Array<Component*>& selectables, Array<Inspectable*>& inspectables) {}

    virtual void setSeekManipulationMode(bool isManipulating);

	void controllableFeedbackUpdateInternal(Controllable * c) override;


	void timerCallback() override;

	virtual void visibilityChanged() override;
    
    class TimelineNeedle :
        public Component
    {
    public:
        TimelineNeedle();
        ~TimelineNeedle();
        
        const Colour defaultTimeBarColor = Colours::white.withAlpha(.4f);
        Colour timeBarColor;
        
        void paint(Graphics &g) override;
        
    };
    
    TimelineNeedle needle;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SequenceLayerTimeline)
};