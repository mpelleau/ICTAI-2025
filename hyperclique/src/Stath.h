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

#ifndef STATH_H
#define STATH_H

#include "Utils.h"
#include <cmath>

class Stath
{
public:
  unsigned int nbVertices, nbEdges;

  vector<unsigned int> degrees;                       // nodes' degree
  map<unsigned int, unsigned int> nbEdgesByLaurent;   // nb edges by arity
  map<unsigned int, set<vertex_t>> laurentToVertices; // vertices by arity

  unsigned long long sumArities; // sum of arities = sum of degrees

  unsigned long long sumDegSq; // sum of squared degrees
  unsigned long long sumArtSq; // sum of squared arities

  double avgArity;
  double maxArity;
  double stdArity;
  double avgDegree;
  double stdDegree;
  double avgEdgesPerNode;

  Stath(unsigned int n, unsigned int m) : nbVertices(n), nbEdges(m), degrees(n)
  {
    sumArities = 0;
    sumDegSq = 0;
    sumArtSq = 0;

    /* optional */
    avgArity = 0;
    stdArity = 0;
    maxArity = 0;
    avgDegree = 0;
    stdDegree = 0;
  }

  void addEdge(vector<vertex_t> elements)
  {
    unsigned int a = elements.size();
    if (a > 1)
    {
      laurentToVertices[a].insert(elements.begin(), elements.end());
      nbEdgesByLaurent[a] += 1;
      if (a > maxArity)
        maxArity = a;
      sumArities += a;
      sumArtSq += a * a;

      for (vertex_t v : elements)
      {
        degrees[v - 1]++;
      }
    }
  }

  void computeStat()
  {
    avgArity = sumArities / (double)nbEdges;
    stdArity = sqrt(sumArtSq / (double)nbEdges - avgArity * avgArity);

    avgDegree = sumArities / (double)nbVertices;
    sumDegSq = 0;
    for (unsigned int d : degrees)
    {
      sumDegSq += d * d;
    }
    stdDegree = sqrt(sumDegSq / (double)nbVertices - avgDegree * avgDegree);
    avgEdgesPerNode = nbEdges / (double)nbVertices;
  }

  void print()
  {
    cout << nbVertices << "," << nbEdges << "," << avgEdgesPerNode << ",";
    cout << avgArity << " (" << stdArity << "),";
    cout << avgDegree << " (" << stdDegree << ")";
    for (auto lt : laurentToVertices)
    {
      unsigned int laurent = lt.first;
      unsigned int nbV = lt.second.size();
      unsigned int nbE = nbEdgesByLaurent[laurent];
      unsigned long all = Utils::binom(nbV, laurent);
      double density = nbE / (double)all;
      cout << "," << nbV << "," << nbE << "," << laurent << "," << density;
    }
    cout << endl;
  }

  void printJ()
  {
    cout << ", \"nbnodes\": " << nbVertices << ", \"nbedges\": " << nbEdges << ", \"maxrank\": "
         << maxArity << ", \"avgEdge\": " << avgEdgesPerNode << ", \"avgArity\": " << avgArity
         << ", \"stdArity\": " << stdArity << ", \"avgDegree\": " << avgDegree
         << ", \"stdDegree\": " << stdDegree;
    cout << ", \"densities\": [";
    int i = 0;
    for (auto lt : laurentToVertices)
    {
      if (i)
        cout << ", ";
      unsigned int laurent = lt.first;
      unsigned int nbV = lt.second.size();
      unsigned int nbE = nbEdgesByLaurent[laurent];
      unsigned long all = Utils::binom(nbV, laurent);
      double density = nbE / (double)all;
      cout << "{\"nbnodes\": " << nbV << ", \"nbedges\": " << nbE << ", \"rank\": "
           << laurent << ", \"density\": " << density << "}";
      i++;
    }
  }
};

#endif // STATH_H