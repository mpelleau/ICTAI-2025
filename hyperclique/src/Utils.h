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

#ifndef UTILS_H
#define UTILS_H

#include <set>
#include <map>
#include <htd/main.hpp>
#include "Constants.h"
#include "Globals.h"

using namespace htd;

class Utils
{
public:
  /********************************/
  /********** Arithmetics *********/
  /********************************/

  /**
   * Computes the binomial coefficient (nCk, k among n).
   */
  static unsigned long binom(unsigned long n, unsigned long k)
  {
    unsigned long res = 1;
    k = k > n - k ? n - k : k;

    for (unsigned long j = 1; j <= k; j++, n--)
    {
      if (n % j == 0)
      {
        res *= n / j;
      }
      else if (res % j == 0)
      {
        res = res / j * n;
      }
      else
      {
        res = (res * n) / j;
      }
    }

    return res;
  }

  /********************************/
  /************** Set *************/
  /********************************/

  /**
   * A function to print all combinations of a given length from the given array.
   */
  static void generateSubSets(vector<vertex_t> &a, unsigned int reqLen, unsigned int start,
                              set<vertex_t> &current, vector<set<vertex_t>> &res)
  {

    if (current.size() > reqLen)
      return;

    // If currLen is equal to required length then print the sequence.
    else if (current.size() == reqLen)
    {
      res.push_back(current);
      return;
    }
    // If start equals to len then return since no further element left.
    if (start == a.size())
    {
      return;
    }
    // If not enought elements left
    if (current.size() + a.size() - start < reqLen)
      return;

    current.insert(a[start]);
    generateSubSets(a, reqLen, start + 1, current, res);

    current.erase(a[start]);
    generateSubSets(a, reqLen, start + 1, current, res);
  }

  /**
   * Check if a sorted vector is included in another.
   * Returns `true` if `b` is included in `a`, `false` otherwise.
   */
  template <typename T>
  static bool is_included(vector<T> a, vector<T> b)
  {
    return includes(a.begin(), a.end(), b.begin(), b.end());
  }

  /**
   * Check if a set is included in another.
   * Returns `true` if `b` is included in `a`, `false` otherwise.
   */
  template <typename T>
  static bool is_included(set<T> a, set<T> b)
  {
    return includes(a.begin(), a.end(), b.begin(), b.end());
  }

  /**
   * Check if a set is included in a set of set.
   * Returns `true` if `b` is included in `a`, `false` otherwise.
   */
  template <typename T>
  static bool is_included(set<set<T>> a, set<T> b)
  {
    for (const set<T> &s : a)
    {
      if (is_included(s, b))
        return true;
    }
    return false;
  }

  /**
   * Computes the union of two sets.
   */
  template <typename T>
  static set<T> set_union(set<T> a, set<T> b)
  {
    set<T> c;
    std::merge(a.begin(), a.end(), b.begin(), b.end(), inserter(c, c.end()));
    return c;
  }

  /**
   * Computes the union of a vector and a set.
   */
  template <typename T>
  static set<T> set_union(vector<T> a, set<T> b)
  {
    set<T> c;
    sort(a.begin(), a.end());
    std::merge(a.begin(), a.end(), b.begin(), b.end(), inserter(c, c.end()));
    return c;
  }

  /**
   * Computes the union of a set and a vector.
   */
  template <typename T>
  static set<T> set_union(set<T> a, vector<T> b) { return set_union(b, a); }

  /**
   * Computes the union of a ConstCollection and a set.
   */
  template <typename T>
  static set<T> set_union(ConstCollection<T> a, set<T> b) { return set_union(toSet(a), b); }

  /**
   * Computes the union of a set and a ConstCollection.
   */
  template <typename T>
  static set<T> set_union(set<T> a, ConstCollection<T> b) { return set_union(b, a); }

  /**
   * Computes the union of two sets with comparator.
   */
  template <typename T, typename C>
  static set<T, C> set_union(set<T, C> a, set<T, C> b, C comp)
  {
    set<T, C> c(comp);
    std::merge(a.begin(), a.end(), b.begin(), b.end(), inserter(c, c.end()), comp);
    return c;
  }

  /**
   * Computes the union of two sets with comparator.
   */
  template <typename T, typename C>
  static set<T, C> set_union(set<T, C> a, set<T> b, C comp)
  {
    set<T, C> bBis(comp);
    for (T e : b)
      bBis.insert(e);
    return set_union(a, bBis, comp);
  }

  /**
   * Computes the union of two sets with comparator.
   */
  template <typename T, typename C>
  static set<T, C> set_union(set<T> a, set<T, C> b, C comp)
  {
    return set_union(b, a, comp);
  }

  /**
   * Computes the union of two sets with comparator.
   */
  template <typename T, typename C>
  static set<T, C> set_union(set<T, C> a, vector<T> b, C comp)
  {
    set<T, C> bBis(comp);
    for (T t : b)
      bBis.insert(t);
    return set_union(a, bBis, comp);
  }

  /**
   * Computes the union of two sets with comparator.
   */
  template <typename T, typename C>
  static set<T, C> set_union(vector<T> a, set<T, C> b, C comp)
  {
    return set_union(b, a, comp);
  }

  /**
   * Computes the intersection of two sets.
   */
  template <typename T>
  static set<T> set_intersection(set<T> a, set<T> b)
  {
    set<T> c;
    std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), inserter(c, c.end()));
    return c;
  }

  /**
   * Computes the intersection of a vector and a set.
   */
  template <typename T>
  static set<T> set_intersection(vector<T> a, set<T> b)
  {
    set<T> c;
    sort(a.begin(), a.end());
    std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), inserter(c, c.end()));
    return c;
  }

  /**
   * Computes the intersection of a set and a vector.
   */
  template <typename T>
  static set<T> set_intersection(set<T> a, vector<T> b) { return set_intersection(b, a); }

  /**
   * Computes the intersection of a ConstCollection and a set.
   */
  template <typename T>
  static set<T> set_intersection(ConstCollection<T> a, set<T> b) { return set_intersection(toSet(a), b); }

  /**
   * Computes the intersection of a set and a ConstCollection.
   */
  template <typename T>
  static set<T> set_intersection(set<T> a, ConstCollection<T> b) { return set_intersection(b, a); }

  /**
   * Computes the intersection of two sets with comparator.
   */
  template <typename T, typename C>
  static set<T, C> set_intersection(set<T, C> a, set<T, C> b, C comp)
  {
    set<T> aBis, bBis, cBis;
    for (T e : a)
      aBis.insert(e);
    for (T e : b)
      bBis.insert(e);
    cBis = set_intersection(aBis, bBis);
    set<T, C> c(comp);
    for (T e : cBis)
      c.insert(e);
    return c;
  }

  /**
   * Computes the intersection of two sets with comparator.
   */
  template <typename T, typename C>
  static set<T, C> set_intersection(set<T, C> a, set<T> b, C comp)
  {
    set<T> aBis;
    for (T e : a)
      aBis.insert(e);
    set<T> cBis = set_intersection(aBis, b);
    set<T, C> c(comp);
    for (T e : cBis)
      c.insert(e);
    return c;
  }

  /**
   * Computes the intersection of two sets with comparator.
   */
  template <typename T, typename C>
  static set<T, C> set_intersection(set<T> a, set<T, C> b, C comp)
  {
    return set_intersection(b, a, comp);
  }

  /**
   * Computes the intersection of a ConstCollection and a set.
   */
  template <typename T, typename C>
  static set<T, C> set_intersection(ConstCollection<T> a, set<T, C> b, C comp)
  {
    return set_intersection(toSet(a), b, comp);
  }

  /**
   * Computes the intersection of a set and a ConstCollection.
   */
  template <typename T, typename C>
  static set<T, C> set_intersection(set<T, C> a, ConstCollection<T> b, C comp)
  {
    return set_intersection(b, a, comp);
  }

  /**
   * Computes the difference of two sets `a`and `b`.
   * Returns `a\b`.
   */
  template <typename T>
  static set<T> set_difference(set<T> a, set<T> b)
  {
    set<T> c;
    std::set_difference(a.begin(), a.end(), b.begin(), b.end(), inserter(c, c.end()));
    return c;
  }

  /**
   * Computes the difference of a vector `a` and a set `b`.
   * Returns `a\b`.
   */
  template <typename T>
  static set<T> set_difference(vector<T> a, set<T> b)
  {
    set<T> c;
    sort(a.begin(), a.end());
    std::set_difference(a.begin(), a.end(), b.begin(), b.end(), inserter(c, c.end()));
    return c;
  }

  /**
   * Computes the difference of a set `a` and a vector `b`.
   * Returns `a\b`.
   */
  template <typename T>
  static set<T> set_difference(set<T> a, vector<T> b)
  {
    set<T> c;
    sort(b.begin(), b.end());
    std::set_difference(a.begin(), a.end(), b.begin(), b.end(), inserter(c, c.end()));
    return c;
  }

  /**
   * Computes the difference of a ConstCollection `a` and a set `b`.
   * Returns `a\b`.
   */
  template <typename T>
  static set<T> set_difference(ConstCollection<T> a, set<T> b) { return set_difference(toSet(a), b); }

  /**
   * Computes the difference of a set `a` and a ConstCollection `b`.
   * Returns `a\b`.
   */
  template <typename T>
  static set<T> set_difference(set<T> a, ConstCollection<T> b) { return set_difference(a, toSet(b)); }

  /**
   * Computes the difference of two sets with comparator.
   * Returns `a\b`.
   */
  template <typename T, typename C>
  static set<T, C> set_difference(set<T, C> a, set<T, C> b, C comp)
  {
    set<T, C> c(comp);
    std::set_difference(a.begin(), a.end(), b.begin(), b.end(), inserter(c, c.end()), comp);
    return c;
  }

  /**
   * Computes the difference of two sets with comparator.
   * Returns `a\b`.
   */
  template <typename T, typename C>
  static set<T, C> set_difference(set<T, C> a, set<T> b, C comp)
  {
    set<T, C> bBis(comp);
    for (T e : b)
      bBis.insert(e);
    return set_difference(a, bBis, comp);
  }

  /**
   * Computes the difference of two sets with comparator.
   * Returns `a\b`.
   */
  template <typename T, typename C>
  static set<T, C> set_difference(set<T> a, set<T, C> b, C comp)
  {
    set<T, C> aBis(comp);
    for (T e : a)
      aBis.insert(e);
    return set_difference(aBis, b, comp);
  }

  /**
   * Converts a ConstCollection into a set.
   */
  template <typename T>
  static set<T> toSet(ConstCollection<T> a)
  {
    set<T> b;
    for (const auto e : a)
      b.insert(e);
    return b;
  }

  /**
   * Converts a ConstCollection into a vector.
   */
  template <typename T>
  static vector<T> toVector(ConstCollection<T> a)
  {
    vector<T> b;
    for (const auto e : a)
      b.push_back(e);
    return b;
  }

  /**
   * Converts a set into a vector.
   */
  template <typename T>
  static vector<T> toVector(set<T> a)
  {
    vector<T> b;
    for (const auto e : a)
      b.push_back(e);
    return b;
  }

  /**
   * Converts a ConstCollection into a set of ids.
   */
  static set<id_t> toSetId(ConstCollection<Hyperedge> hyperedges)
  {
    set<id_t> a;
    for (const Hyperedge &hyperedge : hyperedges)
    {
      a.insert(hyperedge.id());
    }
    return a;
  }

  /**
   * Converts a ConstCollection into a pair of vectors.
   */
  static pair<vector<Hyperedge>, vector<id_t>> toVectors(ConstCollection<Hyperedge> hyperedges)
  {
    vector<Hyperedge> a;
    vector<id_t> b;
    for (const Hyperedge &hyperedge : hyperedges)
    {
      a.push_back(hyperedge);
      b.push_back(hyperedge.id());
    }
    return {a, b};
  }

  /**
   * Converts a ConstCollection into a pair of vectors.
   */
  static pair<vector<Hyperedge>, vector<index_t>> toVectorsIdx(ConstCollection<Hyperedge> hyperedges)
  {
    vector<Hyperedge> a;
    vector<index_t> b;
    unsigned long i = 0;
    for (const Hyperedge &hyperedge : hyperedges)
    {
      a.push_back(hyperedge);
      b.push_back(i++);
    }
    return {a, b};
  }

  /**
   * Converts a ConstCollection into a pair of vectors.
   */
  static pair<vector<id_t>, vector<index_t>> toVectorIds(ConstCollection<Hyperedge> hyperedges)
  {
    vector<id_t> a;
    vector<index_t> b;
    unsigned long i = 0;
    for (const Hyperedge &hyperedge : hyperedges)
    {
      a.push_back(hyperedge.id());
      b.push_back(i++);
    }
    return {a, b};
  }

  /**
   * Converts a ConstCollection into a map.
   */
  static map<id_t, vector<vertex_t>> toMap(ConstCollection<Hyperedge> hyperedges)
  {
    map<id_t, vector<vertex_t>> m;
    for (const Hyperedge &hyperedge : hyperedges)
    {
      m[hyperedge.id()] = hyperedge.elements();
    }
    return m;
  }

  /**
   * Returns the keys of a map.
   */
  template <typename T1, typename T2>
  static set<T1> keys(map<T1, T2> a)
  {
    set<T1> b;
    for (const auto &keyval : a)
      b.insert(keyval.first);
    return b;
  }

  /**
   * Returns the values of a map.
   */
  template <typename T1, typename T2>
  static set<T2> values(map<T1, T2> a)
  {
    set<T2> b;
    for (const auto &keyval : a)
      b.insert(keyval.second);
    return b;
  }

  /********************************/
  /************* Print ************/
  /********************************/

  static void printFiltering()
  {
    printFiltering(42);
  }

  static void printFiltering(unsigned int width)
  {
    unsigned int length = width - 8;
    unsigned int remain = LENGTH - width;
    switch (Globals::filteringValue)
    {
    case NO_FILTER:
      cout << "c | Filter:" << setw(length) << "OFF" << setw(remain) << "|" << endl;
      break;
    case BLOOM_FILTER:
      cout << "c | Filter:" << setw(length) << "Bloom" << setw(remain) << "|" << endl;
      break;
    case CLIQUE_FILTER:
      cout << "c | Filter:" << setw(length) << "Clique" << setw(remain) << "|" << endl;
      break;
    case NEIGHBOURHOOD_FILTER:
      cout << "c | Filter:" << setw(length) << "Neighbourhood" << setw(remain) << "|" << endl;
      break;
    case INCREMENTAL_FILTER:
      cout << "c | Filter:" << setw(length) << "Incremental" << setw(remain) << "|" << endl;
      break;
    case NB_FILTER:
      cout << "c | Filter:" << setw(length) << "ALL" << setw(remain) << "|" << endl;
      break;
    }
  }

  static void printOrdering()
  {
    printOrdering(42);
  }

  static void printOrdering(unsigned int width)
  {
    unsigned int length = width - 10;
    unsigned int remain = LENGTH - width;
    switch (Globals::orderingValue)
    {
    case RANDOM_ORDERING:
      cout << "c | Ordering:" << setw(length) << "random" << setw(remain) << "|" << endl;
      break;
    case MIN_DEGREE_ORDERING:
      cout << "c | Ordering:" << setw(length) << "min-degree" << setw(remain) << "|" << endl;
      break;
    case MIN_FILL_ORDERING:
      cout << "c | Ordering:" << setw(length) << "min-fill" << setw(remain) << "|" << endl;
      break;
    case NATURAL_ORDERING:
      cout << "c | Ordering:" << setw(length) << "natural" << setw(remain) << "|" << endl;
      break;
    case DEGENERACY_ORDERING:
      cout << "c | Ordering:" << setw(length) << "degeneracy" << setw(remain) << "|" << endl;
      break;
    case MAX_DEGREE_ORDERING:
      cout << "c | Ordering:" << setw(length) << "max-degree" << setw(remain) << "|" << endl;
      break;
    case NB_ORDER:
      cout << "c | Ordering:" << setw(length) << "ALL" << setw(remain) << "|" << endl;
      break;
    }
  }
  static void printNodeFiltering()
  {
    printNodeFiltering(42);
  }

  static void printNodeFiltering(unsigned int width)
  {
    unsigned int length = width - 13;
    unsigned int remain = LENGTH - width;
    switch (Globals::nodeFilteringValue)
    {
    case NO_FILTER:
      cout << "c | Node filter:" << setw(length) << "OFF" << setw(remain) << "|" << endl;
      break;
    case DEGREE_FILTER:
      cout << "c | Node filter:" << setw(length) << "Degree" << setw(remain) << "|" << endl;
      break;
    case COOCCURRENCE_FILTER:
      cout << "c | Node filter:" << setw(length) << "Co-occurrences" << setw(remain) << "|" << endl;
      break;
    case BOTH_FILTER:
      cout << "c | Node filter:" << setw(length) << "Both" << setw(remain) << "|" << endl;
      break;
    case NB_NODE_FILTER:
      cout << "c | Node filter:" << setw(length) << "ALL" << setw(remain) << "|" << endl;
      break;
    }
  }

  template <typename T, typename C>
  static void printCurrentState(set<T> &clique, set<T, C> &candidates, set<vertex_t> &excluded)
  {
    cout << "c | ... current clique (" << clique.size() << ") = {";
    for (T v : clique)
      cout << " " << v;
    cout << " }" << endl;
    cout << "c | ... candidates (" << candidates.size() << ") = {";
    for (T v : candidates)
      cout << " " << v;
    cout << " }" << endl;
    cout << "c | ... excluded (" << excluded.size() << ") = {";
    for (T v : excluded)
      cout << " " << v;
    cout << " }" << endl;
    cout << DLINE << endl;
  }
};

#endif // UTILS_H