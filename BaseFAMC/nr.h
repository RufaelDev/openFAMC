#ifndef _NR_H_
#define _NR_H_
#include <fstream>
#include <complex>
#include "nrutil.h"
#include "nrtypes.h"
using namespace std;

namespace NR {
DP pythag(const DP a, const DP b);
void svdcmp(Mat_IO_DP &a, Vec_O_DP &w, Mat_O_DP &v);
}
#endif /* _NR_H_ */
