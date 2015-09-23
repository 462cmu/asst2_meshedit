#ifndef STUDENT_CODE_H
#define STUDENT_CODE_H

#include "halfEdgeMesh.h"


using namespace std;

namespace CMU462 {

  class MeshResampler{

  public:

	MeshResampler(){};
	~MeshResampler(){}

	void mesh_up_sample  (HalfedgeMesh & half_edge_structure);
	void mesh_down_sample(HalfedgeMesh & half_edge_structure);
	void mesh_resample   (HalfedgeMesh & half_edge_structure);

	// One could add functions for other structures, such as winged edge.

  };


}

#endif // STUDENT_CODE_H
