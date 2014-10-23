/*
 * =====================================================================================
 * 
 *        Filename:  LayeredDecomposition.hh
 * 
 *     Description:  
 * 
 *         Version:  1.0
 *         Created:  07/12/07 15:43:34 CEST
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst), stefanos@tnt.uni-hannover.de
 *         Company:  Institut für Informationsverarbeitung
 * 
 * =====================================================================================
 */

#ifndef  LAYERED_DECOMPOSITION_HH
#define  LAYERED_DECOMPOSITION_HH

#include <vector>
#include "MeshTypes.h"

struct VertexContext2{
    int to;
    std::vector<int> from;
};

struct LayerContext{
    std::vector<VertexContext2> vertexContext;
};

struct LayeredDecomposition{
    std::vector<LayerContext> layerContext;
    std::vector<IFS> layerIFS;
};

#endif   /* ----- #ifndef LAYERED_DECOMPOSITION_HH  ----- */

