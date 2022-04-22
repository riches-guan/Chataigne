/*
  ==============================================================================

    HTTPCommand.cpp
    Created: 13 Feb 2019 11:28:44am
    Author:  bkupe

  ==============================================================================
*/

HTTPCommand::HTTPCommand(HTTPModule * _module, CommandContext context, var params, Multiplex* multiplex) :
	BaseCommand(_module, context, params, multiplex),
	contentType((ContentType)(int)params.getProperty("contentType", URLENCODED)),
    httpModule(_module),
    payload(nullptr)
{
	method = addEnumParameter("Method", "Request Method");
	for (int i = 0; i < HTTPModule::TYPE_MAX; ++i) method->addOption(HTTPModule::requestMethodNames[i], (HTTPModule::RequestMethod)i);

	resultDataType = addEnumParameter("Result Type", "The type of data to parse the received data. Default will let the module decide, depending on its Protocol parameter");
	resultDataType->addOption("Default", HTTPModule::DEFAULT)->addOption("Raw", HTTPModule::RAW)->addOption("JSON", HTTPModule::JSON)->addOption("XML", HTTPModule::XML);

	address = addStringParameter("Address", "Address to append to the module's base address", "anything");

	setUseCustomValues(true);

	extraHeaders = addStringParameter("Extra Headers", "HTTP headers to add to the request", "");
	extraHeaders->multiline = true;

	if (contentType == PLAIN)
	{
		extraHeaders->setValue("Content-Type: text/plain");
		payload = addStringParameter("Payload", "This is content that you can put in the request body.", "");
		payload->multiline = true;
	}
}

HTTPCommand::~HTTPCommand()
{
}


void HTTPCommand::triggerInternal(int multiplexIndex)
{
	StringPairArray requestParams;
	for (auto &p : customValuesManager->items) requestParams.set(p->niceName, p->getLinkedValue(multiplexIndex));
	
	StringPairArray headers;
	
	httpModule->sendRequest(
		getLinkedValue(address, multiplexIndex).toString(),
		method->getValueDataAsEnum<HTTPModule::RequestMethod>(),
		resultDataType->getValueDataAsEnum<HTTPModule::ResultDataType>(),
		requestParams,
		getLinkedValue(extraHeaders, multiplexIndex).toString(),
		contentType == PLAIN ? getLinkedValue(payload, multiplexIndex).toString():""
		);
}

var HTTPCommand::getJSONData()
{
	var data = BaseCommand::getJSONData();
	var customValuesData = customValuesManager->getJSONData();
	if (!customValuesData.isVoid()) data.getDynamicObject()->setProperty("argManager", customValuesData);
	return data;
}

void HTTPCommand::loadJSONDataInternal(var data)
{
	BaseCommand::loadJSONDataInternal(data);
	customValuesManager->loadJSONData(data.getProperty("argManager", var()), true);
}
