
#include "WiiFishController.h"
#include <Logging/Logger.h>
#include <Devices/WiiMote.h>

WiiFishController::WiiFishController(FishMaster* fm, Camera* cam) : fm(fm), cam(cam) {

}

void WiiFishController::Handle(WiiButtonEventArg arg) {
    logger.info << "button" << logger.end;
    if (arg.type == EVENT_PRESS) {
        if (arg.button == WII_REMOTE_A)
            cam->Move(Vector<3,float>(0,10,0));
        else if (arg.button == WII_REMOTE_B)
            cam->Move(Vector<3,float>(0,-10,0));

    }
}
void WiiFishController::Handle(WiiMoteFoundEventArg arg) {
    WiiMote* mote = arg.wiimote;
    mote->WiiMoteButtonEvent().Attach(*this);
    logger.info << "Found a wiimote!" << logger.end;
}

void WiiFishController::Handle(InitializeEventArg arg) {
    WiiMoteManager* mgr = new WiiMoteManager();
    mgr->WiiMoteFoundEvent().Attach(*this);
    mgr->LookForMote();
    logger.info << "Looking for wiimotes" << logger.end;
}
void WiiFishController::Handle(DeinitializeEventArg arg) {} 
void WiiFishController::Handle(ProcessEventArg arg) {}  
