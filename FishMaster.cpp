#include "FishMaster.h"

#include <Geometry/FaceBuilder.h>
#include <Logging/Logger.h>
#include <Scene/VertexArrayNode.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem/operations.hpp>
//#include "boost/timer.hpp"


using namespace OpenEngine::Geometry;
using namespace OpenEngine::Math;



FishMaster::FishMaster(OceanFloorNode* ocean, unsigned int n) : ocean(ocean) {
    Reload();

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

void FishMaster::Reload() {
    using namespace boost::filesystem;


    string fname = DirectoryManager::FindFileInPath("boids.conf");

    time_t new_ts = last_write_time(fname);
    if (new_ts != last_ts) {
        logger.info << "Reloading properties" << logger.end;
        ReloadProperties();
        last_ts = new_ts;
    }
}

void FishMaster::ReloadProperties() {
    string fname = DirectoryManager::FindFileInPath("boids.conf");

    using boost::property_tree::ptree;

    ptree pt;
    read_ini(fname,pt);

    socialSphereRadius = pt.get("socialSphereRadius",10.0);
    maxSpeed = pt.get("maxSpeed",200.0f);
    minSpeed = pt.get("minSpeed",30.0f);
    followScalar = pt.get("followScalar",16.0f);
    home = Vector<3,float>(0,0,0);
    homeScalar = pt.get("homeScalar",100.0f);
    privacyRadius = pt.get("privacyRadius",10.0f);
    boxSpeed = pt.get("boxSpeed",10.0f);
    heightSpeed = pt.get("heightSpeed",30.0f);
    heightMin = pt.get("heightMin",10.0f);
    heightMax = pt.get("heightMax",70.0f);
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
    if ((p - f->position).GetLength() < 100.0) {
        v = (f->position - p);
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
    if (dt < heightMin){
        if (dt < 0)
            f->position[1] = h;
        v = GetNormal(f->position)*heightSpeed;
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
        return Vector<3,float>(rg->UniformFloat(0,1),
                               rg->UniformFloat(0,1),
                               rg->UniformFloat(0,1))*10.0f;
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

    if (reloadTimer.GetElapsedIntervals(1000000)) {
        reloadTimer.Reset();
        Reload();
    }

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
        f->velocity += TopRule(f);
        f->velocity += Flee(f, shark->position);
        f->velocity += Randomize(f);

        LimitSpeed(f);

        f->Update(dt);
    }

    shark->velocity += HeightRule(shark);
    shark->velocity += HeadForDirection(shark, shark->direction);
    
    // Daming
    shark->velocity = shark->velocity*0.5;
    
    shark->Update(dt);
}
void FishMaster::Handle(DeinitializeEventArg arg) {}

Shark* FishMaster::GetShark() {
    return shark;
}
