/**
 * \file IsolatedEndPoint.h
 *
 * \ingroup 3DMichel
 * 
 * \brief Class def header for a class IsolatedEndPoint
 *
 * @author david
 */

/** \addtogroup 3DMichel

    @{*/

#ifndef LARLITE_ISOLATEDENDPOINT_H
#define LARLITE_ISOLATEDENDPOINT_H

#include "Analysis/ana_base.h"

#include "TTree.h"

namespace larlite {
  /**
     \class IsolatedEndPoint
     User custom analysis class made by SHELL_USER_NAME
   */
  class IsolatedEndPoint : public ana_base{
  
  public:

    /// Default constructor
    IsolatedEndPoint()
      : _tree(nullptr)
      { _name="IsolatedEndPoint"; _fout=0;}

    /// Default destructor
    virtual ~IsolatedEndPoint(){}

    /** IMPLEMENT in IsolatedEndPoint.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in IsolatedEndPoint.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in IsolatedEndPoint.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

  protected:

    double _w2cm, _t2cm;

    TTree* _tree;
    double _x, _y, _z;
    double _q;
    double _dmin;
    
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
