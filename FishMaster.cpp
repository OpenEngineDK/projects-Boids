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


void FishMaster::Handle(InitializeEventArg arg) {}
void FishMaster::Handle(ProcessEventArg arg) {
    for (vector<Fish*>::iterator itr = fishes.begin();
         itr != fishes.end();
         itr++) {        
        Fish *f = *itr;
        f->node->Move(rg->UniformFloat(-1,1),
                      rg->UniformFloat(-1,1),
                      rg->UniformFloat(-1,1));

    }
}
void FishMaster::Handle(DeinitializeEventArg arg) {}
