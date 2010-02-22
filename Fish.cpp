
#include "Fish.h"

#include <Logging/Logger.h>
#include <Scene/GeometryNode.h>


Fish::Fish(ISceneNode* child, RandomGenerator* rg) {
    
    node = new TransformationNode();
    node->AddNode(child);
    // position = Vector<3,float>(rg->UniformFloat(100,300),
    //                            rg->UniformFloat(100,300),
    //                            rg->UniformFloat(100,300));
    velocity = Vector<3,float>(0,0,0);
    position = Vector<3,float>(100,100,100);
}


TransformationNode* Fish::GetNode() {
    return node;
}

void Fish::AddVelocity(Vector<3,float> v) {
    velocity += v;

}

void Fish::Update(Time dt) {
    float delta = dt.AsInt()/1000000.0;

    position += velocity*delta;
    node->SetPosition(position);

    if (velocity.GetLength() < 0.01)
        return;

    Vector<3,float> x = velocity.GetNormalize(); // x vector
    Vector<3,float> y(0,1,0); // up vector
    Vector<3,float> z = x % y;
    if (z.GetLength() < 0.01)
        return;
    z.Normalize();

    Matrix<3,3,float> rotMat(x,y,z);
    
    Quaternion<float> newRot(rotMat);

    rotation = Quaternion<float>(rotation,newRot,0.01); // Rotate 1%

    node->SetRotation(rotation);

}
