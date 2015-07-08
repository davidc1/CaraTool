/**
 * \file VertexFinder.h
 *
 * \ingroup Playground
 * 
 * \brief Class def header for a class VertexFinder
 *
 * @author david caratelli
 */

/** \addtogroup Playground

    @{*/

#ifndef LARLITE_VERTEXFINDER_H
#define LARLITE_VERTEXFINDER_H

#include "Analysis/ana_base.h"
#include <utility>

namespace larlite {
  /**
     \class VertexFinder
     User custom analysis class made by SHELL_USER_NAME
   */
  class VertexFinder : public ana_base{
  
  public:

    /// Default constructor
    VertexFinder(){ _name="VertexFinder"; _fout=0; _verbose=false; }

    /// Default destructor
    virtual ~VertexFinder(){}

    virtual bool initialize();

    virtual bool analyze(storage_manager* storage);

    virtual bool finalize();

    /// Set the size of each cell
    void setCellSize(double d) { _cellSize = d; }
    /// Set the radius around which to search for hits
    void setRadius(double d) { _radius = d; }
    /// Set which plane to select hits from
    void setPlane(int pl) { _plane = pl; }
    /// Verbosity setter
    void setVerbose(bool on) { _verbose = on; }

  protected:

    /// verbosity flag
    bool _verbose;

    /// conversion factors for hits
    double _wire2cm, _time2cm;

    /// map connecting coordinate index (i,j) to [h1,h2,h3] (hit index list)
    std::map<std::pair<int,int>, std::vector<size_t> > _hitMap;

    /// size of each cell [cm]
    double _cellSize;

    /// radius to count charge around [cm]
    double _radius;
    
    /// plane to select hits from
    int _plane;

    /// Map making function
    void MakeHitMap(const event_hit* hitlist, int plane);

    /// find the best hit on a plane
    size_t GetBestHit(const event_hit* evt_hits, int plane);

    TTree* _vtx_tree;
    double _distU, _distV, _distY;
    double _distYZ, _distX, _dist3D;
    double _nu_x, _nu_y, _nu_z;
    double _xAvg, _yAvg, _zAvg;
    std::vector<double> _QvsTheta;
    int _nProtons;
    int _evt;
    double _area; // area spanned by triangle from projection of 3 points on YV plane

    // detector width
    double _detwidth;
    
  };
}
#endif

