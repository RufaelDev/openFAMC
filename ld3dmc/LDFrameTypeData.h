/*
 * =====================================================================================
 * 
 *        Filename:  LDFrameTypeData.hh
 * 
 *     Description:  A class specifying inter frame dependecies
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

#ifndef LD_FRAME_TYPE_DATA_H
#define LD_FRAME_TYPE_DATA_H

enum LDFrameType {
	I=0,
	P=1,
	B=2
};

struct LDFrameTypeData {
	int			frameNumberDis;		
	LDFrameType	frameType;			
	int			refFrameNumberDis0;	
	int			refFrameNumberDis1;	

	LDFrameTypeData(){
		frameNumberDis=0;
		frameType=I;
		refFrameNumberDis0=-1;
		refFrameNumberDis1=-1;
	}
};

#endif//LD_FRAME_TYPE_DATA_H