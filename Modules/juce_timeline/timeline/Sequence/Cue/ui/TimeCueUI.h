
class TimeCueUI :
	public BaseItemMinimalUI<TimeCue>,
	public SettableTooltipClient,
	public Label::Listener
{
public:
	TimeCueUI(TimeCue *);
	~TimeCueUI();

	Path drawPath;
	float timeAtMouseDown;
	Label itemLabel;
	
	const int arrowSize = 10;
	
	void paint(Graphics &g) override;
	void resized() override;

	void mouseDoubleClick(const MouseEvent &e) override;
	void mouseDown(const MouseEvent &e) override;
	void mouseDrag(const MouseEvent &e) override;
	void mouseUp(const MouseEvent &e) override;
	virtual void labelTextChanged(Label * l) override;

	void controllableFeedbackUpdateInternal(Controllable *) override;
	void containerChildAddressChangedAsync(ControllableContainer *) override;

	class TimeCueUIListener
	{
	public:
		virtual ~TimeCueUIListener() {}
		virtual void cueTimeChanged(TimeCueUI *) {}
		virtual void cueMouseDown(TimeCueUI*, const MouseEvent&) {}
		virtual void cueDragged(TimeCueUI *, const MouseEvent &) {}
	};

	ListenerList<TimeCueUIListener> cueUIListeners;
	void addCueUIListener(TimeCueUIListener* newListener) { cueUIListeners.add(newListener); }
	void removeCueUIListener(TimeCueUIListener* listener) { cueUIListeners.remove(listener); }

};

