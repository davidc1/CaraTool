/**
 * \file MicheldEdx.h
 *
 * \ingroup Playground
 * 
 * \brief Class def header for a class MicheldEdx
 *
 * @author david
 */

/** \addtogroup Playground

    @{*/

#ifndef LARLITE_MICHELDEDX_H
#define LARLITE_MICHELDEDX_H

#include "Analysis/ana_base.h"

#include "DataFormat/track.h"

namespace larlite {
  /**
     \class MicheldEdx
     User custom analysis class made by SHELL_USER_NAME
   */
  class MicheldEdx : public ana_base{
  
  public:

    /// Default constructor
    MicheldEdx();

    /// Default destructor
    virtual ~MicheldEdx(){}

    /** IMPLEMENT in MicheldEdx.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in MicheldEdx.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in MicheldEdx.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

    void AddMichel(const int& run, const int& event, const double& xpos, const double& zpos);

  protected:

    bool MatchTrack(const larlite::track& trk, const std::pair<double,double> mustop);

    TVector3 TrackEnd(const larlite::track& trk);

    std::map< std::pair<int,int>, std::pair<double,double> > _michel_map;

    TTree* _tree_dist;
    double _dw;
    double _dt;
    double _d;

    TTree* _tree_calo;
    double _rr;
    double _dend; // distance from end of track
    double _dqds;
    double _x, _y, _z;
    double _px, _py, _pz;
    
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
