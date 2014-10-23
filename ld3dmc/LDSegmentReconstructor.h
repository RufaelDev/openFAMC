/*
 * =====================================================================================
 * 
 *        Filename:  LDSegmentReconstructor.h
 * 
 *     Description:  A DPCM based reconsturctor guided by a Layered Decomposition
 * 
 *         Version:  1.0
 *         Created:  07/25/07 17:15:18 CEST
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst)
 *         Company:  Institut für Informationsverarbeitung
 *           Email:  stefanos@tnt.uni-hannover.de
 * 
 * =====================================================================================
 */

#ifndef LD_SEGMENT_RECONSTRUCTOR_H
#define LD_SEGMENT_RECONSTRUCTOR_H

#include "LayeredDecomposition.h"
#include "LDFrameTypeData.h"
#include "Animation.h"
#include "MotionModel.h"
#include "IndexedFaceSet.h"
#include "meshtypes.h"
#include "DynamicMesh.h"
#include <vector>


class LDSegmentReconstructor {
	
public:
	LDSegmentReconstructor(const LayeredDecomposition& _ld, double _quantizationStep, int _numberOfFrames, int _numberOfVertices);
	~LDSegmentReconstructor();

public:
	enum DataType { coordData=0, normalData=1, colorData=2 };
	void getAnimation(Animation& _anim, DataType _dataType, int _startFrameNumber);
	bool reconstructLDFrameLayer(	int _layerNumber,
										const LDFrameTypeData& _frameTypeData, 
										unsigned int _predictionMode, 
										const std::vector<IPoint>& _res,
										const std::vector<bool>& _predBits);
	bool reconstructLDFrameLayerLS(	int _layerNumber,
										const LDFrameTypeData& _frameTypeData, 
										unsigned int _predictionMode, 
										const std::vector<IPoint>& _res,
										const std::vector<bool>& _predBits);


private:
	

	const LayeredDecomposition& ld_;	
	double quantizationStep_;
	int numberOfFrames_;
	int numberOfVertices_;
	

	unsigned int numberOfDecomposedLayers_;
	std::vector<int> numberOfVerticesInLayer_;
	
	
	DynamicMesh* dmRecon_;

};

#endif//LD_SEGMENT_RECONSTRUCTOR_H