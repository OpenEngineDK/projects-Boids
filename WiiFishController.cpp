
#include "WiiFishController.h"
#include <Logging/Logger.h>
#include <Devices/WiiMote.h>

WiiFishController::WiiFishController(FishMaster* fm, Camera* cam) 
    : fm(fm)
    , cam(cam)
    , speed(0)
    , jaw(0)
    , up(false) 
    , down(false) 
    , left(false) 
    , right(false) 
    , forward(false) 
    , back(false) 
{
    
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

    switch(arg.sym) {
    case KEY_UP:
        up = arg.type == EVENT_PRESS;

        break;
    case KEY_DOWN:
        down = arg.type == EVENT_PRESS;                

        break;
    case KEY_RIGHT:
        right = arg.type == EVENT_PRESS;                

        break;
    case KEY_LEFT:
        left = arg.type == EVENT_PRESS;                

        break;        
    case KEY_a:
        forward = arg.type == EVENT_PRESS;                

        break;
    case KEY_z:
        back = arg.type == EVENT_PRESS;                

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

    float dt = arg.approx / 10000.0;


    if (forward) speed += 10.0*dt;
    if (back) speed -= 10.0*dt;
    if (up) jaw += 0.01*dt;
    if (down) jaw -= 0.01*dt;
    if (left) direction -= 0.01*dt;
    if (right) direction += 0.01*dt;

        

    cam->Move(camMove);
    fm->GetShark()->SetSpeed(speed);
    fm->GetShark()->SetDirection(jaw,direction);
}  
