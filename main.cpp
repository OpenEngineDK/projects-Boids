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

// SimpleSetup
#include <Utils/SimpleSetup.h>
#include <Scene/RenderStateNode.h>
#include <Scene/PointLightNode.h>
#include <Scene/TransformationNode.h>

// Game factory
//#include "GameFactory.h"

#include "FishMaster.h"

// name spaces that we will be using.
// this combined with the above imports is almost the same as
// fx. import OpenEngine.Logging.*; in Java.
using namespace OpenEngine::Logging;
using namespace OpenEngine::Core;
using namespace OpenEngine::Utils;
using namespace OpenEngine::Display;
using namespace OpenEngine::Scene;


/**
 * Main method for the first quarter project of CGD.
 * Corresponds to the
 *   public static void main(String args[])
 * method in Java.
 */
int main(int argc, char** argv) {
    // Setup logging facilities.
    //Logger::AddLogger(new StreamLogger(&std::cout));

    // Print usage info.
    //logger.info << "========= Running OpenEngine Test Project =========" << logger.end;

    // Create simple setup
    SimpleSetup* setup = new SimpleSetup("Example Project Title");
    FishMaster *fm = new FishMaster();    

    SceneNode *root = new SceneNode();
    RenderStateNode *rsn = new RenderStateNode();

    rsn->EnableOption(RenderStateNode::LIGHTING);    
    rsn->EnableOption(RenderStateNode::COLOR_MATERIAL);

    root->AddNode(rsn);
    setup->SetScene(*root);
    
    PointLightNode *ln = new PointLightNode();
    //ln->ambient = Vector<4,float>(1,1,1,1);

    TransformationNode *lt = new TransformationNode();
    lt->AddNode(ln);
    lt->SetPosition(Vector<3,float>(100,100,-100));
    

    rsn->AddNode(lt);
    
    rsn->AddNode(fm->GetFishNode());


    Camera* cam = setup->GetCamera();

    cam->SetPosition(Vector<3,float>(100,100,-100));
    cam->LookAt(Vector<3,float>(0,0,0));

    setup->GetEngine().InitializeEvent().Attach(*fm);
    setup->GetEngine().ProcessEvent().Attach(*fm);
    setup->GetEngine().DeinitializeEvent().Attach(*fm);

    // Start the engine.
    setup->GetEngine().Start();

    // Return when the engine stops.
    return EXIT_SUCCESS;
}


