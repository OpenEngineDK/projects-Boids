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
#include <Display/Camera.h>
#include <Resources/ResourceManager.h>

// SimpleSetup
#include <Utils/SimpleSetup.h>
#include <Scene/RenderStateNode.h>
#include <Scene/PointLightNode.h>
#include <Scene/TransformationNode.h>

// Terrain stuff
#include "Scene/OceanFloorNode.h"
#include <Scene/SunNode.h>
#include <Utils/TerrainUtils.h>
#include <Utils/TerrainTexUtils.h>
#include <Renderers/OpenGL/TerrainRenderingView.h>

#include <Display/SDLEnvironment.h>


#include "FishMaster.h"

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
ISceneNode* SetupTerrain(SimpleSetup* setup);

/**
 * Main method for the first quarter project of CGD.
 * Corresponds to the
 *   public static void main(String args[])
 * method in Java.
 */
int main(int argc, char** argv) {

    // Create simple setup
    IEnvironment* env = new SDLEnvironment(800,600);
    Viewport* vp = new Viewport(env->GetFrame());
    IRenderingView* rv = new TerrainRenderingView(*vp);

    SimpleSetup* setup = new SimpleSetup("Larry - The not so Friendly Shark",
                                         vp, env, rv);
    DirectoryManager::AppendPath("projects/Boids/data/");

    setup->GetRenderer().SetBackgroundColor(Vector<4, float>(0.12, 0.16, 0.35, 1.0));

    FishMaster *fm = new FishMaster();

    SceneNode *root = new SceneNode();
    RenderStateNode *rsn = new RenderStateNode();

    rsn->EnableOption(RenderStateNode::LIGHTING);    
    rsn->EnableOption(RenderStateNode::COLOR_MATERIAL);

    //SetupTerrain(setup);

    root->AddNode(rsn);
    root->AddNode(SetupTerrain(setup));
    setup->SetScene(*root);
    
    PointLightNode *ln = new PointLightNode();
    //ln->ambient = Vector<4,float>(1,1,1,1);

    TransformationNode *lt = new TransformationNode();
    lt->AddNode(ln);
    lt->SetPosition(Vector<3,float>(100,100,-100));
    

    rsn->AddNode(lt);
    
    rsn->AddNode(fm->GetFishNode());


    Camera* cam = setup->GetCamera();

    //cam->SetPosition(Vector<3,float>(100,100,-100));
    //cam->LookAt(Vector<3,float>(0,0,0));
    cam->SetPosition(Vector<3, float>(-256.0, 200.0, -256.0));
    cam->LookAt(0.0, 127.0, 0.0);

    setup->GetEngine().InitializeEvent().Attach(*fm);
    setup->GetEngine().ProcessEvent().Attach(*fm);
    setup->GetEngine().DeinitializeEvent().Attach(*fm);

    // Start the engine.
    setup->GetEngine().Start();

    // Return when the engine stops.
    return EXIT_SUCCESS;
}

ISceneNode* SetupTerrain(SimpleSetup* setup){
    // Create the map
    FloatTexture2DPtr map = FloatTexture2DPtr(new FloatTexture2D(1024, 1024, 1));
    map = CreateSmoothTerrain(map, 1000, 40, 20);
    map = CreateSmoothTerrain(map, 2000, 20, -6);
    map = CreateSmoothTerrain(map, 4000, 10, 3);
    
    float widthScale = 2.0;
    float origo[] = {map->GetHeight() * widthScale / 2, 0, map->GetWidth() * widthScale / 2};
    float sunDir[] = {1448, 2048, 1448};
    SunNode* sun = new SunNode(sunDir, origo);
    setup->GetEngine().ProcessEvent().Attach(*sun);

    IShaderResourcePtr floorShader = ResourceManager<IShaderResource>::Create("projects/Boids/data/shaders/oceanfloor/oceanfloor.glsl");

    OceanFloorNode* node = new OceanFloorNode(map);
    node->SetLandscapeShader(floorShader);
    node->SetTextureDetail(1.0f / 16.0f);
    node->SetWidthScale(widthScale);
    node->SetSun(sun);
    setup->GetRenderer().InitializeEvent().Attach(*node);
    setup->GetEngine().ProcessEvent().Attach(*node);

    return node;
}
