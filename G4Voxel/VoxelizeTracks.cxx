#ifndef VOXELIZETRACKS_CXX
#define VOXELIZETRACKS_CXX

#include "VoxelizeTracks.h"

namespace vx {

  VoxelizeTracks::VoxelizeTracks(double step)
    : _voxel_tree(nullptr)
  {
    
    _step = step;
    
    _etot = _evox = 0;
    
    if (_voxel_tree) delete _voxel_tree;
    _voxel_tree = new TTree("voxel_tree","voxel tree");
    _voxel_tree->Branch("edep",&_edep,"edep/D");
    _voxel_tree->Branch("dx",&_dx,"dx/D");
    
  }
  
  void VoxelizeTracks::AddG4Step(const double& x0, const double& y0, const double& z0,
				 const double& x1, const double& y1, const double& z1,
				 const double& e) {


    if ( (x0 < 0) || (y0 < 0) || (z0 < 0) || (x1 < 0) || (y1 < 0) || (z1 < 0) ) {
      std::cout << "ERROR! MODULE CANNOT FUNCTION WITH NEGATIVE COORDINATES" << std::endl;
      return;
    }

    double efracadded = 0;

    // prepare map of used IDs for this step
    std::map<ID,bool> UsedIDs;

    _etot += e;

    if (_verbose) {
    std::cout << "start @ [ " << x0 << ", " << y0 << ", " << z0 << " ] \t"
	      << "end   @ [ " << x1 << ", " << y1 << ", " << z1 << " ] \t"
	      << "Energy : " << e << std::endl;
    }

    // indices of start point voxel
    int i0 = x0 / _step;
    int j0 = y0 / _step;
    int k0 = z0 / _step;

    // indices of end-point voxel
    int i1 = x1 / _step;
    int j1 = y1 / _step;
    int k1 = z1 / _step;

    if (_verbose) {

      std::cout << "ID start : [ " << i0 << ", " << j0 << ", " << k0 << " ] \t"
		<< "end : [ " << i1 << ", " << j1 << ", " << k1 << " ]" << std::endl;
      
    }
    
    // are the points in the same voxel?
    // if yes add charge to voxel and quit
    if ( (i0==i1) && (j0==j1) && (k0==k1) ) {
      if (_verbose) {
	std::cout << "All energy contained in voxel [ " << i0 << ", " << j0 << ", " << k0 << " ]"
		  << "\t adding energy : " << e << std::endl;
      }
      getVoxel(i0,j0,k0).AddEnergy(e);
      _evox += e;
      return;
    }
    
    // create a LineSegment representing this step
    ::geoalgo::LineSegment_t step(x0,y0,z0,x1,y1,z1);
    
    // full step length
    double stepLength = step.Start().Dist( step.End() );

    // keep track of whether 1st and last voxels have been added
    bool addStart = false;
    bool addEnd   = false;
    
    // get slope in each direction
    double dx = x1-x0;
    double dy = y1-y0;
    double dz = z1-z0;

    double mag = sqrt(dx*dx + dy*dy + dz*dz);

    dx /= mag;
    dy /= mag;
    dz /= mag;
    
    int itmp = i0;
    int jtmp = j0;
    int ktmp = k0;

    double xtmp = x0;
    double ytmp = y0;
    double ztmp = z0;

    // get list of IDs in which to search for energy deposition
    // by this step
    std::vector<ID> id_v;
    
    while ( (itmp != i1) || (jtmp != j1) || (ktmp != k1) ) {

      if (_verbose)
	std::cout << "creating new ID : [ " << itmp << ", " << jtmp << ", " << ktmp << " ]"
		  << " @ index " << id_v.size() 
		  << std::endl;
      
      id_v.push_back( ID(itmp,jtmp,ktmp) );

      xtmp += _step * dx;
      ytmp += _step * dy;
      ztmp += _step * dz;

      // get distance from start. if larger than stepLength -> quit (we've passed our final voxel)
      double d = sqrt( (xtmp-x0) * (xtmp-x0) + (ytmp-y0) * (ytmp-y0) + (ztmp-z0) * (ztmp-z0) );
      if (d > stepLength) break;

      itmp = (int)(xtmp / _step);
      jtmp = (int)(ytmp / _step);
      ktmp = (int)(ztmp / _step);
      
    }

    // add last voxel as well
    id_v.push_back( ID(i1,j1,k1) );

    // now make sure we didn't miss any cells
    std::vector<ID> intermediate_voxels_v;

    if (_verbose)
      std::cout << "id_v size : " << id_v.size() << std::endl;
    
    for (size_t n=0; n < id_v.size() -1 ; n++) {

      auto intermediate_voxels = getIntermediateVoxels( id_v[n], id_v[n+1] );

      for (auto const& iv : intermediate_voxels)
	intermediate_voxels_v.push_back( iv );
      
    }// for all contiguous voxel-pairs

    // add all intermediate voxels to id_v
    for (auto const& iv : intermediate_voxels_v)
      id_v.push_back( iv );
    
    // loop through all voxels
    // measure segment length in the given voxel
    // and assign energy appropriately
    for (auto const& voxelID : id_v) {

      if (_verbose) {
	std::cout << " @ voxel ";
	voxelID.Print();
	std::cout << std::endl;
      }

      if (UsedIDs.find(voxelID) != UsedIDs.end() ) continue;
      UsedIDs[voxelID] = true;
      
      // get current voxel
      auto thisVoxel = getVoxel(voxelID);
      
      // get AABox matching this voxel
      auto AAVoxel = getAAVoxel(thisVoxel);

      if (_verbose)
	std::cout << "AA min : " << AAVoxel.Min() << "\t AA max : " << AAVoxel.Max() << std::endl;
      
      // get the intersection length between this voxel and the segment
      auto points = _geoAlgo.Intersection(step,AAVoxel);

      if (_verbose)
	std::cout << "there are " << points.size() << " intersections" << std::endl;

      // if a single intersection: check if 1st and last point
      // is in this voxel. if so then add the energy for this segment
      if (points.size() == 1) {

	if ( (AAVoxel.Contain(step.Start()) == true) && (addStart == false) ) {
	  double efrac = points[0].Dist(step.Start()) / stepLength;
	  if (_verbose) 
	    std::cout << "\t intersection length : " << points[0].Dist(step.Start())
		      << "\t add frac : " << efrac << std::endl;
	  thisVoxel.AddEnergy( e * efrac );
	  efracadded += efrac;
	  _evox += (e*efrac);
	  _edep = e*efrac;
	  _dx   = points[0].Dist(step.Start());
	  _voxel_tree->Fill();
	  addStart = true;
	}

	if ( (AAVoxel.Contain(step.End()) == true) && (addEnd == false) ) {
	  double efrac = points[0].Dist(step.End()) / stepLength;
	  if (_verbose) 
	    std::cout << "\t intersection length : " << points[0].Dist(step.End())
		      << "\t add frac : " << efrac << std::endl;
	  thisVoxel.AddEnergy( e * efrac );
	  efracadded += efrac;
	  _evox += (e*efrac);
	  _edep = e*efrac;
	  _dx   = points[0].Dist(step.End());
	  _voxel_tree->Fill();
	  addEnd = true;
	}
      }// if a single intersection point
	
      // if segment fully traverses the voxel
      // figure out the length of the segment
      // in the voxel and add energy appropriately
      if (points.size() == 2) {
	
	// fraction of energy to assign to this voxel
	// (equal to fraction of length of segment in the voxel)
	double efrac = points[0].Dist(points[1]) / stepLength;
	thisVoxel.AddEnergy( e * efrac );
	efracadded += efrac;
	_evox += (e*efrac);
	_edep = e*efrac;
	_dx   = points[0].Dist(points[1]);
	_voxel_tree->Fill();

	if (_verbose) {
	  std::cout << "\t intersection length : " << points[0].Dist(points[1])
		    << "\t add frac : " << efrac << std::endl;
	}
	
      }// if segment fully crosses the voxel
      
    }

    //if (_verbose)
    //std::cout << "ADDED " << efracadded << " frac of total energy " << std::endl;

    return;
  }
  
  Voxel& VoxelizeTracks::getVoxel(const double& x, const double& y, const double& z) {
    
    int i = x/_step;
    int j = x/_step;
    int k = x/_step;
    
    return getVoxel(i,j,k);
  }
  
  Voxel& VoxelizeTracks::getVoxel(const int& i, const int& j, const int& k) {
    
    ID id(i,j,k);

    return getVoxel(id);
  }

  Voxel& VoxelizeTracks::getVoxel(const ID& id) {

    // if the voxel already exists -> grab it!
    if ( _voxelMap.find( id ) != _voxelMap.end() ) {
      return _voxelMap[id];
    }

    // did not already find voxel -> make a new one
    Voxel vox(id.geti(),id.getj(),id.getk(),_step);
    _voxelMap[id] = vox;

    return _voxelMap[id];
  }
  
  ::geoalgo::AABox_t VoxelizeTracks::getAAVoxel(const Voxel& thisVoxel) {

    return ::geoalgo::AABox_t( thisVoxel.x() - thisVoxel.HalfLength(),
			       thisVoxel.y() - thisVoxel.HalfLength(),
			       thisVoxel.z() - thisVoxel.HalfLength(),
			       thisVoxel.x() + thisVoxel.HalfLength(),
			       thisVoxel.y() + thisVoxel.HalfLength(),
			       thisVoxel.z() + thisVoxel.HalfLength() );
    
  }

  std::vector<ID> VoxelizeTracks::getIntermediateVoxels(const ID& id1, const ID& id2) {

    // get vect of index differences between id1 and id2:
    //auto posdiff = id1.PositionDiff(id2);

    int aaa = id2.geti();

    aaa += 1;
    
    int idiff = id2.geti() - id1.geti();
    int jdiff = id2.getj() - id1.getj();
    int kdiff = id2.getk() - id1.getk();

    // return list of voxels needed to directly link to voxels
    std::vector<ID> intermediateIDs;

    std::vector<int> posdiff = {idiff,jdiff,kdiff};


    // count number of indices that are different:
    int ndiff = 0;
    for (auto const& d : posdiff) {
      if (d != 0) ndiff += 1;
      // what if we skip 2 steps!!
      if ( (d > 1) || (d < -1) ) {
	return intermediateIDs;
      }
    }
    
    // if only one index difference -> they are directly connected
    if (ndiff == 1)
      return intermediateIDs;

    // if two  indices are different
    if (ndiff == 2) {

      if (posdiff[0] == 0) {
	intermediateIDs.push_back( ID(id1.geti(), id1.getj() + posdiff[1], id1.getk() ) );
	intermediateIDs.push_back( ID(id1.geti(), id1.getj(), id1.getk() + posdiff[2] ) );
      }

      if (posdiff[1] == 0) {
	intermediateIDs.push_back( ID(id1.geti() + posdiff[0], id1.getj(), id1.getk() ) );
	intermediateIDs.push_back( ID(id1.geti(), id1.getj(), id1.getk() + posdiff[2] ) );
      }

      if (posdiff[2] == 0) {
	intermediateIDs.push_back( ID(id1.geti() + posdiff[0], id1.getj(), id1.getk() ) );
	intermediateIDs.push_back( ID(id1.geti(), id1.getj() + posdiff[1], id1.getk() ) );
      }
      
    }// if 2-step difference

    // if all 3 indices are different
    if (ndiff == 3) {

      intermediateIDs.push_back( ID(id1.geti() + posdiff[0], id1.getj(), id1.getk() ) );
      intermediateIDs.push_back( ID(id1.geti(), id1.getj() + posdiff[1], id1.getk() ) );
      intermediateIDs.push_back( ID(id1.geti(), id1.getj(), id1.getk() + posdiff[2] ) );

      intermediateIDs.push_back( ID(id1.geti() + posdiff[0], id1.getj() + posdiff[1], id1.getk() ) );
      intermediateIDs.push_back( ID(id1.geti() + posdiff[0], id1.getj(), id1.getk() + posdiff[2] ) );
      intermediateIDs.push_back( ID(id1.geti(), id1.getj() + posdiff[1], id1.getk() + posdiff[2] ) );
      
    }// if 3-step difference

    return intermediateIDs;
  }


  double VoxelizeTracks::applyThreshold(const double& emin) {

    // loop over voxels
    // if above threshold -> add
    // if below and neighbors in x (i.e. time-coordinate) also below
    // -> do not add

    double etotal  = 0;
    double ethresh = 0;
    
    for (auto it = _voxelMap.begin(); it != _voxelMap.end(); it++) {

      auto const& voxel = it->second;

      etotal += voxel.Energy();

      //_edep  = voxel.Energy();
      //_voxel_tree->Fill();

      if (voxel.Energy() > emin)
	{
	  ethresh += voxel.Energy();
	  continue;
	}

      // if the voxel itself does not have enough energy, check the neighbors
      
      // does a neighbor in x pass the condition?
      bool neighbor = false;
      
      // check neighbords in x
      
      ID idlowX( it->first.geti()-1, it->first.getj(), it->first.getk() );
      if (_voxelMap.find(idlowX) != _voxelMap.end() ) {
	if (_voxelMap[idlowX].Energy() + voxel.Energy() > emin)
	  neighbor = true;
      }
      
      ID idhighX( it->first.geti()+1, it->first.getj(), it->first.getk() );
      if (_voxelMap.find(idhighX) != _voxelMap.end() ) {
	if (_voxelMap[idhighX].Energy() + voxel.Energy() > emin)
	  neighbor = true;
      }

      // does a neighbor in y pass the condition?
      ID idlowY( it->first.geti(), it->first.getj()-1, it->first.getk() );
      if (_voxelMap.find(idlowY) != _voxelMap.end() ) {
	if (_voxelMap[idlowY].Energy() + voxel.Energy() > emin)
	  neighbor = true;
      }
      
      ID idhighY( it->first.geti(), it->first.getj()+1, it->first.getk() );
      if (_voxelMap.find(idhighY) != _voxelMap.end() ) {
	if (_voxelMap[idhighY].Energy() + voxel.Energy() > emin)
	  neighbor = true;
      }
      
      if (neighbor == true) {
	//std::cout << "neighbor above threshold!" << std::endl;
	ethresh += voxel.Energy();
      }

    }// for all voxels in event
    
    
    return ethresh/etotal;
  }

  void VoxelizeTracks::WriteTree() {

    TFile* file = new TFile("voxelize.root","NEW");
    
    file->cd();
    _voxel_tree->Write();
    file->Close();
    
  }

}

#endif
