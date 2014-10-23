#include "LDSegmentReconstructor.h"
#include "DPCMPredDelta.h"
#include "DPCMPredLin.h"
#include "DPCMPredNonLin.h"
#include "DPCMDec.h"
#include "MathTools.h"
#include "FAMCCabacVx3Encoder.h"
#include "SConnectivity.h"
#include "Animation.h"
#include "LayeredDecomposerAutomatic.h"
#include <algorithm>


LDSegmentReconstructor::LDSegmentReconstructor(const LayeredDecomposition& _ld, double _quantizationStep, int _numberOfFrames, int _numberOfVertices)
		: ld_(_ld)
		, numberOfFrames_(_numberOfFrames)
		, numberOfVertices_(_numberOfVertices)
		, quantizationStep_(_quantizationStep){
			numberOfDecomposedLayers_=_ld.layerContext.size();
			numberOfVerticesInLayer_.resize(numberOfDecomposedLayers_);
			for (int l=0; l<numberOfDecomposedLayers_; ++l){
				numberOfVerticesInLayer_[l]=ld_.layerContext[l].vertexContext.size();
			}
			
			dmRecon_= new DynamicMesh(_numberOfVertices, _numberOfFrames);
}

LDSegmentReconstructor::~LDSegmentReconstructor(){
	delete dmRecon_;
}

bool LDSegmentReconstructor::reconstructLDFrameLayer(	
	int _layerNumber,
	const LDFrameTypeData& _frameTypeData, 
	unsigned int _predictionMode, 
	const std::vector<IPoint>& _res,
	const std::vector<bool>& _predBits) {

		//init dpcm decoders 
		DPCMPredDelta predDelta(dmRecon_);
		DPCMPredLin predLin(dmRecon_);
		DPCMPredNonLin predNonLin(dmRecon_);
		DPCMDec dpcmDecDelta(&predDelta, quantizationStep_);
		DPCMDec dpcmDecLin(&predLin, quantizationStep_);
		DPCMDec dpcmDecNonLin(&predNonLin, quantizationStep_);

		//some local variables
		Point p(0,0,0);
		Point pRecon(0,0,0);
		int predType=0;

		//decompose 
		const LayerContext& lc = ld_.layerContext[_layerNumber];
		for (int c=0; c<numberOfVerticesInLayer_[_layerNumber]; ++c){
			
			//decode
			predType = _predictionMode;
			if (_predictionMode==3) predType = (_predBits[c]==1) ? 1 : 2;
			if (_predictionMode==4) predType = (_predBits[c]==1) ? 1 : 0;

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

bool LDSegmentReconstructor::reconstructLDFrameLayerLS(	
	int _layerNumber,
	const LDFrameTypeData& _frameTypeData, 
	unsigned int _predictionMode, 
	const std::vector<IPoint>& _res,
	const std::vector<bool>& _predBits) {

		//init dpcm decoders 
		DPCMPredDelta predDelta(dmRecon_);
		DPCMPredLin predLin(dmRecon_);
		DPCMPredNonLin predNonLin(dmRecon_);
		DPCMDecLS dpcmDecDelta(&predDelta);
		DPCMDecLS dpcmDecLin(&predLin);
		DPCMDecLS dpcmDecNonLin(&predNonLin);

		//some local variables
		IPoint ipRecon(0,0,0);
		int predType=0;

		//decompose 
		const LayerContext& lc = ld_.layerContext[_layerNumber];
		for (int c=0; c<numberOfVerticesInLayer_[_layerNumber]; ++c){
			
			//decode
			predType = _predictionMode;
			if (_predictionMode==3) predType = (_predBits[c]==1) ? 1 : 2;
			if (_predictionMode==4) predType = (_predBits[c]==1) ? 1 : 0;

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

void LDSegmentReconstructor::getAnimation(Animation& _anim, DataType _dataType, int _startFrameNumber){
	
	//fill animation!
	float coord[3] = {0.0, 0.0, 0.0};
	if (_dataType==coordData){
		assert(_anim.GetNCoord()==numberOfVertices_);
		assert(_anim.GetNKeyCoord()-_startFrameNumber>=numberOfFrames_);
		for (int f=0; f<numberOfFrames_; ++f){
			for (int v=0; v<numberOfVertices_; ++v){
				_anim.GetCoord(_startFrameNumber + f, v, coord);
				coord[0] = (*dmRecon_)[f][v][0];
				coord[1] = (*dmRecon_)[f][v][1];
				coord[2] = (*dmRecon_)[f][v][2];
				_anim.SetCoord(_startFrameNumber + f, v, coord);
			}
		}
	}
	if (_dataType==normalData){
		assert(_anim.GetNNormal()==numberOfVertices_);
		assert(_anim.GetNKeyNormal()-_startFrameNumber>=numberOfFrames_);
		for (int f=0; f<numberOfFrames_; ++f){
			for (int v=0; v<numberOfVertices_; ++v){
				_anim.GetNormal(_startFrameNumber + f, v, coord);
				coord[0] = (*dmRecon_)[f][v][0];
				coord[1] = (*dmRecon_)[f][v][1];
				coord[2] = (*dmRecon_)[f][v][2];
				_anim.SetNormal(_startFrameNumber + f, v, coord);
			}
		}
	}
	if (_dataType==colorData){
		assert(_anim.GetNColor()==numberOfVertices_);
		assert(_anim.GetNKeyColor()-_startFrameNumber>=numberOfFrames_);
		for (int f=0; f<numberOfFrames_; ++f){
			for (int v=0; v<numberOfVertices_; ++v){
				_anim.GetColor(_startFrameNumber + f, v, coord);
				coord[0] = (*dmRecon_)[f][v][0];
				coord[1] = (*dmRecon_)[f][v][1];
				coord[2] = (*dmRecon_)[f][v][2];
				_anim.SetColor(_startFrameNumber + f, v, coord);
			}
		}
	}
}

