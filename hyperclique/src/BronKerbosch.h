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

#ifndef BRON_KERBOSCH_H
#define BRON_KERBOSCH_H

#include "Graph.h"
#include <utility>
#include <algorithm>
#include <htd/main.hpp>
#include <htd/Hypergraph.hpp>
#include "DegenerencyOrderingAlgorithm.h"
#include "MaxDegreeOrderingAlgorithm.h"

class BronKerbosch
{
private:
  static void jsonClique(set<set<vertex_t>>::iterator it)
  {
    cout << "[";
    set<vertex_t>::iterator vt = it->begin();
    cout << *vt;
    vt++;
    for (; vt != it->end(); ++vt)
    {
      cout << ", " << *vt;
    }
    cout << "]";
  }

public:
  static set<set<vertex_t>> cliques;
  static unsigned int nbCallToBK;
  static set<vertex_t> biggestClique;

  static void findHypercliques(set<vertex_t> clique, set<vertex_t, MyComparator> candidates,
                               set<vertex_t> excluded, SweetHBK::Graph *graph);
  static void findCliques(set<vertex_t> clique, set<vertex_t, MyComparator> candidates,
                          set<vertex_t> excluded, SweetHBK::Graph *graph);
  static void findCliquesBis(set<vertex_t> clique, set<vertex_t, MyComparator> candidates,
                             set<vertex_t> excluded, SweetHBK::Graph *graph);
  static void findNonUniformCliques(set<vertex_t> clique, set<vertex_t, MyComparator> candidates,
                                    set<vertex_t> excluded, SweetHBK::Graph *graph);

  static void print()
  {
    for (const set<vertex_t> &clique : cliques)
    {
      cout << "c | {";
      for (const vertex_t v : clique)
      {
        cout << " " << v;
      }
      cout << " }" << endl;
    }
  }

  static void json()
  {
    cout << "[";
    if (cliques.size() > 0)
    {
      set<set<vertex_t>>::iterator it = cliques.begin();
      jsonClique(it);
      it++;
      for (; it != cliques.end(); it++)
      {
        cout << ", ";
        jsonClique(it);
      }
    }
    cout << "]";
  }

  static void cliqueFound(set<vertex_t> clique)
  {
    /* If it is the biggest clique found for now, then we mark it as such. */
    if (Globals::maxClique && (biggestClique.size() < clique.size()))
    {
      biggestClique = clique;
    }
    if (Globals::verbose)
    {
      cout << "c | ... clique found !" << setw(LENGTH - 19) << "|" << endl;
      cout << DLINE << endl;
    }
    if (!Globals::maxClique)
      cliques.insert(clique);
  }

  static void cleanBiggest()
  {
    biggestClique.clear();
  }

  static void printMaxClique()
  {
    cout << "c | Biggest: {";
    for (const vertex_t v : biggestClique)
    {
      cout << " " << v;
    }
    cout << " }" << endl;
  }

  static void printEdge(SweetHBK::Graph *graph)
  {
    for (const set<vertex_t> &clique : cliques)
    {
      cout << "c | {";
      for (const vertex_t v : clique)
      {
        cout << " " << v;
      }
      cout << " }" << endl;
      set<id_t> edges = graph->gillesCoeurCoeur->getEdges(clique);
      for (const id_t e : edges)
      {
        cout << "c | " << graph->edgesMap[e] << endl;
      }
    }
  }
};

#endif // BRON_KERBOSCH_H