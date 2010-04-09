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
#include <Display/InterpolatedViewingVolume.h>
#include <Resources/ResourceManager.h>
#include <Resources/ColladaResource.h>
#include <Renderers/DataBlockBinder.h>

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
#include <Renderers/OpenGL/StereoRenderer.h>

#include <Display/GLUTEnvironment.h>
#include <Display/SDLEnvironment.h>
#include <Core/GLUTEngine.h>

// sound stuff
#include <Sound/OpenALSoundSystem.h>
#include <Sound/ISoundSystem.h>
#include <Sound/ISound.h>
#include <Sound/MusicPlayer.h>
#include <Scene/SoundNode.h>

#include <Resources/VorbisResource.h>
#include <Resources/ISoundResource.h>

#include <Scene/DotVisitor.h>

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
using namespace OpenEngine::Renderers;
using namespace OpenEngine::Sound;


// Forward method declarations
void SetupTerrain(SimpleSetup* setup);

SunNode* sun;
OceanFloorNode* oceanFloor;

ISoundSystem* soundsystem;
MusicPlayer* mplayer;


/**
 * Main method for the first quarter project of CGD.
 * Corresponds to the
 *   public static void main(String args[])
 * method in Java.
 */
int main(int argc, char** argv) {

    ResourceManager<IModelResource>::AddPlugin(new ColladaPlugin());
    ResourceManager<ISoundResource>::AddPlugin(new VorbisResourcePlugin());

    const bool useStereo = false;

    // Create simple setup
    //IEnvironment* env = new SDLEnvironment(800,600);
    //IEnvironment* env = new GLUTEnvironment(1440,900,32,FRAME_FULLSCREEN);
    
    IEnvironment* env = new GLUTEnvironment(1024,768,32);
    if (useStereo)
        env->GetFrame().ToggleOption(FRAME_STEREO);
    Viewport* vp = new Viewport(env->GetFrame());
    IRenderingView* rv = new TerrainRenderingView(*vp);


    
    StereoRenderer* rend = (useStereo?new StereoRenderer(vp):NULL);
    

    SimpleSetup* setup = new SimpleSetup("Larry - The not so Friendly Shark",
                                         vp, env, rv, 
                                         new GLUTEngine(), 
                                         rend);
    //                                     vp, env, rv );
    DirectoryManager::AppendPath("projects/Boids/data/");
    DirectoryManager::AppendPath("projects/Boids/");

    setup->GetRenderer().SetBackgroundColor(Vector<4, float>(0.12, 0.16, 0.35, 1.0));
    SetupTerrain(setup);
    
    // sound stuff
    OpenALSoundSystem* openal = new OpenALSoundSystem(/*root, camera*/);
    soundsystem = openal;
    openal->SetDevice(0);
    setup->GetEngine().InitializeEvent().Attach(*openal);
    setup->GetEngine().DeinitializeEvent().Attach(*openal);
    setup->GetRenderer().PreProcessEvent().Attach(*openal);


    mplayer = new MusicPlayer(NULL, openal);
    mplayer->AddSound("underwater.ogg");
    setup->GetEngine().ProcessEvent().Attach(*mplayer);
    mplayer->Play();


    string confPath = DirectoryManager::FindFileInPath("boids.yaml");
    PropertyTree* ptree = new PropertyTree(confPath);
    FishMaster *fm = new FishMaster(oceanFloor,
                                    *ptree,
                                    ResourceManager<ISoundResource>::Create("bubbles.ogg"));


    IModelResourcePtr sharkModel = 
        ResourceManager<IModelResource>::Create("leopardshark/models/lepord.dae");
    sharkModel->Load();
    
    TransformationNode* sharkGeom = new TransformationNode();
    sharkGeom->AddNode(sharkModel->GetSceneNode());
    sharkGeom->Rotate(0,PI,0);
    //sharkGeom->Move(200,0,-150);
    sharkGeom->Scale(0.5, 0.5, 0.5);

    // TransformationNode* sharkGeom = new TransformationNode();
    // sharkGeom->AddNode(sharkModel->GetSceneNode());
    // sharkGeom->Move(200,0,-150);
    // sharkGeom->Scale(200,200,200);

    VertexArrayTransformer transf;
    transf.Transform(*sharkGeom);
    fm->GetShark()->GetNode()->AddNode(sharkGeom);

    SceneNode *root = new SceneNode();
    RenderStateNode *rsn = new RenderStateNode();

    rsn->EnableOption(RenderStateNode::LIGHTING);    
    rsn->EnableOption(RenderStateNode::COLOR_MATERIAL);

    root->AddNode(sun);
    sun->AddNode(rsn);
    rsn->AddNode(oceanFloor);
    rsn->AddNode(fm->GetFishNode());
    setup->SetScene(*root);

    IViewingVolume* innerV = new InterpolatedViewingVolume(*(new PerspectiveViewingVolume(10)));
    FollowCamera* cam = new FollowCamera(*innerV);
    StereoCamera* sc = new StereoCamera(*cam);

    if (rend)
        rend->SetStereoCamera(sc);

    
    cam->Follow(fm->GetShark()->GetNode());
    setup->SetCamera(*sc);
    cam->SetDirection(Vector<3,float>(1,0,0),Vector<3,float>(0,1,0));
    cam->Move(Vector<3, float>(0, 100, 600));

    WiiFishController *ctrl = new WiiFishController(fm,cam,setup,*ptree,sc);
    setup->GetKeyboard().KeyEvent().Attach(*ctrl);
    setup->GetEngine().InitializeEvent().Attach(*ctrl);
    setup->GetEngine().ProcessEvent().Attach(*ctrl);
    setup->GetEngine().DeinitializeEvent().Attach(*ctrl);

    setup->GetEngine().InitializeEvent().Attach(*fm);
    setup->GetEngine().ProcessEvent().Attach(*fm);
    setup->GetEngine().DeinitializeEvent().Attach(*fm);

    setup->GetEngine().InitializeEvent().Attach(*ptree);
    setup->GetEngine().ProcessEvent().Attach(*ptree);
    setup->GetEngine().DeinitializeEvent().Attach(*ptree);

    DataBlockBinder* bob = new DataBlockBinder(setup->GetRenderer());
    bob->Bind(*root);
    setup->ShowFPS();


    // Write dot graph    
    DotVisitor dv;
    ofstream os("graph.dot", ofstream::out);
    dv.Write(*(setup->GetScene()), &os);
    // Start the engine.
    setup->GetEngine().Start();

    // Return when the engine stops.
    return EXIT_SUCCESS;
}

void SetupTerrain(SimpleSetup* setup){
    // Create the map
    /**/
#if 1
    FloatTexture2DPtr map = FloatTexture2DPtr(new FloatTexture2D(1057, 1057, 1));
    Empty(map);
    map = CreateSmoothTerrain(map, 20, 160, 300);
    map = CreateSmoothTerrain(map, 1600, 20, 60);
    map = CreateSmoothTerrain(map, 8000, 10, 40);
    map = CreateSmoothTerrain(map, 16000, 3, -6);
    map = CreateSmoothTerrain(map, 40000, 2, 3);
#else
    FloatTexture2DPtr map = FloatTexture2DPtr(new FloatTexture2D(193, 193, 1));
    Empty(map);
    map = CreateSmoothTerrain(map, 1, 160, 300);
    map = CreateSmoothTerrain(map, 25, 20, 60);
    map = CreateSmoothTerrain(map, 125, 5, 40);
    map = CreateSmoothTerrain(map, 250, 3, -6);
    map = CreateSmoothTerrain(map, 625, 2, 3);
#endif
    map = MakePlateau(map, 700, 30);
    float widthScale = 16.0;

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
