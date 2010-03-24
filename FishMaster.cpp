#include "FishMaster.h"

#include <Geometry/FaceBuilder.h>
#include <Logging/Logger.h>
#include <Scene/VertexArrayNode.h>
#include <Resources/ResourceManager.h>
#include <Resources/IModelResource.h>
#include <Scene/VertexArrayTransformer.h>

using namespace OpenEngine::Geometry;
using namespace OpenEngine::Math;

FishMaster::FishMaster(OceanFloorNode* ocean, PropertyTree& ptree, ISoundResourcePtr snd)
    : ptree(ptree)
    , root(new SceneNode())
    , ocean(ocean)
    , blubSound(snd)
    , rg(new RandomGenerator()) {
    ptree.PropertiesChangedEvent().Attach(*this);
    ptree.Reload();



    FaceSet* fs = new FaceSet();
    FaceBuilder::FaceState state;
    state.color = Vector<4,float>(.5,.5,.5,1);
    FaceBuilder::MakeABox(fs,state,Vector<3,float>(),Vector<3,float>(8) );  

    Vector<3,float> p = ptree.Get("shark.pos",Vector<3,float>());

    shark = new Shark(new GeometryNode(fs),ScaledPos(p),blubSound);


    

    root->AddNode(shark->GetNode());

    //delete fs;
}

void FishMaster::Handle(PropertiesChangedEventArg arg) {
    ReloadProperties();
}


void FishMaster::ReloadProperties() {

    PropertyTreeNode shoalNode = ptree.GetNode("shoals");

    unsigned int num = shoalNode.GetSize();

    vector<Shoal*> oldShoals = shoals;
    shoals.clear();

    for (unsigned int i=0;i<num;i++) {
        Shoal* s = NULL;
        if (i < oldShoals.size()) {
            s = oldShoals[i];
        } else {
            s = new Shoal(this,rg);
            root->AddNode(s->GetNode());
        }
        s->ReloadProperties(shoalNode.GetNode(i));
        shoals.push_back(s);
    }
    if (num < oldShoals.size()) {
        for (unsigned int j=num;j<oldShoals.size();j++) {
            root->DeleteNode(oldShoals[j]->GetNode());
            delete oldShoals[j];
        }
    }
    
    PropertyTreeNode ptn = ptree.GetNode("shark");

    heightEnabled = ptn.GetPath("height.enabled",true);
    topEnabled = ptn.GetPath("height.top.enabled",true);
    heightSpeed = ptn.GetPath("height.speed",30.0f);
    heightMin = ptn.GetPath("height.min",10.0f);
    heightMax = ptn.GetPath("height.max",70.0f);
    
    
    logger.info << "Reloading" << logger.end;


}

ISceneNode* FishMaster::GetFishNode() {
    return root;
}


float FishMaster::GetHeight(Vector<3,float> p) {
    if (p[0] >= startPoint[0] && p[2] >= 0 &&
        p[0] < endPoint[0] && p[2] < endPoint[2])
        return ocean->GetHeight(p);
    return 0;

}
Vector<3,float> FishMaster::GetNormal(Vector<3,float> p) {
    if (p[0] >= startPoint[0] && p[2] >= 0 &&
        p[0] < endPoint[0] && p[2] < endPoint[2])
        return ocean->GetNormal(p);
    return Vector<3,float>(0,1,0);
}

Vector<3,float> FishMaster::GetReflect(Vector<3,float> p, Vector<3,float> d) {
    if (p[0] >= startPoint[0] && p[2] >= 0 &&
        p[0] < endPoint[0] && p[2] < endPoint[2])
        return ocean->GetReflectedDirection(p, d);
    return Vector<3,float>(0,1,0);
}


void FishMaster::Handle(InitializeEventArg arg) {

    float* startVertex = ocean->GetVertex(0, 0);
    startPoint = Vector<3,float>(startVertex[0], 0, startVertex[2]);
    endPoint = Vector<3,float>(ocean->GetWidth(), 1000, ocean->GetDepth());
    endPoint += startPoint;

    logger.info << "Box: " << startPoint
                <<  " " << endPoint << logger.end;

    loopTimer.Start();
    Reset();

}
void FishMaster::Handle(ProcessEventArg arg) {
    Time dt = loopTimer.GetElapsedTimeAndReset();

    for (vector<Shoal*>::iterator itr = shoals.begin();
         itr != shoals.end();
         itr++) {
        Shoal* s = *itr;
        s->Update(dt);
    }



    shark->AddVelocity(HeightRule(shark));
    shark->AddVelocity(HeadForDirection(shark, shark->direction));
    //    if (boxRuleEnabled) shark->AddVelocity(BoxRule(shark));

    // Daming
    shark->velocity = shark->velocity*0.5;
    shark->Update(dt);
    BoxLimit(shark);
}

void FishMaster::Reset() {
    Vector<3,float> startPos = ptree.Get("shark.pos",Vector<3,float>(0,0,0));

    for (vector<Shoal*>::iterator itr = shoals.begin();
         itr != shoals.end();
         itr++) {
        Shoal *s = *itr;

        s->Reset();
    }
    shark->position = ScaledPos(startPos);
    shark->velocity = Vector<3,float>(0,0,0);
                         //shark->Update(0);

}


void FishMaster::Handle(DeinitializeEventArg arg) {}

Shark* FishMaster::GetShark() {
    return shark;
}

Vector<3,float> FishMaster::ScaledPos(Vector<3,float> p) {
    Vector<3,float> r;
    r[0] = endPoint[0]*p[0] + startPoint[0];
    r[2] = endPoint[2]*p[2] + startPoint[2];

    r[1] = p[1]+GetHeight(r);

    return r;
}

// Boids rules
Vector<3,float> FishMaster::HeadForDirection(Fish* f, Vector<3,float> d) {
    return d*0.1;
}
Vector<3,float> FishMaster::HeightRule(Shark* f) {
    float h = GetHeight(f->position);

    float dt = f->position[1] - h;

    Vector<3,float> v;
    if (dt < heightMin){
        if (dt < 0) {
            f->jaw = 0;
            f->position[1] = h;
        }
        v = Vector<3,float>(0,1,0)*heightSpeed;
            //GetNormal(f->position)*heightSpeed;
    }
    return v;
}
 void FishMaster::BoxLimit(Fish* f) {
    Vector<3,float> p = f->position;

    if (p[0] < startPoint[0]) {
        f->position[0] = startPoint[0];
    } else if (p[0] > endPoint[0] ) {
        f->position[0] = endPoint[0];
    }
    if (p[1] < startPoint[1]) {
        f->position[1] = startPoint[1];
    } else if (p[1] > endPoint[1] ) {
        f->position[1] = endPoint[1];
    }
    if (p[2] < startPoint[2]) {
        f->position[2] = startPoint[2];
    } else if (p[2] > endPoint[2] ) {
        f->position[2] = endPoint[2];
    }

}

