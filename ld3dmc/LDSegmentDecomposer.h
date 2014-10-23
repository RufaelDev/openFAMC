/*
 * =====================================================================================
 * 
 *        Filename:  LDSegmentDecomposer.h
 * 
 *     Description:  A DPCM based decomposer guided by a Layered Decomposition
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

#ifndef LD_SEGMENT_DECOMPOSER_H
#define LD_SEGMENT_DECOMPOSER_H

#include "LayeredDecomposition.h"
#include "LDFrameTypeData.h"
#include "Animation.h"
#include "MotionModel.h"
#include "IndexedFaceSet.h"
#include "meshtypes.h"
#include "DynamicMesh.h"
#include <vector>


class LDSegmentDecomposer {
	
public:
	LDSegmentDecomposer(const LayeredDecomposition& _ld, double _quantizationStep);
	~LDSegmentDecomposer();

public:
	enum DataType { coordData=0, normalData=1, colorData=2 };
	void setDynamicMesh(Animation& _anim, DataType _dataType, int _startFrameNumber, int _endFrameNumber);
	bool decomposeLDFrameLayer(	int _layerNumber,
										const LDFrameTypeData& _frameTypeData, 
										unsigned int _predictionMode, 
										std::vector<IPoint>& _res,
										std::vector<bool>& _predBits);
	bool decomposeLDFrameLayerLS(int _layerNumber,
										const LDFrameTypeData& _frameTypeData, 
										unsigned int _predictionMode, 
										std::vector<IPoint>& _res,
										std::vector<bool>& _predBits);

private:
	
	
	const LayeredDecomposition& ld_;	
	double quantizationStep_;				

	unsigned int numberOfDecomposedLayers_;	
	std::vector<int> numberOfVerticesInLayer_;
	
	int numberOfFrames_;
	int numberOfVertices_;
	
	DynamicMesh* dm_;
	DynamicMesh* dmRecon_;

};

#endif//LD_SEGMENT_DECOMPOSER_H