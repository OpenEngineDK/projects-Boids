
#include "Shark.h"
#include <Logging/Logger.h>

Shark::Shark(ISceneNode* child, Vector<3,float> start) 
  : Fish(child,start) {
    direction = Vector<3,float>(1,0,0);
    speed = 1.0f;
    jaw = 0;
    dir = 0;
    this->velocity = direction;
}

void Shark::SetSpeed(float s) {
    speed = s;
}

void Shark::AddDirection(float j, float r) {
    jaw += j;
    dir += r;
    
    //Vector<3,float> v(cos(r),j,sin(r));    

    //Quaternion<float> q(0,0,0);

    //this->velocity *= speed;
    //this->velocity = q.RotateVector(this->velocity*speed);


    SetDirection(jaw,dir);
}

void Shark::SetDirection(float j, float r) {
    Vector<3,float> v(cos(r),j,sin(r));    

    this->direction = v*speed;
    //this->velocity = v*speed;
}
