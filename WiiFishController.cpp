
#include "WiiFishController.h"
#include <Logging/Logger.h>
#include <Devices/WiiMote.h>

WiiFishController::WiiFishController(FishMaster* fm, Camera* cam) 
  : fm(fm), cam(cam),speed(0),jaw(0) {
    
}

void WiiFishController::Handle(WiiButtonEventArg arg) {
    logger.info << "button " << arg.button << logger.end;
    Vector<3,float> relVec;
    switch (arg.button) {
    case WII_REMOTE_UP:        
        relVec = Vector<3,float>(0,0,-1);
        break;
    case WII_REMOTE_DOWN:
        relVec = Vector<3,float>(0,0,1);
        break;
    case WII_REMOTE_LEFT:
        relVec = Vector<3,float>(-1,0,0);
        break;
    case WII_REMOTE_RIGHT:
        relVec = Vector<3,float>(1,0,0);
        break;
    case WII_REMOTE_TWO:
        if (arg.type == EVENT_PRESS)
            speed += 10.0;
        break;
    case WII_REMOTE_ONE:
        if (arg.type == EVENT_PRESS)
            speed -= 10.0;
        break;
    default:
        break;
    }
    
    camMove += (arg.type == EVENT_PRESS)?relVec*10:-relVec*10;    
}

void WiiFishController::Handle(KeyboardEventArg arg) {
    if (arg.type == EVENT_RELEASE)
        return;

    switch(arg.sym) {
    case KEY_UP:
        jaw -= 0.1;
        break;
    case KEY_DOWN:
        jaw += 0.1;
        break;
    case KEY_RIGHT:
        direction -= 0.1;
        break;
    case KEY_LEFT:
        direction += 0.1;
        break;        
    case KEY_a:
        speed += 10.0;
        break;
    default:
        logger.info << arg.sym << logger.end;
        break;
    }
}

void WiiFishController::Handle(WiiAccelerationEventArg arg) {
    //cam->SetDirection(arg.acc,Vector<3,float>(0,1,0));

    //logger.info << arg.acc << logger.end;
    direction += arg.acc[1]*0.01;
    jaw = arg.acc[2];
    

    Vector<3,float> v(arg.acc[0],arg.acc[2],arg.acc[1]);

    //fm->GetShark()->velocity = v*100.0;
}

void WiiFishController::Handle(WiiMoteFoundEventArg arg) {
    WiiMote* mote = arg.wiimote;
    mote->WiiMoteButtonEvent().Attach(*this);
    mote->WiiMoteAccelerationEvent().Attach(*this);


    logger.info << "Found a wiimote!" << logger.end;
}

void WiiFishController::Handle(InitializeEventArg arg) {
    WiiMoteManager* mgr = new WiiMoteManager();
    mgr->WiiMoteFoundEvent().Attach(*this);
    mgr->LookForMote();
    logger.info << "Looking for wiimotes" << logger.end;
}
void WiiFishController::Handle(DeinitializeEventArg arg) {} 
void WiiFishController::Handle(ProcessEventArg arg) {
    cam->Move(camMove);
    fm->GetShark()->SetSpeed(speed);
    fm->GetShark()->SetDirection(jaw,direction);
}  
