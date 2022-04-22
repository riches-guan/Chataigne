/*
  ==============================================================================

    PowerpointModule.h
    Created: 13 Jul 2018 12:11:43am
    Author:  Ben

  ==============================================================================
*/

#pragma once

class PowerPointModule :
	public OSCModule,
	public URL::DownloadTask::Listener 
{
public:
	PowerPointModule();
	~PowerPointModule();

	IntParameter* currentSlide;
	IntParameter * totalSlides;

	File downloadedFile;
	std::unique_ptr<URL::DownloadTask> downloadTask; 
	void installPowerPointPlugin();

	void progress(URL::DownloadTask *, int64 bytesDownloaded, int64 totalBytes) override;
	void finished(URL::DownloadTask *, bool success) override;

	void processMessageInternal(const OSCMessage& m) override;


	static PowerPointModule * create() { return new PowerPointModule(); }
	virtual String getDefaultTypeString() const override { return "PowerPoint"; }

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PowerPointModule)

};
