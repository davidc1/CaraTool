/**
 * \file CopyVtx.h
 *
 * \ingroup Playground
 * 
 * \brief Class def header for a class CopyVtx
 *
 * @author davidc1
 */

/** \addtogroup Playground

    @{*/

#ifndef LARLITE_COPYVTX_H
#define LARLITE_COPYVTX_H

#include "Analysis/ana_base.h"
#include "TVector3.h"

namespace larlite {
  /**
     \class CopyVtx
     User custom analysis class made by SHELL_USER_NAME
   */
  class CopyVtx : public ana_base{
  
  public:

    /// Default constructor
    CopyVtx(){ _name="CopyVtx"; _fout=0;}

    /// Default destructor
    virtual ~CopyVtx(){}

    virtual bool initialize();

    virtual bool analyze(storage_manager* storage);

    virtual bool finalize();

    void addEvent(const int run, const int event);
    
    void addVertex(const double x, const double y, const double z);

  protected:

    std::vector<std::pair<int,int> > RunEvent_v;
    std::vector< TVector3 > RunVertex_v;
    
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
