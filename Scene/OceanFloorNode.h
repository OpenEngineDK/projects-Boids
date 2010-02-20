// Heightfield node.
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _OCEANFLOOR_NODE_H_
#define _OCEANFLOOR_NODE_H_

#include <Scene/HeightMapNode.h>
#include <Resources/IShaderResource.h>
#include <Resources/ResourceManager.h>

namespace OpenEngine {
namespace Scene {
        
    class OceanFloorNode : public HeightMapNode {
    private:
        unsigned int elapsedTime;
    public:
        OceanFloorNode() : HeightMapNode() {}
        OceanFloorNode(FloatTexture2DPtr tex) : HeightMapNode(tex) {
            elapsedTime = 0;
        }
        ~OceanFloorNode() {}

        void Initialize(RenderingEventArg arg) {
            landscapeShader = ResourceManager<IShaderResource>::Create("projects/Boids/data/shaders/oceanfloor/oceanfloor.glsl");
            SetTextureDetail(1.0f / 16.0f);
        }

        void Process(ProcessEventArg arg){
            elapsedTime += arg.approx;
        }

        void PreRender(Display::Viewport view) {
            this->landscapeShader->SetUniform("time", (float) elapsedTime / 12000000);
        }
    };
}
}

#endif
