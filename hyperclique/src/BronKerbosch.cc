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

#include "BronKerbosch.h"

#include <iostream>  // cout
#include <algorithm> // shuffle
#include <array>     // array
#include <random>    // default_random_engine
#include <chrono>    // chrono::system_clock

set<set<vertex_t>> BronKerbosch::cliques;
set<vertex_t> BronKerbosch::biggestClique;
unsigned int BronKerbosch::nbCallToBK = 0;

void BronKerbosch::findHypercliques(set<vertex_t> clique, set<vertex_t, MyComparator> candidates,
                                    set<vertex_t> excluded, SweetHBK::Graph *graph)
{
  nbCallToBK++;

  if (Globals::verbose)
    Utils::printCurrentState(clique, candidates, excluded);
  if (clique.size() + candidates.size() < graph->gillesCoeurCoeur->laurent)
    return;

  if (candidates.empty() && excluded.empty())
  {
    cliqueFound(clique);
  }

  set<vertex_t, MyComparator> new_candidates = candidates;
  set<vertex_t, MyComparator>::iterator v = new_candidates.begin();

  while (!new_candidates.empty() && v != new_candidates.end() && !Globals::timedout)
  {
    if (Globals::verbose)
      cout << "c | *** try with " << *v << endl;
    set<vertex_t> singleton;
    singleton.insert(*v);

    set<vertex_t, MyComparator> ordSingleton(graph->comp);
    ordSingleton.insert(*v);

    set<vertex_t> cliqueNeighbourhood;
    set<vertex_t, MyComparator> ordCliqueNeighbourhood(graph->comp);
    set<vertex_t> newClique = Utils::set_union(clique, singleton);
    set<vertex_t, MyComparator> diff = Utils::set_difference(candidates, ordSingleton, graph->comp);
    set<vertex_t, MyComparator> suburbs = Utils::set_union(diff, excluded, graph->comp);

    switch (Globals::filteringValue)
    {
    case BLOOM_FILTER:
      graph->candidateBloomFilter(newClique, suburbs);
      break;
    case CLIQUE_FILTER:
      graph->candidateCliqueFilter(newClique, suburbs);
      break;
    case NEIGHBOURHOOD_FILTER:
      graph->candidateNeighbourhoodFilter(suburbs, *v);
      break;
    case INCREMENTAL_FILTER:
      graph->candidateIncrementalFilter(clique, suburbs, *v);
      break;
    default:
      break;
    }

    if (Globals::filteringValue == INCREMENTAL_FILTER || (Globals::filteringValue == BLOOM_FILTER && graph->vertices.size() < 128))
    {
      ordCliqueNeighbourhood = suburbs;
      for (vertex_t v : ordCliqueNeighbourhood)
        cliqueNeighbourhood.insert(v);
    }
    else
    {
      cliqueNeighbourhood = graph->cliqueNeighbourhood(newClique, suburbs);
      for (vertex_t v : cliqueNeighbourhood)
        ordCliqueNeighbourhood.insert(v);
    }

    /* If there is no way, with the current cliqueNeighbourhood, to find a biggest clique
       and we search one: then let us go to the next singleton. */
    if ((Globals::maxClique && (biggestClique.size() < newClique.size() + cliqueNeighbourhood.size())) || (!Globals::maxClique && (graph->gillesCoeurCoeur->laurent <= newClique.size() + cliqueNeighbourhood.size())))
    {
      set<vertex_t, MyComparator> new_candidates = Utils::set_intersection(candidates,
                                                                           ordCliqueNeighbourhood, graph->comp);
      set<vertex_t> new_excluded = Utils::set_intersection(excluded, cliqueNeighbourhood);
      findHypercliques(newClique, new_candidates, new_excluded, graph);
    }

    candidates = Utils::set_difference(candidates, singleton, graph->comp);
    excluded = Utils::set_union(excluded, singleton);
    v++;
  }
}

void BronKerbosch::findCliques(set<vertex_t> clique, set<vertex_t, MyComparator> candidates,
                               set<vertex_t> excluded, SweetHBK::Graph *graph)
{

  nbCallToBK++;

  if (Globals::verbose)
    Utils::printCurrentState(clique, candidates, excluded);
  if (clique.size() + candidates.size() < graph->gillesCoeurCoeur->laurent)
    return;

  if (candidates.empty() && excluded.empty())
  {
    if (Globals::verbose)
    {
      cout << "c | ... clique found !" << setw(LENGTH - 19) << "|" << endl;
      cout << DLINE << endl;
    }
    cliques.insert(clique);
  }

  set<vertex_t, MyComparator>::iterator v = candidates.begin();
  while (!candidates.empty() && v != candidates.end() && !Globals::timedout)
  {
    set<vertex_t> singleton;
    singleton.insert(*v);

    findCliques(Utils::set_union(clique, singleton),
                Utils::set_intersection(candidates, graph->gillesCoeurCoeur->g->neighbors(*v), graph->comp),
                Utils::set_intersection(excluded, graph->gillesCoeurCoeur->g->neighbors(*v)),
                graph);

    candidates = Utils::set_difference(candidates, singleton, graph->comp);
    excluded = Utils::set_union(excluded, singleton);
    if (!candidates.empty())
      v = candidates.begin();
  }
}

void BronKerbosch::findCliquesBis(set<vertex_t> clique, set<vertex_t, MyComparator> candidates,
                                  set<vertex_t> excluded, SweetHBK::Graph *graph)
{

  nbCallToBK++;

  if (Globals::verbose)
    Utils::printCurrentState(clique, candidates, excluded);
  if (clique.size() + candidates.size() < graph->gillesCoeurCoeur->laurent)
    return;

  set<vertex_t, MyComparator> ordExcluded(graph->comp);
  for (vertex_t v : excluded)
    ordExcluded.insert(v);

  if (candidates.empty() && (excluded.empty() || (graph->cliqueNeighbourhood(clique, ordExcluded)).empty()))
  {
    cliqueFound(clique);
  }
  else if (!Globals::timedout)
  {
    set<vertex_t, MyComparator> new_candidates = candidates;
    set<vertex_t, MyComparator>::iterator v = new_candidates.begin();

    while (!new_candidates.empty() && v != new_candidates.end() && !Globals::timedout)
    {
      set<vertex_t> singleton;
      singleton.insert(*v);

      set<vertex_t> newClique = Utils::set_union(clique, singleton);

      if (graph->gillesCoeurCoeur->isHyperClique(newClique))
      {
        /* If it is the biggest clique found for now, then we mark it as such. */
        if (Globals::maxClique && (biggestClique.size() < newClique.size()))
        {
          biggestClique = newClique;
        }

        ConstCollection<vertex_t> voisins = graph->gillesCoeurCoeur->g->neighbors(*v);
        if ((Globals::maxClique && (biggestClique.size() < newClique.size() + voisins.size())) || (!Globals::maxClique && graph->gillesCoeurCoeur->laurent <= newClique.size() + voisins.size()))
          findCliquesBis(newClique,
                         Utils::set_intersection(candidates, voisins, graph->comp),
                         Utils::set_intersection(excluded, voisins),
                         graph);
      }

      candidates = Utils::set_difference(candidates, singleton, graph->comp);
      excluded = Utils::set_union(excluded, singleton);
      v++;

      ordExcluded.clear();
      for (vertex_t v : excluded)
        ordExcluded.insert(v);
      if (candidates.empty() && clique.size() >= graph->gillesCoeurCoeur->laurent && (excluded.empty() || (graph->cliqueNeighbourhood(clique, ordExcluded)).empty()))
      {
        cliqueFound(clique);
      }
    }
  }
}

void BronKerbosch::findNonUniformCliques(set<vertex_t> clique, set<vertex_t, MyComparator> candidates,
                                         set<vertex_t> excluded, SweetHBK::Graph *graph)
{

  nbCallToBK++;

  if (Globals::verbose)
    Utils::printCurrentState(clique, candidates, excluded);

  if (candidates.empty() && clique.size() > 1 && (excluded.empty() || (graph->nonUniformNeighbourhood(clique, excluded)).empty()))
  {
    cliqueFound(clique);
  }
  else
  {
    set<vertex_t, MyComparator> new_candidates = candidates;
    set<vertex_t, MyComparator>::iterator v = new_candidates.begin();

    while (!new_candidates.empty() && v != new_candidates.end() && !Globals::timedout)
    {
      set<vertex_t> singleton;
      singleton.insert(*v);

      set<vertex_t> newClique = Utils::set_union(clique, singleton);
      if (graph->gillesCoeurCoeur->isNonUniformClique(newClique))
      {
        findNonUniformCliques(newClique,
                              Utils::set_intersection(candidates, graph->gillesCoeurCoeur->g->neighbors(*v), graph->comp),
                              Utils::set_intersection(excluded, graph->gillesCoeurCoeur->g->neighbors(*v)),
                              graph);
      }

      candidates = Utils::set_difference(candidates, singleton, graph->comp);
      excluded = Utils::set_union(excluded, singleton);
      v++;

      if (candidates.empty() && clique.size() > 1 && (excluded.empty() || (graph->nonUniformNeighbourhood(clique, excluded)).empty()))
      {
        cliqueFound(clique);
      }
    }
  }
}
