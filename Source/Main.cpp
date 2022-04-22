#include "Main.h"

#if JUCE_MAC //for chmod
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include "Module/ModuleIncludes.h"

//==============================================================================

ChataigneApplication::ChataigneApplication() :
	OrganicApplication("ICCControl", true, ImageCache::getFromMemory(BinaryData::tray_icon_png, BinaryData::tray_icon_pngSize))
{
	enableSendAnalytics = appSettings.addBoolParameter(L"发送分析", L"这有助于我通过发送基本的启动/停止/崩溃信息来改进软件", true);//Send Analytics  //This helps me improve the software by sending basic start/stop/crash infos
}


void ChataigneApplication::initialiseInternal(const String &)
{
	engine.reset(new ChataigneEngine());
	if(useWindow) mainComponent.reset(new MainContentComponent());

	//Call after engine init
	AppUpdater::getInstance()->setURLs("", "", "ICCControl");
	//http://benjamin.kuperberg.fr/chataigne/releases/update.json
	//http://benjamin.kuperberg.fr/chataigne/user/data/
	HelpBox::getInstance()->helpURL = URL("");
	//http://benjamin.kuperberg.fr/chataigne/help/

	CrashDumpUploader::getInstance()->init("",ImageCache::getFromMemory(BinaryData::crash_png, BinaryData::crash_pngSize));
	//http://benjamin.kuperberg.fr/chataigne/support/crash_report.php

	//DashboardManager::getInstance()->setupDownloadURL("http://benjamin.kuperberg.fr/download/dashboard/dashboard.php?folder=dashboard");

	ShapeShifterManager::getInstance()->setDefaultFileData(BinaryData::default_chalayout);
	ShapeShifterManager::getInstance()->setLayoutInformations("chalayout", "ICCControl/layouts");

}


void ChataigneApplication::afterInit()
{
	//ANALYTICS
	if (!launchedFromCrash && enableSendAnalytics->boolValue())
	{
		StringPairArray options;
		options.set("new_visit", "1");
		MatomoAnalytics::getInstance()->log(MatomoAnalytics::START, options);
	}

	DashboardManager::getInstance()->setupDownloadURL("http://benjamin.kuperberg.fr/download/dashboard/dashboard.php?folder=dashboard");

	if (mainWindow != nullptr)
	{
		mainWindow->setMenuBarComponent(new ChataigneMenuBarComponent((MainContentComponent*)mainComponent.get(), (ChataigneEngine*)engine.get()));
	}

}

void ChataigneApplication::shutdown()
{   
	for (auto& m : ModuleManager::getInstance()->getItemsWithType<OSModule>())
	{
		m->terminateTrigger->trigger();
	}

	OrganicApplication::shutdown();

	if (enableSendAnalytics->boolValue())
	{
		MatomoAnalytics::getInstance()->log(MatomoAnalytics::STOP);
	}

	if(MatomoAnalytics::getInstanceWithoutCreating() != nullptr) MatomoAnalytics::deleteInstance();
	AppUpdater::deleteInstance();
}

void ChataigneApplication::handleCrashed()
{
	for (auto& m : ModuleManager::getInstance()->getItemsWithType<OSModule>())
	{
		m->crashedTrigger->trigger();
	}

	if (enableSendAnalytics->boolValue())
	{
		MatomoAnalytics::getInstance()->log(MatomoAnalytics::CRASH);
		while (MatomoAnalytics::getInstance()->isThreadRunning())
		{
			//wait until thread is done
		}
	}

	OrganicApplication::handleCrashed();
}
