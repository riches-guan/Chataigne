/*
  ==============================================================================

    ProgressWindow.h
    Created: 5 Oct 2016 11:37:24am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class ProgressWindow : public Component,public ProgressNotifier::ProgressListener
{
public:

	ProgressWindow(const String &title,ProgressNotifier * notifier=nullptr);
	~ProgressWindow();
	

	const int windowWidth = 300;
	const int windowHeight = 100;

	Label titleLabel;
	FloatParameter progressParam;
	std::unique_ptr<FloatSliderUI> progressUI;
	
	void paint(Graphics & g) override;
	void resized() override;

  void startedProgress(ProgressTask *  task)override;
  void endedProgress(ProgressTask * task) override;
  void newProgress(ProgressTask *  task ,float advance)override;

	void setProgress(float progress);
};