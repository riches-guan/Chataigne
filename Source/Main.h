
#pragma once
#pragma warning(disable:4244 4100 4305 26451 26495)

#include "MainIncludes.h"

//==============================================================================
class ChataigneApplication : public OrganicApplication
{
public:
	//==============================================================================
	ChataigneApplication();

	//---- GLOBAL SETTINGS CHATAIGNE SPECIFIC
	BoolParameter * enableSendAnalytics;
	//

	void initialiseInternal(const String& /*commandLine*/) override;
	void afterInit() override;

	void shutdown() override;

	void handleCrashed() override;
};

START_JUCE_APPLICATION(ChataigneApplication)