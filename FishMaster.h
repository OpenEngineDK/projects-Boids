#ifndef _BOIDS_FISH_MASTER_H_
#define _BOIDS_FISH_MASTER_H_

#include <Core/IModule.h>
#include <Scene/SceneNode.h>

#include "Fish.h"
#include "Shoal.h"
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
    std::vector<Shoal*> shoals;
    
    OceanFloorNode* ocean;
    RandomGenerator *rg;
    
    Timer loopTimer;



    // boids

    bool heightEnabled,topEnabled;
    float heightMin, heightMax;
    float heightSpeed;


    Vector<3,float> HeadForDirection(Fish* f, Vector<3,float> d);
    Vector<3,float> HeightRule(Shark* f);
    void BoxLimit(Fish *f);


    void ReloadProperties();
public:

    ISoundResourcePtr blubSound;
    
    float GetHeight(Vector<3,float>);
    Vector<3,float> GetNormal(Vector<3,float>);
                              Vector<3,float> GetReflect(Vector<3,float>,Vector<3,float>);


    Vector<3,float> startPoint;
    Vector<3,float> endPoint;

    Vector<3,float> ScaledPos(Vector<3,float>);

    FishMaster(OceanFloorNode* ocean, PropertyTree& ptree, ISoundResourcePtr snd);

    ISceneNode* GetFishNode();
    Shark* GetShark();

    void Reset();

    void Handle(Core::InitializeEventArg arg);
    void Handle(Core::ProcessEventArg arg);
    void Handle(Core::DeinitializeEventArg arg);

    void Handle(PropertiesChangedEventArg arg);
};

#endif
