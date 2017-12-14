/**
 * \file AddClusters.h
 *
 * \ingroup HandClusterInput
 * 
 * \brief Class def header for a class AddClusters
 *
 * @author david
 */

/** \addtogroup HandClusterInput

    @{*/

#ifndef LARLITE_ADDCLUSTERS_H
#define LARLITE_ADDCLUSTERS_H

#include "Analysis/ana_base.h"

namespace larlite {
  /**
     \class AddClusters
     User custom analysis class made by SHELL_USER_NAME
   */
  class AddClusters : public ana_base{
  
  public:

    /// Default constructor
    AddClusters(){ _name="AddClusters"; _fout=0;}

    /// Default destructor
    virtual ~AddClusters(){}

    /** IMPLEMENT in AddClusters.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in AddClusters.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in AddClusters.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

    void clearHits() { _hit_idx_v.clear(); _vtx_hits.clear(); }
    void addHit(int run, int evt, int shr, int hitidx)
    {
      // does this event exist? if not add it
      std::pair<int,int> evtinfo(run,evt);
      if (_hit_idx_v.find( evtinfo ) == _hit_idx_v.end() ) {
	_hit_idx_v[ evtinfo ] = std::vector< std::pair<int, unsigned int> >();
	_vtx_hits[ evtinfo ] = std::vector<int>();
      }
      if ((shr < 3) && (shr>0)) { _hit_idx_v[evtinfo].push_back( std::make_pair(shr-1,hitidx) ); }
      if (shr==0) { _vtx_hits[evtinfo].push_back(hitidx); }
    }

  protected:

    std::map< std::pair<int,int>, std::vector< std::pair<int,unsigned int> > > _hit_idx_v;
    std::map< std::pair<int,int>, std::vector<int> > _vtx_hits;
    
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
