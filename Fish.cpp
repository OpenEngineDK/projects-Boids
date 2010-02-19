
#include "Fish.h"


#include <Scene/GeometryNode.h>


Fish::Fish(ISceneNode* child, RandomGenerator* rg) {
    
    node = new TransformationNode();
    node->AddNode(child);
    position = Vector<3,float>(rg->UniformFloat(100,300),
                               rg->UniformFloat(100,300),
                               rg->UniformFloat(100,300));
    //position = Vector<3,float>(0,0,0);
}


TransformationNode* Fish::GetNode() {
    return node;
}

void Fish::Update(Time dt) {
    float delta = dt.AsInt()/1000000.0;

    position += velocity*delta;
    node->SetPosition(position);

    if (velocity.IsZero())
        return;

    Vector<3,float> x = velocity.GetNormalize(); // x vector
    Vector<3,float> y(0,1,0); // up vector
    Vector<3,float> z = x % y;
    z.Normalize();

    Matrix<3,3,float> rotMat(x,y,z);
    
    Quaternion<float> newRot(rotMat);

    rotation = Quaternion<float>(rotation,newRot,0.01); // Rotate 1%

    node->SetRotation(rotation);

}
