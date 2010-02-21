// main
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

// OpenEngine stuff
#include <Meta/Config.h>
#include <Logging/Logger.h>
#include <Logging/StreamLogger.h>
#include <Core/Engine.h>
#include <Display/FollowCamera.h>
#include <Display/PerspectiveViewingVolume.h>
#include <Resources/ResourceManager.h>
#include <Resources/ColladaResource.h>

// SimpleSetup
#include <Utils/SimpleSetup.h>
#include <Scene/RenderStateNode.h>
#include <Scene/PointLightNode.h>
#include <Scene/TransformationNode.h>
#include <Scene/VertexArrayTransformer.h>

// Terrain stuff
#include "Scene/OceanFloorNode.h"
#include <Scene/SunNode.h>
#include <Utils/TerrainUtils.h>
#include <Utils/TerrainTexUtils.h>
#include <Renderers/OpenGL/TerrainRenderingView.h>

#include <Display/GLUTEnvironment.h>
#include <Display/SDLEnvironment.h>
#include <Core/GLUTEngine.h>

#include "FishMaster.h"
#include "WiiFishController.h"

// name spaces that we will be using.
// this combined with the above imports is almost the same as
// fx. import OpenEngine.Logging.*; in Java.
using namespace OpenEngine::Logging;
using namespace OpenEngine::Core;
using namespace OpenEngine::Utils;
using namespace OpenEngine::Display;
using namespace OpenEngine::Scene;
using namespace OpenEngine::Renderers::OpenGL;

// Forward method declarations
void SetupTerrain(SimpleSetup* setup);

SunNode* sun;
OceanFloorNode* oceanFloor;

/**
 * Main method for the first quarter project of CGD.
 * Corresponds to the
 *   public static void main(String args[])
 * method in Java.
 */
int main(int argc, char** argv) {

    ResourceManager<IModelResource>::AddPlugin(new ColladaPlugin());

    // Create simple setup
    //IEnvironment* env = new SDLEnvironment(800,600);
    IEnvironment* env = new GLUTEnvironment(800,600);
    Viewport* vp = new Viewport(env->GetFrame());
    IRenderingView* rv = new TerrainRenderingView(*vp);
   
    SimpleSetup* setup = new SimpleSetup("Larry - The not so Friendly Shark",
                                          vp, env, rv, new GLUTEngine());
    //                                     vp, env, rv );
    DirectoryManager::AppendPath("projects/Boids/data/");
    DirectoryManager::AppendPath("projects/Boids/");

    setup->GetRenderer().SetBackgroundColor(Vector<4, float>(0.12, 0.16, 0.35, 1.0));

    SetupTerrain(setup);
    FishMaster *fm = new FishMaster(oceanFloor,50);

    IModelResourcePtr sharkModel = ResourceManager<IModelResource>::Create("shark/models/shark.dae");

    sharkModel->Load();
    
    TransformationNode* sharkGeom = new TransformationNode();
    sharkGeom->AddNode(sharkModel->GetSceneNode());

    sharkGeom->Move(0,0,-150);
    sharkGeom->Scale(200,200,200);


    VertexArrayTransformer transf;
    transf.Transform(*sharkGeom);

    fm->GetShark()->GetNode()->AddNode(sharkGeom);

    SceneNode *root = new SceneNode();
    RenderStateNode *rsn = new RenderStateNode();

    rsn->EnableOption(RenderStateNode::LIGHTING);    
    rsn->EnableOption(RenderStateNode::COLOR_MATERIAL);

    root->AddNode(sun);
    sun->AddNode(oceanFloor);
    sun->AddNode(rsn);
    rsn->AddNode(fm->GetFishNode());
    setup->SetScene(*root);

    
    
    FollowCamera* cam = new FollowCamera(*(new PerspectiveViewingVolume()));
    cam->Follow(fm->GetShark()->GetNode());
    setup->SetCamera(*cam);

    //cam->SetPosition(Vector<3, float>(-256.0, 800.0, -256.0));
    //cam->LookAt(1024.0, 127.0, 1024.0);

    cam->SetDirection(Vector<3,float>(1,0,0),Vector<3,float>(0,1,0));
    cam->Move(Vector<3, float>(0, 0, 600));
    //cam->LookAt(0.0, 0.0, 0.0);


    WiiFishController *ctrl = new WiiFishController(fm,cam);

    setup->GetKeyboard().KeyEvent().Attach(*ctrl);


    setup->GetEngine().InitializeEvent().Attach(*fm);
    setup->GetEngine().ProcessEvent().Attach(*fm);
    setup->GetEngine().DeinitializeEvent().Attach(*fm);

    setup->GetEngine().InitializeEvent().Attach(*ctrl);
    setup->GetEngine().ProcessEvent().Attach(*ctrl);
    setup->GetEngine().DeinitializeEvent().Attach(*ctrl);


    setup->ShowFPS();

    // Start the engine.
    setup->GetEngine().Start();

    // Return when the engine stops.
    return EXIT_SUCCESS;
}

void SetupTerrain(SimpleSetup* setup){
    // Create the map
    FloatTexture2DPtr map = FloatTexture2DPtr(new FloatTexture2D(1024, 1024, 1));
    Empty(map);
    map = CreateSmoothTerrain(map, 400, 40, 60);
    map = CreateSmoothTerrain(map, 2000, 10, 40);
    map = CreateSmoothTerrain(map, 4000, 5, -6);
    map = CreateSmoothTerrain(map, 10000, 3, 3);
    /*
    FloatTexture2DPtr map = FloatTexture2DPtr(new FloatTexture2D(256, 256, 1));
    Empty(map);
    map = CreateSmoothTerrain(map, 40, 40, 40);
    map = CreateSmoothTerrain(map, 80, 20, -6);
    map = CreateSmoothTerrain(map, 160, 10, 3);
    */
    float widthScale = 8.0;
    /*
    FloatTexture2DPtr map = FloatTexture2DPtr(new FloatTexture2D(64, 64, 1));
    Empty(map);
    map = CreateSmoothTerrain(map, 40, 10, 40);
    map = CreateSmoothTerrain(map, 80, 5, -6);
    map = CreateSmoothTerrain(map, 160, 3, 3);
    
    float widthScale = 8.0;
    */
    Vector<3, float> origo = Vector<3, float>(map->GetHeight() * widthScale / 2, 0, map->GetWidth() * widthScale / 2);
    Vector<3, float> sunDir = Vector<3, float>(1448, 2048, 1448);
    sun = new SunNode(sunDir, origo);
    sun->SetAmbient(Vector<4, float>(0.06, 0.12, 0.17, 1.0));
    sun->SetDiffuse(Vector<4, float>(0.8, 1.0, 0.8, 1.0));
    setup->GetEngine().ProcessEvent().Attach(*sun);

    OceanFloorNode* node = new OceanFloorNode(map);
    node->SetWidthScale(widthScale);
    node->SetSun(sun);
    setup->GetRenderer().InitializeEvent().Attach(*node);
    setup->GetEngine().ProcessEvent().Attach(*node);
    oceanFloor = node;
}
