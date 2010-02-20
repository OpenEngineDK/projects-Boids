#ifndef _BOIDS_FISH_MASTER_H_
#define _BOIDS_FISH_MASTER_H_

#include <Core/IModule.h>
#include <Scene/SceneNode.h>

#include "Fish.h"
#include "Shark.h"
#include "Scene/OceanFloorNode.h"

#include <vector>

/**
 * Short description.
 *
 * @class FishMaster FishMaster.h ts/Boids/FishMaster.h
 */

using namespace OpenEngine::Core;
using namespace OpenEngine::Scene;

class FishMaster : public IModule {
private:
    SceneNode *root;
    Shark* shark;
    std::vector<Fish*> fishes;
    RandomGenerator *rg;
    OceanFloorNode* ocean;
    Timer loopTimer;
    Timer reloadTimer;

    Vector<3,float> startPoint;
    Vector<3,float> endPoint;

    Vector<3,float> Rule1(Fish* f);
    Vector<3,float> Rule2(Fish* f);
    Vector<3,float> Rule3(Fish* f);

    Vector<3,float> BoxRule(Fish* f);
    Vector<3,float> HeightRule(Fish* f);
        

    void LimitSpeed(Fish* f);
    Vector<3,float> TendToPlace(Fish* f);

    // Boid properties
    float socialSphereRadius;
    float maxSpeed;
    float minSpeed;
    float followScalar;
    Vector<3, float> home;
    float homeScalar;
    float privacyRadius;
    float boxSpeed;
    float heightSpeed;
    float heightMin, heightMax;

    void InitProperties();

    float GetHeight(Vector<3,float>);
    Vector<3,float> GetNormal(Vector<3,float>);

public:
    FishMaster(OceanFloorNode* ocean, unsigned int n=10);

    ISceneNode* GetFishNode();
    Shark* GetShark();

    void Handle(InitializeEventArg arg);
    void Handle(ProcessEventArg arg);
    void Handle(DeinitializeEventArg arg);
};

#endif
