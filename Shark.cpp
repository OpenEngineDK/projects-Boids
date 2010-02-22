
#include "Shark.h"
#include <Logging/Logger.h>

Shark::Shark(ISceneNode* child, RandomGenerator* rg) : Fish(child,rg) {
    direction = Vector<3,float>(1,0,0);
    speed = 1.0f;
}

void Shark::SetSpeed(float s) {
    speed = s;
}

void Shark::SetDirection(float d, float r) {
    Vector<3,float> v(cos(r),tan(d),sin(r));    

    this->direction = v*speed;
}
