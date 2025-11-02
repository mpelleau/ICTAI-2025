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

#ifndef DEGENERENCY_ORDERING_ALGORITHM_H
#define DEGENERENCY_ORDERING_ALGORITHM_H

#include "Graph.h"
#include "MyVertexOrdering.h"
#include <utility>
#include <algorithm>

class DegenerencyOrderingAlgorithm : public IOrderingAlgorithm
{
private:
  /**
   *  The management instance to which the current object instance belongs.
   */
  const htd::LibraryInstance *managementInstance_;

public:
  DegenerencyOrderingAlgorithm(const htd::LibraryInstance *const manager) : managementInstance_(manager) {}
  virtual ~DegenerencyOrderingAlgorithm() {}

  virtual const htd::LibraryInstance *managementInstance() const noexcept { return managementInstance_; }
  virtual void setManagementInstance(const htd::LibraryInstance *const manager) { managementInstance_ = manager; }

  virtual htd::IVertexOrdering *computeOrdering(const htd::IMultiHypergraph &graph) const noexcept;
  virtual htd::IVertexOrdering *computeOrdering(const htd::IMultiHypergraph &graph, const htd::IPreprocessedGraph &preprocessedGraph) const noexcept { return computeOrdering(graph); };

  virtual IOrderingAlgorithm *clone(void) const { return new DegenerencyOrderingAlgorithm(managementInstance_); }
  void computeOrderingAsVec(const htd::IMultiHypergraph &graph, std::vector<htd::vertex_t> &out_order) const noexcept;
};

#endif // DEGENERENCY_ORDERING_ALGORITHM_H
