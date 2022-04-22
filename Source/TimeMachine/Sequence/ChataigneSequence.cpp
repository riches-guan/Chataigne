﻿
ChataigneSequence::ChataigneSequence() :
	Sequence(),
	masterAudioModule(nullptr),
	masterAudioLayer(nullptr),
	ltcAudioModule(nullptr)
{
	midiSyncDevice = new MIDIDeviceParameter(L"Sync设备");//Sync Devices
	midiSyncDevice->canBeDisabledByUser = true;
	midiSyncDevice->enabled = false;
	addParameter(midiSyncDevice);

	ltcModuleTarget = addTargetParameter(L"LTC Sync模块", L"选择要用作此序列LTC Sync的音频模块。", ModuleManager::getInstance(), false);//Module  //Choose an Audio Module to use as LTC Sync for this sequence
	ltcModuleTarget->canBeDisabledByUser = true;
	ltcModuleTarget->targetType = TargetParameter::CONTAINER;
	ltcModuleTarget->maxDefaultSearchLevel = 0;

	syncOffset = addFloatParameter(L"Sync偏移", L"发送和接收时的偏移时间。", 0, 0);//Offset  //The time to offset when sending and receiving
	syncOffset->defaultUI = FloatParameter::TIME;
	reverseOffset = addBoolParameter(L"反向偏移", L"这里接受负偏移。", false);//Reverse Offset  //This allows negative offset
	resetTimeOnMTCStopped = addBoolParameter(L"在MTC停止时重置", L"如果选中，当MTC不再发送数据时，序列将停止并重置时间。如果未选中，序列将保持其当前时间。", false);
	//Reset on MTC Stop
	//If checked, sequence will stop and reset time when MTC doesn't send data anymore. If not checked, sequence will just keep its current time



	std::function<bool(ControllableContainer*)> typeCheckFunc = [](ControllableContainer* cc) { return dynamic_cast<AudioModule*>(cc) != nullptr; };
	ltcModuleTarget->defaultContainerTypeCheckFunc = typeCheckFunc;

	layerManager->factory.defs.add(SequenceLayerManager::LayerDefinition::createDef("", "Trigger", &ChataigneTriggerLayer::create, this));
	layerManager->factory.defs.add(SequenceLayerManager::LayerDefinition::createDef("", Mapping1DLayer::getTypeStringStatic(), &Mapping1DLayer::create, this));
	layerManager->factory.defs.add(SequenceLayerManager::LayerDefinition::createDef("", Mapping2DLayer::getTypeStringStatic(), &Mapping2DLayer::create, this));
	layerManager->factory.defs.add(SequenceLayerManager::LayerDefinition::createDef("", "Audio", &ChataigneAudioLayer::create, this, true));
	layerManager->factory.defs.add(SequenceLayerManager::LayerDefinition::createDef("", ColorMappingLayer::getTypeStringStatic(), &ColorMappingLayer::create, this));
	layerManager->factory.defs.add(SequenceLayerManager::LayerDefinition::createDef("", "Sequences", &SequenceBlockLayer::create, this)->addParam("manager", ChataigneSequenceManager::getInstance()->getControlAddress()));

	layerManager->addBaseManagerListener(this);

	ChataigneSequenceManager::getInstance()->snapKeysToFrames->addParameterListener(this);
}

ChataigneSequence::~ChataigneSequence()
{
	if (ChataigneSequenceManager::getInstanceWithoutCreating())
	{
		ChataigneSequenceManager::getInstance()->snapKeysToFrames->removeParameterListener(this);
	}
	clearItem();
}

void ChataigneSequence::clearItem()
{
	BaseItem::clearItem();

	setMasterAudioLayer(nullptr);
	setLTCAudioModule(nullptr);
	Sequence::clearItem();
}

void ChataigneSequence::setMasterAudioModule(AudioModule* module)
{
	if (masterAudioModule == module) return;

	if (masterAudioModule != nullptr)
	{
		masterAudioModule->enabled->removeParameterListener(this);
	}

	masterAudioModule = module;

	if (masterAudioModule != nullptr)
	{
		masterAudioModule->enabled->addParameterListener(this);
		setAudioDeviceManager(&masterAudioModule->am);
	}
	else
	{
		setAudioDeviceManager(nullptr);
	}


	sequenceListeners.call(&SequenceListener::sequenceMasterAudioModuleChanged, this);
}

void ChataigneSequence::updateTargetAudioLayer(ChataigneAudioLayer* excludeLayer)
{
	if (masterAudioLayer == nullptr)
	{
		for (auto& i : layerManager->items)
		{
			if (i == excludeLayer) continue;
			ChataigneAudioLayer* a = dynamic_cast<ChataigneAudioLayer*>(i);
			if (a != nullptr && a->audioModule != nullptr)
			{
				setMasterAudioLayer(a);
				return;
			}
		}

		setMasterAudioLayer(nullptr);
	}
	else
	{
		if (masterAudioLayer->audioModule == nullptr)
		{
			//DBG("master is not null but has no audioModule, setting master to null and searching");
			masterAudioLayer = nullptr;
			updateTargetAudioLayer();
		}
		else
		{
			//DBG("master has changed its module");
			setMasterAudioLayer(masterAudioLayer); //force refresh
		}
	}
}

void ChataigneSequence::setMasterAudioLayer(ChataigneAudioLayer* layer)
{
	masterAudioLayer = layer;
	setMasterAudioModule(masterAudioLayer != nullptr ? masterAudioLayer->audioModule : nullptr);
}

void ChataigneSequence::targetAudioModuleChanged(ChataigneAudioLayer* layer)
{
	updateTargetAudioLayer();
}

void ChataigneSequence::itemAdded(SequenceLayer* layer)
{
	ChataigneAudioLayer* audioLayer = dynamic_cast<ChataigneAudioLayer*>(layer);
	if (audioLayer != nullptr)
	{
		audioLayer->addAudioLayerListener(this);

		//if already an audio module, assign it
		if (!isCurrentlyLoadingData)
		{
			for (auto& i : ModuleManager::getInstance()->items)
			{
				AudioModule* a = dynamic_cast<AudioModule*>(i);
				if (a != nullptr)
				{
					audioLayer->setAudioModule(a);
					break;
				}
			}

			if (audioLayer->audioModule == nullptr)
			{
				AlertWindow::showAsync(
					MessageBoxOptions().withIconType(AlertWindow::WarningIcon)
					.withTitle(L"需要声卡模块")
					//Sound Card Module is required
					.withMessage(L"这个音频层需要一个声卡模块才能实际输出声音。你想现在创建一个吗？")
					//This Audio layer needs a Sound Card module to be able to actually output sound. Do you want to create one now ?
					.withButton(L"是")
					.withButton(L"否"),
					[this, audioLayer](int result)
					{
						if (result == 1)
						{
							AudioModule* m = AudioModule::create();
							ModuleManager::getInstance()->addItem(m);
							audioLayer->setAudioModule(m);
						}

						if (audioLayer->audioModule != nullptr) updateTargetAudioLayer();
					}
				);
			}
		}
		else
		{
			if (audioLayer->audioModule != nullptr) updateTargetAudioLayer();
		}
	}
}

void ChataigneSequence::itemRemoved(SequenceLayer* layer)
{
	ChataigneAudioLayer* a = dynamic_cast<ChataigneAudioLayer*>(layer);
	if (a != nullptr)
	{
		a->removeAudioLayerListener(this);
		if (masterAudioLayer == a)
		{
			masterAudioLayer = nullptr;
			updateTargetAudioLayer(a);
		}
	}
}


bool ChataigneSequence::timeIsDrivenByAudio()
{
	return Sequence::timeIsDrivenByAudio() && masterAudioModule != nullptr && masterAudioModule->enabled->boolValue();
}

void ChataigneSequence::addNewMappingLayerFromValues(Array<Point<float>> keys)
{
	Mapping1DLayer* layer = (Mapping1DLayer*)layerManager->addItem(layerManager->factory.create("Mapping"));
	layer->automation->addFromPointsAndSimplifyBezier(keys, false);
}

void ChataigneSequence::updateLayersSnapKeys()
{
	bool snap = ChataigneSequenceManager::getInstance()->snapKeysToFrames->boolValue();

	Array<AutomationMappingLayer*> aLayers = layerManager->getItemsWithType<AutomationMappingLayer>();
	for (auto& l : aLayers)
	{
		if (l->automation != nullptr) l->automation->setUnitSteps(snap ? fps->intValue() : 0);
	}
}

void ChataigneSequence::setupMidiSyncDevices()
{
	//	if ((mtcSender != nullptr && midiSyncDevice->outputDevice != mtcSender->device) || midiSyncDevice->outputDevice != nullptr)
	//	{
	if (midiSyncDevice->outputDevice == nullptr || !midiSyncDevice->enabled) mtcSender.reset();
	else
	{
		mtcSender.reset(new MTCSender(midiSyncDevice->outputDevice));
		mtcSender->setSpeedFactor(playSpeed->floatValue());
	}
	//	}

	//	if ((mtcReceiver != nullptr && midiSyncDevice->inputDevice != mtcReceiver->device) || midiSyncDevice->inputDevice != nullptr)
	//	{
	if (midiSyncDevice->inputDevice == nullptr || !midiSyncDevice->enabled) mtcReceiver.reset();
	else
	{
		mtcReceiver.reset(new MTCReceiver(midiSyncDevice->inputDevice));
		mtcReceiver->addMTCListener(this);
	}
	//	}
}

void ChataigneSequence::setLTCAudioModule(AudioModule* am)
{
	if (ltcAudioModule == am) return;
	if (ltcAudioModule != nullptr)
	{
		ltcAudioModule->ltcTime->removeParameterListener(this);
		ltcAudioModule->ltcPlaying->removeParameterListener(this);
	}

	ltcAudioModule = am;

	if (ltcAudioModule != nullptr)
	{
		ltcAudioModule->ltcTime->addParameterListener(this);
		ltcAudioModule->ltcPlaying->addParameterListener(this);
	}
}

void ChataigneSequence::onContainerParameterChangedInternal(Parameter* p)
{
	Sequence::onContainerParameterChangedInternal(p);

	if (p == fps)
	{
		updateLayersSnapKeys();
	}
	else if (mtcSender != nullptr && midiSyncDevice->enabled)
	{
		float time = jmax<float>(0, currentTime->floatValue() - (syncOffset->floatValue() * (reverseOffset->boolValue() ? -1 : 1)));

		if (p == currentTime)
		{
			if ((!isPlaying->boolValue() || isSeeking)) mtcSender->setPosition(time, true);
		}
		else if (p == playSpeed) mtcSender->setSpeedFactor(playSpeed->floatValue());
		else if (p == isPlaying)
		{
			if (isPlaying->boolValue()) mtcSender->start(time);
			else mtcSender->pause(false);
		}
		else if (p == syncOffset)
		{
			mtcSender->setPosition(time, true);
		}
	}

	if (p == midiSyncDevice)
	{
		setupMidiSyncDevices();
	}

	if (p == ltcModuleTarget)
	{
		if (ltcModuleTarget->enabled) setLTCAudioModule((AudioModule*)ltcModuleTarget->targetContainer.get());
		else setLTCAudioModule(nullptr);
	}
}

void ChataigneSequence::onControllableStateChanged(Controllable* c)
{
	Sequence::onControllableStateChanged(c);
	if (c == midiSyncDevice)
	{
		setupMidiSyncDevices();
		if (mtcSender != nullptr && midiSyncDevice->enabled && isPlaying->boolValue())
		{
			float time = jmax<float>(0, currentTime->floatValue() - (syncOffset->floatValue() * (reverseOffset->boolValue() ? -1 : 1)));
			mtcSender->start(time);
		}
	}
	else if (c == ltcModuleTarget)
	{
		if (ltcModuleTarget->enabled) setLTCAudioModule((AudioModule*)ltcModuleTarget->targetContainer.get());
		else setLTCAudioModule(nullptr);
	}
}

void ChataigneSequence::onContainerTriggerTriggered(Trigger* t)
{
	Sequence::onContainerTriggerTriggered(t);
	if (t == stopTrigger)
	{
		if (mtcSender != nullptr) mtcSender->stop();
	}
}

void ChataigneSequence::onExternalParameterValueChanged(Parameter* p)
{
	if (masterAudioModule != nullptr && p == masterAudioModule->enabled)
	{
		sequenceListeners.call(&SequenceListener::sequenceMasterAudioModuleChanged, this);
	}
	else if (p == ChataigneSequenceManager::getInstance()->snapKeysToFrames)
	{
		updateLayersSnapKeys();
	}
	else if (ltcAudioModule != nullptr)
	{
		if (p == ltcAudioModule->ltcPlaying)
		{
			if (ltcAudioModule->ltcPlaying->boolValue())
			{
				double time = ltcAudioModule->ltcTime->floatValue() + (syncOffset->floatValue() * (reverseOffset->boolValue() ? -1 : 1));
				if (time >= 0 && time < totalTime->floatValue()) playTrigger->trigger();
			}
			else
			{
				pauseTrigger->trigger();
			}
		}
		else if (p == ltcAudioModule->ltcTime)
		{
			double time = ltcAudioModule->ltcTime->floatValue() + (syncOffset->floatValue() * (reverseOffset->boolValue() ? -1 : 1));
			double diff = fabs(currentTime->floatValue() - time);
			bool isJump = diff > 1;
			bool seekMode = isJump || !ltcAudioModule->ltcPlaying->boolValue();
			if (!isPlaying->boolValue() && time >= 0 && time < totalTime->floatValue()) playTrigger->trigger();
			setCurrentTime(time, isJump, seekMode);
		}
	}
}

void ChataigneSequence::mtcStarted()
{
	double time = mtcReceiver->getTime() + (syncOffset->floatValue() * (reverseOffset->boolValue() ? -1 : 1));
	if (time >= 0 && time < totalTime->floatValue()) playTrigger->trigger();
}

void ChataigneSequence::mtcStopped()
{
	if (resetTimeOnMTCStopped->boolValue()) stopTrigger->trigger();
	else pauseTrigger->trigger();
}

void ChataigneSequence::mtcTimeUpdated(bool isFullFrame)
{
	if (mtcReceiver == nullptr) return;

	double time = mtcReceiver->getTime() + (syncOffset->floatValue() * (reverseOffset->boolValue() ? -1 : 1));
	double diff = fabs(currentTime->floatValue() - time);
	bool isJump = diff > 1;
	bool seekMode = isJump || !mtcReceiver->isPlaying;
	if (!isPlaying->boolValue() && time >= 0 && time < totalTime->floatValue()) playTrigger->trigger();
	setCurrentTime(time, isJump, seekMode);
}
