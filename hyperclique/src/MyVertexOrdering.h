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

#ifndef MY_VERTEX_ORDERING_H
#define MY_VERTEX_ORDERING_H

#include <htd/main.hpp>
#include <htd/Hypergraph.hpp>

class MyIVertexOrdering : public IVertexOrdering
{
private:
  const std::vector<htd::vertex_t> seq;

public:
  MyIVertexOrdering(std::vector<htd::vertex_t> order) : seq(order) {}
  ~MyIVertexOrdering() {}
  const std::vector<htd::vertex_t> &sequence(void) const override { return seq; }
  std::size_t requiredIterations(void) const override { return 0; };
};

class fastNodeMap
{
public:
  std::vector<int32_t> data;
  std::vector<int32_t> nodes;
  fastNodeMap() {};
  bool has(uint32_t i)
  {
    assert(nodes.size() >= i);
    return nodes[i] >= 0;
  }
  void remove(uint32_t i)
  {
    assert(nodes.size() > i);
    assert(nodes[i] >= 0);
    assert(data.size() > 0);
    assert(data[nodes[data.back()]] == data.back());
    assert(data[nodes[i]] == (int32_t)i);
    data[nodes[i]] = data.back();
    nodes[data.back()] = nodes[i];
    data.pop_back();
    nodes[i] = -1;
  }
  void insert(uint32_t i)
  {
    while (nodes.size() <= i)
      nodes.push_back(-1);
    assert(nodes[i] == -1); // Don't insert it twice
    nodes[i] = data.size();
    data.push_back(i);
  }
  int size()
  {
    return data.size();
  }
  void clear()
  {
    data.clear();
    nodes.clear();
  }
  void clearbis()
  {
    for (uint32_t i = 0; i < data.size(); i++)
      nodes[i] = -1;
    data.clear();
  }
  void print()
  {
    for (uint32_t i = 0; i < nodes.size(); i++)
      if (nodes[i] >= 0)
        cout << "(" << i << ", " << nodes[i] << ") ";
    cout << endl;
    for (uint32_t i = 0; i < data.size(); i++)
      cout << "(" << i << ", " << data[i] << ") ";
    cout << endl;
  }
};

#endif // MY_VERTEX_ORDERING_H
