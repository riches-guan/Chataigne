DashboardControllableItemUI::DashboardControllableItemUI(DashboardControllableItem* controllableItem) :
	DashboardInspectableItemUI(controllableItem),
	controllableItem(controllableItem),
	itemUI(nullptr)
{
	controllableItem->addAsyncDashboardControllableItemListener(this);
	setSize(item->viewUISize->x, item->viewUISize->y);
}

DashboardControllableItemUI::~DashboardControllableItemUI()
{
	if (!inspectable.wasObjectDeleted()) controllableItem->removeAsyncDashboardControllableItemListener(this);
}

void DashboardControllableItemUI::paint(Graphics& g)
{
	DashboardInspectableItemUI::paint(g);
}

void DashboardControllableItemUI::resizedDashboardItemInternal()
{
	if (itemUI != nullptr) itemUI->setBounds(getLocalBounds());
}

ControllableUI* DashboardControllableItemUI::createControllableUI()
{
	return controllableItem->controllable->createDefaultUI();
}

void DashboardControllableItemUI::rebuildUI()
{
	if (itemUI != nullptr) removeChildComponent(itemUI.get());

	if (!inspectable.wasObjectDeleted()
		&& controllableItem->inspectable != nullptr
		&& !controllableItem->inspectable.wasObjectDeleted()) itemUI.reset(createControllableUI());
	else itemUI.reset();

	updateUIParameters();
	updateEditMode();

	if (itemUI != nullptr)
	{
		addAndMakeVisible(itemUI.get());
		if (getWidth() == 0 || getHeight() == 0) setSize(itemUI->getWidth(), itemUI->getHeight());
	}
}

void DashboardControllableItemUI::updateUIParameters()
{
	if (inspectable.wasObjectDeleted() || controllableItem->inspectable == nullptr || controllableItem->inspectable.wasObjectDeleted()) return;

	itemUI->showLabel = controllableItem->showLabel->boolValue();
	itemUI->useCustomTextColor = controllableItem->textColor->enabled;
	itemUI->customTextColor = controllableItem->textColor->getColor();

	itemUI->useCustomContour = controllableItem->contourColor->enabled;
	itemUI->customContourColor = controllableItem->contourColor->getColor();
	itemUI->customContourThickness = controllableItem->contourThickness->floatValue();
	itemUI->forceFeedbackOnly = controllableItem->forceReadOnly->boolValue();

	String customLabel = controllableItem->customLabel->stringValue();
	if (customLabel.isNotEmpty() && controllableItem->customLabel->enabled) itemUI->customLabel = customLabel;
	else itemUI->customLabel = "";

	itemUI->customDescription = controllableItem->customDescription->enabled ? controllableItem->customDescription->stringValue() : "";

	itemUI->setOpaqueBackground(controllableItem->opaqueBackground->boolValue());
	itemUI->updateTooltip();

	updateUIParametersInternal();
	itemUI->updateUIParams();
	updateEditMode();

	itemUI->repaint();
}


void DashboardControllableItemUI::updateEditModeInternal(bool editMode)
{
	if (itemUI != nullptr)
	{
		setTooltip((editMode && !controllableItem->controllable.wasObjectDeleted())  ? "Target : " + controllableItem->controllable->getControlAddress() : "");
		itemUI->setInterceptsMouseClicks(!editMode, !editMode);
	}
	else setTooltip("### " + controllableItem->inspectableGhostAddress);
}

void DashboardControllableItemUI::inspectableChanged()
{
	DashboardInspectableItemUI::inspectableChanged();
	rebuildUI();
}

void DashboardControllableItemUI::controllableFeedbackUpdateInternal(Controllable* c)
{
	DashboardInspectableItemUI::controllableFeedbackUpdateInternal(c);

	if (c == controllableItem->showLabel
		|| c == controllableItem->textColor
		|| c == controllableItem->customLabel
		|| c == controllableItem->customDescription
		|| c == controllableItem->opaqueBackground
		|| c == controllableItem->contourColor
		|| c == controllableItem->contourThickness
		|| c == controllableItem->forceReadOnly
		|| c == controllableItem->textSize
		)
	{
		updateUIParameters();
	}
}

void DashboardControllableItemUI::controllableStateUpdateInternal(Controllable* c)
{
	DashboardInspectableItemUI::controllableStateUpdateInternal(c);

	if (c == controllableItem->showLabel
		|| c == controllableItem->textColor
		|| c == controllableItem->customLabel
		|| c == controllableItem->customDescription
		|| c == controllableItem->opaqueBackground
		|| c == controllableItem->contourColor
		|| c == controllableItem->contourThickness
		|| c == controllableItem->textSize
		)
	{
		updateUIParameters();
	}
}

void DashboardControllableItemUI::newMessage(const DashboardControllableItem::DashboardControllableItemEvent& e)
{
	if (e.type == e.NEEDS_UI_UPDATE)
	{
		updateUIParameters();
	}
}
