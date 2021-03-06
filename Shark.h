
//
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS)
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------


#ifndef _OE_SHARK_H_
#define _OE_SHARK_H_

#include "Fish.h"

using namespace OpenEngine::Scene;
using namespace OpenEngine::Geometry;
using namespace OpenEngine::Math;
using namespace OpenEngine::Utils;


/**
 * Short description.
 *
 * @class Shark Shark.h ts/Boids/Shark.h
 */
class Shark : public Fish {
private:

    float speed;

public:
    float jaw,dir;
    Vector<3,float> direction;

    Shark(ISceneNode* child, Vector<3,float> start, ISoundResourcePtr fishres);

    void SetSpeed(float s);
    void AddDirection(float j, float r);
    void SetDirection(float j, float r);
};

#endif // _OE_SHARK_H_
