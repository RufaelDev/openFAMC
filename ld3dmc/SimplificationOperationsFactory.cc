/*
 * =====================================================================================
 * 
 *        Filename:  SimplificationOperationsFactory.cc
 * 
 *     Description:  
 * 
 *         Version:  1.0
 *         Created:  07/13/07 16:11:11 CEST
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst), stefanos@tnt.uni-hannover.de
 *         Company:  Institut für Informationsverarbeitung
 * 
 * =====================================================================================
 */


#include "SimplificationOperationsFactory.h"
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

//-------------------
SimplificationOperationsFactory::SimplificationOperationsFactory(const SConnectivity& _conn) : 
    conn_(_conn),
    faceTag(conn_.nFaces(), false),
    vertexTag1(conn_.nVertices(), false),
    vertexTag2(conn_.nVertices(), false) {
	initTags();
    }

//-------------------
	void SimplificationOperationsFactory::produce(std::vector<SimplificationOperation>& _op){
		//printf("SimplificationOperationsFactory::produce start!\n");
		vector<int> patches;
		traverse(patches);
		for (int k=0; k<patches.size(); ++k){
			SimplificationOperation sop;
			sop.vertexIndex = patches[k];
			sop.mode = determineSimplificationMode(sop.vertexIndex);
			if (sop.mode>=0 && conn_.simplify(sop.vertexIndex, sop.mode)){
				_op.push_back(sop);
			}
			else{
				//printf("No valid SimplificationOperation!!!\n");
			}
		}
		//printf("%d patches\n",patches.size());
		//printf("%d simplification operations\n",_op.size());
		//printf("SimplificationOperationsFactory::produce end!\n");
	}

	void SimplificationOperationsFactory::produceOrderedByVertexIndex(std::vector<SimplificationOperation>& _op){
		//printf("SimplificationOperationsFactory::produce start!\n");
		vector<int> patches;
		traverse(patches);
		sort(patches.begin(), patches.end());
		for (int k=0; k<patches.size(); ++k){
			SimplificationOperation sop;
			sop.vertexIndex = patches[k];
			sop.mode = determineSimplificationMode(sop.vertexIndex);
			if (sop.mode>=0 && conn_.simplify(sop.vertexIndex, sop.mode)){
				_op.push_back(sop);
			}
			else{
				//printf("No valid SimplificationOperation!!!\n");
			}
		}
		//printf("%d patches\n",patches.size());
		//printf("%d simplification operations\n",_op.size());
		//printf("SimplificationOperationsFactory::produce end!\n");
	}

	
//-------------------
void SimplificationOperationsFactory::initTags(){
    for (int f=0; f<conn_.nFaces(); ++f){
	faceTag[f]=conn_.isFaceDeleted(f);
    }
    for (int v=0; v<conn_.nVertices(); ++v){
	vertexTag1[v]=conn_.isVertexDeleted(v);
	vertexTag2[v]=conn_.isVertexDeleted(v);
    }
}

//-------------------
void SimplificationOperationsFactory::traverse(std::vector<int>& _patches) {
    initTags();

    int seed;
    int vCurrent;
    std::queue<int> queue;
    std::vector<int> localVertices;

    while ((seed=getNextSeed())!=-1){
	queue.push(seed);
	while (!queue.empty()){
	    vCurrent=queue.front();
	    queue.pop();
	    if (isFreePatch(vCurrent)){
		int deg = conn_.degree(vCurrent);
		if (3<=deg && deg<=6){
		    if (conn_.hasOrientableOneRingVertices(vCurrent)){
			conquerPatch(vCurrent);
			_patches.push_back(vCurrent);
		    }
		}
	    }
	    if (vertexTag2[vCurrent]==false){
		vertexTag2[vCurrent]=true;
		localVertices=conn_.getOneRingVertices(vCurrent);
		for (int i=0; i<localVertices.size(); ++i){
		    if (vertexTag2[localVertices[i]]==false){
			queue.push(localVertices[i]);
		    }
		}
	    }
	}
    }
}

//--------------------
int SimplificationOperationsFactory::determineSimplificationMode(int _v) const {
    int mode = -1;
    int cost = -1;
    int costTmp = -1;
    vector<int> ring = conn_.getOneRingVertices(_v);
    for (int k=0; k<ring.size(); ++k){
	costTmp = edgeCollapseCost(_v, k);
	if ((cost<0 && costTmp>=0) || (0<=costTmp && costTmp<cost)){
	    cost = costTmp;
	    mode = k;
	}
    }
    if (ring.size()==6){
	for (int k=6; k<8; ++k){
	    costTmp = regularSimpificationCost(_v, k);
	    if ((cost<0 && costTmp>=0) || (0<=costTmp && costTmp<cost)){
		cost = costTmp;
		mode = k;
	    }
	}
	
    }
    return mode;
}

//--------------------
int SimplificationOperationsFactory::edgeCollapseCost(int _vertexIndex, int _simplificationMode) const {
    vector<int> ring = conn_.getOneRingVertices(_vertexIndex);
    if (ring.size()<3) return -1;

    if (ring.size()<=_simplificationMode) return -1;
    int vertexIndexCollapse = ring[_simplificationMode];
    
    if (!conn_.isEdgeCollapseAllowed(_vertexIndex, vertexIndexCollapse)) return -1;
    
    adjustToIdx(ring, vertexIndexCollapse);
    vector<int> deg(ring.size());
    for (int k=0; k<ring.size(); ++k){
	deg[k] = conn_.degree(ring[k]);
	deg[k]-=6;
    }
    deg[0] += conn_.degree(_vertexIndex) - 4;
    deg[1] += -1;
    deg.back() += -1;
    return sumAbs(deg);
}

//--------------------
int SimplificationOperationsFactory::regularSimpificationCost(int _vertexIndex, int _simplificationMode) const {
    if(_simplificationMode!=6 && _simplificationMode!=7) return -1;
    std::vector<int> ring = conn_.getOneRingVertices(_vertexIndex);
    if (ring.size()!=6) return -1;
    
    int vertexIndexCollapse=ring[1];;
    if (_simplificationMode==6){
	vertexIndexCollapse = ring[0];
    }
    
    if (!conn_.isRegularSimplificationAllowed(_vertexIndex, vertexIndexCollapse)) return -1;
    
    adjustToIdx(ring, vertexIndexCollapse);
    vector<int> deg(ring.size());
    for (int k=0; k<ring.size(); ++k){
	deg[k] = conn_.degree(ring[k]);
	deg[k]-=6;
	deg[k] += ((k%2)==0) ? 1 : -1;
    }
    return sumAbs(deg);
}

//--------------------
int SimplificationOperationsFactory::sumAbs(std::vector<int> _vec) const {
    int s=0;
    for (int k=0; k<_vec.size(); ++k){
	s+=abs(_vec[k]);
    }
    return s;
}

//--------------------
bool SimplificationOperationsFactory::adjustToIdx(std::vector<int>& _vec, int _idx) const {
    vector<int>::iterator idxIter = find(_vec.begin(), _vec.end(), _idx);
    if (idxIter==_vec.end()) return false;
    vector<int> vecOut(idxIter, _vec.end());
    vecOut.insert(vecOut.end(), _vec.begin(), idxIter);
    _vec=vecOut;
    return true;
}

//--------------------
int SimplificationOperationsFactory::getNextSeed() const {
    for (int v=0; v<conn_.nVertices(); ++v){
	if (!vertexTag2[v]) return v;
    }
    return -1;
}

//--------------------
bool SimplificationOperationsFactory::isFreePatch(int _v) const {
    if (vertexTag1[_v]==true) return false;

    const vector<int>& patchFaces = conn_.getVF(_v);
    for (int i=0; i<patchFaces.size(); ++i){
	if (faceTag[patchFaces[i]]==true) return false;
    }
    return true;
}

//--------------------
void SimplificationOperationsFactory::conquerPatch(int _v){
    vertexTag1[_v]=true;

    vector<int> patchFaces=conn_.getVF(_v);
    for (int i=0; i<patchFaces.size(); ++i){
	faceTag[patchFaces[i]]=true;
    }
    
}
