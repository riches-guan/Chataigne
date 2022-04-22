/*
  ==============================================================================

    ScriptCommand.h
    Created: 1 Aug 2018 5:47:34pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class ScriptCommand :
	public BaseCommand
{
public:
	ScriptCommand(Module * module, CommandContext context, var data, Multiplex* multiplex = nullptr);
	~ScriptCommand();

	OwnedArray<ControllableContainer> paramContainers;
	Identifier callback;
	Array<Parameter *> scriptParams; 

	class Dependency
	{
	public:
		enum CheckType { CHECK_NOT_SET, EQUALS, NOT_EQUALS, MAX_TYPES};
		const Array<String> checkTypeNames{ "notset", "equals","notEquals" };

		enum DepAction { ACTION_NOT_SET, SHOW, ENABLE, MAX_ACTIONS };
		const Array<String> actionNames{ "notset", "show", "enable" };

		Dependency(Parameter * source, Parameter * target, var value, CheckType checkType, DepAction depAction);
		Dependency(Parameter * source, Parameter * target, var value, StringRef typeName, StringRef actionName);
		
		Parameter * source;
		WeakReference<Parameter> target;
		var value;
		CheckType type;
		DepAction action;

		bool process(); //returns true if something changed
	};

	OwnedArray<Dependency> dependencies;
	HashMap<Parameter *, Dependency *> dependencyMap;

	void processDependencies(Parameter * p);

	void onContainerParameterChanged(Parameter * p) override;
	void onControllableFeedbackUpdate(ControllableContainer * cc, Controllable *c) override;

	void createControllablesForContainer (var data, ControllableContainer * cc);

	void triggerInternal(int multiplexIndex) override;

	static BaseCommand * create(ControllableContainer * module, CommandContext context, var params, Multiplex * multiplex)  { return new ScriptCommand((Module *)module, context, params, multiplex); }

	//InspectableEditor * getEditor(bool isRoot) override;
};
