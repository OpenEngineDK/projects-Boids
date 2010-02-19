
#include "Shark.h"
#include <Logging/Logger.h>

Shark::Shark(ISceneNode* child, RandomGenerator* rg) : Fish(child,rg) {
    direction = Vector<3,float>(1,0,0);
}

void Shark::SetSpeed(float s) {
    this->velocity = direction * s;
}

void Shark::SetDirection(float d, float r) {
    Quaternion<float> q(0,r,d);
    this->velocity = q.RotateVector(this->velocity);

}
