﻿
AudioLayerClip::AudioLayerClip() :
	LayerBlock(getTypeString()),
	Thread("AudioClipReader"),
	resamplingAudioSource(&channelRemapAudioSource, false),
    channelRemapAudioSource(&transportSource, false),
    clipDuration(0),
	sampleRate(0),
	clipSamplePos(0),
	isLoading(false),

	audioClipAsyncNotifier(10)
{
	itemDataType = "AudioClip";

	filePath = new FileParameter(L"文件路径", L"文件路径", "");//File Path
	addParameter(filePath);

	clipLength = addFloatParameter(L"片段长度", L"片段的长度（秒）。", 0);//Clip Length  //Length of the clip (in seconds)
	clipLength->defaultUI = FloatParameter::TIME;
	clipLength->setControllableFeedbackOnly(true);
	clipLength->isSavable = false;

	stretchFactor = addFloatParameter(L"拉伸系数", L"这个片段的拉伸", 1);//Stretch Factor   //Stretching of  this clip
	stretchFactor->defaultUI = FloatParameter::TIME;
	stretchFactor->setControllableFeedbackOnly(true);
	stretchFactor->isSavable = false;

	clipStartOffset = addFloatParameter(L"片段开始偏移", L"片段开始的偏移量", 0, 0);//Clip Start Offset   //Offset at which the clip starts
	clipStartOffset->defaultUI = FloatParameter::TIME;

	fadeIn = addFloatParameter(L"淡入", L"片段开始时的淡入时间", 0, 0);//Fade In   //Fade time at start of the clip
	fadeIn->defaultUI = FloatParameter::TIME;
	fadeOut = addFloatParameter(L"淡出", L"片段辑结束时的淡出时间", 0, 0);//Fade Out  //Fade time at end of the clip
	fadeOut->defaultUI = FloatParameter::TIME;

	resetStretch = addTrigger(L"重置拉伸", L"将拉伸系数重置为1");//Reset Stretch  //Reset the stretch factor to 1

	volume = addFloatParameter(L"音量", L"音量增加因子", 1, 0);//Volume  //Volume multiplier

	formatManager.registerBasicFormats();

}

AudioLayerClip::~AudioLayerClip()
{
	stopThread(3000);
	masterReference.clear();
	transportSource.releaseResources();
}

void AudioLayerClip::updateAudioSourceFile()
{
	if (filePath->stringValue().isEmpty()) return;

#if JUCE_WINDOWS
	if (filePath->stringValue().startsWithChar('/')) return;
#endif

	startThread();
}

void AudioLayerClip::onContainerTriggerTriggered(Trigger* t)
{
	LayerBlock::onContainerTriggerTriggered(t);
	if (t == resetStretch)
	{
		stretchFactor->setValue(1);
		if (coreLength->floatValue() > clipLength->floatValue()) coreLength->setValue(clipLength->floatValue());
	}
}

void AudioLayerClip::onContainerParameterChangedInternal(Parameter* p)
{
	LayerBlock::onContainerParameterChangedInternal(p);
	if (p == filePath)
	{
		updateAudioSourceFile();
	}
	else if (p == isActive)
	{
		if (isActive->boolValue())
		{
			clipSamplePos = 0;
		}
		else
		{
			transportSource.stop();
			clipSamplePos = -1;
		}
	}

}

void AudioLayerClip::setCoreLength(float value, bool stretch, bool stickToCoreEnd)
{
	if (stickToCoreEnd)
	{

		float offsetToAdd = value - coreLength->floatValue();
		clipStartOffset->setValue(jlimit<float>(0, (float)clipLength->floatValue() - (float)coreLength->minimumValue, clipStartOffset->floatValue() - offsetToAdd)); //invert to get actual start time of the clip
	}

	if (stretch)
	{
		stretchFactor->setValue(stretchFactor->floatValue() + ((value / coreLength->floatValue()) - 1) * stretchFactor->floatValue());
	}

	LayerBlock::setCoreLength(value, stretch, stickToCoreEnd);
}

void AudioLayerClip::setStartTime(float value, bool stretch, bool stickToCoreEnd)
{
	if (stickToCoreEnd)
	{

	}

	LayerBlock::setStartTime(value, stretch, stickToCoreEnd);

}

void AudioLayerClip::setPlaySpeed(float value)
{
	if (value <= 0) return;
	resamplingAudioSource.setResamplingRatio(stretchFactor->floatValue() / value);
}

void AudioLayerClip::prepareToPlay(int blockSize, int _sampleRate)
{
	resamplingAudioSource.prepareToPlay(blockSize, _sampleRate);
}

void AudioLayerClip::run()
{
	if (filePath == nullptr) return;

	isLoading = true;
	audioClipAsyncNotifier.addMessage(new ClipEvent(ClipEvent::SOURCE_LOAD_START, this));

	transportSource.setSource(nullptr);
	readerSource.reset(nullptr);

	setupFromSource();

	isLoading = false;
	audioClipAsyncNotifier.addMessage(new ClipEvent(ClipEvent::SOURCE_LOAD_END, this));
	clipListeners.call(&ClipListener::clipSourceLoaded, this);
}

void AudioLayerClip::setupFromSource()
{
	AudioFormatReader* reader = formatManager.createReaderFor(filePath->getAbsolutePath());

	if (reader != nullptr)
	{
		std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader, true));
		transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate, reader->numChannels);
		readerSource.reset(newSource.release());
		sampleRate = reader->sampleRate;
		clipDuration = reader->lengthInSamples / sampleRate;

		clipLength->setValue(clipDuration);
		if (!coreLength->isOverriden)
		{
			coreLength->defaultValue = clipLength->floatValue();
			coreLength->resetValue();
		}

		//buffer.setSize((int)reader->numChannels, (int)reader->lengthInSamples);
		//reader->read(&buffer, 0, (int)reader->lengthInSamples, 0, true, true);
	}
}
