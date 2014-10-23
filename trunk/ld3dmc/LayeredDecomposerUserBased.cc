#include "LayeredDecomposerUserBased.h"
#include "SimplificationDecomposer.h"
#include "TraversalDecomposer.h"

//-------------------
bool LayeredDecomposerUserBased::produce(LayeredDecomposition& _ld){
	//init sizes
	int nLayersMinusOne = simplificationOperation_.size();
	_ld.layerContext.resize(nLayersMinusOne+1);
	_ld.layerIFS.resize(nLayersMinusOne+1);

	//simplify
	bool successful= false;
	SimplificationDecomposer simpDecomp(conn_);
	for (int k=0; k<nLayersMinusOne; ++k){
		_ld.layerIFS[nLayersMinusOne-k]=conn_.getIFS();
		successful = simpDecomp.createLayerContext(simplificationOperation_[k], _ld.layerContext[nLayersMinusOne-k]);
		printf("Size of layer %d: %d\n",nLayersMinusOne-k, _ld.layerContext[nLayersMinusOne-k].vertexContext.size());
		if (!successful) return false;
	}

	//traverse
	TraversalDecomposer travDecomp(conn_);
	_ld.layerIFS[0]=conn_.getIFS();
	travDecomp.createLayerContext(_ld.layerContext[0]);
	printf("Size of layer 0: %d\n",_ld.layerContext[0].vertexContext.size());

	return true;
}

//-------------------
bool LayeredDecomposerUserBased::isConsistent(const LayeredDecomposition& _ld) const {
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
