#pragma once


class DashboardControllableItem :
	public DashboardInspectableItem
{
public:
	DashboardControllableItem(Controllable* item = nullptr);
	~DashboardControllableItem();

	WeakReference<Controllable> controllable;

	BoolParameter * showLabel;
	ColorParameter* textColor;
	IntParameter* textSize;
	BoolParameter * opaqueBackground;
	ColorParameter* contourColor;
	FloatParameter* contourThickness;
	StringParameter* customLabel;
	StringParameter* customDescription;
	BoolParameter* forceReadOnly;


	virtual var getItemParameterFeedback(Parameter* p) override;

    virtual var getJSONData() override;
	virtual void loadJSONData(var data, bool createIfNotThere = false) override; //needs to not override ItemInternal to force loading data after set inspectable

	virtual void setInspectableInternal(Inspectable * i) override;

	virtual void ghostInspectable() override;
	virtual void checkGhost() override;

	//override all to avoid sending events to the real parent container
	virtual void controllableStateChanged(Controllable*) override;
	virtual void controllableFeedbackStateChanged(Controllable*) override;
	virtual void controllableControlAddressChanged(Controllable* c) override;
	virtual void controllableNameChanged(Controllable*) override;
	virtual void askForRemoveControllable(Controllable*, bool /*addToUndo*/ = false) override;

	virtual var getServerData() override;

	virtual String getTypeString() const override { return "DashboardControllableItem"; }

	DECLARE_ASYNC_EVENT(DashboardControllableItem, DashboardControllableItem, dashboardItem, ENUM_LIST(NEEDS_UI_UPDATE))
};
