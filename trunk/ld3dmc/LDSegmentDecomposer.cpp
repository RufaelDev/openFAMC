#include "LDSegmentDecomposer.h"
#include "DPCMPredDelta.h"
#include "DPCMPredLin.h"
#include "DPCMPredNonLin.h"
#include "DPCMEnc.h"
#include "DPCMDec.h"
#include "MathTools.h"
#include "FAMCCabacVx3Encoder.h"
#include "SConnectivity.h"
#include "Animation.h"
#include "LayeredDecomposerAutomatic.h"
#include <algorithm>


LDSegmentDecomposer::LDSegmentDecomposer(const LayeredDecomposition& _ld, double _quantizationStep)
		: ld_(_ld)
		, quantizationStep_(_quantizationStep){
			numberOfDecomposedLayers_=_ld.layerContext.size();
			numberOfVerticesInLayer_.resize(numberOfDecomposedLayers_);
			for (int l=0; l<numberOfDecomposedLayers_; ++l){
				numberOfVerticesInLayer_[l]=ld_.layerContext[l].vertexContext.size();
			}
			numberOfFrames_=0;
			numberOfVertices_=0;
			dm_=0;
			dmRecon_=0;
}

LDSegmentDecomposer::~LDSegmentDecomposer(){
	if (dm_)		delete dm_;
	if (dmRecon_)	delete dmRecon_;
}

bool LDSegmentDecomposer::decomposeLDFrameLayer(	
	int _layerNumber,
	const LDFrameTypeData& _frameTypeData, 
	unsigned int _predictionMode, 
	std::vector<IPoint>& _res,
	std::vector<bool>& _predBits) {

		_res.resize(numberOfVerticesInLayer_[_layerNumber]);
		_predBits.resize(numberOfVerticesInLayer_[_layerNumber]);

		//init dpcm coders and quant- and dequantizers
		DPCMPredDelta predDelta(dmRecon_);
		DPCMPredLin predLin(dmRecon_);
		DPCMPredNonLin predNonLin(dmRecon_);
		DPCMEnc dpcmEncDelta(&predDelta, quantizationStep_);
		DPCMEnc dpcmEncLin(&predLin, quantizationStep_);
		DPCMEnc dpcmEncNonLin(&predNonLin, quantizationStep_);
		DPCMDec dpcmDecDelta(&predDelta, quantizationStep_);
		DPCMDec dpcmDecLin(&predLin, quantizationStep_);
		DPCMDec dpcmDecNonLin(&predNonLin, quantizationStep_);

		//some local variables
		Point p(0,0,0);
		Point pRecon(0,0,0);
		IPoint resQDelta(0,0,0);
		IPoint resQLin(0,0,0);
		IPoint resQNonLin(0,0,0);	
		int predType=0;

		//decompose 
		const LayerContext& lc = ld_.layerContext[_layerNumber];
		for (int c=0; c<numberOfVerticesInLayer_[_layerNumber]; ++c){
			
			//encode
			p = (*dm_)[_frameTypeData.frameNumberDis][lc.vertexContext[c].to];
			if (_predictionMode==0){
				_res[c] = dpcmEncDelta(p, _frameTypeData.frameNumberDis, lc.vertexContext[c], _frameTypeData.frameType, _frameTypeData.refFrameNumberDis0, _frameTypeData.refFrameNumberDis1);
				predType = 0;
			}

			if (_predictionMode==1){
				_res[c] = dpcmEncLin(p, _frameTypeData.frameNumberDis, lc.vertexContext[c], _frameTypeData.frameType, _frameTypeData.refFrameNumberDis0, _frameTypeData.refFrameNumberDis1);
				predType = 1;
			}
			if (_predictionMode==2){
				_res[c] = dpcmEncNonLin(p, _frameTypeData.frameNumberDis, lc.vertexContext[c], _frameTypeData.frameType, _frameTypeData.refFrameNumberDis0, _frameTypeData.refFrameNumberDis1);
				predType = 2;
			}

			if (_predictionMode==3){	// we choose between mode 1 and 2
				resQLin = dpcmEncLin(p, _frameTypeData.frameNumberDis, lc.vertexContext[c], _frameTypeData.frameType, _frameTypeData.refFrameNumberDis0, _frameTypeData.refFrameNumberDis1);
				resQNonLin = dpcmEncNonLin(p, _frameTypeData.frameNumberDis, lc.vertexContext[c], _frameTypeData.frameType, _frameTypeData.refFrameNumberDis0, _frameTypeData.refFrameNumberDis1);
				if (resQLin.max_abs()<=resQNonLin.max_abs()){
					_res[c]=resQLin;
					predType = 1;
					_predBits[c] = true;
				}
				else {
					_res[c]=resQNonLin;
					predType = 2;	
					_predBits[c] = false;
				}
			}
			if (_predictionMode==4){ // we choose between mode 1 and 0
				resQLin = dpcmEncLin(p, _frameTypeData.frameNumberDis, lc.vertexContext[c], _frameTypeData.frameType, _frameTypeData.refFrameNumberDis0, _frameTypeData.refFrameNumberDis1);
				resQDelta = dpcmEncDelta(p, _frameTypeData.frameNumberDis, lc.vertexContext[c], _frameTypeData.frameType, _frameTypeData.refFrameNumberDis0, _frameTypeData.refFrameNumberDis1);
				if (resQLin.max_abs()<=resQDelta.max_abs()){
					_res[c]=resQLin;
					predType = 1;
					_predBits[c] = true;
				}
				else {
					_res[c]=resQDelta;
					predType = 0;
					_predBits[c] = false;
				}
			}

			//decode
			if (predType==0){
				pRecon = dpcmDecDelta(_res[c], _frameTypeData.frameNumberDis, lc.vertexContext[c], _frameTypeData.frameType, _frameTypeData.refFrameNumberDis0, _frameTypeData.refFrameNumberDis1);
			}
			if (predType==1){
				pRecon = dpcmDecLin(_res[c], _frameTypeData.frameNumberDis, lc.vertexContext[c], _frameTypeData.frameType, _frameTypeData.refFrameNumberDis0, _frameTypeData.refFrameNumberDis1);
			}
			if (predType==2){
				pRecon = dpcmDecNonLin(_res[c], _frameTypeData.frameNumberDis, lc.vertexContext[c], _frameTypeData.frameType, _frameTypeData.refFrameNumberDis0, _frameTypeData.refFrameNumberDis1);
			}
			(*dmRecon_)[_frameTypeData.frameNumberDis][lc.vertexContext[c].to]=pRecon;
		}


		return true;
}

bool LDSegmentDecomposer::decomposeLDFrameLayerLS(	
	int _layerNumber,
	const LDFrameTypeData& _frameTypeData, 
	unsigned int _predictionMode, 
	std::vector<IPoint>& _res,
	std::vector<bool>& _predBits) {

		_res.resize(numberOfVerticesInLayer_[_layerNumber]);
		_predBits.resize(numberOfVerticesInLayer_[_layerNumber]);

		//init dpcm coders and quant- and dequantizers
		DPCMPredDelta predDelta(dmRecon_);
		DPCMPredLin predLin(dmRecon_);
		DPCMPredNonLin predNonLin(dmRecon_);
		DPCMEncLS dpcmEncDelta(&predDelta);
		DPCMEncLS dpcmEncLin(&predLin);
		DPCMEncLS dpcmEncNonLin(&predNonLin);
		DPCMDecLS dpcmDecDelta(&predDelta);
		DPCMDecLS dpcmDecLin(&predLin);
		DPCMDecLS dpcmDecNonLin(&predNonLin);

		//some local variables
		IPoint ip(0,0,0);
		IPoint ipRecon(0,0,0);
		IPoint resQDelta(0,0,0);
		IPoint resQLin(0,0,0);
		IPoint resQNonLin(0,0,0);	
		int predType=0;

		//decompose 
		const LayerContext& lc = ld_.layerContext[_layerNumber];
		for (int c=0; c<numberOfVerticesInLayer_[_layerNumber]; ++c){
			
			//encode
			ip = (*dm_)[_frameTypeData.frameNumberDis][lc.vertexContext[c].to];
			
			if (_predictionMode==0){
				_res[c] = dpcmEncDelta(ip, _frameTypeData.frameNumberDis, lc.vertexContext[c], _frameTypeData.frameType, _frameTypeData.refFrameNumberDis0, _frameTypeData.refFrameNumberDis1);
				predType = 0;
			}

			if (_predictionMode==1){
				_res[c] = dpcmEncLin(ip, _frameTypeData.frameNumberDis, lc.vertexContext[c], _frameTypeData.frameType, _frameTypeData.refFrameNumberDis0, _frameTypeData.refFrameNumberDis1);
				predType = 1;
			}
			if (_predictionMode==2){
				_res[c] = dpcmEncNonLin(ip, _frameTypeData.frameNumberDis, lc.vertexContext[c], _frameTypeData.frameType, _frameTypeData.refFrameNumberDis0, _frameTypeData.refFrameNumberDis1);
				predType = 2;
			}

			if (_predictionMode==3){	// we choose between mode 1 and 2
				resQLin = dpcmEncLin(ip, _frameTypeData.frameNumberDis, lc.vertexContext[c], _frameTypeData.frameType, _frameTypeData.refFrameNumberDis0, _frameTypeData.refFrameNumberDis1);
				resQNonLin = dpcmEncNonLin(ip, _frameTypeData.frameNumberDis, lc.vertexContext[c], _frameTypeData.frameType, _frameTypeData.refFrameNumberDis0, _frameTypeData.refFrameNumberDis1);
				if (resQLin.max_abs()<=resQNonLin.max_abs()){
					_res[c]=resQLin;
					predType = 1;
					_predBits[c] = true;
				}
				else {
					_res[c]=resQNonLin;
					predType = 2;	
					_predBits[c] = false;
				}
			}
			if (_predictionMode==4){ // we choose between mode 1 and 0
				resQLin = dpcmEncLin(ip, _frameTypeData.frameNumberDis, lc.vertexContext[c], _frameTypeData.frameType, _frameTypeData.refFrameNumberDis0, _frameTypeData.refFrameNumberDis1);
				resQDelta = dpcmEncDelta(ip, _frameTypeData.frameNumberDis, lc.vertexContext[c], _frameTypeData.frameType, _frameTypeData.refFrameNumberDis0, _frameTypeData.refFrameNumberDis1);
				if (resQLin.max_abs()<=resQDelta.max_abs()){
					_res[c]=resQLin;
					predType = 1;
					_predBits[c] = true;
				}
				else {
					_res[c]=resQDelta;
					predType = 0;
					_predBits[c] = false;
				}
			}

			//decode
			if (predType==0){
				ipRecon = dpcmDecDelta(_res[c], _frameTypeData.frameNumberDis, lc.vertexContext[c], _frameTypeData.frameType, _frameTypeData.refFrameNumberDis0, _frameTypeData.refFrameNumberDis1);
			}
			if (predType==1){
				ipRecon = dpcmDecLin(_res[c], _frameTypeData.frameNumberDis, lc.vertexContext[c], _frameTypeData.frameType, _frameTypeData.refFrameNumberDis0, _frameTypeData.refFrameNumberDis1);
			}
			if (predType==2){
				ipRecon = dpcmDecNonLin(_res[c], _frameTypeData.frameNumberDis, lc.vertexContext[c], _frameTypeData.frameType, _frameTypeData.refFrameNumberDis0, _frameTypeData.refFrameNumberDis1);
			}
			(*dmRecon_)[_frameTypeData.frameNumberDis][lc.vertexContext[c].to]=ipRecon;
		}


		return true;
}

void LDSegmentDecomposer::setDynamicMesh(Animation& _anim, DataType _dataType, int _startFrameNumber, int _endFrameNumber){
	//init variables
	if (_dataType==coordData) numberOfVertices_ = _anim.GetNCoord();
	if (_dataType==normalData) numberOfVertices_ = _anim.GetNNormal();
	if (_dataType==colorData) numberOfVertices_ = _anim.GetNColor();

	numberOfFrames_=_endFrameNumber-_startFrameNumber+1;

	//allocate memory!
	dm_ = new DynamicMesh(numberOfVertices_, numberOfFrames_);
	dmRecon_ = new DynamicMesh(numberOfVertices_, numberOfFrames_);

	//fill memory!
	float coord[3] = {0.0, 0.0, 0.0};
	if (_dataType==coordData){
		for (int f=0; f<numberOfFrames_; ++f){
			for (int v=0; v<numberOfVertices_; ++v){
				_anim.GetCoord(_startFrameNumber + f, v, coord);//if (startFrameNumber=1) skip first frame
				(*dm_)[f][v][0] = coord[0];
				(*dm_)[f][v][1] = coord[1];
				(*dm_)[f][v][2] = coord[2];
				(*dmRecon_)[f][v] = Point(0.0,0.0,0.0);
			}
		}
	}
	if (_dataType==normalData){
		for (int f=0; f<numberOfFrames_; ++f){
			for (int v=0; v<numberOfVertices_; ++v){
				_anim.GetNormal(_startFrameNumber + f, v, coord);
				(*dm_)[f][v][0] = coord[0];
				(*dm_)[f][v][1] = coord[1];
				(*dm_)[f][v][2] = coord[2];
				(*dmRecon_)[f][v] = Point(0.0,0.0,0.0);
			}
		}
	}
	if (_dataType==colorData){
		for (int f=0; f<numberOfFrames_; ++f){
			for (int v=0; v<numberOfVertices_; ++v){
				_anim.GetColor(_startFrameNumber + f, v, coord);
				(*dm_)[f][v][0] = coord[0];
				(*dm_)[f][v][1] = coord[1];
				(*dm_)[f][v][2] = coord[2];
				(*dmRecon_)[f][v] = Point(0.0,0.0,0.0);
			}
		}
	}
}


