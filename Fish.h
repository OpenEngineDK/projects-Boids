#ifndef _BOIDS_FISH_H_
#define _BOIDS_FISH_H_

#include <Scene/TransformationNode.h>
#include <Math/RandomGenerator.h>
#include <Geometry/FaceSet.h>
#include <Utils/Timer.h>

using namespace OpenEngine::Scene;
using namespace OpenEngine::Geometry;
using namespace OpenEngine::Math;
using namespace OpenEngine::Utils;

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

    TransformationNode *node;
    ISceneNode* childNode;

    Fish(ISceneNode* child, Vector<3,float> start);

    TransformationNode* GetNode();
    void SetChildNode(ISceneNode* child);

    
    void AddVelocity(Vector<3,float> v);
    void Update(Time dt);
};

#endif
