#include "LayeredDecomposerAutomatic.h"
#include "LayeredDecomposerUserBased.h"
#include "SimplificationOperationsFactory.h"
#include "LayeredSimplificationOperationsFactory.h"

using namespace std;

bool LayeredDecomposerAutomatic::produce(LayeredDecomposition& _ld){

    //define simplifications automatically
	vector< vector<SimplificationOperation> > lsop(nLayers_-1);
	LayeredSimplificationOperationsFactory::getSimplificationOperationsAutomatic(lsop, conn_);

    //create layered decomposition
    LayeredDecomposerUserBased lddub(lsop, conn_);
    return lddub.produce(_ld);
}

//-------------------
bool LayeredDecomposerAutomatic::isConsistent(const LayeredDecomposition& _ld) const {
    int nLayers = _ld.layerContext.size();
    std::vector<bool> isEncoded(conn_.nVertices());
    for (int l=0; l<nLayers; ++l){
	const LayerContext& lc = _ld.layerContext[l];
	for (int k=0; k<lc.vertexContext.size(); ++k){
	    for (int i=0; i<lc.vertexContext[k].from.size(); ++i){
		if (!isEncoded[lc.vertexContext[k].from[i]]) return false;
	    }
	    isEncoded[lc.vertexContext[k].to]=true;
	}
    }
    return true;
}
