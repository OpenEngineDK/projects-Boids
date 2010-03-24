//
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS)
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------


#ifndef _OE_WII_FISH_CONTROLLER_H_
#define _OE_WII_FISH_CONTROLLER_H_

#include <Core/IListener.h>
#include <Core/IModule.h>


#include <Devices/WiiMote.h>
#include <Devices/WiiMoteManager.h>
#include <Devices/IKeyboard.h>
#include <Utils/SimpleSetup.h>



#include "FishMaster.h"
#include <Display/Camera.h>
#include <Renderers/OpenGL/StereoRenderer.h>

#include <Devices/KeyboardActionMapper.h>


using namespace OpenEngine::Core;
using namespace OpenEngine::Devices;
using namespace OpenEngine::Display;
using namespace OpenEngine::Utils;

/**
 * Short description.
 *
 * @class WiiFishController WiiFishController.h ts/Boids/WiiFishController.h
 */
class WiiFishController : public IModule
                        , public IListener<WiiButtonEventArg>
                        , public IListener<WiiMoteFoundEventArg>
                        , public IListener<WiiAccelerationEventArg>
                        , public IListener<KeyboardEventArg>
                        , public IListener<PropertiesChangedEventArg>
{
    struct KeyArg {    
        ButtonEvent type;
        KeyArg(ButtonEvent t) : type(t) {}
    };

    template <class T, class A>
    class ActionCallback : public IListener<KeyArg> {
        T* callee;
        void (T::*callback)(A);
    public:
        ActionCallback(T* who, void (T::*func)(A)) 
            : callee(who),callback(func) {}
        
        void Handle(A arg) {
            (callee->*callback)(arg);
        }
    };

    class WiiActionCallback : public ActionCallback<WiiFishController,KeyArg> {
    public:
        WiiActionCallback(WiiFishController* who, void (WiiFishController::*func)(KeyArg))
            : ActionCallback<WiiFishController,KeyArg>(who,func) {}
    };

    class KM : public KeyboardActionMapper<KeyArg> {
        KeyArg toAction(KeyboardEventArg arg) {return KeyArg(arg.type); }
    };

private:    
    FishMaster* fm;
    Camera* cam;
    StereoCamera* stereoCam;
    SimpleSetup* setup;
    PropertyTree& ptree;    
    
    vector<Camera*> cams;
    int curCamIdx;

    KM km;

    Vector<3,float> camMove;
    float speed;
    float direction,jaw;

    bool up,down,left,right,forward,back;

    void ReloadConfig();

public:    
    
    WiiFishController(FishMaster* fm, Camera* cam, SimpleSetup* setup, PropertyTree& ptree, StereoCamera* sc = NULL);

    void SetKeyBindings();
    
    void LeftAction(KeyArg);
    void RightAction(KeyArg);
    void UpAction(KeyArg);
    void DownAction(KeyArg);
    void AccelerateAction(KeyArg);
    void DeaccelerateAction(KeyArg);

    void Handle(WiiButtonEventArg arg);
    void Handle(WiiMoteFoundEventArg arg);
    void Handle(WiiAccelerationEventArg arg);

    void Handle(KeyboardEventArg arg);
    void Handle(PropertiesChangedEventArg arg);

    void Handle(InitializeEventArg arg);
    void Handle(DeinitializeEventArg arg);
    void Handle(ProcessEventArg arg);

};

#endif // _OE_WII_FISH_CONTROLLER_H_
