
#include "Fish.h"

#include <Logging/Logger.h>
#include <Scene/GeometryNode.h>

#include <Resources/ISoundResource.h>
#include <Resources/ResourceManager.h>
#include <Scene/SoundNode.h>
#include <Sound/IMonoSound.h>
#include <Sound/ISoundSystem.h>


extern ISoundSystem * soundsystem;


Fish::Fish(ISceneNode* child, Vector<3,float> start, ISoundResourcePtr fishres) {
    node = new TransformationNode();
    node->AddNode(child);
    childNode = child;
    velocity = Vector<3,float>(0,0,0);
    position = start;

    sound = (IMonoSound*)soundsystem->CreateSound(fishres);
    sound->SetGain(100.0);
    sound->SetMaxDistance(1000.0);
    SoundNode* soundNode = new SoundNode(sound);
    node->AddNode(soundNode);
}


TransformationNode* Fish::GetNode() {
    return node;
}

void Fish::SetChildNode(ISceneNode* n) {
    node->DeleteNode(childNode);
    childNode = n;
    node->AddNode(childNode);
}

void Fish::AddVelocity(Vector<3,float> v) {
    velocity += v;
}

void Fish::Update(Time dt) {
    float delta = dt.AsInt()/1000000.0;

    position += velocity*delta;
    node->SetPosition(position);
    if (velocity.GetLength() < 0.01)
        return;

    Vector<3,float> x = velocity.GetNormalize(); // x vector
    Vector<3,float> up(0,1,0); // up vector
    Vector<3,float> z = x % up;
    if (x.GetLength() < 0.01)
        return;
    Vector<3,float> y = z % x;

    Matrix<3,3,float> rotMat(x,y,z);
    Quaternion<float> newRot(rotMat);
    rotation = Quaternion<float>(rotation,newRot.GetNormalize(),0.1); // Rotate 10%
    node->SetRotation(rotation);

    prev = velocity;
}
