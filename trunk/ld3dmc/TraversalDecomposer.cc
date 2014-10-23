#include "TraversalDecomposer.h"
#include "SConnectivity.h"

#include <queue>
#include <vector>

void TraversalDecomposer::traverse(std::vector<int>& _vertexIndices){
    int seed;
    int fCurrent;
    std::queue<int> queue;
    std::vector<int> localFaces;

    while ((seed=getNextSeed())!=-1){
	queue.push(seed);
	while (!queue.empty()){
	    fCurrent=queue.front();
	    queue.pop();
	    if (faceTag[fCurrent]==false){
		faceTag[fCurrent]=true;
		localFaces=conn_.getNeighboringFaces(fCurrent);
		for (int i=0; i<localFaces.size(); ++i){
		    if (faceTag[localFaces[i]]==false){
			queue.push(localFaces[i]);
		    }
		}
		addVertices(fCurrent, _vertexIndices);
	    }
	}
    }
    addRemainingVertices(_vertexIndices);
}

//---------------
int TraversalDecomposer::getNextSeed() const {
    for (int f=0; f<conn_.nFaces(); ++f){
	if (!faceTag[f]) return f;
    }
    return -1;
}

void TraversalDecomposer::addVertices(int _f, std::vector<int>& _newVertices){
    std::vector<int> faceVertices=conn_.getFV(_f);
    int v;
    for (int i=0; i<3; ++i){
	v = faceVertices[i];
	if (!vertexTag[v]){
	    vertexTag[v]=true;
	    _newVertices.push_back(v);
	    assert(!conn_.isVertexDeleted(v));
	}
    }
}

void TraversalDecomposer::addRemainingVertices(std::vector<int>& _vertexIndices){
    for (int v=0; v<vertexTag.size(); ++v){
	if (vertexTag[v]==false){
	    _vertexIndices.push_back(v);
	    vertexTag[v]=true;
	}
    }
}

void TraversalDecomposer::createVertexContext(int _to, VertexContext2& _vc, const std::vector<bool>& _isVisited) const{
    std::vector<int> from;
    std::vector<int> ring = conn_.getOneRingVertices(_to);
    for (int i=0; i<ring.size(); ++i){
	if (_isVisited[ring[i]]) from.push_back(ring[i]);
    }

    _vc.to=_to;
    _vc.from=from;
}

void TraversalDecomposer::createLayerContext(LayerContext& _lc){
    std::vector<bool> isVisited(conn_.nVertices(), false);
    std::vector<int> vertexIndices;
    traverse(vertexIndices);
    _lc.vertexContext.resize(vertexIndices.size());
    
    for (int k=0; k<vertexIndices.size(); ++k){
	createVertexContext(vertexIndices[k], _lc.vertexContext[k], isVisited);
	isVisited[vertexIndices[k]]=true;
    }
    
}
bool TraversalDecomposer::isConsistent(const LayerContext& _lc) const{
    std::vector<bool> isVisited(conn_.nVertices(), false);
    for (int k=0; k<_lc.vertexContext.size(); ++k){
	for (int l=0; l<_lc.vertexContext[k].from.size(); ++l){
	    if (!isVisited[_lc.vertexContext[k].from[l]]) return false;
	}
	isVisited[_lc.vertexContext[k].to]=true;
    }
    return true;
}
