#include "FishMaster.h"

#include <Geometry/FaceBuilder.h>

using namespace OpenEngine::Geometry;
using namespace OpenEngine::Math;

FishMaster::FishMaster() {
    root = new SceneNode();

    FaceSet* fs = new FaceSet();

    FaceBuilder::FaceState state;
    state.color = Vector<4,float>(.5,.5,.5,1);
    FaceBuilder::MakeABox(fs,state,Vector<3,float>(),Vector<3,float>(10,10,10) );

    rg = new RandomGenerator();

    for (int i=0;i<10;i++) {
        Fish *f = new Fish(fs,rg);
        fishes.push_back(f);
        root->AddNode(f->GetNode());
    }


}

ISceneNode* FishMaster::GetFishNode() {
    return root;
}

Vector<3,float> FishMaster::Rule1(Fish* f) {
    Vector<3,float> pc;
    for (vector<Fish*>::iterator itr = fishes.begin();
         itr != fishes.end();
         itr++) {
        Fish *n = *itr;

        if (n == f) // Skip self
            continue;

        pc += n->position;

    }

    pc = pc / float(fishes.size() - 1);

    return (pc - f->position) / 100;
}

Vector<3,float> FishMaster::Rule2(Fish* f) {
    Vector<3,float> c;
    for (vector<Fish*>::iterator itr = fishes.begin();
         itr != fishes.end();
         itr++) {
            Fish *n = *itr;
            if (n == f) // Skip self
                continue;
            Vector<3,float> d = (n->position - f->position);

            if (d.GetLength() < 40)
                c = c - d;

    }

    return c/10.0;
}

Vector<3,float> FishMaster::Rule3(Fish* f) {
    Vector<3,float> pv;
    for (vector<Fish*>::iterator itr = fishes.begin();
         itr != fishes.end();
         itr++) {
            Fish *n = *itr;
            if (n == f) // Skip self
                continue;

            pv = pv + n->velocity;
    }

    pv = pv / float(fishes.size() - 1);

    return (pv - f->velocity)/16;
}

Vector<3,float> FishMaster::TendToPlace(Fish* f) {
    Vector<3,float> home(0,0,0);

    return (home - f->position) / 100;
}

void FishMaster::LimitSpeed(Fish* f) {

    float maxSpeed = 100.0;

    float len = (f->velocity).GetLength();

    if (len > maxSpeed) {
        f->velocity = (f->velocity / len) * maxSpeed;
    }
}


void FishMaster::Handle(InitializeEventArg arg) {
    loopTimer.Start();
}
void FishMaster::Handle(ProcessEventArg arg) {
    Time dt = loopTimer.GetElapsedTimeAndReset();

    for (vector<Fish*>::iterator itr = fishes.begin();
         itr != fishes.end();
         itr++) {
        Fish *f = *itr;
        Vector<3,float> v1,v2,v3;

        
        f->velocity += Rule1(f);
        f->velocity += Rule2(f);
        f->velocity += Rule3(f);
        f->velocity += TendToPlace(f);


        LimitSpeed(f);

        f->Update(dt);
    }


}
void FishMaster::Handle(DeinitializeEventArg arg) {}
