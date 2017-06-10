#ifndef TRUNCMEAN_CXX
#define TRUNCMEAN_CXX

#include "TruncMean.h"

void TruncMean::CalcTruncMean(const std::vector<double>& rr_v, const std::vector<double>& dq_v,
			      std::vector<double>& dq_trunc_v)
{

  // how many points to sample 
  int Nneighbor = (int)(_rad * 3 * 2);

  dq_trunc_v.clear();
  dq_trunc_v.reserve( rr_v.size() );

  int Nmax = dq_v.size()-1;
  
  for (size_t n=0; n < dq_v.size(); n++) {

    // current residual range
    double rr = rr_v.at(n);

    int nmin = n - Nneighbor;
    int nmax = n + Nneighbor;

    if (nmin < 0) nmin = 0;
    if (nmax > Nmax) nmax = Nmax;

    // vector for local dq values
    std::vector<double> dq_local_v;

    for (size_t i=nmin; i < nmax; i++) {
      
      double dr = rr - rr_v[i];
      if (dr < 0) dr *= -1;

      if (dr > _rad) continue;

      dq_local_v.push_back( dq_v[i] );
      
    }// for all ticks we want to scan

    if (dq_local_v.size() == 0) {
      dq_trunc_v.push_back( dq_v.at(n) );
      continue;
    }
    
    // calculate median and rms
    double median = Median(dq_local_v);
    double rms    = RMS(dq_local_v);

    double truncated_dq = 0.;
    int npts = 0;
    for (auto const& dq : dq_local_v) {
      if ( ( dq < (median+rms) ) && ( dq > (median-rms) ) ){
	truncated_dq += dq;
	npts += 1;
      }
    }

    dq_trunc_v.push_back( truncated_dq / npts );
  }// for all values

  return;
}


double TruncMean::Median(const std::vector<double>& v)
{

  if (v.size() == 1) return v[0];
  
  std::vector<double> vcpy = v;

  std::sort(vcpy.begin(), vcpy.end());

  double median = vcpy[ vcpy.size() / 2 ];

  return median;
}

double TruncMean::RMS(const std::vector<double>& v)
{

  double avg = 0.;
  for (auto const& val : v) avg += val;
  avg /= v.size();
  double rms = 0.;
  for (auto const& val : v) rms += (val-avg)*(val-avg);
  rms = sqrt( rms / ( v.size() -  1 ) );

  return rms;
}

#endif
