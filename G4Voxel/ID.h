/**
 * \file ID.h
 *
 * \ingroup G4Voxel
 * 
 * \brief Class def header for a class ID
 *
 * @author david caratelli
 */

/** \addtogroup G4Voxel

    @{*/
#ifndef ID_H
#define ID_H

#include <iostream>
#include <vector>

/**
   \class ID
   User defined class ID ... these comments are used to generate
   doxygen documentation!
 */

namespace vx {
  
  class ID{
    
  public:


    /// Default constructor
    ID(){_i = _j = _k = 0;}
    
    ID(int i, int j, int k) { _i=i; _j=j; _k=k; }

    void Print() const { std::cout << "[ " << _i << ", " << _j << ", " << _k << " ]"; }
    
    /// Default destructor
    ~ID(){}

    int geti() const { return _i; }
    int getj() const { return _j; }
    int getk() const { return _k; }


    bool operator < (const ID& id2) const {
      
      if ( ( this->geti() == id2.geti() ) && ( this->getj() == id2.getj() ) && ( this->getk() == id2.getk() ) )
	return false;
      
      // compare i
      if      ( this->geti() < id2.geti() ) return true;
      else if ( this->geti() > id2.geti() ) return false;
      // compare j
      else {
	if      ( this->getj() < id2.getj() ) return true;
	else if ( this->getj() > id2.getj() ) return false;
	// compare k
	else {
	  if      ( this->getk() < id2.getk() ) return true;
	  else if ( this->getk() > id2.getk() ) return false;
	}
      }
    
      return false;
  }
    

  private:
    
    int _i;
    int _j;
    int _k;    


  };

  /*
  inline bool operator < (const ID& id1, const ID& id2) {
    
    if ( ( id1.geti() == id2.geti() ) && ( id1.getj() == id2.getj() ) && ( id1.getk() == id2.getk() ) )
      return false;
    
    // compare i
    if      ( id1.geti() < id2.geti() ) return true;
    else if ( id1.geti() > id2.geti() ) return false;
    // compare j
    else {
      if      ( id1.getj() < id2.getj() ) return true;
      else if ( id1.getj() > id2.getj() ) return false;
      // compare k
      else {
	if      ( id1.getk() < id2.getk() ) return true;
	else if ( id1.getk() > id2.getk() ) return false;
      }
    }
    
    return false;
  }
  */
  
}

#endif
/** @} */ // end of doxygen group 

