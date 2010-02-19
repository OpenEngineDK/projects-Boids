#include "FishMaster.h"

#include <Geometry/FaceBuilder.h>
#include <Logging/Logger.h>
#include <Scene/VertexArrayNode.h>


using namespace OpenEngine::Geometry;
using namespace OpenEngine::Math;



FishMaster::FishMaster(OceanFloorNode* ocean, unsigned int n) : ocean(ocean) {
    InitProperties();

    root = new SceneNode();
    
    FaceSet* fs = new FaceSet();

    FaceBuilder::FaceState state;
    state.color = Vector<4,float>(.5,.5,.5,1);
    FaceBuilder::MakeABox(fs,state,Vector<3,float>(),Vector<3,float>(8) );

    VertexArray* arr = new VertexArray(*fs);
    
    rg = new RandomGenerator();

    shark = new Shark(new GeometryNode(fs),rg);

    for (unsigned int i=0;i<n;i++) {
        VertexArrayNode* node = new VertexArrayNode();
        node->AddVertexArray(*arr);

        Fish *f = new Fish(node,rg);
        fishes.push_back(f);
        root->AddNode(f->GetNode());
    }
    root->AddNode(shark->GetNode());

    //delete fs;
}

void FishMaster::InitProperties(){
    socialSphereRadius = 20;
    maxSpeed = 200.0f;
    minSpeed = 30.0f;
    followScalar = 8.0f;
    home = Vector<3,float>(0,0,0);
    homeScalar = 100.0f;
    privacyRadius = 10.0f;
    boxSpeed = 10.0f;
    heightSpeed = 30.0f;
}

ISceneNode* FishMaster::GetFishNode() {
    return root;
}

// Follow center of mass
Vector<3,float> FishMaster::Rule1(Fish* f) {
    Vector<3,float> pc;
    for (vector<Fish*>::iterator itr = fishes.begin();
         itr != fishes.end();
         itr++) {
        
        Fish *n = *itr;

        if (n != f) // Skip self
            pc += n->position;
    }

    pc = pc / float(fishes.size() - 1);

    return (pc - f->position) / 100.0f;
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
    for (vector<Fish*>::iterator itr = fishes.begin();
         itr != fishes.end();
         itr++) {
            Fish *n = *itr;
            if (n != f) // If not self
                pv = pv + n->velocity;
    }

    pv = pv / float(fishes.size() - 1);

    return (pv - f->velocity) / followScalar;
}

Vector<3,float> FishMaster::TendToPlace(Fish* f) {
    return (home - f->position) / homeScalar;
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

    if (p[0] < startPoint[0]) {
        v[0] = boxSpeed;
    } else if (p[0] > endPoint[0] ) {
        v[0] = -boxSpeed;
    }
    if (p[1] < startPoint[1]) {
        v[1] = boxSpeed;
    } else if (p[1] > endPoint[1] ) {
        v[1] = -boxSpeed;
    }
    if (p[2] < startPoint[2]) {
        v[2] = boxSpeed;
    } else if (p[2] > endPoint[2] ) {
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

    if (dt < 10.0f) 
        v = GetNormal(f->position)*heightSpeed;
    else if (dt > 70.0f) 
        v = -GetNormal(f->position)*heightSpeed;

    return v;
}


void FishMaster::LimitSpeed(Fish* f) {

    float len = (f->velocity).GetLength();

    if (len > maxSpeed) {
        f->velocity = (f->velocity / len) * maxSpeed;
    } else if (len < minSpeed) {
        f->velocity = (f->velocity / len) * minSpeed;
    }
}


void FishMaster::Handle(InitializeEventArg arg) {

    float* startVertex = ocean->GetVertex(0, 0);
    startPoint = Vector<3,float>(startVertex[0], 0, startVertex[2]);
    endPoint = Vector<3,float>(ocean->GetWidth(), 50, ocean->GetDepth());
    endPoint += startPoint;

    loopTimer.Start();
    reloadTimer.Start();
}
void FishMaster::Handle(ProcessEventArg arg) {
    Time dt = loopTimer.GetElapsedTimeAndReset();

    // if (reloadTimer.GetElapsedIntervals(1000000)) {
    //     logger.info << "Reload" << logger.end;
    //     reloadTimer.Reset();
    //     ReloadChangedTweakableValues();
    // }

    for (vector<Fish*>::iterator itr = fishes.begin();
         itr != fishes.end();
         itr++) {
        Fish *f = *itr;
        Vector<3,float> v1,v2,v3;


        // Based on: http://www.vergenet.net/~conrad/boids/pseudocode.html
        
        f->velocity += Rule1(f);
        f->velocity += Rule2(f);
        f->velocity += Rule3(f);
        //f->velocity += TendToPlace(f);
        f->velocity += BoxRule(f);
        f->velocity += HeightRule(f);

        LimitSpeed(f);

        f->Update(dt);
    }

    shark->Update(dt);
}
void FishMaster::Handle(DeinitializeEventArg arg) {}

Shark* FishMaster::GetShark() {
    return shark;
}
