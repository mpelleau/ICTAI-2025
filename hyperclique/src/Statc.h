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

#ifndef STATC_H
#define STATC_H

#include "Utils.h"

class Statc
{
public:
  map<vertex_t, set<int>> vclique;

  Statc(set<set<vertex_t>> cliques)
  {
    int i = 0;
    for (const set<vertex_t> &clique : cliques)
    {
      for (const vertex_t v : clique)
      {
        vclique[v].insert(i);
      }
      i++;
    }
  }

  void print()
  {
    cout << "c | how many times a vertex appears in a clique:" << endl
         << "c | { ";
    for (const auto &keyval : vclique)
    {
      cout << "(" << keyval.first << ": " << keyval.second.size() << ") ";
      if (keyval.first % 10 == 0)
        cout << endl
             << "c |   ";
    }
    cout << "}" << endl;
  }

  /**
   * Computes the number of cliques in which all vertices in `vertices` appears.
   */
  int nCliques(set<vertex_t> vertices)
  {
    set<vertex_t>::iterator v = vertices.begin();
    set<int> result = vclique[*v];
    for (v++; v != vertices.end(); v++)
    {
      result = Utils::set_intersection(result, vclique[*v]);
    }
    return result.size();
  }

  /**
   * Computes the number of cliques in which all vertices in `vertices` appears.
   */
  int nCliques(vector<vertex_t> vertices)
  {
    vector<vertex_t>::iterator v = vertices.begin();
    set<int> result(v, vertices.end());
    for (v++; v != vertices.end(); v++)
    {
      result = Utils::set_intersection(result, vclique[*v]);
    }
    return result.size();
  }

  /**
   * Computes the number of cliques in which a hyperedge appears.
   */
  map<set<vertex_t>, int> nCliques(set<set<vertex_t>> hyperedges)
  {
    map<set<vertex_t>, int> result;
    for (const set<vertex_t> &hyperedge : hyperedges)
    {
      result[hyperedge] = nCliques(hyperedge);
    }
    return result;
  }

  /**
   * Computes the number of cliques in which a hyperedge appears.
   */
  map<vector<vertex_t>, int> nCliques(set<vector<vertex_t>> hyperedges)
  {
    map<vector<vertex_t>, int> result;
    for (const vector<vertex_t> &hyperedge : hyperedges)
    {
      result[hyperedge] = nCliques(hyperedge);
    }
    return result;
  }
};

#endif // STATC_H