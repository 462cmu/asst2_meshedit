The Halfedge Datastructure                         {#mainpage}
============

# The Halfedge Datastructure

A HalfedgeMesh is a data structure that makes it easy to iterate over (and
modify) a polygonal mesh.  The basic idea is that each edge of the mesh
gets associated with two "halfedges," one on either side, that point in
opposite directions.  These halfedges essentially serve as the "glue"
between different mesh elements (vertices, edges, and faces).  A half edge
mesh has the same basic flavor as a tree or linked list data structure:
each node has pointers that reference other nodes.  In particular, each
half edge points to:

* its root vertex,
* its associated edge,
* the face it sits on,
* its "twin", i.e., the halfedge on the other side of the edge,
* and the next halfedge in cyclic order around the face.

Vertices, edges, and faces each point to just one of their incident
halfedges.  For instance, an edge will point arbitrarily to either
its "left" or "right" halfedge.  Each vertex will point to one of
many halfedges leaving that vertex.  Each face will point to one of
many halfedges going around that face.  The fact that these choices
are arbitrary does not at all affect the practical use of this data
structure: they merely provide a starting point for iterating over
the local region (e.g., walking around a face, or visiting the
neighbors of a vertex).  A practical example of iterating around a
face might look like:
~~~~~~~~~~~~~{.c}
   HalfEdgeIter h = myFace->halfedge();
   do
   {
      // do something interesting with h
      h = h->next();
   }
   while( h != myFace->halfEdge() );
~~~~~~~~~~~~~
At each iteration we walk to the "next" halfedge, until we return
to the original starting point.  A slightly more interesting
example is iterating around a vertex:
~~~~~~~~~~~~~{.c}
   HalfEdgeIter h = myVertex->halfedge();
   do
   {
      // do something interesting with h
      h = h->twin()->next();
   }
   while( h != myVertex->halfedge() );
~~~~~~~~~~~~~
(Can you draw a picture that explains this iteration?)  A very
different kind of iteration is when we want to iterate over, say,
*all* the edges of a mesh:
~~~~~~~~~~~~~{.c}
   for( EdgeIter e = mesh.edges.begin(); e != mesh.edges.end(); e++ )
   {
      // do something interesting with e
   }
~~~~~~~~~~~~~
A very important consequence of the halfedge representation is that
---by design---it can only represent manifold, orientable triangle
meshes.  I.e., every point should have a neighborhood that looks disk-
like, and you should be able to assign to each polygon a normal
direction such that all these normals "point the same way" as you walk
around the surface.

At a high level, that's all there is to know about the half edge
data structure.  But it's worth making a few comments about how this
particular implementation works---especially how things like boundaries
are handled.  First and foremost, the "pointers" used in this
implementation are actually STL iterators.  STL stands for the "standard
template library," and is a basic part of C++ that provides some very
convenient and powerful data structures and algorithms---if you've never
looked at STL before, now would be a great time to get familiar!  At
a high level, STL iterators behave a lot like pointers: they don't store
data, but rather reference some data that is allocated elsewhere.  And
the syntax is also very similar; for instance, if p is an iterator, then
*p yields the value referred to by p.  (As for the rest, Google is a
terrific resource! :-))

Rather than accessing raw iterators, the HalfedgeMesh encapsulates these
pointers using methods like Halfedge::twin(), Halfedge::next(), etc.  The
reason for this encapsulation (as in most object-oriented programming)
is that it allows the user to make changes to the internal representation
later down the line.  For instance, if you know that the connectivity of
the mesh is never going to change, you might be able to improve performance
by (internally) replacing the linked lists with fixed-length arrays,
without breaking any code that might have been written using the abstract
interface.  (There are deeper reasons for this kind of encapsulation
when working with polygon meshes, but that's a story for another time!)

Finally, some surfaces have "boundary loops," e.g., a pair of pants has
three boundaries: one at the waist, and two at the ankles.  These boundaries
are represented by special faces in our halfedge mesh---in fact, rather than
being stored in the usual list of faces (HalfedgeMesh::faces), they are
stored in a separae list of boundary loops (HalfedgeMesh::boundaries).  Each
face (boundary or regular) also stored a flag Face::_isBoundary that
indicates whether or not it is a boundary.  This value can be queried via the
public method Face::isBoundary() (again: encapsulation!)  So for instance, if
I wanted to know the area of all polygons that touch a given vertex, I might
write some code like this:
~~~~~~~~~~~~~{.c}
   double totalArea = 0.;
   HalfEdgeIter h = myVertex->halfedge();
   do
   {
      // don't add the area of boundary faces!
      if( !h->face()->isBoundary() )
      {
         totalArea != h->face()->area();
      }
      h = h->twin()->next();
   }
   while( h != myVertex->halfedge() );
~~~~~~~~~~~~~
In other words, whenever I'm processing a face, I should stop and ask: is
this really a geometric face in my mesh?  Or is it just a "virtual" face
that represents a boundary loop?  Finally, for convenience, the halfedge
associated with a boundary vertex is the first halfedge on the boundary.
In other words, if we want to iterate over, say, all faces touching a
boundary vertex, we could write
~~~~~~~~~~~~~{.c}
   HalfEdgeIter h = myBoundaryVertex->halfedge();
   do
   {
      // do something interesting with h
      h = h->twin()->next();
   }
   while( !h->isBoundary() );
~~~~~~~~~~~~~
(Notice that this loop will never terminate for an interior vertex!)

More documentation can be found in the comments in the source files.