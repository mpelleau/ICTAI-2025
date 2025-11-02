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

#include "HBKGraph.h"

/*************************************************************************************************/
/******************************************* HBKGraph ********************************************/
/*************************************************************************************************/

bool HBKGraph::isNeighbour(set<vertex_t> vertices, vertex_t vertex)
{
  for (const vertex_t v : vertices)
  {
    if (!(g->isNeighbor(v, vertex)))
      return false;
  }
  return true;
}

set<id_t> HBKGraph::getEdges(vector<index_t> edgesId, vector<vertex_t> vertices)
{
  set<id_t> result;

  FilteredHyperedgeCollection filteredHedges = g->hyperedgesAtPositions(edgesId);
  filteredHedges.restrictTo(vertices);

  for (Hyperedge edge : filteredHedges)
    result.insert(edge.id());

  return result;
}

set<id_t> HBKGraph::getEdges(vector<vertex_t> vertices)
{
  pair<vector<Hyperedge>, vector<index_t>> pair = Utils::toVectorsIdx(g->hyperedges());
  return getEdges(pair.second, vertices);
}

set<id_t> HBKGraph::getEdges(set<vertex_t> vertices)
{
  vector<vertex_t> vecVertices(vertices.begin(), vertices.end());
  return getEdges(vecVertices);
}

set<id_t> HBKGraph::getEdges(vector<index_t> edgesId, set<vertex_t> vertices)
{
  vector<vertex_t> vecVertices(vertices.begin(), vertices.end());
  return getEdges(edgesId, vecVertices);
}

set<id_t> HBKGraph::getEdges(vertex_t vertex)
{
  set<id_t> result;
  for (Hyperedge edge : g->hyperedges(vertex))
  {
    result.insert(edge.id());
  }
  return result;
}

set<id_t> HBKGraph::getEdges(vector<index_t> edgesId, vertex_t vertex)
{
  set<id_t> result;
  vector<index_t>::iterator it;
  for (Hyperedge edge : g->hyperedges(vertex))
  {
    if ((it = find(edgesId.begin(), edgesId.end(), edge.id())) != edgesId.end())
      result.insert(edge.id());
  }
  return result;
}

bool HBKGraph::isHyperClique(set<vertex_t> vertices)
{
  return (Utils::binom(vertices.size(), laurent) == getEdges(vertices).size());
}

set<Hyperedge> HBKGraph::getEdgesSet(set<vertex_t> vertices)
{
  set<Hyperedge> result;
  vector<vertex_t> vecVertices(vertices.begin(), vertices.end());
  pair<vector<Hyperedge>, vector<index_t>> pair = Utils::toVectorsIdx(g->hyperedges());

  FilteredHyperedgeCollection filteredHedges = g->hyperedgesAtPositions(pair.second);
  filteredHedges.restrictTo(vecVertices);

  for (Hyperedge edge : filteredHedges)
    result.insert(edge);

  return result;
}

void simplify(set<Hyperedge> &edges)
{
  set<Hyperedge>::iterator it;
  set<Hyperedge>::iterator jt;

  for (it = edges.begin(); it != edges.end();)
  {
    vector<vertex_t> edgei = it->sortedElements();
    if (edgei.size() == 1)
    {
      if (it != edges.begin())
      {
        set<Hyperedge>::iterator tmp;
        tmp = it;
        tmp--;
        edges.erase(it);
        it = tmp;
      }
      else
      {
        edges.erase(it);
        it = edges.begin();
      }
      continue;
    }
    for (jt = it, jt++; jt != edges.end(); jt++)
    {
      vector<vertex_t> edgej = jt->sortedElements();
      if (edgej.size() == 1)
      {
        set<Hyperedge>::iterator tmp = jt;
        tmp--;
        edges.erase(jt);
        jt = tmp;
        continue;
      }
      if (Utils::is_included(edgei, edgej))
      {
        if (it != edges.begin())
        {
          set<Hyperedge>::iterator tmp;
          tmp = it;
          tmp--;
          edges.erase(it);
          it = tmp;
        }
        else
        {
          edges.erase(it);
          it = edges.begin();
        }
      }
      else if (Utils::is_included(edgej, edgei))
      {
        set<Hyperedge>::iterator tmp = jt;
        tmp--;
        edges.erase(jt);
        jt = tmp;
      }
    }
    it++;
  }
}

bool HBKGraph::isNonUniformClique(set<vertex_t> vertices)
{
  if (Globals::verbose)
  {
    cout << "c | ... vertices (" << vertices.size() << ") = {";
    for (vertex_t v : vertices)
      cout << " " << v;
    cout << " }" << endl;
  }
  set<Hyperedge> edges = getEdgesSet(vertices);
  if (Globals::verbose)
  {
    cout << "c | ... edges (" << edges.size() << ") = [";
    for (Hyperedge e : edges)
    {
      cout << " {";
      for (vertex_t v : e.elements())
        cout << " " << v;
      cout << " }";
    }
    cout << " ]" << endl;
  }
  simplify(edges);
  if (Globals::verbose)
  {
    cout << "c | ... simp edges (" << edges.size() << ") = [";
    for (Hyperedge e : edges)
    {
      cout << " {";
      for (vertex_t v : e.elements())
        cout << " " << v;
      cout << " }";
    }
    cout << " ]" << endl;
  }

  unsigned long max = 0;
  for (Hyperedge h : edges)
  {
    max = (max < h.elements().size()) ? h.elements().size() : max;
  }
  if (Globals::verbose)
    cout << "c | ... max = " << max << endl;

  unsigned long nb = 0;
  for (Hyperedge h : edges)
  {
    unsigned long e = h.elements().size();
    nb += Utils::binom(vertices.size() - e, max - e);
  }
  if (Globals::verbose)
    cout << "c | ... nb = " << nb << endl;

  return (edges.size() <= 1 || Utils::binom(vertices.size(), max) <= nb);
}
