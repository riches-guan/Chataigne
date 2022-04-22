/*
  ==============================================================================

	EnumParameter.cpp
	Created: 29 Sep 2016 5:34:59pm
	Author:  bkupe

  ==============================================================================
*/


EnumParameter::EnumParameter(const String& niceName, const String& description, bool enabled) :
	Parameter(Type::ENUM, niceName, description, "", var(), var(), enabled),
	enumParameterNotifier(5)
{
	lockManualControlMode = true;

	scriptObject.setMethod("getKey", EnumParameter::getValueKeyFromScript);
	scriptObject.setMethod("setData", EnumParameter::setValueWithDataFromScript);
	scriptObject.setMethod("addOption", EnumParameter::addOptionFromScript);
	scriptObject.setMethod("removeOptions", EnumParameter::removeOptionsFromScript);

	value = "";
}

EnumParameter::~EnumParameter()
{
	if (enumParameterNotifier.isUpdatePending())
	{
		MessageManagerLock mmLock(Thread::getCurrentThread());
		if (mmLock.lockWasGained()) enumParameterNotifier.handleUpdateNowIfNeeded();
		enumParameterNotifier.cancelPendingUpdate();
	}
}

EnumParameter* EnumParameter::addOption(String key, var data, bool selectIfFirstOption)
{
	enumValues.add(new EnumValue(key, data));
	if (enumValues.size() == 1 && selectIfFirstOption)
	{
		defaultValue = key;
		setValue(key, true, false, false);
	}

	enumListeners.call(&Listener::enumOptionAdded, this, key);
	enumParameterNotifier.addMessage(new EnumParameterEvent(EnumParameterEvent::ENUM_OPTION_ADDED, this));
	updateArgDescription();
	return this;
}

void EnumParameter::updateOption(int index, String key, var data, bool addIfNotThere)
{
	if (index >= enumValues.size())
	{
		if (!addIfNotThere) return;
		while(index > enumValues.size()) addOption("#"+String(enumValues.size()), var());
		addOption(key, data);
		return;
	}

	String k = enumValues[index]->key;
	if (k == key && enumValues[index]->value == data) return;

	enumValues.set(index, new EnumValue(key, data));
	enumListeners.call(&Listener::enumOptionUpdated, this, index, k, key);
	enumParameterNotifier.addMessage(new EnumParameterEvent(EnumParameterEvent::ENUM_OPTION_UPDATED, this));
	updateArgDescription();
}

void EnumParameter::removeOption(String key)
{
	enumValues.remove(getIndexForKey(key));
	enumListeners.call(&Listener::enumOptionRemoved, this, key);
	enumParameterNotifier.addMessage(new EnumParameterEvent(EnumParameterEvent::ENUM_OPTION_REMOVED, this));
	updateArgDescription();

	if (getValueKey() == key) setValue("");
}

void EnumParameter::setOptions(Array<EnumValue*> options)
{
	for (int i = 0; i < options.size(); i++)
	{
		updateOption(i, options[i]->key, options[i]->value, true);
	}

	while (enumValues.size() > options.size()) removeOption(enumValues[enumValues.size() - 1]->key);
}

void EnumParameter::clearOptions()
{
	StringArray keysToRemove;
	for (auto& ev : enumValues) keysToRemove.add(ev->key);
	for (auto& k : keysToRemove) removeOption(k);
}

void EnumParameter::updateArgDescription()
{
	argumentsDescription = "";
	for (int i = 0; i < enumValues.size(); ++i)
	{
		argumentsDescription += enumValues[i]->key;
		if (i < enumValues.size() - 1) argumentsDescription += " | ";
	}
}

var EnumParameter::getValue()
{
	return getValueData();
}

var EnumParameter::getValueData() {
	EnumValue* ev = getEntryForKey(value.toString());
	if (ev == nullptr) return var();
	return ev->value;
}

String EnumParameter::getValueKey() {
	return value.toString();
}

int EnumParameter::getIndexForKey(StringRef key)
{
	int numValues = enumValues.size();
	for (int i = 0; i < numValues; ++i) if (enumValues[i]->key == key) return i;
	return -1;
}

EnumParameter::EnumValue* EnumParameter::getEntryForKey(StringRef key)
{
	int index = getIndexForKey(key);
	if (index == -1) return nullptr;
	return enumValues[index];
}

StringArray EnumParameter::getAllKeys()
{
	StringArray result;
	for (auto& ev : enumValues) result.add(ev->key);
	return result;
}

void EnumParameter::setValueWithData(var data)
{
	for (auto& ev : enumValues)
	{
		if (ev->value == data)
		{
			setValueWithKey(ev->key);
			break;
		}
	}
}

void EnumParameter::setValueWithKey(String key)
{
	setValue(key);
}

void EnumParameter::setPrev(bool loop, bool addToUndo)
{
	int targetIndex = getIndexForKey(value.toString()) - 1;
	if (targetIndex < 0)
	{
		if (loop) targetIndex = enumValues.size() - 1;
		else return;
	}

	String newValue = enumValues[targetIndex]->key;

	if (addToUndo) setUndoableValue(value, newValue);
	else setValueWithKey(newValue);
}

void EnumParameter::setNext(bool loop, bool addToUndo)
{

	int targetIndex = getIndexForKey(value.toString()) + 1;
	if (targetIndex >= enumValues.size())
	{
		if (loop) targetIndex = 0;
		else return;
	}

	String newValue = enumValues[targetIndex]->key;

	if (addToUndo) setUndoableValue(value, newValue);
	else setValueWithKey(newValue);
}

bool EnumParameter::checkValueIsTheSame(var oldValue, var newValue)
{
	return oldValue.toString() == newValue.toString();
}

var EnumParameter::getJSONDataInternal()
{
	var data = Parameter::getJSONDataInternal();
	if (!saveValueOnly)
	{
		var optData(new DynamicObject());
		for (auto& e : enumValues) optData.getDynamicObject()->setProperty(e->key, e->value);
		data.getDynamicObject()->setProperty("enumOptions", optData);
	}
	return data;
}

void EnumParameter::loadJSONDataInternal(var data)
{
	if (!saveValueOnly)
	{
		var optData = data.getProperty("enumOptions", var());
		if (optData.isObject())
		{
			NamedValueSet props = optData.getDynamicObject()->getProperties();
			for (auto& p : props)
			{
				addOption(p.name.toString(), p.value, false);
			}
		}
	}
	Parameter::loadJSONDataInternal(data);
}

void EnumParameter::setupFromJSONData(var data)
{
	Parameter::setupFromJSONData(data);
	var optionsData = data.getProperty("options", var());
	if (optionsData.isObject())
	{
		NamedValueSet options = optionsData.getDynamicObject()->getProperties();
		for (auto& o : options)
		{
			addOption(o.name.toString(), o.value);
		}
	}
	else
	{
		LOGWARNING("Options property is not valid : " << optionsData.toString());
	}
}

var EnumParameter::getValueKeyFromScript(const juce::var::NativeFunctionArgs& a)
{
	WeakReference<Parameter> c = getObjectFromJS<Parameter>(a);
	if (c == nullptr || c.wasObjectDeleted()) return var();
	EnumParameter* ep = dynamic_cast<EnumParameter*>(c.get());
	return ep->getValueKey();
}

var EnumParameter::addOptionFromScript(const juce::var::NativeFunctionArgs& a)
{
	WeakReference<Parameter> c = getObjectFromJS<Parameter>(a);
	if (c == nullptr || c.wasObjectDeleted()) return var();
	EnumParameter* ep = dynamic_cast<EnumParameter*>(c.get());

	if (a.numArguments < 2)
	{
		NLOGWARNING("Script", "EnumParameter addOption should at least have 2 arguments");
		return var();
	}

	ep->addOption(a.arguments[0].toString(), a.arguments[1]);

	return var();
}

var EnumParameter::removeOptionsFromScript(const juce::var::NativeFunctionArgs& a)
{
	WeakReference<Parameter> c = getObjectFromJS<Parameter>(a);
	if (c == nullptr || c.wasObjectDeleted()) return var();
	EnumParameter* ep = dynamic_cast<EnumParameter*>(c.get());
	ep->clearOptions();
	return var();
}

var EnumParameter::setValueWithDataFromScript(const juce::var::NativeFunctionArgs& a)
{
	WeakReference<Parameter> c = getObjectFromJS<Parameter>(a);
	if (c == nullptr || c.wasObjectDeleted()) return var();
	EnumParameter* ep = dynamic_cast<EnumParameter*>(c.get());

	if (a.numArguments < 1)
	{
		NLOGWARNING("Script", "EnumParameter setData requires 1 argument !");
		return var();
	}

	ep->setValueWithData(a.arguments[0]);

	return var();
}

EnumParameterUI* EnumParameter::createUI(Array<EnumParameter*> parameters)
{
	if (parameters.size() == 0) parameters = { this };
	return new EnumParameterUI(parameters);
}

EnumParameterButtonBarUI* EnumParameter::createButtonBarUI(Array<EnumParameter*> parameters)
{
	if (parameters.size() == 0) parameters = { this };
	return new EnumParameterButtonBarUI(parameters);
}

ControllableUI* EnumParameter::createDefaultUI(Array<Controllable*> controllables) {
	return createUI(getArrayAs<Controllable, EnumParameter>(controllables));
}
