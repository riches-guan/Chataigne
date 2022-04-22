
#pragma once

class WarningReporter :
	public EngineListener
{
public:
	juce_DeclareSingleton(WarningReporter, true);

	Array<WeakReference<WarningTarget>> targets;

	WarningReporter();
	~WarningReporter();

	void clear();

	void registerWarning(WeakReference<WarningTarget>);
	void unregisterWarning(WeakReference<WarningTarget>);

	void endLoadFile() override;

	// ASYNC
	class  WarningReporterEvent
	{
	public:
		enum Type { WARNING_REGISTERED, WARNING_UNREGISTERED };

		WarningReporterEvent(Type t, WeakReference<WarningTarget> target) :
			type(t), target(target) {}

		Type type;
		WeakReference<WarningTarget> target;
	};

	QueuedNotifier<WarningReporterEvent> warningReporterNotifier;
	typedef QueuedNotifier<WarningReporterEvent>::Listener AsyncListener;

	void addAsyncWarningReporterListener(AsyncListener* newListener) { warningReporterNotifier.addListener(newListener); }
	void addAsyncCoalescedWarningReporterListener(AsyncListener* newListener) { warningReporterNotifier.addAsyncCoalescedListener(newListener); }
	void removeAsyncWarningReporterListener(AsyncListener* listener) { warningReporterNotifier.removeListener(listener); }

};
