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
    class KeyArg {

    };

    template <class T>
    class ActionCallBack : public IListener<KeyArg> {
        T* callee;
        void (T::*callback)(void);
    public:
        ActionCallBack(T* who, void (T::*func)(void)) 
            : callee(who),callback(func)
        {}
        
        void Handle(KeyArg arg) {
            (callee->*callback)();
        }
    };

    class KM : public KeyboardActionMapper<KeyArg> {
        KeyArg toAction(KeyboardEventArg arg) {return KeyArg(); }
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
    void LeftAction();

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
