#ifndef VXDClusterParameters_h
#define VXDClusterParameters_h 1


#include "LCRTRelations.h"
#include "gear/GEAR.h"

/** ======= VXDClusterParameters ========== <br>
 * Holds cluster parameters for a VXD hit - to be attached at runtime to the SimTrackerHit.
 * Cluster is defined by two  (principal component) axis and the corresponding extensions (eigenvalues).
 * 
 * @version $Id: VXDClusterParameters.h,v 1.1 2009-05-15 15:53:08 gaede Exp $
 * @author F.Gaede, DESY
 */

class VXDClusterParameters { 
  
public:
  
  VXDClusterParameters() : 
    _cluAxisA(0,0,0),
    _cluAxisB(0,0,0) {
  }
  VXDClusterParameters(const gear::Vector3D& a, const gear::Vector3D& b) : 
    _cluAxisA(a),
    _cluAxisB(b) {
  }
  
  gear::Vector3D getClusterAxisA() {return _cluAxisA ; }
  gear::Vector3D getClusterAxisB() {return _cluAxisB ; }
  
  
protected:
  gear::Vector3D _cluAxisA ;
  gear::Vector3D _cluAxisB ;

} ;




/** Allows to use VXDClusterParameters as runtime extension object.
 */
struct ClusterParams: public lcio::LCOwnedExtension< ClusterParams, VXDClusterParameters >  {};

#endif



