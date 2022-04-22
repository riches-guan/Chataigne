#pragma once

class ParameterDetectiveWatcher :
	public ControllableDetectiveWatcher
{
public:
	ParameterDetectiveWatcher();
	virtual ~ParameterDetectiveWatcher();

	WeakReference<Parameter> parameter;

	virtual void setControllable(Controllable* c) override;

	void onExternalParameterValueChanged(Parameter* p) override;

    virtual ControllableDetectiveWatcherUI* getUI() override;
};
