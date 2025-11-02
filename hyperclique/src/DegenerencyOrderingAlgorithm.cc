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

#include "DegenerencyOrderingAlgorithm.h"

// FIXME: this version does do nothing... Use the other function
htd::IVertexOrdering *DegenerencyOrderingAlgorithm::computeOrdering(const htd::IMultiHypergraph &graph) const noexcept
{
  std::vector<htd::vertex_t> out_order;
  computeOrderingAsVec(graph, out_order);
  IVertexOrdering *order = new MyIVertexOrdering(out_order);
  return order;
}

// May be change the prototype to pass the order on which you write the result
void DegenerencyOrderingAlgorithm::computeOrderingAsVec(const htd::IMultiHypergraph &graph,
                                                        std::vector<htd::vertex_t> &out_order) const noexcept
{
  bool verbose = false;
  bool trace = false;
  out_order.clear();
  out_order.reserve(graph.vertexCount());
  std::vector<uint32_t> upper; // upper bound
  // std::vector<uint32_t> ncoreness; // coreness of a node

  fastNodeMap S;
  for (uint32_t i = 0; i < graph.vertexCount(); i++)
  {
    upper.push_back(graph.neighborCount(i + 1));
    // Pour le degrés des noeuds c'est plutôt ça : upper.push_back(graph.hyperedges(i+1).size());

    // ncoreness.push_back(0);
    S.insert(i);
  }
  if (trace)
    S.print();
  assert(S.data.size() == graph.vertexCount());

  fastNodeMap mapA; // For double buffering of nodemaps
  fastNodeMap mapB;

  uint32_t lastk = 0;
  // vec<int> lastremoved;
  for (uint32_t k = 1;; k++)
  {
    if (verbose)
      printf("c core progress %u : %lu / %lu\n", k, graph.vertexCount() - S.size(), graph.vertexCount());
    // lastremoved.clear();
    lastk = k;
    if (S.size() == 0)
      break;

    assert(mapA.size() == 0);
    assert(mapB.size() == 0);
    // adds nodes from S that have a degree strictly less than k
    for (int i = 0; i < S.size(); i++)
    {
      uint32_t d = S.data[i];
      assert(d < upper.size());
      if (upper[d] < k)
        mapA.insert(d); // Will be the first set toDel
    }
    // if (mapA.size()==0) break;

    bool AB = true;
    bool finished = false;
    while (!finished)
    {
      if (trace)
        S.print();
      fastNodeMap &toDel = AB ? mapA : mapB;
      fastNodeMap &toDelNew = AB ? mapB : mapA;

      // printf("%s %d %d %d %d\n", AB?"T":"F", mapA.size(), mapB.size(), toDel.size(), toDelNew.size());
      assert(toDelNew.size() == 0);
      AB = !AB;
      for (uint32_t i = 0; i < toDel.data.size(); i++)
      {
        uint32_t d = toDel.data[i];
        assert(toDel.nodes[d] >= 0);
        assert(toDel.nodes[d] == (int)i);
        for (uint32_t ii = 0; ii < graph.neighborCount(d + 1); ii++)
        {
          htd::vertex_t other = graph.neighborAtPosition(d + 1, ii); // graph[d][ii];
          if (--upper[other - 1] == k - 1)
          { // by construction, can only be added once
            toDelNew.insert(other - 1);
          }
        }
        if (trace)
          printf("Removing node %d\n", d + 1);
        if (S.has(d))
          S.remove(d);
        if (trace)
          S.print();
        // assert(ncoreness[d] == 0);
        // ncoreness[d] = k - 1;
        out_order.push_back(d + 1); // Must add 1, this is a vertex number
      }
      toDel.clearbis();
      if (toDelNew.size() == 0) // || S.size() == 0)
        finished = true;
      assert(toDel.size() == 0 && (!finished || toDelNew.size() == 0));
    }
    assert(mapA.size() == 0 && mapB.size() == 0);
  }
  lastk -= 2;
  if (verbose)
  {
    printf("c Coreness of the graph: %d\n", lastk);
    printf("c coreness: %d\n", lastk);
  }
}
