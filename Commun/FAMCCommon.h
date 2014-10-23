

#ifndef FAMC_COMMON_H_INCLUDED__
#define FAMC_COMMON_H_INCLUDED__

#ifndef MAX_HHI
#define MAX_HHI(x,y) ((x)<(y)?(y):(x))
#endif

#ifndef MIN_HHI
#define MIN_HHI(x,y) ((x)<(y)?(x):(y))
#endif


#define CABAC_MOD     1      // enable/disable normative changes here
#define TRACE_SIG_MAP 1



#define CALC_NEW_PRED   1     // enable/disable new selection rule for the prediction here
#define PRED_QUANT_BITS 2     // number of bits for quantization of prediction dimension
#include<assert.h>



template<bool> class CompileTimeAOT;
template<>     class CompileTimeAOT<false> {};

#define CAOT(x) { CompileTimeAOT< (x)>(); }
#define CAOF(x) { CompileTimeAOT<!(x)>(); }

#define AOT(x) { if(  x ) assert(0); }
#define AOF(x) { if(!(x)) assert(0); }


template<class T>
class Vec3D
{
public:
  T data[3];
};

#endif