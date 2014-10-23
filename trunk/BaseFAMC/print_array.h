#ifndef _PRINT_ARR_
#define _PRINT_ARR_

#include <iostream>
#include <iomanip>
#include "nrutil.h"
using namespace std;

template<class T>
void print_array(const NRVec<T> &a, const int n_per_row, const int col_width)
{
	int i,j=0;

	int n_elements=a.size();
	for (i=0; i < n_elements; i++) {
		if (j == n_per_row) {
			cout << endl;
			j=0;
		}
		cout << setw(col_width) << a[i];
		j++;
	}
	cout << endl;
}

#endif /* _PRINT_ARR_ */
