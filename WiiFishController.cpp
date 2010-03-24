
#include "WiiFishController.h"
#include <Logging/Logger.h>
#include <Display/PerspectiveViewingVolume.h>
#include <Display/TrackingCamera.h>
#include <Devices/WiiMote.h>


WiiFishController::WiiFishController(FishMaster* fm, Camera* cam, SimpleSetup* setup, PropertyTree& ptree, StereoCamera* sc) 
    : fm(fm)
    , cam(cam)
    , stereoCam(sc)
    , setup(setup)
    , ptree(ptree)
    , curCamIdx(-1)
    , speed(0)
    , direction(0)
    , jaw(0)
    , up(false) 
    , down(false) 
    , left(false) 
    , right(false) 
    , forward(false) 
    , back(false) 
{
    ptree.PropertiesChangedEvent().Attach(*this);
    ReloadConfig();
    SetKeyBindings();
}

void WiiFishController::LeftAction(KeyArg arg) {left = arg.type == EVENT_PRESS;}
void WiiFishController::RightAction(KeyArg arg) {right = arg.type == EVENT_PRESS;}
void WiiFishController::UpAction(KeyArg arg) {up = arg.type == EVENT_PRESS;}
void WiiFishController::DownAction(KeyArg arg) {down = arg.type == EVENT_PRESS;}
void WiiFishController::AccelerateAction(KeyArg arg) {forward = arg.type == EVENT_PRESS;}
void WiiFishController::DeaccelerateAction(KeyArg arg) {back = arg.type == EVENT_PRESS;}

void WiiFishController::SetKeyBindings() {
    // ActionCallBack<WiiFishController>* left
    //     = new ActionCallBack<WiiFishController>(this,&WiiFishController::LeftAction);
    

    km.onKeyChange(KEY_LEFT, *(new WiiActionCallback(this,&WiiFishController::LeftAction)));
    km.onKeyChange(KEY_RIGHT, *(new WiiActionCallback(this,&WiiFishController::RightAction)));
    km.onKeyChange(KEY_UP, *(new WiiActionCallback(this,&WiiFishController::UpAction)));
    km.onKeyChange(KEY_DOWN, *(new WiiActionCallback(this,&WiiFishController::DownAction)));
    km.onKeyChange(KEY_a, *(new WiiActionCallback(this,&WiiFishController::AccelerateAction)));
    km.onKeyChange(KEY_z, *(new WiiActionCallback(this,&WiiFishController::DeaccelerateAction)));

    
}

void WiiFishController::ReloadConfig() {        
    
    for(vector<Camera*>::iterator itr = cams.begin();
        itr != cams.end();
        itr++) {
        delete *itr;
    }
    
    cams.clear();
   

    PropertyTreeNode camNode = ptree.GetNode("cams");
    unsigned int size = camNode.GetSize();

    for (unsigned int i=0;i<size;i++) {
        PropertyTreeNode cam = camNode.GetNode(i);
        
        Vector<3,float> pos = cam.Get("pos",Vector<3,float>(0,0,0));
        Vector<3,float> lookat = cam.Get("lookat",Vector<3,float>(0,0,0));
        bool track = cam.Get("tracking",false);
                
        
        Camera* c;
        if (track) {
            TrackingCamera *fc = new TrackingCamera(*(new PerspectiveViewingVolume(10)));
            fc->Follow(fm->GetShark()->GetNode());
            c = fc;
        }
        else
            c = new Camera(*(new PerspectiveViewingVolume(10)));
        c->SetPosition(pos);
        c->LookAt(lookat);
        cams.push_back(c);
    }
    if (curCamIdx == -1)
        setup->SetCamera(*cam);
    else if (curCamIdx < int(cams.size()))
        setup->SetCamera(*(cams[curCamIdx]));
    else
        setup->SetCamera(*cam);
}

void WiiFishController::Handle(PropertiesChangedEventArg arg) {
    // Read camera configuration...
    ReloadConfig();
    logger.info << "Load configuration " << logger.end;
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
        forward = arg.type == EVENT_PRESS;
        break;
    case WII_REMOTE_ONE:
        back = arg.type == EVENT_PRESS;
        break;
    default:
        break;
    }
    
    camMove += (arg.type == EVENT_PRESS)?relVec*10:-relVec*10;    
}

void WiiFishController::Handle(KeyboardEventArg arg) {

    km.Handle(arg);

    switch(arg.sym) {
    default:
        break;
    }
    if (arg.type == EVENT_PRESS) 
        switch (arg.sym) {
        case KEY_r:
            fm->Reset();
            break;
            
        case KEY_1:
            curCamIdx = -1;
            setup->SetCamera(*cam);
            break;
        case KEY_2:
        case KEY_3:
        case KEY_4:
        case KEY_5:
        case KEY_6:
        case KEY_7:
        case KEY_8:
        case KEY_9:
            {
                unsigned int idx = arg.sym - KEY_2;
                if (idx < cams.size()) {
                    logger.info << "Set cam: " << idx << logger.end;
                    setup->SetCamera(*(cams[idx]));
                    curCamIdx = idx;
                }
                break;
            }
        case KEY_PLUS:
            if (stereoCam)
                stereoCam->dist += 1;
            break;
        case KEY_MINUS:
            if (stereoCam)
                stereoCam->dist -= 1;
            break;
            
        default:
            break;
    }
}

void WiiFishController::Handle(WiiAccelerationEventArg arg) {
    //cam->SetDirection(arg.acc,Vector<3,float>(0,1,0));

    //logger.info << arg.acc << logger.end;
    direction += -arg.acc[1]*0.02;
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

    setup->GetEngine().ProcessEvent().Attach(*mgr);

    mgr->WiiMoteFoundEvent().Attach(*this);
    mgr->LookForMote();
    logger.info << "Looking for wiimotes" << logger.end;
}
void WiiFishController::Handle(DeinitializeEventArg arg) {} 
void WiiFishController::Handle(ProcessEventArg arg) {

    float dt = arg.approx / 10000.0;

    //logger.info << dt << logger.end;

    if (forward) speed += 15.0*dt;
    if (back) speed -= 15.0*dt;
    if (up) jaw += 0.01*dt;
    if (down) jaw -= 0.01*dt;
    if (left) direction -= 0.02*dt;
    if (right) direction += 0.02*dt;

    if (jaw > 0.9) jaw = 0.9;
    if (jaw < -0.9) jaw = -0.9;
  

    cam->Move(camMove);
    fm->GetShark()->SetSpeed(speed);
    fm->GetShark()->SetDirection(jaw,direction);
    //fm->GetShark()->AddDirection(jaw,direction);
    //jaw = 0;
    //direction = 0;

}  
