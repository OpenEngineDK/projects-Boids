#include "Shoal.h"
#include "FishMaster.h"

#include <Geometry/FaceBuilder.h>
#include <Utils/PropertyTree.h>
#include <Scene/SceneNode.h>
#include <Resources/ResourceManager.h>
#include <Resources/IModelResource.h>
#include <Scene/VertexArrayTransformer.h>
#include <Scene/VertexArrayNode.h>
#include <Logging/Logger.h>

using namespace OpenEngine::Utils;
using namespace OpenEngine::Scene;
using namespace OpenEngine::Geometry;
using namespace OpenEngine::Math;

Shoal::Shoal(FishMaster* fm, RandomGenerator* rg) 
    : fm(fm)
    , root(new SceneNode())
    , oldModelName("")
    , rg(rg) {

}

void Shoal::ReloadProperties(PropertyTreeNode ptn) {
    string fname = ptn.GetPath<string>("model.file","");
    if (oldModelName != fname) {

        logger.info << "Model: " << fname << logger.end;
        IModelResourcePtr model = ResourceManager<IModelResource>::Create(fname);
        model->Load();
        TransformationNode* node = new TransformationNode();

        Vector<3,float> scale = ptn.GetPath("model.scale",Vector<3,float>(1,1,1));
        Quaternion<float> rot(ptn.GetPath("model.rotation",Vector<3,float>(0,0,0)));
        Vector<3,float> translation = ptn.GetPath("model.trans", Vector<3,float>(0,0,0));

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
        
        // VertexArrayTransformer trans;
        // trans.Transform(*node);

        childNode = node;
        
    }
    
    vector<Fish*> oldFishes = fishes;
    fishes.clear();

    startPos = ptn.Get("startPos",Vector<3,float>());

    unsigned int n = ptn.Get("numFish",10);
    for (unsigned int i=0;i<n;i++) {
        
        Fish* f;
       
        if (i < oldFishes.size()) {
            f = oldFishes[i];
            if (oldModelName != fname) {
                FaceSet* fs = new FaceSet();
                FaceBuilder::FaceState state;
                state.color = Vector<4,float>(.5,.5,.5,1);
                FaceBuilder::MakeABox(fs,state,Vector<3,float>(),Vector<3,float>(8) );

                VertexArray* arr = new VertexArray(*fs);

                SceneNode* n = new SceneNode();
                VertexArrayNode* van = new VertexArrayNode();
                van->AddVertexArray(*arr);        
                ISceneNode* geom = childNode->Clone();
                n->AddNode(van);
                n->AddNode(geom);
                f->SetChildNode(n);
            }
            fishes.push_back(f);
        }
        else {
            FaceSet* fs = new FaceSet();
            FaceBuilder::FaceState state;
            state.color = Vector<4,float>(.5,.5,.5,1);
            FaceBuilder::MakeABox(fs,state,Vector<3,float>(),Vector<3,float>(8) );
            VertexArray* arr = new VertexArray(*fs);


            SceneNode* n = new SceneNode();
            VertexArrayNode* van = new VertexArrayNode();
            van->AddVertexArray(*arr);
        
            ISceneNode* geom = childNode->Clone();
            n->AddNode(van);
            n->AddNode(geom);
            
            Fish *f = new Fish(n, startPos);
            fishes.push_back(f);
            root->AddNode(f->GetNode());
        }
    }

    if (n < oldFishes.size()) {
        for (unsigned int j=n;j<oldFishes.size();j++) {
            root->DeleteNode(oldFishes[j]->GetNode());
            delete oldFishes[j];
        }
    }


    oldModelName = fname;
    // Load properties

    speedEnabled = ptn.GetPath("speed.enabled",true);
    maxSpeed = ptn.GetPath("speed.max",200.0f);
    minSpeed = ptn.GetPath("speed.min",30.0f);

    nearDist = ptn.GetPath("nearDist",200.0f);

    rule1Enabled = ptn.GetPath("rule1.enabled",true);
    privacyRadius = ptn.GetPath("rule1.privacyRadius",10.0f);

    rule2Enabled = ptn.GetPath("rule2.enabled",true);
    followScalar = ptn.GetPath("rule2.followScalar",16.0f);

    rule3Enabled = ptn.GetPath("rule3.enabled",true);
    massFactor = ptn.GetPath("rule3.massFactor",100);

    homeEnabled = ptn.GetPath("home.enabled", true);
    home = ptn.GetPath("home.position", Vector<3,float>(0,0,0));
    home = fm->ScaledPos(home);
    homeScalar = ptn.GetPath("home.factor",100.0f);

    boxSpeed = ptn.GetPath("boxrule.speed",10.0f);
    boxDist = ptn.GetPath("boxrule.dist", 100.0f);
    boxRuleEnabled = ptn.GetPath("boxrule.enabled",true);

    randomEnabled = ptn.GetPath("randomize.enabled",true);
    randomFactor = ptn.GetPath("randomize.factor",10.0);

    heightEnabled = ptn.GetPath("height.enabled",true);
    topEnabled = ptn.GetPath("height.top.enabled",true);
    heightSpeed = ptn.GetPath("height.speed",30.0f);
    heightMin = ptn.GetPath("height.min",10.0f);
    heightMax = ptn.GetPath("height.max",70.0f);

    fleeEnabled = ptn.GetPath("flee.enabled",true);
    sharkDistance = ptn.GetPath("flee.sharkDistance",100);
    fleeScale = ptn.GetPath("flee.scale", 1);



}

ISceneNode* Shoal::GetNode() {
    return root;
}

void Shoal::Update(Time dt) {

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
        if (fleeEnabled)    f->AddVelocity(Flee        (f, fm->GetShark()->position));
        if (randomEnabled)  f->AddVelocity(Randomize   (f));

        if (speedEnabled) LimitSpeed(f);



        f->Update(dt);
        BoxLimit(f);
    }

}


void Shoal::Reset() {

    for (vector<Fish*>::iterator itr = fishes.begin();
         itr != fishes.end();
         itr++) {
        Fish *f = *itr;

        f->position = fm->ScaledPos(startPos);
        f->velocity = Vector<3,float>(0,0,0);
    }
}


/// Boids rules
// Keep distance to other boids
Vector<3,float> Shoal::Rule1(Fish* f) {
    Vector<3,float> c;
    for (vector<Fish*>::iterator itr = fishes.begin();
         itr != fishes.end();
         itr++) {
            Fish *n = *itr;
            if (n != f){ // if not self
                Vector<3,float> d = (n->position - f->position);
                float dist = d.GetLength();
                if (dist < nearDist && dist < privacyRadius)
                    c = c - d;
            }
    }

    return c;
}

// Match velocity
Vector<3,float> Shoal::Rule2(Fish* f) {
    Vector<3,float> pv;
    int c = 0;
    for (vector<Fish*>::iterator itr = fishes.begin();
         itr != fishes.end();
         itr++) {
            Fish *n = *itr;
            if (n != f) { // If not self
                Vector<3,float> d = (n->position - f->position);
                float dist = d.GetLength();
                if (dist < nearDist) {
                    pv = pv + n->velocity;
                    c++;
                }
            }
    }

    pv = pv / max(c,1);
    return (pv - f->velocity) / followScalar;
}

// Follow center of mass
Vector<3,float> Shoal::Rule3(Fish* f) {
    Vector<3,float> pc;
    int c = 0;
    for (vector<Fish*>::iterator itr = fishes.begin();
         itr != fishes.end();
         itr++) {

        Fish *n = *itr;

        if (n != f){ // Skip self
            Vector<3,float> d = (n->position - f->position);
            float dist = d.GetLength();
            if (dist < nearDist) {
                pc += n->position;
                c++;
            }
        }
    }

    pc = pc / max(c,1);

    return (pc - f->position) / massFactor;
}


Vector<3,float> Shoal::TendToPlace(Fish* f) {
    return (home - f->position) / homeScalar;
}


Vector<3,float> Shoal::Flee(Fish* f, Vector<3,float> p) {
    Vector<3,float> v;
    float len = (p - f->position).GetLength();
    if (len < sharkDistance) {
        v = (f->position - p);
        v *= (sharkDistance - len)*fleeScale;
    }
    return v;
}

Vector<3,float> Shoal::BoxRule(Fish* f) {

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
        
    Vector<3,float> startPoint = fm->startPoint;
    Vector<3,float> endPoint = fm->endPoint;

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

             
Vector<3,float> Shoal::HeightRule(Fish* f) {
    float h = fm->GetHeight(f->position);

    float dt = f->position[1] - h;

    Vector<3,float> v;
    if (dt < heightMin){
        if (dt < 0)
            f->position[1] = h;
        //v = Vector<3,float>(0,1,0)*heightSpeed;
        //v = fm->GetNormal(f->position)*heightSpeed;
        v = fm->GetReflect(f->position,f->prev)*heightSpeed;
    }
    return v;
}

Vector<3,float> Shoal::TopRule(Fish* f) {
    float h = fm->GetHeight(f->position);
    float dt = f->position[1] - h;
    Vector<3,float> v;
    if (dt > heightMax)
        v = -fm->GetNormal(f->position)*heightSpeed;
    return v;
}


Vector<3,float> Shoal::Randomize(Fish* f) {
        return Vector<3,float>(rg->UniformFloat(-1,1),
                               rg->UniformFloat(-1,1),
                               rg->UniformFloat(-1,1))*randomFactor;;
}


void Shoal::LimitSpeed(Fish* f) {
    float len = (f->velocity).GetLength();
    if (len > maxSpeed) {
        f->velocity = (f->velocity / len) * maxSpeed;
    } else if (len < minSpeed) {
        f->velocity = (f->velocity / len) * minSpeed;
    }
}

 void Shoal::BoxLimit(Fish* f) {
    Vector<3,float> p = f->position;
    Vector<3,float> startPoint = fm->startPoint;
    Vector<3,float> endPoint = fm->endPoint;

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
