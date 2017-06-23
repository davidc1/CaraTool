/**
 * \file ClusterMergeBase.h
 *
 * \ingroup PolarClustering
 * 
 * \brief Class def header for a class ClusterMergeBase
 *
 * @author david
 */

/** \addtogroup PolarClustering

    @{*/
#ifndef CLUSTERMERGEBASE_H
#define CLUSTERMERGEBASE_H

#include <iostream>

#include "PolarCluster.h"

/**
   \class ClusterMergeBase
   User defined class ClusterMergeBase ... these comments are used to generate
   doxygen documentation!
 */
class ClusterMergeBase{

public:

  /// Default constructor
  ClusterMergeBase(){}

  /// Default destructor
  ~ClusterMergeBase(){}

  bool Merge(const PolarCluster& c1, const PolarCluster& c2);

};

#endif
/** @} */ // end of doxygen group 

