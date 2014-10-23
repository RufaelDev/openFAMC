#include "SimplificationDecomposer.h"
#include <algorithm>

bool SimplificationDecomposer::createLayerContext(const std::vector<SimplificationOperation>& _op, LayerContext& _lc){
	_lc.vertexContext.resize(_op.size());
	for (int c=0; c<_op.size(); ++c){
		_lc.vertexContext[c].to=_op[c].vertexIndex;
		_lc.vertexContext[c].from=conn_.getOneRingVertices(_op[c].vertexIndex);
		if (!conn_.simplify(_op[c].vertexIndex, _op[c].mode)){ 
			return false;
		}
	}
	reverse(_lc.vertexContext.begin(), _lc.vertexContext.end());
	return true;
}

//-------------------------
bool SimplificationDecomposer::isConsistent(const LayerContext& _lc) const {
	std::vector<bool> isVertexDeleted(conn_.nVertices());
	for (int v=0; v<conn_.nVertices(); ++v){
		isVertexDeleted[v] = conn_.isVertexDeleted(v);
	}
	for (int k=0; k<_lc.vertexContext.size(); ++k){
		for (int l=0; l<_lc.vertexContext[k].from.size(); ++l){
			if (isVertexDeleted[_lc.vertexContext[k].from[l]]) return false;
		}
		isVertexDeleted[_lc.vertexContext[k].to]=false;
	}
	return true;
}
