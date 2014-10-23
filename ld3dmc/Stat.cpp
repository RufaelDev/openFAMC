/*===============================================*\
 * Copyright (C) 2006-2007 by Nikolce Stefanoski * 
\*===============================================*/

#include "Stat.h"
#include <numeric>
#include <algorithm>
#include<math.h>


//HISTO
void Stat::histo(const std::vector<int>& _v, std::vector<int>& _histo, int& _min){
    assert(!_v.empty());
    assert(_histo.empty());
    _min = *min_element(_v.begin(), _v.end());
    int max = *max_element(_v.begin(), _v.end());
    assert( _histo.empty() );
    _histo.resize(max-_min+1, 0);
    for(int k=0; k<(int) _v.size(); ++k){
	++_histo[ _v[k]-_min ];
    }
}

//PROB
void Stat::prob(const std::vector<int>& _v, std::vector<double>& _prob, int& _min){
    std::vector<int> myhisto;
    histo(_v, myhisto, _min);
    assert( !myhisto.empty() );
    assert( _prob.empty() );

    int sum=0;
    sum = accumulate(myhisto.begin(), myhisto.end(), sum);
    assert( sum>0 );
    for (int k=0; k<(int) myhisto.size(); ++k){
	_prob.push_back( 
		static_cast<double>(myhisto[k])/
		static_cast<double>(sum));
    }
}

//CDF
void Stat::cdf(const std::vector<int>& _v, std::vector<double>& _cdf, int& _min){
    std::vector<double> myprob;
    prob(_v, myprob, _min);
    assert( _cdf.empty() );
    assert( !myprob.empty() );
    
    double tmp=0;
    for (int k=0; k<(int) myprob.size(); ++k){
	tmp+=myprob[k];
	if (tmp>1.0) tmp=1.0;
	if (tmp<0.0) tmp=0.0;
	assert(tmp>=0.0);
	assert(tmp<=1);
	_cdf.push_back(tmp);
    }
}

//MEDIAN
int Stat::median(const std::vector<int>& _v){
    assert(!_v.empty());
    std::vector<double> mycdf;
    int min;
    cdf(_v, mycdf, min);
    for (int k=0; k<(int) mycdf.size(); k++){
	if (mycdf[k]>=0.5) return k+min;
    }
	return -1;		// why???
}

//ENTROPY
double Stat::entropy(const std::vector<int>& _v){
    //get probs
    std::vector<double> myprob;
    int min;
    prob(_v, myprob, min);
    
    //calculate entropy
    double ent=0;
    double lg=0;
    for (int k=0; k<(int) myprob.size(); k++){
	lg = (myprob[k]==0.0) ? 0.0 : log(myprob[k]);
	ent+=-myprob[k]*lg;
    }
    return ent/log(2.0);
}
