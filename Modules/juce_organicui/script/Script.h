/*
==============================================================================

Script.h
Created: 20 Feb 2017 5:01:11pm
Author:  Ben

==============================================================================
*/

#pragma once


class Script :
	public BaseItem,
	public Timer,
	public Thread,
	public EngineListener
{
public:
	Script(ScriptTarget* parentTarget = nullptr, bool canBeDisabled = true, bool canBeRemoved = true);
	~Script();

	enum ScriptState { SCRIPT_LOADING, SCRIPT_LOADED, SCRIPT_ERROR, SCRIPT_EMPTY, SCRIPT_CLEAR };

	bool forceDisabled; //for scripts that can not be disabled by users directly

	String* scriptTemplate;
	FileParameter* filePath;
	BoolParameter* logParam;
	Trigger* reload;
	IntParameter* updateRate;

	ScriptState state;
	String fileName;
	Time fileLastModTime;
	var paramsContainerData; //for keeping overriden values

	bool updateEnabled; //When loading the script, checks if the update function is present
	const Identifier updateIdentifier = "update";

	std::unique_ptr<ControllableContainer> scriptParamsContainer;

	ScriptTarget* parentTarget;

	std::unique_ptr<JavascriptEngine> scriptEngine;
	CriticalSection engineLock;
	float executionTimeout;


	void setParamsContainer(ControllableContainer* cc);
	void chooseFileScript();

	void loadScript();
	void buildEnvironment();

	void setState(ScriptState newState);


	var callFunction(const Identifier& function, const Array<var> args, juce::Result* result = (juce::Result*)nullptr);

	void onContainerParameterChangedInternal(Parameter*) override;
	void onContainerTriggerTriggered(Trigger*) override;

	void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) override;
	void childStructureChanged(ControllableContainer* cc) override;

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

	void endLoadFile() override;

	InspectableEditor* getEditorInternal(bool isRoot, Array<Inspectable*> inspectables = Array<Inspectable*>()) override;


	// Inherited via Timer
	virtual void timerCallback() override;

	virtual void run() override;

	class ScriptEvent
	{
	public:
		enum Type { STATE_CHANGE };
		ScriptEvent(Type _type) : type(_type) {}
		Type type;
	};

	QueuedNotifier<ScriptEvent> scriptAsyncNotifier;
	typedef QueuedNotifier<ScriptEvent>::Listener AsyncListener;


	void addAsyncScriptListener(AsyncListener* newListener) { scriptAsyncNotifier.addListener(newListener); }
	void addAsyncCoalescedScriptListener(AsyncListener* newListener) { scriptAsyncNotifier.addAsyncCoalescedListener(newListener); }
	void removeAsyncScriptListener(AsyncListener* listener) { scriptAsyncNotifier.removeListener(listener); }

	//Script functions
	static void logFromArgs(const var::NativeFunctionArgs& args, int logLevel);
	static var logFromScript(const var::NativeFunctionArgs& args);
	static var logWarningFromScript(const var::NativeFunctionArgs& args);
	static var logErrorFromScript(const var::NativeFunctionArgs& args);

	static var addTriggerFromScript(const var::NativeFunctionArgs& args);
	static var addBoolParameterFromScript(const var::NativeFunctionArgs& args);
	static var addIntParameterFromScript(const var::NativeFunctionArgs& args);
	static var addFloatParameterFromScript(const var::NativeFunctionArgs& args);
	static var addStringParameterFromScript(const var::NativeFunctionArgs& args);
	static var addEnumParameterFromScript(const var::NativeFunctionArgs& args);
	static var addTargetParameterFromScript(const var::NativeFunctionArgs& args);
	static var addColorParameterFromScript(const var::NativeFunctionArgs& args);
	static var addPoint2DParameterFromScript(const var::NativeFunctionArgs& args);
	static var addPoint3DParameterFromScript(const var::NativeFunctionArgs& args);
	static var addFileParameterFromScript(const var::NativeFunctionArgs& args);

	static var setUpdateRateFromScript(const var::NativeFunctionArgs& args);
	static var setExecutionTimeoutFromScript(const var::NativeFunctionArgs& args);

	static bool checkNumArgs(const String& logName, const var::NativeFunctionArgs& args, int expectedArgs);


};
