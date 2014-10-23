#include "LDFrameOrderFactory.h"
#include <cassert>


LDFrameOrderFactory::LDFrameOrderFactory(int _nFrames, int _LD_P, int _LD_B)
: nFrames_(_nFrames)
, LD_P_(_LD_P)
, LD_B_(_LD_B)
, frameType_(_nFrames, I)
, refFrameNumberDis0_(_nFrames,-1)
, refFrameNumberDis1_(_nFrames,-1)
, frameNumberEnc2DisList_(_nFrames, -1)
, frameNumberDis2EncList_(_nFrames, -1)
, ftd_(_nFrames){

	assert(nFrames_>0);
	assert(LD_P_>=-1);
	assert(LD_B_>=0);
	assert( (LD_B_ & (LD_B_+1))==0 ); //LD_B_+1==2^k

	int frameNumberEnc=-1;
	int frameNumberDis=0;
	int size_B = LD_B_+1;
	for (; frameNumberDis<nFrames_; frameNumberDis+=size_B){
		if ( (frameNumberDis==0) || (LD_P_>-1) && (frameNumberDis%(LD_P_+1)*(LD_B_+1))==0 ) {
			//I-frame
			++frameNumberEnc;
			frameNumberEnc2DisList_[frameNumberEnc]=frameNumberDis;
			frameNumberDis2EncList_[frameNumberDis]=frameNumberEnc;
			frameType_[frameNumberEnc]=I;
		}
		else{
			//P-frame
			++frameNumberEnc;
			frameNumberEnc2DisList_[frameNumberEnc]=frameNumberDis;
			frameNumberDis2EncList_[frameNumberDis]=frameNumberEnc;
			frameType_[frameNumberEnc]=P;
			refFrameNumberDis0_[frameNumberEnc]=frameNumberDis-size_B;

			assert( refFrameNumberDis0_[frameNumberEnc]>=0 );
		}

		if (frameNumberDis>0){
			for (int b_start_offset=(LD_B_+1)/2; b_start_offset>=1; b_start_offset/=2){
				for (int b=frameNumberDis-LD_B_-1+b_start_offset; b<frameNumberDis; b+=(2*b_start_offset)){
					//B-frame
					++frameNumberEnc;
					frameNumberEnc2DisList_[frameNumberEnc]=b;
					frameNumberDis2EncList_[b]=frameNumberEnc;
					frameType_[frameNumberEnc]=B;

					refFrameNumberDis0_[frameNumberEnc]=b-b_start_offset;
					refFrameNumberDis1_[frameNumberEnc]=b+b_start_offset;

					assert( refFrameNumberDis0_[frameNumberEnc]>=0 );
					assert( refFrameNumberDis1_[frameNumberEnc]>=0 );
				}
			}
		}
	}

	frameNumberDis-=(size_B-1); //correct increment
	for (; frameNumberDis<nFrames_; ++frameNumberDis){
		//P-frame
		++frameNumberEnc;
		frameNumberEnc2DisList_[frameNumberEnc]=frameNumberDis;
		frameNumberDis2EncList_[frameNumberDis]=frameNumberEnc;
		frameType_[frameNumberEnc]=P;
		refFrameNumberDis0_[frameNumberEnc]=frameNumberDis-1;

		assert( refFrameNumberDis0_[frameNumberEnc]>=0 );
	}

	//check if lists are OK
	for (int k=0; k<nFrames_; ++k){
		assert(frameNumberEnc2DisList_[k]>=0);
		assert(frameNumberDis2EncList_[k]>=0);
	}

	//create frame type data
	for (int frameNumberEnc=0; frameNumberEnc<nFrames_; ++frameNumberEnc){
		ftd_[frameNumberEnc].frameNumberDis		= frameNumberEnc2DisList_[frameNumberEnc];
		ftd_[frameNumberEnc].frameType			= frameType_[frameNumberEnc];
		ftd_[frameNumberEnc].refFrameNumberDis0 = refFrameNumberDis0_[frameNumberEnc];
		ftd_[frameNumberEnc].refFrameNumberDis1 = refFrameNumberDis1_[frameNumberEnc];
	}
}


//---------------------------------------------------------------------------------------------- 


LDFrameTypeData LDFrameOrderFactory::getFrameTypeData(int _frameNumberEnc){
	assert(_frameNumberEnc>=0);
	assert(_frameNumberEnc<nFrames_);
	printf("t:%d\tf:%d\t%d\t%d\n",ftd_[_frameNumberEnc].frameType, ftd_[_frameNumberEnc].frameNumberDis, ftd_[_frameNumberEnc].refFrameNumberDis0, ftd_[_frameNumberEnc].refFrameNumberDis1);
	return ftd_[_frameNumberEnc];
}
