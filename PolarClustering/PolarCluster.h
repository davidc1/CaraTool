/**
 * \file PolarCluster.h
 *
 * \ingroup PolarClustering
 * 
 * \brief Class def header for a class PolarCluster
 *
 * @author David Caratelli
 */

/** \addtogroup PolarClustering

    @{*/
#ifndef POLARCLUSTER_H
#define POLARCLUSTER_H

#include <iostream>
#include <vector>

/**
   \class PolarCluster
   User defined class PolarCluster ... these comments are used to generate
   doxygen documentation!
 */

class PolarCluster {

 public:

  PolarCluster(){};
  virtual ~PolarCluster(){};

  double _angle;
  double _angle_rms;
  std::pair<double,double> _angle_span;
  double _charge;
  std::vector<unsigned int> _hit_idx_v;
  std::vector<double> _hit_angle_v;
  std::vector<double> _hit_radius_v;
  std::vector<double> _hit_charge_v;
  std::pair<double,double> _origin; // (radius,angle) coordinates for hit in cluster with smallest radius
  
};

#endif
/** @} */ // end of doxygen group 

