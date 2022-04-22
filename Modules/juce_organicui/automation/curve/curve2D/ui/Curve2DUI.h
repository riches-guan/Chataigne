/*
  ==============================================================================

    Curve2DUI.h
    Created: 21 Mar 2020 4:06:30pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class Curve2DUI :
    public BaseManagerViewUI<Curve2D, Curve2DKey, Curve2DKeyUI>,
    public Curve2DKey::AsyncListener,
    public Curve2DKeyUI::KeyUIListener,
    public ContainerAsyncListener,
    public AutomationRecorder::AsyncListener
{
public:
    Curve2DUI(Curve2D * manager);
    ~Curve2DUI();

    bool paintingMode;
    Array<Point<float>> paintingPoints;

    void paintOverChildren(Graphics& g) override;

    void updateViewUIPosition(Curve2DKeyUI * ui) override;
    void updateHandlesForUI(Curve2DKeyUI* ui, bool checkSideItems);

    void addItemUIInternal(Curve2DKeyUI* ui) override;
    void removeItemUIInternal(Curve2DKeyUI* ui) override;

    void mouseDown(const MouseEvent& e) override;
    void mouseDrag(const MouseEvent& e) override;
    void mouseUp(const MouseEvent& e) override;

    void mouseDoubleClick(const MouseEvent& e) override;

    Component* getSelectableComponentForItemUI(Curve2DKeyUI* ui) override;

    void newMessage(const Curve2DKey::Curve2DKeyEvent& e) override;
    void newMessage(const ContainerAsyncEvent& e) override;
    void newMessage(const AutomationRecorder::RecorderEvent& e) override;

    void keyEasingHandleMoved(Curve2DKeyUI* ui, bool syncOtherHandle, bool isFirst) override;
};