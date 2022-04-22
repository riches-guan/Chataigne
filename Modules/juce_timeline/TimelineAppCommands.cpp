#include "TimelineAppCommands.h"

namespace TimelineCommandIDs
{
	const int playPauseSequenceEditor = 0x80000;
	const int prevCue = 0x80001;
	const int nextCue = 0x80002;
	const int prevTimeStep = 0x80003;
	const int nextTimeStep = 0x80004;
	const int goToStart = 0x80005;
	const int goToEnd = 0x80006;
	const int addCueAtPosition = 0x80007;
	const int moveOneFrameAfter = 0x80008;
	const int moveOneFrameBefore = 0x80009;
}

void TimelineAppCommands::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{


	switch (commandID)
	{

	case TimelineCommandIDs::playPauseSequenceEditor:
		result.setInfo(L"播放/暂停 序列", "", L"时间轴", 0);//Play/Pause Sequence  //Timeline
		result.addDefaultKeypress(KeyPress::spaceKey, ModifierKeys::noModifiers);
		break;

	case TimelineCommandIDs::prevCue:
		result.setInfo(L"前一cue点", "", L"时间轴", 0);//Previous cue
		result.addDefaultKeypress(KeyPress::pageUpKey, ModifierKeys::shiftModifier);
		break;

	case TimelineCommandIDs::nextCue:
		result.setInfo(L"下一cue点", "", L"时间轴", 0);//Next cue
		result.addDefaultKeypress(KeyPress::pageDownKey, ModifierKeys::shiftModifier);
		break;

	case TimelineCommandIDs::prevTimeStep:
		result.setInfo(L"上一时间步", "", L"时间轴", 0);//Previous time step
		result.addDefaultKeypress(KeyPress::pageUpKey, ModifierKeys::noModifiers);
		break;

	case TimelineCommandIDs::nextTimeStep:
		result.setInfo(L"下一时间步", "", L"时间轴", 0);//Next time step
		result.addDefaultKeypress(KeyPress::pageDownKey, ModifierKeys::noModifiers);
		break;

	case TimelineCommandIDs::goToStart:
		result.setInfo(L"跳转到开始", "", L"时间轴", 0);//Go to start
		result.addDefaultKeypress(KeyPress::homeKey, ModifierKeys::noModifiers);
		break;

	case TimelineCommandIDs::goToEnd:
		result.setInfo(L"跳转到结尾", "", L"时间轴", 0);//Go to end
		result.addDefaultKeypress(KeyPress::endKey, ModifierKeys::noModifiers);
		break;

	case TimelineCommandIDs::addCueAtPosition:
		result.setInfo(L"在当前位置增加cue点", "", L"时间轴", 0);//Add Cue at position
		result.addDefaultKeypress(KeyPress::createFromDescription("b").getKeyCode(), ModifierKeys::ctrlModifier);
		break;

	case TimelineCommandIDs::moveOneFrameAfter:
		result.setInfo(L"移动后一帧", "", L"时间轴", 0);//Move one frame after
		result.addDefaultKeypress(KeyPress::rightKey, ModifierKeys::ctrlModifier);
		break;

	case TimelineCommandIDs::moveOneFrameBefore:
		result.setInfo(L"移动前一帧", "", L"时间轴", 0);//Move one frame before
		result.addDefaultKeypress(KeyPress::leftKey, ModifierKeys::ctrlModifier);
		break;
	}
}


void TimelineAppCommands::getAllCommands(Array<CommandID>& commands) {

	const CommandID ids[] = {

		TimelineCommandIDs::playPauseSequenceEditor,
		TimelineCommandIDs::prevCue,
		TimelineCommandIDs::nextCue,
		TimelineCommandIDs::prevTimeStep,
		TimelineCommandIDs::nextTimeStep,
		TimelineCommandIDs::goToStart,
		TimelineCommandIDs::goToEnd,
		TimelineCommandIDs::addCueAtPosition,
		TimelineCommandIDs::moveOneFrameAfter,
		TimelineCommandIDs::moveOneFrameBefore
	};

	commands.addArray(ids, numElementsInArray(ids));
}


void TimelineAppCommands::fillMenu(ApplicationCommandManager* commandManager, PopupMenu* menu, StringRef menuName)
{
	if (String(menuName) == L"时间轴")//Timeline
	{
		menu->addCommandItem(commandManager, TimelineCommandIDs::playPauseSequenceEditor);
		menu->addCommandItem(commandManager, TimelineCommandIDs::prevTimeStep);
		menu->addCommandItem(commandManager, TimelineCommandIDs::nextTimeStep);
		menu->addCommandItem(commandManager, TimelineCommandIDs::prevCue);
		menu->addCommandItem(commandManager, TimelineCommandIDs::nextCue);
		menu->addCommandItem(commandManager, TimelineCommandIDs::addCueAtPosition);
		menu->addSeparator();
		menu->addCommandItem(commandManager, TimelineCommandIDs::moveOneFrameAfter);
		menu->addCommandItem(commandManager, TimelineCommandIDs::moveOneFrameBefore);
	}
}

bool TimelineAppCommands::perform(const ApplicationCommandTarget::InvocationInfo& info)
{
	switch (info.commandID)
	{
	case TimelineCommandIDs::playPauseSequenceEditor:
	{
		Sequence* s = getCurrentEditingSequence();
		if (s != nullptr) s->togglePlayTrigger->trigger();
	}
	break;

	case TimelineCommandIDs::prevTimeStep:
	case TimelineCommandIDs::nextTimeStep:
	{
		Sequence* s = getCurrentEditingSequence();
		if (s != nullptr)
		{
			float step = 1.0f / s->fps->intValue();
			if (info.commandID == TimelineCommandIDs::prevTimeStep) step = -step;
			s->currentTime->setValue(s->currentTime->floatValue() + step);
		}
	}
	break;

	case TimelineCommandIDs::prevCue:
	{
		Sequence* s = getCurrentEditingSequence();
		if (s != nullptr) s->prevCue->trigger();

	}
	break;

	case TimelineCommandIDs::nextCue:
	{
		Sequence* s = getCurrentEditingSequence();
		if (s != nullptr) s->nextCue->trigger();
	}
	break;

	case TimelineCommandIDs::goToStart:
	{
		Sequence* s = getCurrentEditingSequence();
		if (s != nullptr) s->currentTime->setValue(0);
	}
	break;

	case TimelineCommandIDs::goToEnd:
	{
		Sequence* s = getCurrentEditingSequence();
		if (s != nullptr) s->currentTime->setValue(s->totalTime->floatValue());
	}
	break;

	case TimelineCommandIDs::addCueAtPosition:
	{
		Sequence* s = getCurrentEditingSequence();
		if (s != nullptr) s->cueManager->addCueAt(s->currentTime->floatValue());
	}
	break;

	case TimelineCommandIDs::moveOneFrameAfter:
	case TimelineCommandIDs::moveOneFrameBefore:
	{
		Sequence* s = getCurrentEditingSequence();
		if (s != nullptr)
		{
			BaseItem* i = InspectableSelectionManager::mainSelectionManager->getInspectableAs<BaseItem>();
			if (i != nullptr)
			{
				float step = 1.0f / s->fps->intValue();
				if (info.commandID == TimelineCommandIDs::moveOneFrameBefore) step = -step;
				i->setMovePositionReference(true);
				i->movePosition(Point<float>(step, 0), true);
			}
		}
	}
	break;

	}

	return true;
}

Sequence* TimelineAppCommands::getCurrentEditingSequence()
{
	ShapeShifterContent* sContent = ShapeShifterManager::getInstance()->getContentForType<TimeMachineView>();
	if (sContent != nullptr)
	{
		Component* c = sContent->contentComponent;
		TimeMachineView* tmw = dynamic_cast<TimeMachineView*>(c);
		if (tmw != nullptr)
		{
			SequenceEditorView* se = tmw->editor.get();
			if (se != nullptr) return se->sequence;
		}
	}

	return nullptr;
}

StringArray TimelineAppCommands::getMenuBarNames()
{
	return StringArray(L"时间轴");//Timeline
}
