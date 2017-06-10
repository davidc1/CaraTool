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

  /**
     @brief Given residual range and dq vectors return truncated local dq.
     Input vectors are assumed to be match pair-wise (nth entry in rr_v
     corresponds to nth entry in dq_v vector).
     Input rr_v values are also assumed to be ordered: monotonically increasing
     or decreasing.
     For every dq value a truncated linear dq value is calculated as follows:
     0) all dq values within a rr range set by the class variable _rad are selected.
     1) the median and rms of these values is calculated.
     2) the subset of local dq values within the range [median-rms, median+rms] is selected.
     3) the resulting local truncated dq is the average of this truncated subset.
   */
  void CalcTruncMean(const std::vector<double>& rr_v, const std::vector<double>& dq_v,
		     std::vector<double>& dq_trunc_v);

  double Median(const std::vector<double>& v);
  double RMS(const std::vector<double>& v);

  double _rad;

};

#endif
/** @} */ // end of doxygen group 

