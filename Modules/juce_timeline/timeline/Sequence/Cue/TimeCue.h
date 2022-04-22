
class TimeCue :
	public BaseItem
{
public:
	TimeCue(const float &_time = 0, TimeCueManager * manager = nullptr);
	~TimeCue();

	FloatParameter * time;

	enum CueAction { NOTHING, PAUSE, LOOP_JUMP };
	EnumParameter * cueAction;
	TargetParameter* loopCue;

	void onContainerParameterChangedInternal(Parameter* p) override;

	Sequence * getSequence();
};
