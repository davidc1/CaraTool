/**
 * \file TruncMean.h
 *
 * \ingroup 3DMichel
 * 
 * \brief Class def header for a class TruncMean
 *
 * @author david caratelli
 */

/** \addtogroup 3DMichel

    @{*/
#ifndef TRUNCMEAN_H
#define TRUNCMEAN_H

#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>

/**
   \class TruncMean
   User defined class TruncMean ... these comments are used to generate
   doxygen documentation!
 */
class TruncMean{

public:

  /// Default constructor
  TruncMean(){}

  /// Default destructor
  ~TruncMean(){}

  void CalcTruncMean(const std::vector<double>& rr_v, const std::vector<double>& dq_v,
		     std::vector<double>& dq_trunc_v);

  double Median(const std::vector<double>& v);
  double RMS(const std::vector<double>& v);

  double _rad;

};

#endif
/** @} */ // end of doxygen group 

