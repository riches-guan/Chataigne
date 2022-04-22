
#pragma once

class AudioLayerProcessor;

class AudioLayer :
	public SequenceLayer,
	public AudioLayerClip::ClipListener,
	public AudioLayerClipManager::ManagerListener,
	public Thread,
	public Inspectable::InspectableListener
{
public:
	AudioLayer(Sequence * sequence, var params);
	~AudioLayer();
	
	AudioLayerClipManager clipManager;
	
	AudioProcessorGraph * currentGraph;

	WeakReference<AudioLayerClip> currentClip;
	AudioLayerProcessor * currentProcessor;

	ControllableContainer channelsCC;
	Array<int> selectedOutChannels;
	var channelsData; //for ghosting

    FloatParameter * volume;
	FloatParameter * panning;
	FloatParameter * enveloppe;

	int numActiveOutputs;

	AudioProcessorGraph::NodeID graphID;
	static int graphIDIncrement;
	int audioOutputGraphID;

	//Volume animation
	float targetVolume;
	Automation* volumeInterpolationAutomation;
	WeakReference<Inspectable> volumeAutomationRef;
	float volumeInterpolationTime;
	bool stopAtVolumeInterpolationFinish;

	//thread transportSource stop flag
	bool clipIsStopping;

	virtual void clearItem() override;

	void setAudioProcessorGraph(AudioProcessorGraph * graph, int audioOutputGraphID = 2);
	virtual AudioLayerProcessor* createAudioLayerProcessor();

	virtual AudioLayerClip* createAudioClip();

	virtual void updateCurrentClip();

	void itemAdded(LayerBlock*) override;
	void itemRemoved(LayerBlock* clip) override;

	void clipSourceLoaded(AudioLayerClip* clip) override;

	void updateSelectedOutChannels();
	void updateClipConfig(AudioLayerClip* clip, bool updateOutputChannelRemapping = true);

	virtual float getVolumeFactor();
	virtual void setVolume(float value, float time = 0, Automation * automation = nullptr, bool stopSequenceAtFinish = false);

	void onControllableFeedbackUpdateInternal(ControllableContainer *cc, Controllable * c) override;

	void selectAll(bool addToSelection = false) override;

	virtual var getJSONData() override;
	virtual void loadJSONDataInternal(var data) override;

	virtual SequenceLayerPanel * getPanel() override;
	virtual SequenceLayerTimeline * getTimelineUI() override;
	
	void sequenceCurrentTimeChanged(Sequence *, float prevTime, bool evaluatedSkippedData) override;
	void sequencePlayStateChanged(Sequence *) override;
	void sequencePlaySpeedChanged(Sequence*) override;
	void sequencePlayDirectionChanged(Sequence*) override;

	static AudioLayer * create(Sequence * sequence, var params) { return new AudioLayer(sequence, params); }
	virtual String getTypeString() const override { return "Audio"; }

	virtual void getSnapTimes(Array<float>* arrayToFill) override;
	
	//For volume interpolation
    void run() override;

	void inspectableDestroyed(Inspectable* i) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioLayer)
	
};


class AudioLayerProcessor :
	public AudioProcessor
{
public:
	AudioLayerProcessor(AudioLayer * layer);
	~AudioLayerProcessor();
	
	AudioLayer * layer;
	
	const int minEnveloppeSamples = 1024;
	int rmsCount;
	float tempRMS;
	float currentEnveloppe;

	void clear();

	// H�rit?via AudioProcessor
	virtual const String getName() const override;
	virtual void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
	virtual void releaseResources() override;
	virtual void processBlock(AudioBuffer<float>& buffer, MidiBuffer & midiMessages) override;
	virtual double getTailLengthSeconds() const override;
	virtual bool acceptsMidi() const override;
	virtual bool producesMidi() const override;
	virtual AudioProcessorEditor * createEditor() override;
	virtual bool hasEditor() const override;
	virtual int getNumPrograms() override;
	virtual int getCurrentProgram() override;
	virtual void setCurrentProgram(int index) override;
	virtual const String getProgramName(int index) override;
	virtual void changeProgramName(int index, const String & newName) override;
	virtual void getStateInformation(juce::MemoryBlock & destData) override;
	virtual void setStateInformation(const void * data, int sizeInBytes) override;
};
