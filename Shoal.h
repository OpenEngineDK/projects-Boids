// 
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS)
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------


#ifndef _OE_SHOAL_H_
#define _OE_SHOAL_H_

#include "Fish.h"
#include <vector>

namespace OpenEngine {
    namespace Scene {
        class ISceneNode;
        class SceneNode;
    }
    namespace Utils {
        class PropertyTreeNode;
    }
}

class FishMaster;

/**
 * Short description.
 *
 * @class Shoal Shoal.h ts/Boids/Shoal.h
 */
class Shoal {
private:
    FishMaster* fm;
    Vector<3,float> startPos;


    std::vector<Fish*> fishes;
    SceneNode* root;
    ISceneNode* childNode;
    std::string oldModelName;


    RandomGenerator* rg;

    // Boids rules
    Vector<3,float> Rule1(Fish* f);
    Vector<3,float> Rule2(Fish* f);
    Vector<3,float> Rule3(Fish* f);

    Vector<3,float> BoxRule(Fish* f);
    Vector<3,float> HeightRule(Fish* f);
    Vector<3,float> TopRule(Fish* f);
    

    void BoxLimit(Fish *f);
    void LimitSpeed(Fish* f);

    Vector<3,float> TendToPlace(Fish* f);


    Vector<3,float> Flee(Fish* f, Vector<3,float> p);
    Vector<3,float> Randomize(Fish *f);



    // Boid properties
    float nearDist;

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


public:
    Shoal(FishMaster* fm, RandomGenerator* rg);
    ISceneNode* GetNode();
    void ReloadProperties(OpenEngine::Utils::PropertyTreeNode node);   
    void Update(Time dt);
    void Reset();
};

#endif // _OE_SHOAL_H_
