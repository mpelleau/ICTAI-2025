/************************************************************************************
Copyright (c) 2020 Marie Pelleau, Université Côte d'Azur, I3S, France
Copyright (c) 2020 Laurent Simon, Bordeaux INP, Université de Bordeaux, LaBRI, Talence, France
Last edit, March 2, 2020

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**************************************************************************************************/

#ifndef HBKGRAPH_H
#define HBKGRAPH_H

#include "Utils.h"

class HBKGraph
{
public:
  Hypergraph *g;
  unsigned int laurent;

  HBKGraph() { g = new Hypergraph(createManagementInstance(Id::FIRST)); }
  HBKGraph(unsigned int laurent) : laurent(laurent) { g = new Hypergraph(createManagementInstance(Id::FIRST)); }

  HBKGraph(unsigned int laurent, unsigned int nbVertices) : laurent(laurent)
  {
    g = new Hypergraph(createManagementInstance(Id::FIRST));
    unsigned int i = 0;
    while (i < nbVertices)
    {
      g->addVertex();
      i++;
    }
  }

  ~HBKGraph() { delete g; }

  /**
   * Returns `true` if `vertex` is in at least an edge with each vertex in `vertices`.
   */
  bool isNeighbour(set<vertex_t> vertices, vertex_t vertex);

  /**
   * Returns the set of edges in which `vertex` is a member.
   */
  set<id_t> getEdges(vertex_t vertex);

  /**
   * Returns the set of edges present in the given set in which `vertex` is a member.
   */
  set<id_t> getEdges(vector<index_t> edgesId, vertex_t vertex);

  /**
   * Returns the set of edges containing only the vertices in the given set.
   */
  set<id_t> getEdges(set<vertex_t> vertices);

  /**
   * Returns the set of edges which ids are in `edgesId` and containing only the vertices in the given set.
   */
  set<id_t> getEdges(vector<index_t> edgesId, set<vertex_t> vertices);

  /**
   * Returns the set of edges containing only the vertices in the given set.
   */
  set<id_t> getEdges(vector<vertex_t> vertices);

  /**
   * Returns the set of edges which ids are in `edgesId` and containing only the vertices in the given set.
   */
  set<id_t> getEdges(vector<index_t> edgesId, vector<vertex_t> vertices);

  bool isHyperClique(set<vertex_t> vertices);

  set<Hyperedge> getEdgesSet(set<vertex_t> vertices);

  bool isNonUniformClique(set<vertex_t> vertices);
};

#endif // HBKGRAPH_H
