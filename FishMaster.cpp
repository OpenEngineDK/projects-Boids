#include "FishMaster.h"

#include <Geometry/FaceBuilder.h>
#include <Logging/Logger.h>
#include <Scene/VertexArrayNode.h>
#include <Resources/ResourceManager.h>
#include <Resources/IModelResource.h>
#include <Scene/VertexArrayTransformer.h>

using namespace OpenEngine::Geometry;
using namespace OpenEngine::Math;

FishMaster::FishMaster(OceanFloorNode* ocean, PropertyTree& ptree)
    : ptree(ptree)
    , ocean(ocean) {
    ptree.PropertiesChangedEvent().Attach(*this);
    ptree.Reload();
    //ReloadProperties();

    
    string fname = ptree.Get<string>("model.file","");
    
    logger.info << "Model: " << fname << logger.end;

    IModelResourcePtr model = ResourceManager<IModelResource>::Create(fname);
    model->Load();
    TransformationNode* node = new TransformationNode();
    
    Vector<3,float> scale = ptree.Get("model.scale",Vector<3,float>(1,1,1));
    Quaternion<float> rot(ptree.Get("model.rotation",Vector<3,float>(0,0,0)));
    Vector<3,float> translation = ptree.Get("model.trans", Vector<3,float>(0,0,0));
    
    

    logger.info << "Scale: " << scale << logger.end;
    logger.info << "Translation: " << translation << logger.end;
    logger.info << "Rotation: " << rot << logger.end;

    node->SetScale(scale);
    if (rot.GetNorm() != 0.0) {
        rot.Normalize();
        node->SetRotation(rot);
    }
    node->SetPosition(translation);
    node->AddNode(model->GetSceneNode());

    VertexArrayTransformer trans;
    trans.Transform(*node);
            
    root = new SceneNode();   
    FaceSet* fs = new FaceSet();

    FaceBuilder::FaceState state;
    state.color = Vector<4,float>(.5,.5,.5,1);
    FaceBuilder::MakeABox(fs,state,Vector<3,float>(),Vector<3,float>(8) );

    VertexArray* arr = new VertexArray(*fs);
    
    rg = new RandomGenerator();

    shark = new Shark(new GeometryNode(fs),
                      ptree.Get("startPos",Vector<3,float>()),
                      rg);
    
    unsigned int n = ptree.Get("numFish",10);
    for (unsigned int i=0;i<n;i++) {
        SceneNode *n = new SceneNode();
        VertexArrayNode* van = new VertexArrayNode();
        van->AddVertexArray(*arr);
        
        ISceneNode* geom = node->Clone();
        n->AddNode(van);
        n->AddNode(geom);
        Fish *f = new Fish(n,
                           ptree.Get("startPos",Vector<3,float>()),
                           rg);
        fishes.push_back(f);
        root->AddNode(f->GetNode());
    }
    root->AddNode(shark->GetNode());

    //delete fs;
}

void FishMaster::Handle(PropertiesChangedEventArg arg) {
    ReloadProperties();
}


void FishMaster::ReloadProperties() {

    logger.info << "Reloading" << logger.end;
    
    speedEnabled = ptree.Get("speed.enabled",true);
    maxSpeed = ptree.Get("speed.max",200.0f);
    minSpeed = ptree.Get("speed.min",30.0f);

    rule1Enabled = ptree.Get("rule1.enabled",true);
    massFactor = ptree.Get("rule1.massFactor",100);

    rule2Enabled = ptree.Get("rule2.enabled",true);
    privacyRadius = ptree.Get("rule2.privacyRadius",10.0f);
    
    rule3Enabled = ptree.Get("rule3.enabled",true);
    followScalar = ptree.Get("rule3.followScalar",16.0f);
    
    homeEnabled = ptree.Get("home.enabled", true);
    home = ptree.Get("home.position", Vector<3,float>(0,0,0));
    homeScalar = ptree.Get("home.factor",100.0f);


    boxSpeed = ptree.Get("boxrule.speed",10.0f);
    boxDist = ptree.Get("boxrule.dist", 100.0f);
    boxRuleEnabled = ptree.Get("boxrule.enabled",true);

    randomEnabled = ptree.Get("randomize.enabled",true);
    randomFactor = ptree.Get("randomize.factor",10.0);
    
    heightEnabled = ptree.Get("height.enabled",true);
    topEnabled = ptree.Get("height.top.enabled",true);    
    heightSpeed = ptree.Get("height.speed",30.0f);
    heightMin = ptree.Get("height.min",10.0f);
    heightMax = ptree.Get("height.max",70.0f);

    fleeEnabled = ptree.Get("flee.enabled",true);
    sharkDistance = ptree.Get("flee.sharkDistance",100);
    fleeScale = ptree.Get("flee.scale", 1);

}

ISceneNode* FishMaster::GetFishNode() {
    return root;
}

// Follow center of mass
Vector<3,float> FishMaster::Rule1(Fish* f) {
    Vector<3,float> pc;
    int c = 1;
    for (vector<Fish*>::iterator itr = fishes.begin();
         itr != fishes.end();
         itr++) {
        
        Fish *n = *itr;

        if (n != f){ // Skip self
            pc += n->position;
            c++;
        }
    }

    pc = pc / c;

    return (pc - f->position) / massFactor;
}

// Keep distance to other boids
Vector<3,float> FishMaster::Rule2(Fish* f) {
    Vector<3,float> c;
    for (vector<Fish*>::iterator itr = fishes.begin();
         itr != fishes.end();
         itr++) {
            Fish *n = *itr;
            if (n != f){ // if not self
                Vector<3,float> d = (n->position - f->position);
                if (d.GetLength() < privacyRadius)
                    c = c - d;
            }
    }

    return c;
}

// Match velocity
Vector<3,float> FishMaster::Rule3(Fish* f) {
    Vector<3,float> pv;
    int c = 1;
    for (vector<Fish*>::iterator itr = fishes.begin();
         itr != fishes.end();
         itr++) {
            Fish *n = *itr;
            if (n != f){ // If not self
                pv = pv + n->velocity;
                c++;
    }
    }

    pv = pv / c;

    return (pv - f->velocity) / followScalar;
}

Vector<3,float> FishMaster::TendToPlace(Fish* f) {
    return (home - f->position) / homeScalar;
}


Vector<3,float> FishMaster::Flee(Fish* f, Vector<3,float> p) {
    Vector<3,float> v;
    float len = (p - f->position).GetLength();
    if (len < sharkDistance) {
        v = (f->position - p);
        v *= (sharkDistance - len)*fleeScale;
    }
    return v;
}

Vector<3,float> FishMaster::BoxRule(Fish* f) {
        
    /*
   	PROCEDURE bound_position(Boid b)
		Integer Xmin, Xmax, Ymin, Ymax, Zmin, Zmax
		Vector v

		IF b.position.x < Xmin THEN
			v.x = 10
		ELSE IF b.position.x > Xmax THEN
			v.x = -10
		END IF
		IF b.position.y < Ymin THEN
			v.y = 10
		ELSE IF b.position.y > Ymax THEN
			v.y = -10
		END IF
		IF b.position.z < Zmin THEN
			v.z = 10
		ELSE IF b.position.z > Zmax THEN
			v.z = -10
		END IF
		
		RETURN v
	END PROCEDURE
        */
        
    Vector<3,float> p = f->position;
    Vector<3,float> v;
   
    if ((p[0] - startPoint[0]) < boxDist) {
        v[0] = boxSpeed;
    } else if ((endPoint[0] - p[0]) < boxDist ) {
        v[0] = -boxSpeed;
    }
    if ((p[1] - startPoint[1]) < boxDist) {
        v[1] = boxSpeed;
    } else if ((endPoint[1] - p[1]) < boxDist ) {
        v[1] = -boxSpeed;
    }
    if ((p[2] - startPoint[2]) < boxDist) {
        v[2] = boxSpeed;
    } else if ((endPoint[2] - p[2]) < boxDist ) {
        v[2] = -boxSpeed;
    }


    return v;
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


Vector<3,float> FishMaster::HeightRule(Fish* f) {
    float h = GetHeight(f->position);

    float dt = f->position[1] - h;

    Vector<3,float> v;
    if (dt < heightMin){
        if (dt < 0)
            f->position[1] = h;
        v = Vector<3,float>(0,1,0)*heightSpeed;
            //GetNormal(f->position)*heightSpeed;
    } 
    return v;
}

Vector<3,float> FishMaster::TopRule(Fish* f) {
    float h = GetHeight(f->position);

    float dt = f->position[1] - h;
    
    Vector<3,float> v;
    if (dt > heightMax) 
        v = -GetNormal(f->position)*heightSpeed;

    return v;
}


Vector<3,float> FishMaster::HeadForDirection(Fish* f, Vector<3,float> d) {
    return d*0.1;
}

Vector<3,float> FishMaster::Randomize(Fish* f) {
        return Vector<3,float>(rg->UniformFloat(-1,1),
                               rg->UniformFloat(-1,1),
                               rg->UniformFloat(-1,1))*randomFactor;;
}


void FishMaster::LimitSpeed(Fish* f) {
    float len = (f->velocity).GetLength();

    if (len > maxSpeed) {
        f->velocity = (f->velocity / len) * maxSpeed;
    } else if (len < minSpeed) {
        f->velocity = (f->velocity / len) * minSpeed;
    }
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


void FishMaster::Handle(InitializeEventArg arg) {

    float* startVertex = ocean->GetVertex(0, 0);
    startPoint = Vector<3,float>(startVertex[0], 0, startVertex[2]);
    endPoint = Vector<3,float>(ocean->GetWidth(), 1000, ocean->GetDepth());
    endPoint += startPoint;

    loopTimer.Start();

}
void FishMaster::Handle(ProcessEventArg arg) {
    Time dt = loopTimer.GetElapsedTimeAndReset();

    
    for (vector<Fish*>::iterator itr = fishes.begin();
         itr != fishes.end();
         itr++) {
        Fish *f = *itr;
        Vector<3,float> v1,v2,v3;


        // Based on: http://www.vergenet.net/~conrad/boids/pseudocode.html
        
        if (rule1Enabled)   f->AddVelocity(Rule1       (f));
        if (rule2Enabled)   f->AddVelocity(Rule2       (f));
        if (rule3Enabled)   f->AddVelocity(Rule3       (f));
        if (homeEnabled)    f->AddVelocity(TendToPlace (f));
        if (boxRuleEnabled) f->AddVelocity(BoxRule     (f));
        if (heightEnabled)  f->AddVelocity(HeightRule  (f));
        if (topEnabled)     f->AddVelocity(TopRule     (f));
        if (fleeEnabled)    f->AddVelocity(Flee        (f, shark->position));
        if (randomEnabled)  f->AddVelocity(Randomize   (f));

        if (speedEnabled) LimitSpeed(f);
        
        BoxLimit(f);

        f->Update(dt);
        
    }

    shark->AddVelocity(HeightRule(shark));
    shark->AddVelocity(HeadForDirection(shark, shark->direction));
    //    if (boxRuleEnabled) shark->AddVelocity(BoxRule(shark));

    BoxLimit(shark);
    
    // Daming
    shark->velocity = shark->velocity*0.5;    
    shark->Update(dt);
}

void FishMaster::Reset() {
    Vector<3,float> startPos = ptree.Get("startPos",Vector<3,float>(0,0,0));

    for (vector<Fish*>::iterator itr = fishes.begin();
         itr != fishes.end();
         itr++) {
        Fish *f = *itr;
        
        f->position = startPos;
        f->velocity = Vector<3,float>(0,0,0);
        //f->Update(0);
    }
    shark->position = startPos;
    shark->velocity = Vector<3,float>(0,0,0);
                         //shark->Update(0);
                         
}


void FishMaster::Handle(DeinitializeEventArg arg) {}

Shark* FishMaster::GetShark() {
    return shark;
}
