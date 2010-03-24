#ifndef _BOIDS_FISH_H_
#define _BOIDS_FISH_H_

#include <Scene/TransformationNode.h>
#include <Math/RandomGenerator.h>
#include <Geometry/FaceSet.h>
#include <Utils/Timer.h>
#include <Sound/IMonoSound.h>

using namespace OpenEngine::Scene;
using namespace OpenEngine::Geometry;
using namespace OpenEngine::Math;
using namespace OpenEngine::Utils;
using namespace OpenEngine::Sound;

/**
 * Short description.
 *
 * @class Fish Fish.h ts/Boids/Fish.h
 */
class Fish {
private:
    
public:
    Vector<3,float> velocity,prev;
    Vector<3,float> position;
    Quaternion<float> rotation;

    list<Fish*> neighbors;

    TransformationNode *node;
    ISceneNode* childNode;

    IMonoSound* sound;

    Fish(ISceneNode* child, Vector<3,float> start, ISoundResourcePtr fishres);

    TransformationNode* GetNode();
    void SetChildNode(ISceneNode* child);
    
    void AddVelocity(Vector<3,float> v);
    void Update(Time dt);
};

#endif
