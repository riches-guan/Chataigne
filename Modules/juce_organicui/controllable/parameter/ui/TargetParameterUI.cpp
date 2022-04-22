/*
  ==============================================================================

	TargetParameterUI.cpp
	Created: 2 Nov 2016 5:00:10pm
	Author:  bkupe

  ==============================================================================
*/

TargetParameterUI::TargetParameterUI(Array<TargetParameter*> parameters, const String& _noTargetText) :
	ParameterUI(Inspectable::getArrayAs<TargetParameter, Parameter>(parameters)),
	noTargetText(_noTargetText),
	targetParameters(parameters),
	targetParameter(parameters[0]),
	useCustomShowFullAddressInEditor(false),
	customShowFullAddressInEditor(false),
	useCustomShowParentNameInEditor(false),
	customShowParentNameInEditor(false),
	customParentLabelSearch(-1),
	useCustomShowLearnButton(false),
	customShowLearnButton(false)
{
	setInterceptsMouseClicks(true, true);
	showEditWindowOnDoubleClick = false;

	targetBT.reset(AssetManager::getInstance()->getTargetBT());
	targetBT->setInterceptsMouseClicks(false, false);

	if (targetParameter->customCheckAssignOnNextChangeFunc != nullptr)
	{
		listeningToNextChange.reset(new BoolParameter("Learn", "When this parameter is on, any parameter that changes will be auto assigned to this target", false));
		listeningToNextChange->addAsyncParameterListener(this);
		listeningToNextChangeBT.reset(listeningToNextChange->createToggle());
		addAndMakeVisible(listeningToNextChangeBT.get());
	}


	addAndMakeVisible(targetBT.get());

	targetBT->addListener(this);
	setRepaintsOnMouseActivity(true);

	label.setFont(label.getFont().withHeight(14));
	updateLabel();
	label.setInterceptsMouseClicks(false, false);
	addAndMakeVisible(label);
}


TargetParameterUI::~TargetParameterUI()
{
	//in case we deleted with the listener still on
	if (parameter != nullptr && !parameter.wasObjectDeleted() && targetParameter->rootContainer != nullptr) targetParameter->rootContainer->removeAsyncContainerListener(this);
}

void TargetParameterUI::paint(Graphics& g)
{
	if (parameter.wasObjectDeleted()) return;

	Colour c = targetParameter->target != nullptr || targetParameter->targetContainer != nullptr ? (isInteractable() ? GREEN_COLOR : BLUE_COLOR) : NORMAL_COLOR;
	if (isMouseOver() && isInteractable()) c = c.brighter();

	g.setGradientFill(ColourGradient(c.brighter(.2f), (float)getLocalBounds().getCentreX(), (float)getLocalBounds().getCentreY(), c.darker(), 2.f, 2.f, true));
	g.fillRoundedRectangle(targetBT->getBounds().expanded(2).toFloat(), 6);

}

void TargetParameterUI::resized()
{
	juce::Rectangle<int> r = getLocalBounds();
	if (listeningToNextChangeBT != nullptr && listeningToNextChangeBT->isVisible())
	{
		listeningToNextChangeBT->setBounds(r.removeFromRight(50));
		r.removeFromRight(2);
	}

	targetBT->setBounds(r.removeFromLeft(r.getHeight()).reduced(3));
	r.removeFromLeft(2);
	label.setBounds(r.reduced(0, 2));
}

void TargetParameterUI::updateLabel()
{

	bool bShowFullAddress = useCustomShowFullAddressInEditor ? customShowFullAddressInEditor : targetParameter->showFullAddressInEditor;
	bool bShowParentName = useCustomShowParentNameInEditor ? customShowParentNameInEditor : targetParameter->showParentNameInEditor;
	int defaultLevel = customParentLabelSearch >= 0 ? customParentLabelSearch : targetParameter->defaultParentLabelLevel;

	bool forceNoCustomFunc = useCustomShowFullAddressInEditor || useCustomShowParentNameInEditor || (customParentLabelSearch >= 0);

	String newText;
	if (targetParameter->targetType == TargetParameter::TargetType::CONTROLLABLE)
	{
		if (targetParameter->target != nullptr)
		{
			if (!forceNoCustomFunc && targetParameter->customGetControllableLabelFunc != nullptr) newText = targetParameter->customGetControllableLabelFunc(targetParameter->target);
			else
			{
				if (bShowFullAddress) targetParameter->target->getControlAddress();
				else
				{
					if (bShowParentName)
					{
						int curPLevel = 0;
						ControllableContainer* cc = targetParameter->target->parentContainer;
						while (cc != nullptr && (curPLevel <= defaultLevel || cc->skipLabelInTarget))
						{
							if (curPLevel > 0 || cc->skipLabelInTarget) cc = cc->parentContainer;
							if (cc == nullptr || cc == Engine::mainEngine || cc == targetParameter->rootContainer) break;
							if (cc->skipLabelInTarget) continue;

							newText = cc->niceName + " > " + newText;
							curPLevel++;
						}
					}

					newText += targetParameter->target->niceName;
				}
			}
		}
	}
	else //TargetType::CONTAINER
	{
		if (targetParameter->targetContainer != nullptr)
		{
			if (!forceNoCustomFunc && targetParameter->customGetContainerLabelFunc != nullptr) newText = targetParameter->customGetContainerLabelFunc(targetParameter->targetContainer);
			else
			{
				if (bShowFullAddress) targetParameter->target->getControlAddress();
				else
				{
					if (bShowParentName)
					{
						int curPLevel = 0;

						ControllableContainer* cc = targetParameter->targetContainer->parentContainer;

						while (cc != nullptr && (curPLevel < defaultLevel || cc->skipLabelInTarget))
						{
							if (curPLevel > 0 || cc->skipLabelInTarget) cc = cc->parentContainer;
							if (cc == nullptr || cc == Engine::mainEngine) break;
							if (cc->skipLabelInTarget) continue;

							newText = cc->niceName + " > " + newText;
							curPLevel++;
						}
					}

					newText += targetParameter->targetContainer->niceName;
				}
			}
		}
	}

	if (newText.isEmpty())
	{
		String ghostName = "";
		if (targetParameter->value.toString().isNotEmpty()) ghostName = targetParameter->value.toString();
		else if (targetParameter->ghostValue.isNotEmpty()) ghostName = targetParameter->ghostValue;

		if (ghostName.isNotEmpty()) newText = "### " + ghostName;
		else newText = noTargetText;
	}

	label.setText(newText, dontSendNotification);
}

void TargetParameterUI::updateUIParamsInternal()
{
	if (customTextSize > 0) label.setFont(customTextSize);
	else label.setFont(label.getFont().withHeight(14));
	if (useCustomShowLearnButton)
	{
		if (listeningToNextChangeBT != nullptr) listeningToNextChangeBT->setVisible(customShowLearnButton);
	}
	else
	{
		if (listeningToNextChangeBT != nullptr) listeningToNextChangeBT->setVisible(true);
	}

	updateLabel();
}

void TargetParameterUI::showPopupAndGetTarget()
{
	if (!parameter->enabled) return;

	if (targetParameter->targetType == TargetParameter::TargetType::CONTROLLABLE)
	{
		if (targetParameter->customGetTargetFunc != nullptr)
		{
			targetParameter->customGetTargetFunc(targetParameter->typesFilter, targetParameter->excludeTypesFilter, [this](Controllable* c)
				{
					if (c == nullptr) return;
					if (shouldBailOut()) return; 
					targetParameter->setValueFromTarget(c);
				});
		}
		else
		{
			controllableChooser.reset(new ControllableChooserPopupMenu(targetParameter->rootContainer, 0, targetParameter->maxDefaultSearchLevel, targetParameter->typesFilter, targetParameter->excludeTypesFilter, targetParameter->customTargetFilterFunc));
			controllableChooser->showAndGetControllable([this](Controllable* c)
				{
					if (c == nullptr) return;
					if (shouldBailOut()) return;
					targetParameter->setValueFromTarget(c);
				});
		}

	}
	else
	{
		if (targetParameter->customGetTargetContainerFunc != nullptr)
		{
			targetParameter->customGetTargetContainerFunc([this](ControllableContainer* cc)
				{
					if (cc == nullptr) return;
					if (shouldBailOut()) return; 
					targetParameter->setValueFromTarget(cc);
				});
		}
		else
		{
			containerChooser.reset(new ContainerChooserPopupMenu(targetParameter->rootContainer, 0, targetParameter->maxDefaultSearchLevel, targetParameter->defaultContainerTypeCheckFunc, targetParameter->typesFilter, targetParameter->excludeTypesFilter, targetParameter->maxDefaultSearchLevel == -1));
			containerChooser->showAndGetContainer([this](ControllableContainer* cc)
				{
					if (cc == nullptr) return;
					if (shouldBailOut()) return;
					targetParameter->setValueFromTarget(cc);
				}
			);
		}
	}
}

void TargetParameterUI::mouseDownInternal(const MouseEvent&)
{
	if (isInteractable()) showPopupAndGetTarget();
}

void TargetParameterUI::buttonClicked(Button* b)
{
	if (b == targetBT.get()) {} // move code here ?
}

void TargetParameterUI::valueChanged(const var&)
{
	if (listeningToNextChange != nullptr) listeningToNextChange->setValue(false);

	updateLabel();
	repaint();
}

void TargetParameterUI::newMessage(const Parameter::ParameterEvent& e)
{

	if (e.parameter == listeningToNextChange.get())
	{
		if (e.type == Parameter::ParameterEvent::VALUE_CHANGED)
		{
			{
				if (listeningToNextChange->boolValue())
				{
					targetParameter->rootContainer->addAsyncContainerListener(this);
				}
				else
				{
					targetParameter->rootContainer->removeAsyncContainerListener(this);
				}
			}
		}
	}
	else
	{
		ParameterUI::newMessage(e);
	}

}

void TargetParameterUI::newMessage(const ContainerAsyncEvent& e)
{
	if (e.type == ContainerAsyncEvent::ControllableFeedbackUpdate)
	{
		if (e.targetControllable.wasObjectDeleted()) return;
		if (Controllable* c = e.targetControllable)
		{
			if (c == targetParameter->target) return;

			if (targetParameter->excludeTypesFilter.contains(c->getTypeString())) return;
			if (!targetParameter->typesFilter.isEmpty() && !targetParameter->typesFilter.contains(c->getTypeString())) return;

			bool isControllableValid = targetParameter->customCheckAssignOnNextChangeFunc(c);
			if (isControllableValid)
			{
				listeningToNextChange->setValue(false); //do it before value is change so there is no double value possible
				targetParameter->setValueFromTarget(c);
			}
		}
	}

}
