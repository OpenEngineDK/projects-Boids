#ifndef _BOIDS_FISH_H_
#define _BOIDS_FISH_H_

#include <Scene/TransformationNode.h>
#include <Math/RandomGenerator.h>
#include <Geometry/FaceSet.h>

using namespace OpenEngine::Scene;
using namespace OpenEngine::Geometry;
using namespace OpenEngine::Math;

/**
 * Short description.
 *
 * @class Fish Fish.h ts/Boids/Fish.h
 */
class Fish {
private:
public:
    TransformationNode *node;

    Fish(FaceSet *fs, RandomGenerator* rg);

    ISceneNode* GetNode();
};

#endif
