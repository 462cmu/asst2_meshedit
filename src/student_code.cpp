/*
 * Student Code location.
 *
 * Implemented by ____ on ____.
 *
 * Purpose: this class provides a steril laboratory for students to
 * perform mesh operations in.
 *
 * TASKS:
 */

#include "student_code.h"

namespace CMU462
{

  // TASK 1 : Edge Flipping.

  void HalfedgeMesh::flipEdge( EdgeIter e0 )
  {
    return;
  }

  // TASK 2 :: Edge Splitting.
  void HalfedgeMesh::splitEdge( EdgeIter e0 )
  {
    return;
  }

  // Task 3 :: Edge Collapsing.
  void HalfedgeMesh::collapseEdge( EdgeIter e )
  {
    return;
  }


  // TASK 4 : Up Sampling, More Triangles!!!
  void MeshResampler::mesh_up_sample(HalfedgeMesh & half_edge_structure)
  {
	// TODO : Please Implement Me!
	cout << "[Student_code] mesh_up_sample\n";
  }

  // TASK 5: Down sampling, Less Triangles!!!
  void MeshResampler::mesh_down_sample(HalfedgeMesh & half_edge_structure)
  {
	// TODO : Please Implement Me!
	cout << "[Student_code] mesh_down_sample\n";
  }

  // TASK 6: Re Sampling, Better Triangles!!!
  void MeshResampler::mesh_resample(HalfedgeMesh & half_edge_structure)
  {
	// TODO : Please Implement Me!
	cout << "[Student_code] mesh_resample\n";
  }

}
