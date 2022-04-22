/*
  ==============================================================================

	Action.h
	Created: 28 Oct 2016 8:05:02pm
	Author:  bkupe

  ==============================================================================
*/

#pragma once

class Action :
	public Processor,
	public MultiplexTarget,
	public ConditionManager::ConditionManagerListener,
	public ConditionManager::ManagerListener,
	public Condition::ConditionListener,
	public EngineListener
{
public:
	Action(var params = var(), Multiplex* it = nullptr);
	Action(const String& name, var params, Multiplex* it, bool hasConditions, bool hasOffConsequences);
	virtual ~Action();

	enum Role { ACTIVATE, DEACTIVATE };
	Array<Role> actionRoles;

	bool autoTriggerWhenAllConditionAreActives; //default true, but if false, let use Actions as user check tool without auto behavior (like TimeTriggers)

	bool forceNoOffConsequences; // independent of roles
	bool hasOffConsequences;

	std::unique_ptr<ConditionManager> cdm;
	std::unique_ptr<ConsequenceManager> csmOn;
	std::unique_ptr<ConsequenceManager> csmOff;

	Trigger* triggerOn;
	Trigger* triggerPreview;

	//to allow for checking before conditions sending it, to overcome listener-order problems
	bool forceChecking;

	void updateConditionRoles();
	void setHasOffConsequences(bool value);
	virtual void updateDisables(bool force = false) override;

	void forceCheck(bool triggerIfChanged);

	virtual void triggerConsequences(bool triggerTrue, int multiplexIndex = 0);

	void multiplexPreviewIndexChanged() override;

	void onContainerTriggerTriggered(Trigger* t) override;
	void onContainerParameterChangedInternal(Parameter* p) override;
	void controllableFeedbackUpdate(ControllableContainer* cc, Controllable* c) override;

	void notifyActionTriggered(bool triggerTrue, int multiplexIndex);

	void conditionManagerValidationChanged(ConditionManager*, int multiplexIndex, bool dispatchOnChangeOnly) override;

	void itemAdded(Condition*) override;
	void itemRemoved(Condition*) override;

	virtual void highlightLinkedInspectables(bool value) override;

	var getJSONData() override;
	void loadJSONDataItemInternal(var data) override;
	void endLoadFile() override;

	virtual ProcessorUI* getUI() override;

	class ActionListener
	{
	public:
		virtual ~ActionListener() {}
		virtual void actionEnableChanged(Action*) {}
		virtual void actionRoleChanged(Action*) {}
		virtual void actionValidationChanged(Action*) {}
		virtual void actionTriggered(Action*, bool /*triggerTrue*/, int /*multiplexIndex*/ = 0) {}
	};

	ListenerList<ActionListener> actionListeners;
	void addActionListener(ActionListener* newListener) { actionListeners.add(newListener); }
	void removeActionListener(ActionListener* listener) { actionListeners.remove(listener); }


	class ActionEvent {
	public:
		enum Type { ENABLED_CHANGED, ROLE_CHANGED, VALIDATION_CHANGED, MULTIPLEX_PREVIEW_CHANGED };
		ActionEvent(Type type, Action* c) : type(type), action(c) {}
		Type type;
		Action* action;
	};
	QueuedNotifier<ActionEvent> actionAsyncNotifier;
	typedef QueuedNotifier<ActionEvent>::Listener AsyncListener;


	void addAsyncActionListener(AsyncListener* newListener) { actionAsyncNotifier.addListener(newListener); }
	void addAsyncCoalescedActionListener(AsyncListener* newListener) { actionAsyncNotifier.addAsyncCoalescedListener(newListener); }
	void removeAsyncActionListener(AsyncListener* listener) { actionAsyncNotifier.removeListener(listener); }


	String getTypeString() const override { return "Action"; };
	//InspectableEditor * getEditor(bool /*isRoot*/) override;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Action)
};