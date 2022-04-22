
Mapping2DLayer::Mapping2DLayer(Sequence* s, var params) :
	AutomationMappingLayer(getTypeString(), s, params)
{
	helpID = "Mapping2DLayer";

	addChildControllableContainer(&curve);
	
	curve.setControlMode(Curve2D::AUTOMATION);
	curve.recorder = &recorder;
	recorder.input->typesFilter.add(Point2DParameter::getTypeStringStatic());

	setupAutomation((Automation*)curve.position->automation->automationContainer);
	((ParameterNumberAutomation*)curve.position->automation.get())->length->isSavable = false;
	((ParameterNumberAutomation*)curve.position->automation.get())->setLength(automation->length->floatValue());
	setupMappingInputParameter(curve.value);


	uiHeight->setValue(200);
}

Mapping2DLayer::~Mapping2DLayer()
{
}

void Mapping2DLayer::addDefaultContent()
{
	automation->addKey(0, 0, false);
	automation->addKey(sequence->totalTime->floatValue(), 1, false);
}


var Mapping2DLayer::getValueAtPosition(float position)
{
	Point<float> p = curve.getValueAtNormalizedPosition((float)automation->getNormalizedValueAtPosition(position));
	var result;
	result.append(p.x);
	result.append(p.y);
	return result;
}

void Mapping2DLayer::stopRecorderAndAddKeys()
{

	Array<AutomationRecorder::RecordValue> recordedValues = recorder.stopRecordingAndGetKeys();

	if (recordedValues.size() == 0) return;
	if (!recordedValues[0].value.isArray()) return;

	Array<Point<float>> points;
	Array<float> times;

	Array<Point<float>> positions;
	float totalLength = 0;
	Point<float> prevP(recordedValues[0].value[0], recordedValues[0].value[1]);

	for (auto& rv : recordedValues)
	{
		times.add(rv.time);
		Point<float> p(rv.value[0], rv.value[1]);
		totalLength += p.getDistanceFrom(prevP);
		
		positions.add(Point<float>(rv.time, totalLength));
		points.add(p);
		prevP.setXY(p.x, p.y);
	}

	if (totalLength == 0) return;
	
	for (auto& p : positions)
	{
		p.y /= totalLength;
	}
	curve.addFromPointsAndSimplify(points, true, times);
	automation->addFromPointsAndSimplifyBezier(positions);
}

SequenceLayerPanel* Mapping2DLayer::getPanel()
{
	return new Mapping2DLayerPanel(this);
}

SequenceLayerTimeline* Mapping2DLayer::getTimelineUI()
{
	return new Mapping2DTimeline(this);
}
