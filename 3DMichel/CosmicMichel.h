/**
 * \file CosmicMichel.h
 *
 * \ingroup 3DMichel
 * 
 * \brief Class def header for a class CosmicMichel
 *
 * @author david caratelli
 */

/** \addtogroup 3DMichel

    @{*/

#ifndef LARLITE_COSMICMICHEL_H
#define LARLITE_COSMICMICHEL_H

#include "Analysis/ana_base.h"
#include "TTree.h"
#include "TruncMean.h"
#include "DataFormat/hit.h"
#include "TVector3.h"

namespace larlite {
  /**
     \class CosmicMichel
     User custom analysis class made by SHELL_USER_NAME
   */
  class CosmicMichel : public ana_base{
  
  public:

    /// Default constructor
    CosmicMichel()
      : _tree_rc(nullptr)
      , _tree_mc(nullptr)
      { _name="CosmicMichel"; _fout=0;}

    /// Default destructor
    virtual ~CosmicMichel(){}

    /** IMPLEMENT in CosmicMichel.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in CosmicMichel.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in CosmicMichel.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

    void useTruth(bool on) { _truth = on; }

  protected:

    TTree *_tree_rc;
    std::vector<double> _x_v, _y_v, _z_v;
    std::vector<float> _dqdx_v, _dqdx_trunc_v;
    std::vector<float> _rr_v;
    double _xe_rc, _ye_rc, _ze_rc;
    double _xs_rc, _ys_rc, _zs_rc;
    double _px, _py, _pz;
    double _dmin; // distance to nearest true muon end point (2D)
    double _tmin; // time to closest flash
    double _rr_max;
    double _qnear;

    TTree *_tree_mc;
    double _xe_mc, _ye_mc, _ze_mc;
    double _energy_mc;

    TruncMean _tmean;

    bool _truth;

    double _w2cm, _t2cm;

    // create a map which connects position (x,z) in 10-cm blocks to hit index
    std::map< std::pair<int,int>,std::vector<int> > _hitmap;
    void ResetHitMap(larlite::event_hit* ev_hit);
    // find charge near end of track not belonging to track
    double Qtot(const double& rmax, const TVector3& end, const std::vector<unsigned int>& ass_hit_v, larlite::event_hit* ev_hit);
    
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
