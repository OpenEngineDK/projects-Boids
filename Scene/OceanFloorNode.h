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

namespace OpenEngine {
    namespace Scene {
        
        class OceanFloorNode : public HeightMapNode {
        public:
            OceanFloorNode() : HeightMapNode() {}
            OceanFloorNode(FloatTexture2DPtr tex) : HeightMapNode(tex) {}
            ~OceanFloorNode() {}
        };
    }
}

#endif
