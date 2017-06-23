/**
 * \file VtxClustering.h
 *
 * \ingroup PolarClustering
 * 
 * \brief Class def header for a class VtxClustering
 *
 * @author david
 */

/** \addtogroup PolarClustering

    @{*/

#ifndef LARLITE_VTXCLUSTERING_H
#define LARLITE_VTXCLUSTERING_H

#include "Analysis/ana_base.h"
#include "LArUtil/GeometryHelper.h"
#include "DataFormat/hit.h"
#include "DataFormat/vertex.h"
#include "TTree.h"


namespace larlite {
  /**
     \class VtxClustering
     User custom analysis class made by SHELL_USER_NAME
   */
  class VtxClustering : public ana_base{
  
  public:

    /// Default constructor
    VtxClustering();

    /// Default destructor
    virtual ~VtxClustering(){}

    virtual bool initialize();

    virtual bool analyze(storage_manager* storage);

    virtual bool finalize();

    void setHitProducer(std::string s) { _hit_producer = s; }
    void setVtxProducer(std::string s) { _vtx_producer = s; }
    void setClusterProducer(std::string s) { _cluster_producer = s; }
    void setMaxVtxDist(double d) { _max_vtx_dist = d; }

    /**
       @brief Get Polar Hit Map
       @input int -> plane
     */
    std::map<size_t, std::pair<double,double> > getPolarMap(int pl) { return _HitMap_v[pl]; }

    /**
       @brief Get Clusters
       @input int -> plane
     */
    

    std::vector<double> getVtxW() { return vtx_w_cm; }
    std::vector<double> getVtxT() { return vtx_t_cm; }

  protected:

    // Math constants / Services:
    // Pi
    double PI;
    // Geometry Service
    const ::larutil::Geometry       *_geom;
    const ::larutil::GeometryHelper *_geomH;
    double _w2cm;
    double _t2cm;

    std::string _hit_producer;
    std::string _vtx_producer;
    std::string _cluster_producer;


    /**
       @brief vector of polar hit coordinates
     */
    std::vector< std::map< size_t, std::pair<double,double> > > _HitMap_v;
    

    std::vector<double> vtx_w_cm;
    std::vector<double> vtx_t_cm;

    // angular width of bin (radians)
    double _bin_width;
    double _bin_width_deg;
    size_t _n_bins;

    /**
       @brief Angle bins in the 3 planes
       @detail
       1st index -> plane
       2nd index -> angle bin
       3rd index -> hit index in ev_hit vector
     */
    std::vector< std::vector<std::vector<size_t> > > _PolarBin_v;

    /**
       @brief vector containing clustered vectors of indices, per plane
       @detail
       1st index -> plane
       2nd index -> cluster index
       3rd index -> hit index
     */
    std::vector< std::vector< std::vector<size_t> > > _PolarClusters_v;

    /**
       @brief Function to perform polar clustering
       @input vector of vector of hit indices. 1st index denotes the angle bin.
       @output vector of vector of output clusters
     */
    void FindPolarClusters();

    /**
       @brief project vertex onto the 3 planes
     */
    void projectVtx(const larlite::vertex vtx);

    /**
       @brief fill polar hit maps
       @input larlite event_hit object
       @output per-plane maps  of hit index -> polar coordinate pair 
     */
    void fillPolarHitMap(const std::vector<std::vector<unsigned int> > ass_cluster_hit_v,
			 const larlite::event_hit *ev_hit);

    /**
       @brief Bin by polar angle the hits in each plane
       @detail Use _HitMap_v to get the polar coordinates of each hit
       Then fill _PolarBin_v vector by using the angular information.
     */
    void PolarBinning();


    // threshold for number of hits @ a certain angle to identify a cluster
    size_t _thresh;

    // maximum distance from vertex for hits
    double _max_vtx_dist;

    // Add a Tree on which to save the polar, angular, charge amount per event
    TTree* _polar_tree;
    std::vector<double> _angle_v;
    std::vector<double> _radius_v;
    std::vector<double> _charge_v;
    std::vector<int>    _pl_v;

    
  };
}
#endif

//**************************************************************************
// 
// For Analysis framework documentation, read Manual.pdf here:
//
// http://microboone-docdb.fnal.gov:8080/cgi-bin/ShowDocument?docid=3183
//
//**************************************************************************

/** @} */ // end of doxygen group 
