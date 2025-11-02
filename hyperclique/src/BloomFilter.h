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

#ifndef BLOOMFILTER_H_
#define BLOOMFILTER_H_

#include <set>
#include <htd/main.hpp>
using namespace std;
using namespace htd;

typedef unsigned __int128 label_t;

/**
 * Implements a Bloom Filter using Rabin Karp for char* buffer lookups
 */
class BloomFilter
{
public:
  map<id_t, set<label_t>> laurentToLabels;
  BloomFilter() {}
  ~BloomFilter() {}

  label_t createLabel(vector<vertex_t> edge)
  {
    label_t l = 0;
    for (vertex_t v : edge)
    {
      l += 1 << ((v - 1) % 128);
    }
    return l;
  }

  label_t createLabel(set<vertex_t> edge)
  {
    label_t l = 0;
    for (vertex_t v : edge)
    {
      l += 1 << ((v - 1) % 128);
    }
    return l;
  }

  // Adds the specified edge to the bloom filter
  void add(vector<vertex_t> edge);
  void add(label_t label, id_t laurent);
  // Empty the Bloom Filter
  void clear();

  size_t getNb(vector<vertex_t> vertices);
  size_t getNb(set<vertex_t> vertices);
  size_t getNb(vector<vertex_t> vertices, id_t laurent);
  size_t getNb(set<vertex_t> vertices, id_t laurent);

  /**
   * Checks whether an element probably exists in the set, or definitely
   * doesn't.
   * Returns true if the element probably exists in the set
   * Returns false if the element definitely does not exist in the set
   */
  bool exists(vector<vertex_t> edge);
  bool exists(label_t label, id_t laurent);

  /**
   * Checks if any subelement probably exists or
   * definitely doesn't. If false is returned then no subelement of the
   * specified string of the specified length is in the bloom filter.
   */
  bool subExists(vector<vertex_t> edge);
  bool subExists(label_t label);
  bool subExists(vector<vertex_t> edge, id_t laurent);
  bool subExists(label_t label, id_t laurent);
};

#endif // BLOOMFILTER_H_