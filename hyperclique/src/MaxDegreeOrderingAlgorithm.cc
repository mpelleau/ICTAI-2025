/************************************************************************************
Copyright (c) 2019 Valentin Montmirail, Marie Pelleau, Université Côte d'Azur, I3S, France
Copyright (c) 2019 Laurent Simon, Bordeaux INP, Université de Bordeaux, LaBRI, Talence, France
Last edit, April 25, 2019

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

#include "MaxDegreeOrderingAlgorithm.h"

// FIXME: this version does do nothing... Use the other function
htd::IVertexOrdering *MaxDegreeOrderingAlgorithm::computeOrdering(const htd::IMultiHypergraph &graph) const noexcept
{
  std::vector<htd::vertex_t> out_order;
  computeOrderingAsVec(graph, out_order);
  IVertexOrdering *order = new MyIVertexOrdering(out_order);
  return order;
}

// May be change the prototype to pass the order on which you write the result
void MaxDegreeOrderingAlgorithm::computeOrderingAsVec(const htd::IMultiHypergraph &graph, std::vector<htd::vertex_t> &out_order) const noexcept
{
  out_order.clear();
  out_order.reserve(graph.vertexCount());
  std::vector<uint32_t> upper; // upper bound

  fastNodeMap S;
  for (uint32_t i = 0; i < graph.vertexCount(); i++)
  {
    upper.push_back(graph.hyperedges(i + 1).size());
    S.insert(i);
  }

  if (Globals::verbose)
  {
    printf("c | upper :\n");
    for (uint32_t i = 0; i < graph.vertexCount(); i++)
    {
      printf("c | upper[%d] = %d\n", i, upper[i]);
    }

    printf("c | S : \n");
    for (uint32_t i = 0; i < graph.vertexCount(); i++)
    {
      printf("c | S[%d] = %d\n", i, S.data[i]);
    }
  }

  assert(S.data.size() == graph.vertexCount());

  // we create a nodeMap that
  while (S.size() > 0)
  {
    uint32_t curInd = S.data[0];
    assert(curInd < upper.size());
    uint32_t curMax = upper[curInd];
    for (int i = 1; i < S.size(); i++)
    {
      uint32_t d = S.data[i];
      assert(d < upper.size());
      if (upper[d] > curMax)
      {
        curInd = d;
        curMax = upper[d];
      }
    }
    out_order.push_back(curInd + 1);
    S.remove(curInd);
  }

  if (Globals::verbose)
  {
    printf("c | out_order : \n");
    for (uint32_t i = 0; i < out_order.size(); i++)
    {
      printf("c | out_order[%d] = %d\n", i, out_order[i]);
    }
  }
}
