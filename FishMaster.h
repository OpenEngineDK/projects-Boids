#ifndef _BOIDS_FISH_MASTER_H_
#define _BOIDS_FISH_MASTER_H_

#include <Core/IModule.h>
#include <Scene/SceneNode.h>

#include "Fish.h"
#include "Shark.h"
#include "Scene/OceanFloorNode.h"
#include <Utils/PropertyTree.h>

#include <vector>

/**
 * Short description.
 *
 * @class FishMaster FishMaster.h ts/Boids/FishMaster.h
 */

using namespace OpenEngine::Core;
using namespace OpenEngine::Scene;

class FishMaster
    : public IModule
    , public IListener<PropertiesChangedEventArg> {
private:
    PropertyTree& ptree;
    
    time_t last_ts;

    SceneNode *root;
    Shark* shark;
    std::vector<Fish*> fishes;
    RandomGenerator *rg;
    OceanFloorNode* ocean;
    Timer loopTimer;

    Vector<3,float> startPoint;
    Vector<3,float> endPoint;

    Vector<3,float> Rule1(Fish* f);
    Vector<3,float> Rule2(Fish* f);
    Vector<3,float> Rule3(Fish* f);

    Vector<3,float> BoxRule(Fish* f);
    Vector<3,float> HeightRule(Fish* f);
    Vector<3,float> TopRule(Fish* f);
    

    void BoxLimit(Fish *f);
    void LimitSpeed(Fish* f);

    Vector<3,float> TendToPlace(Fish* f);

    Vector<3,float> HeadForDirection(Fish* f, Vector<3,float> d);
    Vector<3,float> Flee(Fish* f, Vector<3,float> p);
    Vector<3,float> Randomize(Fish *f);

    // Boid properties

    bool rule1Enabled,rule2Enabled,rule3Enabled;
    float massFactor;
    float privacyRadius;
    float followScalar;

    bool homeEnabled;
    Vector<3, float> home;
    float homeScalar;    

    bool speedEnabled;
    float maxSpeed;
    float minSpeed;

    bool heightEnabled,topEnabled;
    float heightMin, heightMax;
    float heightSpeed;

    bool fleeEnabled;
    float sharkDistance;
    float fleeScale;

    bool boxRuleEnabled;
    float boxSpeed;
    float boxDist;

    bool randomEnabled;
    float randomFactor;


    void ReloadProperties();

    float GetHeight(Vector<3,float>);
    Vector<3,float> GetNormal(Vector<3,float>);

public:
    FishMaster(OceanFloorNode* ocean, PropertyTree& ptree);

    ISceneNode* GetFishNode();
    Shark* GetShark();

    void Reset();

    void Handle(InitializeEventArg arg);
    void Handle(ProcessEventArg arg);
    void Handle(DeinitializeEventArg arg);

    void Handle(PropertiesChangedEventArg arg);
};

#endif
