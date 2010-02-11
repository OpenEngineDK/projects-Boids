
#include "Fish.h"


#include <Scene/GeometryNode.h>


Fish::Fish(FaceSet* fs, RandomGenerator* rg) {
    GeometryNode *gn = new GeometryNode(fs);
    node = new TransformationNode();
    node->AddNode(gn);
    position = Vector<3,float>(rg->UniformFloat(0,20),
                               rg->UniformFloat(0,20),
                               rg->UniformFloat(0,20));

}


ISceneNode* Fish::GetNode() {
    return node;
}

void Fish::Update(Time dt) {
    float delta = dt.AsInt()/1000000.0;

    position += velocity*delta;
    
    node->SetRotation(Quaternion<float>(velocity.GetNormalize()));
    node->SetPosition(position);
}
