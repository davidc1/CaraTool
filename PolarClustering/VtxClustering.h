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

    std::map<size_t, std::pair<double,double> > getUMappol() { return _Umappol; }
    std::map<size_t, std::pair<double,double> > getVMappol() { return _Vmappol; }
    std::map<size_t, std::pair<double,double> > getYMappol() { return _Ymappol; }

    std::map<size_t, std::pair<double,double> > getUMapcm() { return _Umapcm; }
    std::map<size_t, std::pair<double,double> > getVMapcm() { return _Vmapcm; }
    std::map<size_t, std::pair<double,double> > getYMapcm() { return _Ymapcm; }

    std::vector<double> getVtxW() { return vtx_w_cm; }
    std::vector<double> getVtxT() { return vtx_t_cm; }

    bool _has_vtx;

  protected:

    std::string _hit_producer;
    std::string _vtx_producer;

    // map of idx -> (r,theta) for U plane
    std::map<size_t, std::pair<double,double> > _Umappol;
    // map of idx -> (r,theta) for V plane
    std::map<size_t, std::pair<double,double> > _Vmappol;
    // map of idx -> (r,theta) for Y plane
    std::map<size_t, std::pair<double,double> > _Ymappol;

    // map of idx -> (r,theta) for U plane
    std::map<size_t, std::pair<double,double> > _Umapcm;
    // map of idx -> (r,theta) for V plane
    std::map<size_t, std::pair<double,double> > _Vmapcm;
    // map of idx -> (r,theta) for Y plane
    std::map<size_t, std::pair<double,double> > _Ymapcm;

    std::vector<double> vtx_w_cm;
    std::vector<double> vtx_t_cm;

    // angular width of bin (radians)
    double _bin_width;
    double _bin_width_deg;

    // angle bins in the 3 planes
    // 1st index is anlge bin
    // 2nd index is hit index in ev_hit vector
    std::vector<std::vector<size_t> > _binU, _binV, _binY;

    
    // cluster in polar angle
    void FindPolarClusters(const std::vector<std::vector<size_t> > polarHits,
			   std::vector<std::vector<size_t> > &polarClusters);


    // threshold for number of hits @ a certain angle to identify a cluster
    size_t _thresh;
      
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
