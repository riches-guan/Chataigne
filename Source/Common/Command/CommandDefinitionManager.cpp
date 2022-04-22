/*
  ==============================================================================

    CommandDefinitionManager.cpp
    Created: 19 Dec 2016 3:48:25pm
    Author:  Ben

  ==============================================================================
*/

CommandDefinitionManager::CommandDefinitionManager()
{

}

CommandDefinitionManager::~CommandDefinitionManager()
{
}

void CommandDefinitionManager::add(CommandDefinition * def, int index)
{
	if(index == -1) definitions.add(def);
	else definitions.insert(index, def);
}

void CommandDefinitionManager::remove(CommandDefinition * def)
{
	definitions.removeObject(def);
}

void CommandDefinitionManager::clear()
{
	definitions.clear();
}

PopupMenu CommandDefinitionManager::getCommandMenu(int baseID, CommandContext context)
{
	PopupMenu p;
	addCommandsToMenu(&p, baseID, context);
	return p;
}

void CommandDefinitionManager::addCommandsToMenu(PopupMenu * commandMenu, int baseID, CommandContext context)
{
	OwnedArray<PopupMenu> subMenus;
	Array<String> subMenuNames;

	for (auto &d : definitions)
	{
		if (context != CommandContext::BOTH && d->context != CommandContext::BOTH && context != d->context) continue;

		int itemID = definitions.indexOf(d) + 1 + baseID; //start at baseID + 1 for menu

		if (d->menuPath.isEmpty())
		{
			commandMenu->addItem(itemID, d->commandType);
			continue;
		}

		int subMenuIndex = -1;
		for (int i = 0; i < subMenus.size(); ++i)
		{
			if (subMenuNames[i] == d->menuPath)
			{
				subMenuIndex = i;
				break;
			}
		}
		if (subMenuIndex == -1)
		{
			subMenuNames.add(d->menuPath);
			subMenus.add(new PopupMenu());
			subMenuIndex = subMenus.size() - 1;
		}


		subMenus[subMenuIndex]->addItem(itemID, d->commandType);
	}

	for (int i = 0; i < subMenus.size(); ++i) commandMenu->addSubMenu(subMenuNames[i], *subMenus[i]);
}

CommandDefinition * CommandDefinitionManager::getCommandDefinitionFor(const String & menuPath, const String & moduleType)
{
	for (auto &d : definitions) if (d->menuPath == menuPath && d->commandType == moduleType) return d;
	return nullptr;
}

