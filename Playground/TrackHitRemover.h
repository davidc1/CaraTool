/**
 * \file TrackHitRemover.h
 *
 * \ingroup Playground
 * 
 * \brief Class def header for a class TrackHitRemover
 *
 * @author david caratelli
 */

/** \addtogroup Playground

    @{*/

#ifndef LARLITE_TRACKHITREMOVER_H
#define LARLITE_TRACKHITREMOVER_H

#include "Analysis/ana_base.h"
#include "DataFormat/track.h"
#include "DataFormat/shower.h"
#include "DataFormat/cluster.h"
#include "DataFormat/hit.h"
#include "ClusterParamsAlg.h"
#include <algorithm> // std::find

namespace larlite {
  /**
     \class TrackHitRemover
     User custom analysis class made by SHELL_USER_NAME
   */
  class TrackHitRemover : public ana_base{
  
  public:

    /// Default constructor
    TrackHitRemover();

    /// Default destructor
    virtual ~TrackHitRemover(){}

    virtual bool initialize();

    virtual bool analyze(storage_manager* storage);

    virtual bool finalize();

    /// Set producer name for tracks
    void setTrackProducer(std::string s) { _trackProducer = s; }

    /// Set producer name for showers
    void setShowerProducer(std::string s) { _showerProducer = s; }

    /// Set producer name for output hits
    void setOutHitProducer(std::string s) { _outHitProducer = s; }

    /// Hit Getter (for Track Hits)
    ::larlite::event_hit getTrkHits() { return _event_hit_trk; }

    /// Hit Getter (for Shower Hits)
    ::larlite::event_hit getShrHits() { return _event_hit_shr; }

    /// Ass Getter
    std::vector<std::vector<unsigned int> > getAss() { return _ass_v; }

    /// getter for vector of track-like hit indices
    std::vector<std::vector<unsigned int> > getTrkHitIndices() { return _trk_hit_indices; }

    /// getter for vector of shower-like hit indices
    std::vector<std::vector<unsigned int> > getShrHitIndices() { return _shr_hit_indices; }

    /// getter for vector of output hit indices
    std::vector<unsigned int> getOutHits() { return _out_hits; }

    /// Set Verbosity
    void setVerbose(bool on) { _verbose = on; }

    /// Set whether to save hits associated with showers
    void setGetShowerHits(bool on) { _getShowerHits = on; }

  protected:

    // verbosity boolean
    bool _verbose;

    // boolean to decide if to get hits ass. with showers
    bool _getShowerHits;

    // track producer name
    std::string _trackProducer;

    // shower producer name
    std::string _showerProducer;

    // output hit producer name
    std::string _outHitProducer;
    
    // hit vector (for track hits)
    ::larlite::event_hit _event_hit_trk;

    // hit vector (for shower hits)
    ::larlite::event_hit _event_hit_shr;

    // output hit index vector
    std::vector<unsigned int> _out_hits;

    // associated hit vector
    std::vector<std::vector<unsigned int> > _ass_v;

    /// vector of track-like hit indices
    std::vector<std::vector<unsigned int> > _trk_hit_indices;

    /// vector of shower-like hit indices
    std::vector<std::vector<unsigned int> > _shr_hit_indices;

    // ClusterParamsAlg instance
    ::cluster::ClusterParamsAlg _clusterAlg;
    
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
