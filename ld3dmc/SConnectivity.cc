#include "SConnectivity.h"
#include <cassert>
#include <algorithm>
#include <vector>
#include <set>
#include <deque>

using namespace std;


//-------------------
SConnectivity::SConnectivity(const IFS& _ifs) {
    fv_=_ifs;
    int nVert = maxIdx(fv_)+1;
    initVF( nVert );
    isVertexDeleted_.resize(nVertices(), false);
    isFaceDeleted_.resize(nFaces(), false);
}

//-------------------
SConnectivity::SConnectivity(const IFS& _ifs, int _nVertices) {
    fv_=_ifs;
    initVF(_nVertices);
    isVertexDeleted_.resize(nVertices(), false);
    isFaceDeleted_.resize(nFaces(), false);
}
//-------------------
SConnectivity::IFS SConnectivity::getIFS() const {
    IFS ifs;
    for (int f=0; f<fv_.size(); ++f){
	if (!isFaceDeleted(f)) ifs.push_back(fv_[f]);
    }
    return ifs;
}

//-------------------
const std::vector<int>& SConnectivity::getFV(int _f) const { 
    assert(_f>=0);
    assert(_f<fv_.size());
    return fv_[_f]; 
}

//-------------------
const std::vector<int>& SConnectivity::getVF(int _v) const { 
    assert(_v>=0);
    assert(_v<vf_.size());
    return vf_[_v]; 
}

//-------------------
bool SConnectivity::isVertexDeleted(int _v) const { 
    assert(_v>=0);
    assert(_v<vf_.size());
    return isVertexDeleted_[_v]; 
}

//-------------------
bool SConnectivity::isFaceDeleted(int _f) const { 
    assert(_f>=0);
    assert(_f<fv_.size());
    return isFaceDeleted_[_f]; 
}

//-------------------
vector<int> SConnectivity::getOneRingVertices(int _v) const {
    vector<int> oneRing = getOneRingVerticesCCW(_v);
    if (oneRing.empty()){//if not orientable?
	oneRing = getOneRingVerticesInc(_v);
    }
    return oneRing;
}

//-------------------
vector<int> SConnectivity::getOneRingVerticesInc(int _v) const {
    set<int> oneRingSet;
    for (int kv=0; kv<vf_[_v].size(); ++kv){
	for (int kf=0; kf<3; ++kf){
	    oneRingSet.insert( fv_[vf_[_v][kv]][kf] );
	}
    }
    oneRingSet.erase(_v);
    vector<int> oneRing(oneRingSet.begin(), oneRingSet.end());
    sortIncreasing(oneRing);
    return oneRing;
}

//------------------
std::vector<int> SConnectivity::getOneRingVerticesCCW(int _v) const {
    vector<int> ring;
    vector<int> faces = vf_[_v];
    bool isFan = sortToFanCCW(faces, _v);
    if (!isFan) return ring;
    
    vector<int> faceVertices;
    for (int k=0; k<faces.size(); ++k){
	faceVertices = fv_[faces[k]];
	adjustToIdx( faceVertices, _v);
	ring.push_back(faceVertices[1]);
	if (k==(faces.size()-1)){
	    if (!isClosedFanCCW(faces, _v))  {
		ring.push_back(faceVertices[2]);
	    }
	}
    }
    return ring;
}

//fill vf 
void SConnectivity::initVF(int _size){
    vf_.resize(_size);
    for (int f=0; f<fv_.size(); ++f){
	assert(fv_[f].size()==3);
	for (int k=0; k<3; ++k){
	    assert(fv_[f][k]<_size);
	    vf_[fv_[f][k]].push_back(f);
	}
    }
    for (int v=0; v<vf_.size(); ++v){
	sortIncreasing(vf_[v]);
    }
}

//get max index
int SConnectivity::maxIdx(const IFS& _ifs) const{
    int max=0;
    for (int f=0; f<_ifs.size(); ++f){
	for (int k=0; k<_ifs[f].size(); ++k){
	    if (_ifs[f][k] > max) max=_ifs[f][k];
	}
    }
    return max;
}

//------------------
bool SConnectivity::simplify(int _vertexIndex, int _simplificationMode){
    vector<int> ring = getOneRingVertices(_vertexIndex);
    //printf("simplify(%d, %d), valence=%d\n",_vertexIndex, _simplificationMode, ring.size());
    if (ring.size()==6) {
	if (_simplificationMode==6){
	    return regularSimplification6(_vertexIndex);
	}
	if (_simplificationMode==7){
	    return regularSimplification7(_vertexIndex);
	}
    }
    if (_simplificationMode<ring.size()){
	int vertexIndexCollapse = ring[_simplificationMode];
	return edgeCollapse(_vertexIndex, vertexIndexCollapse);
    }
    return false;
}

//------------------
bool SConnectivity::edgeCollapse(int _vertexIndex, int _vertexIndexCollapse){
    if (!isEdgeCollapseAllowed(_vertexIndex, _vertexIndexCollapse)) return false;

    //replace face indices
    std::vector<int> vertexFaces = vf_[_vertexIndex]; 
    for (int i=0; i<vertexFaces.size(); ++i){
	replaceFaceVertex(vertexFaces[i], _vertexIndex, _vertexIndexCollapse);
    }
    std::vector<int> vertexFacesColl = vf_[_vertexIndexCollapse]; 
    for (int i=0; i<vertexFacesColl.size(); ++i){
	replaceFaceVertex(vertexFacesColl[i], _vertexIndex, _vertexIndexCollapse);
    }

    //merge vertexIndexCollapse faces with vertexIndex faces
    vf_[_vertexIndexCollapse].insert( 
            vf_[_vertexIndexCollapse].end(),  
            vf_[_vertexIndex].begin(),  
            vf_[_vertexIndex].end());

    //delete degenerated faces at vertexIndexCollapse
    deleteDegeneratedFaces(_vertexIndexCollapse);

    //delete double faces at vertexIndexCollapse
    //deleteDoubleFaces(_vertexIndexCollapse);
    
    //delete vertexIndex
    vf_[_vertexIndex].clear();
    isVertexDeleted_[_vertexIndex] = true;
    
    sortIncreasing( vf_[_vertexIndexCollapse] );
    return true;
}

//------------------
bool SConnectivity::isEdgeCollapseAllowed(int _vertexIndex, int _vertexIndexCollapse) const {
    if (!areNeighboringVertices(_vertexIndex, _vertexIndexCollapse)) return false;
    return true;
}

//------------------
//bool SConnectivity::deleteDoubleFaces(int _vertexIndex){
//    const vector<int>& vertexFaces = vf_[_vertexIndex];
//    for (int fp=0; fp<vertexFaces.size(); ++fp){
//	for (int f=0; f<vertexFaces.size(); ++f){
//	    if (equalFaceVertices(vertexFaces[fp], vertexFaces[f]) && fp!=f){
//		deleteFace(fp);
//		deleteFace(f);
//	    }
//	}
//    }
//    
//}

//------------------
bool SConnectivity::deleteDegeneratedFaces(int _vertexIndex){
    const vector<int>& vertexFaces = vf_[_vertexIndex];
    for (int f=0; f<vertexFaces.size(); ++f){
		if(isDegeneratedFace(vertexFaces[f])){
			deleteFace(vertexFaces[f]);
		}
    }
	return true;
}

//------------------
bool SConnectivity::regularSimplification(int _vertexIndex, int _vertexIndexCollapse){
    if (!isRegularSimplificationAllowed(_vertexIndex, _vertexIndexCollapse)) return false;

    //perform edge collapse and flip
    vector<int> ringFaces = vf_[_vertexIndex];
    if (!edgeCollapse(_vertexIndex, _vertexIndexCollapse)) return false;

    vector<int> fanFaces;
    for (int k=0; k<ringFaces.size(); ++k){
	if (!isFaceDeleted(ringFaces[k])){
	    fanFaces.push_back(ringFaces[k]);
	}
    }
    sortToFanCCW(fanFaces, _vertexIndexCollapse);

    if (fanFaces.size()==4){
	flipFacesCCW(fanFaces[1], fanFaces[2], _vertexIndexCollapse);
    }
    return true;
}

//------------------
bool SConnectivity::regularSimplification6(int _vertexIndex){
    vector<int> ring = getOneRingVertices(_vertexIndex);
    if (ring.size()!=6) return false;
    int vertexIndexCollapse = ring[1];
    return regularSimplification(_vertexIndex, vertexIndexCollapse);
}

//------------------
bool SConnectivity::regularSimplification7(int _vertexIndex){
    vector<int> ring = getOneRingVertices(_vertexIndex);
    if (ring.size()!=6) return false;
    int vertexIndexCollapse = ring[0];
    return regularSimplification(_vertexIndex, vertexIndexCollapse);
}

//------------------
bool SConnectivity::isRegularSimplificationAllowed(int _vertexIndex, int _vertexIndexCollapse) const {
    if (!isEdgeCollapseAllowed(_vertexIndex, _vertexIndexCollapse)) return false;
    vector<int> ring = getOneRingVertices(_vertexIndex);
    if (ring.size()!=6) return false;
    vector<int> vertexFaces = vf_[_vertexIndex];
    sortToFanCCW(vertexFaces, _vertexIndex);
    if (!isClosedFanCCW(vertexFaces, _vertexIndex)) return false;
    return true;
}



//-----------------
vector<int> SConnectivity::getNeighboringFaces(int _f) const {
    vector<int> neighborFaces;
    const vector<int>& faceVertices = fv_[_f];

    for (int i=0; i<3; ++i){
	const vector<int>& vertexFaces=vf_[faceVertices[i]];
	for (int k=0; k<vertexFaces.size(); ++k){
	    if (areNeighboringFaces(_f, vertexFaces[k])){
		neighborFaces.push_back(vertexFaces[k]);
	    }
	}
    }
    sortIncreasing(neighborFaces);
    return neighborFaces;
}

//------------------
bool SConnectivity::areNeighboringFaces(int _f0, int _f1) const {
    int n=0;
    for (int i0=0; i0<3; ++i0){
	for (int i1=0; i1<3; ++i1){
	    if (fv_[_f0][i0]==fv_[_f1][i1]) ++n;
	}
    }
    return (n==2);
}

//------------------
bool SConnectivity::areNeighboringVertices(int _v0, int _v1) const {
    const vector<int> ring = getOneRingVerticesInc(_v0);
    return  ( find(ring.begin(), ring.end(), _v1)!=ring.end() );
    
}

//------------------
bool SConnectivity::isNeighborFaceCW(int _f, int _fl, int _v) const{
    std::vector<int> vertexIndices = fv_[_f];
    std::vector<int> vertexIndicesLeft = fv_[_fl];
    bool isAdjusted = adjustToIdx(vertexIndices, _v);
    bool isAdjustedLeft = adjustToIdx(vertexIndicesLeft, _v);
    if (!isAdjusted || !isAdjustedLeft) return false;
    return (vertexIndices[1]==vertexIndicesLeft[2] && vertexIndices[2]!=vertexIndicesLeft[1]);
}

//------------------
bool SConnectivity::isNeighborFaceCCW(int _f, int _fr, int _v) const{
    std::vector<int> vertexIndices = fv_[_f];
    std::vector<int> vertexIndicesRight = fv_[_fr];
    bool isAdjusted = adjustToIdx(vertexIndices, _v);
    bool isAdjustedRight = adjustToIdx(vertexIndicesRight, _v);
    if (!isAdjusted || !isAdjustedRight) return false;
    return (vertexIndices[2]==vertexIndicesRight[1] && vertexIndices[1]!=vertexIndicesRight[2]);
}

//------------------
bool SConnectivity::sortToFanCCW(std::vector<int>& _faces, int _vAnchor) const {
    if (_faces.empty()) return false;
    
    deque<int> fan;
    vector<bool> isAdded(_faces.size(), false);
    
    fan.push_back(_faces[0]);
    isAdded[0]=true;
    
    int faceLast=-1;
    while (faceLast!=fan.back()){
	faceLast=fan.back();
	for (int i=0; i<_faces.size(); ++i){
	    if (!isAdded[i] && isNeighborFaceCCW(faceLast, _faces[i], _vAnchor)){
		fan.push_back(_faces[i]);
		isAdded[i]=true;
		break;
	    }
	}
    }
    
    int faceFirst=-1;
    while (faceFirst!=fan.front()){
	faceFirst=fan.front();
	for (int i=0; i<_faces.size(); ++i){
	    if (!isAdded[i] && isNeighborFaceCW(faceFirst, _faces[i], _vAnchor)){
		fan.push_front(_faces[i]);
		isAdded[i]=true;
		break;
	    }
	}
    }

    bool isFan = (fan.size()==_faces.size());
    if (isFan){
	_faces.clear();
	_faces.insert(_faces.end(), fan.begin(), fan.end());
    }
    return isFan;
}

//------------------
bool SConnectivity::isFanCCW(const std::vector<int>& _faces, int _vAnchor) const {
    for (int k=0; k<_faces.size()-1; ++k){
	if ( !isNeighborFaceCCW(_faces[k], _faces[k+1], _vAnchor) ) return false;
    }
    return true;
}

//------------------
bool SConnectivity::isClosedFanCCW(const std::vector<int>& _faces, int _vAnchor) const {
    assert(!_faces.empty());
    return (isFanCCW(_faces, _vAnchor) 
	&& isNeighborFaceCCW(_faces.back(), _faces[0], _vAnchor));
}

//------------------
bool SConnectivity::hasOrientableOneRingVertices(int _v) const {
    return !getOneRingVerticesCCW(_v).empty();
}

//------------------
bool SConnectivity::deleteFace(int _f){
    if (!isFaceDeleted_[_f]){
	isFaceDeleted_[_f]=true;
	std::vector<int>& faceVertices = fv_[_f];
	for (int k=0; k<faceVertices.size(); ++k){
	    std::vector<int>& localFaces = vf_[faceVertices[k]];
	    localFaces.erase(remove(localFaces.begin(), localFaces.end(), _f), localFaces.end());
	}
	return true;
    }
    else{
	return false;
    }
}

//------------------
bool SConnectivity::deleteVertex(int _v){
    if (!isVertexDeleted_[_v]){
	isVertexDeleted_[_v]=true;
	std::vector<int>& vertexFaces = vf_[_v];
	for (int k=0; k<vertexFaces.size(); ++k){
	    deleteFace(vertexFaces[k]);
	}
	vf_[_v].clear();
	return true;
    }
    else{
	return false;
    }
}
	

//------------------
bool SConnectivity::equalFaceVertices(int _f0, int _f1) const{
    const std::vector<int>& _v0 = fv_[_f0];
    const std::vector<int>& _v1 = fv_[_f1];
    assert(_v0.size()==3);
    assert(_v1.size()==3);
    if (find(_v0.begin(), _v0.end(), _v1[0])==_v0.end()) return false;
    if (find(_v0.begin(), _v0.end(), _v1[1])==_v0.end()) return false;
    if (find(_v0.begin(), _v0.end(), _v1[2])==_v0.end()) return false;
    return true;

}

//------------------
void SConnectivity::replaceFaceVertex(int _f, int _vertexIndex, int _vertexIndexCollapse){
    assert(fv_[_f].size()==3);
    for (int i=0; i<3; ++i){ 
	if (fv_[_f][i]==_vertexIndex) fv_[_f][i]=_vertexIndexCollapse;
    }
}

//------------------
bool SConnectivity::faceContainsVertex(int _f, int _v) const {
    const std::vector<int>& faceVertices = fv_[_f];
    if ( find(faceVertices.begin(), faceVertices.end(), _v)!=faceVertices.end()){
	return true;
    }
    return false;
}

//------------------
bool SConnectivity::isDegeneratedFace(int _f) const { 
    assert(fv_[_f].size()==3);
    if (fv_[_f][0]==fv_[_f][1]) return true;
    if (fv_[_f][0]==fv_[_f][2]) return true;
    if (fv_[_f][1]==fv_[_f][2]) return true;
    return false;
}



//------------------
void SConnectivity::flipFacesCCW(int _f0, int _f1, int _vAnchor){
    assert(isNeighborFaceCCW(_f0, _f1, _vAnchor));

    //remove face index only
    deleteFace(_f0); isFaceDeleted_[_f0]=false;
    deleteFace(_f1); isFaceDeleted_[_f1]=false;
	    
    //flip faces
    std::vector<int> faceVertices0 = fv_[_f0];
    std::vector<int> faceVertices1 = fv_[_f1];
    adjustToIdx(faceVertices0, _vAnchor);
    adjustToIdx(faceVertices1, _vAnchor);
    replaceFaceVertex(_f0, faceVertices0[2], faceVertices1[2]);
    replaceFaceVertex(_f1, faceVertices1[0], faceVertices0[1]);

    //add new face index locally
    for (int k=0; k<3; ++k){
	vf_[fv_[_f0][k]].push_back(_f0);
	sortIncreasing(vf_[fv_[_f0][k]]);
	vf_[fv_[_f1][k]].push_back(_f1);
	sortIncreasing(vf_[fv_[_f1][k]]);
    }
    
}


//------------------
bool SConnectivity::adjustToIdx(std::vector<int>& _vec, int _idx) const {
    vector<int>::iterator idxIter = find(_vec.begin(), _vec.end(), _idx);
    if (idxIter==_vec.end()) return false;
    vector<int> vecOut(idxIter, _vec.end());
    vecOut.insert(vecOut.end(), _vec.begin(), idxIter);
    _vec=vecOut;
    return true;
}

//------------------
void SConnectivity::sortIncreasing(std::vector<int>& _vec) const {
    sort(_vec.begin(), _vec.end());
}
