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

#ifndef Graph_H
#define Graph_H

#include "HBKGraph.h"
#include "BloomFilter.h"
#include "CardinalityConstraint.h"
#include "Clique.h"
#include "Statc.h"
#include "Stath.h"
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

struct MyComparator
{
  vector<vertex_t> sequence;

  MyComparator() : sequence() {}

  MyComparator(vector<vertex_t> val_vec) : sequence(val_vec) {}

  void setSequence(vector<vertex_t> val_vec)
  {
    sequence.clear();
    for (size_t i = 0; i < val_vec.size(); i++)
    {
      sequence.push_back(val_vec[i]);
    }
  }

  bool operator()(const vertex_t i1, const vertex_t i2) const
  {
    return sequence[i1 - 1] < sequence[i2 - 1];
  }
};

struct FreqComparator
{
  bool operator()(const pair<vertex_t, unsigned int> i1, const pair<vertex_t, unsigned int> i2) const
  {
    if (i1.second == i2.second)
      return i1.first < i2.first;
    return i1.second < i2.second;
  }
};

namespace SweetHBK
{
  class Graph
  {

  public:
    map<id_t, vector<vertex_t>> edges;
    map<id_t, id_t> edgesMap;
    map<id_t, set<id_t>> laurentToEdges;
    map<id_t, set<vertex_t>> laurentToVertices;
    map<vertex_t, unsigned int> laurentFrequences;
    set<vertex_t> vertices;

    unsigned int biggestHyperedge;
    HBKGraph *gillesCoeurCoeur;
    BloomFilter *bloom;

    MyComparator comp;

    Graph();

    ~Graph()
    {
      delete gillesCoeurCoeur;
      delete bloom;
    }

    void orderSeq(set<vertex_t> vertices);

    map<id_t, vertex_t> getUnit();

    void getHyperClique(set<vertex_t, MyComparator> &vertices);
    void getHyperClique(set<vertex_t> &clique, set<vertex_t, MyComparator> &candidates,
                        set<vertex_t> &excluded);

    void getHyperCliqueCE(set<vertex_t, MyComparator> &vertices);
    void getHyperCliqueCE(set<vertex_t> &clique, set<vertex_t, MyComparator> &candidates,
                          set<vertex_t> &excluded);

    void getHyperCliqueCE_HBK(set<vertex_t, MyComparator> &vertices);
    void getHyperCliqueCE_HBK(set<vertex_t> &clique, set<vertex_t, MyComparator> &candidates,
                              set<vertex_t> &excluded);

    double enumerateHyperCliques(void (Graph::*func)(set<vertex_t, MyComparator> &));

    set<vertex_t> cliqueNeighbourhood(set<vertex_t> &clique, set<vertex_t, MyComparator> &suburbs);
    set<vertex_t, MyComparator> ordCliqueNeighbourhood(set<vertex_t> &clique, set<vertex_t, MyComparator> &suburbs);

    void candidateBloomFilter(set<vertex_t> &clique, set<vertex_t, MyComparator> &suburbs);
    void candidateIncrementalFilter(set<vertex_t> &clique, set<vertex_t, MyComparator> &suburbs, vertex_t lastAdded);
    void candidateCliqueFilter(set<vertex_t> &clique, set<vertex_t, MyComparator> &suburbs);
    void candidateNeighbourhoodFilter(set<vertex_t, MyComparator> &suburbs, vertex_t lastAdded);

    void enumerateNonUniformCliques();
    set<vertex_t> nonUniformNeighbourhood(set<vertex_t> &clique, set<vertex_t> &suburbs);

    void print()
    {
      cout << gillesCoeurCoeur->g->vertexCount() << " vertices:";
      for (vertex_t v : gillesCoeurCoeur->g->vertices())
        cout << " " << v;
      cout << endl
           << gillesCoeurCoeur->g->edgeCount() << " edges:" << endl;
      for (Hyperedge h : gillesCoeurCoeur->g->hyperedges())
      {
        cout << "\t" << h.id() << ":";
        for (vertex_t v : h.elements())
          cout << " " << v;
        cout << endl;
      }
    }

  private:
    void createGilles(unsigned int laurent, unsigned int vertices);
    void remove(vertex_t v, set<vertex_t> *laurentVertices);
    void filterByFreq(unsigned int laurent, set<vertex_t> *laurentVertices);
    void filterByCoOccurrences(unsigned int laurent, set<vertex_t> *laurentVertices);
  };
}

#endif
