
#include "Fish.h"

#include <Scene/GeometryNode.h>


Fish::Fish(FaceSet* fs, RandomGenerator* rg) {
    GeometryNode *gn = new GeometryNode(fs);
    node = new TransformationNode();
    node->AddNode(gn);
    node->Move(rg->UniformFloat(0,20),
               rg->UniformFloat(0,20),
               rg->UniformFloat(0,20));

}


ISceneNode* Fish::GetNode() {
    return node;
}
