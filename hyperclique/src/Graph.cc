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

#include "Graph.h"
#include "BronKerbosch.h"

SweetHBK::Graph::Graph()
{
  gillesCoeurCoeur = new HBKGraph();
  unsigned int nbVertices, nbEdges;

  bloom = new BloomFilter();
  biggestHyperedge = 0;

  Stath *stats;

  for (string line; getline(cin, line);)
  {
    if (line[0] == 'c')
      continue;

    istringstream buf(line);
    istream_iterator<string> beg(buf), end;
    vector<string> tokens(beg, end);

    if (line[0] == 'p')
    {
      nbVertices = stoi(tokens[2]);
      nbEdges = stoi(tokens[3]);

      if (Globals::statsh)
        stats = new Stath(nbVertices, nbEdges);
      else if (Globals::json)
        Globals::jsonLine += ", \"hyperedges\": " + to_string(nbEdges) + ", \"nodes\": " + to_string(nbVertices);
      else
      {
        cout << "c | *  " << setw(8) << nbEdges << " hyperedges" << setw(LENGTH - 23) << "|" << endl;
        cout << "c | *  " << setw(8) << nbVertices << " nodes" << setw(LENGTH - 18) << "|" << endl;
      }

      unsigned int i = 0;
      while (i < nbVertices)
      {
        vertices.insert(gillesCoeurCoeur->g->addVertex());
        i++;
      }
    }
    else
    {
      vector<vertex_t> elements;

      stringstream ss(line);
      string tmp;
      unsigned int id = 0;

      unsigned int i = 0;
      while (getline(ss, tmp, ' '))
      {
        i++;
        if (i == 1)
          id = stoi(tmp);
        else
        {
          int node = stoi(tmp);
          if (node > 0)
          {
            elements.push_back(node);
          }
        }
      }

      if (elements.size() > biggestHyperedge)
        biggestHyperedge = elements.size();

      if (Globals::statsh)
        stats->addEdge(elements);

      if (elements.size() > 0)
      {
        std::sort(elements.begin(), elements.end());
        int laurent = elements.size();
        edges.insert(pair<id_t, vector<vertex_t>>(id, elements));
        laurentToEdges[laurent].insert(id);
        laurentToVertices[laurent].insert(elements.begin(), elements.end());
        if (Globals::filteringValue == BLOOM_FILTER)
          bloom->add(elements);
      }
    }
  }

  if (Globals::statsh)
  {
    stats->computeStat();
    if (Globals::json)
    {
      cout << Globals::jsonLine;
      stats->printJ();
      cout << "]}" << endl;
    }
    else
      stats->print();
  }
  else if (Globals::json)
    Globals::jsonLine += ", \"maxrank\": " + to_string(biggestHyperedge);
  else
    cout << "c | *  " << setw(8) << biggestHyperedge << " max rank" << setw(LENGTH - 21) << "|" << endl;
}

void SweetHBK::Graph::orderSeq(set<vertex_t> vertices)
{

  // Create a management instance of the 'htd' library in order to allow centralized configuration.
  unique_ptr<LibraryInstance> manager(createManagementInstance(Id::FIRST));

  /**
   *  Optionally, we can set the vertex elimination algorithm.
   *  We decide to use the min-degree heuristic in this case.
   */

  IOrderingAlgorithm *ordering = NULL;
  switch (Globals::orderingValue)
  {
  case MIN_DEGREE_ORDERING:
    ordering = new MinDegreeOrderingAlgorithm(manager.get());
    break;

  case MIN_FILL_ORDERING:
    ordering = new MinFillOrderingAlgorithm(manager.get());
    break;

  case NATURAL_ORDERING:
    ordering = new NaturalOrderingAlgorithm(manager.get());
    break;

  case DEGENERACY_ORDERING:
    ordering = new DegenerencyOrderingAlgorithm(manager.get());
    break;

  case MAX_DEGREE_ORDERING:
    ordering = new MaxDegreeOrderingAlgorithm(manager.get());
    break;

  default:
    ordering = new RandomOrderingAlgorithm(manager.get());
  }

  IVertexOrdering *order = ordering->computeOrdering(*(gillesCoeurCoeur->g));
  vector<vertex_t> sequence = order->sequence();

  vector<vertex_t> val(sequence.size());
  for (size_t i = 0; i < sequence.size(); i++)
  {
    val[sequence[i] - 1] = i + 1;
  }

  delete ordering;
  comp.setSequence(val);
}

map<id_t, vertex_t> SweetHBK::Graph::getUnit()
{
  map<id_t, vertex_t> res;
  for (auto edge : edges)
  {
    if (edge.second.size() == 1)
    {
      res[edge.first] = edge.second[0];
    }
  }
  return res;
}

void SweetHBK::Graph::getHyperClique(set<vertex_t, MyComparator> &vertices)
{
  set<vertex_t> R, X;
  getHyperClique(R, vertices, X);
  return;
}

void SweetHBK::Graph::getHyperClique(set<vertex_t> &clique, set<vertex_t, MyComparator> &candidates, set<vertex_t> &excluded)
{
  if (gillesCoeurCoeur->g->edgeCount() == 0)
    return;
  BronKerbosch::findHypercliques(clique, candidates, excluded, this);
  return;
}

void SweetHBK::Graph::getHyperCliqueCE(set<vertex_t, MyComparator> &vertices)
{
  set<vertex_t> R, X;
  getHyperCliqueCE(R, vertices, X);
  return;
}

void SweetHBK::Graph::getHyperCliqueCE(set<vertex_t> &clique, set<vertex_t, MyComparator> &candidates, set<vertex_t> &excluded)
{
  if (gillesCoeurCoeur->g->edgeCount() == 0)
    return;
  BronKerbosch::findCliquesBis(clique, candidates, excluded, this);
  return;
}

void SweetHBK::Graph::getHyperCliqueCE_HBK(set<vertex_t, MyComparator> &vertices)
{
  set<vertex_t> R, X;
  getHyperCliqueCE_HBK(R, vertices, X);
  return;
}

void SweetHBK::Graph::getHyperCliqueCE_HBK(set<vertex_t> &clique, set<vertex_t, MyComparator> &candidates, set<vertex_t> &excluded)
{
  if (gillesCoeurCoeur->g->edgeCount() == 0)
    return;
  set<set<vertex_t>> cliques(BronKerbosch::cliques);
  BronKerbosch::cliques.clear();
  BronKerbosch::findCliques(clique, candidates, excluded, this);

  if (gillesCoeurCoeur->laurent > 2)
  {
    map<set<vertex_t>, set<id_t>> ncliques;
    ConstCollection<Hyperedge> hyperedges = gillesCoeurCoeur->g->hyperedges();
    pair<vector<id_t>, vector<index_t>> ids = Utils::toVectorIds(hyperedges);
    map<id_t, vector<vertex_t>> mhyperedges = Utils::toMap(hyperedges);

    for (const set<vertex_t> &clique : BronKerbosch::cliques)
    {

      unsigned long order = clique.size();

      if (order < BronKerbosch::biggestClique.size())
        continue;

      if (order >= gillesCoeurCoeur->laurent)
      {
        set<id_t> clqEdges = gillesCoeurCoeur->getEdges(ids.second, clique);
        if (Utils::binom(order, gillesCoeurCoeur->laurent) == clqEdges.size())
        { // a real hyperclique
          if (Globals::maxClique && (BronKerbosch::biggestClique.size() < clique.size()))
            BronKerbosch::biggestClique = clique;

          cliques.insert(clique);
        }
        else
        {
          ncliques.emplace(clique, clqEdges);
        }
      }
    }

    if (ncliques.size() > 0)
    { // at least a non hyperclique
      BronKerbosch::cliques.clear();
      for (pair<set<vertex_t>, set<id_t>> p : ncliques)
      {
        ids = Utils::toVectorIds(gillesCoeurCoeur->g->hyperedges());
        set<vertex_t, MyComparator> vertices(comp);
        for (id_t e : ids.first)
        {
          gillesCoeurCoeur->g->removeEdge(e);
        }
        for (const id_t id : p.second)
        {
          gillesCoeurCoeur->g->addEdge(mhyperedges[id]);
          vertices = Utils::set_union(vertices, mhyperedges[id], comp);
        }
        getHyperClique(vertices);
      }

      set<set<vertex_t>> newCliques;
      for (const set<vertex_t> &clique : BronKerbosch::cliques)
      {
        if (!Utils::is_included(cliques, clique))
          newCliques.insert(clique);
      }

      for (const set<vertex_t> &clique : newCliques)
      {
        if (Globals::maxClique && (BronKerbosch::biggestClique.size() < clique.size()))
        {
          BronKerbosch::biggestClique = clique;
        }
        cliques.insert(clique);
      }

      BronKerbosch::cliques = cliques;
    }
    BronKerbosch::cliques = cliques;
  }
  else
  {
    for (set<vertex_t> clique : cliques)
      BronKerbosch::cliques.insert(clique);
  }

  return;
}

void SweetHBK::Graph::createGilles(unsigned int laurent, unsigned int vertices)
{
  gillesCoeurCoeur = new HBKGraph(laurent, vertices);

  for (id_t edgeId : laurentToEdges[laurent])
  {
    vector<vertex_t> vedge = edges[edgeId];
    id_t givenId = gillesCoeurCoeur->g->addEdge(std::move(vedge));
    edgesMap[givenId] = edgeId;
  }

  if (Globals::nodeFilteringValue != NO_FILTER)
  {
    laurentFrequences.clear();
    for (vertex_t v : laurentToVertices[laurent])
    {
      laurentFrequences[v] = gillesCoeurCoeur->g->hyperedges(v).size();
    }
  }
}

void SweetHBK::Graph::remove(vertex_t v, set<vertex_t> *laurentVertices)
{
  ConstCollection<Hyperedge> hypere = gillesCoeurCoeur->g->hyperedges(v);
  for (Hyperedge he : hypere)
  {
    vector<vertex_t> tmp = he.sortedElements();
    set<vertex_t> hedge;
    for (vector<vertex_t>::iterator it = tmp.begin(); it != tmp.end(); it++)
    {
      hedge.insert(*it);
      laurentFrequences[*it]--;
    }
    BronKerbosch::cliques.insert(hedge);
    gillesCoeurCoeur->g->removeEdge(he.id());
  }
  laurentVertices->erase(v);
}

void SweetHBK::Graph::filterByFreq(unsigned int laurent, set<vertex_t> *laurentVertices)
{

  set<pair<vertex_t, unsigned int>, FreqComparator> freqOrd;
  for (auto freq : laurentFrequences)
  {
    freqOrd.insert(make_pair(freq.first, freq.second));
  }

  for (pair<vertex_t, unsigned int> f : freqOrd)
  {
    // If the vertex is not in at least laurent edges then it cannot be in a clique.
    // We can thus remove it as a candidate and all the edges it is part of.
    if (laurentFrequences[f.first] < laurent)
    {
      remove(f.first, laurentVertices);
    }
  }
}

void SweetHBK::Graph::filterByCoOccurrences(unsigned int laurent, set<vertex_t> *laurentVertices)
{

  set<pair<vertex_t, unsigned int>, FreqComparator> freqOrd;
  for (auto freq : laurentFrequences)
  {
    freqOrd.insert(make_pair(freq.first, freq.second));
  }

  for (pair<vertex_t, unsigned int> f : freqOrd)
  {
    if (laurentFrequences[f.first] < laurent)
    {
      // cout << "(removed) " << flush;
      remove(f.first, laurentVertices);
    }
    // Otherwise, if a vertex does not have at least laurent - 1 occurrences with
    // at least laurent negihbours then it cannot be in a clique.
    // We can thus remove it as a candidate and all the edges it is part of.
    else
    {
      set<pair<vertex_t, unsigned int>, FreqComparator> coOcurOrd;
      set<id_t> hyperedges = Utils::toSetId(gillesCoeurCoeur->g->hyperedges(f.first));
      for (vertex_t v : gillesCoeurCoeur->g->neighbors(f.first))
      {
        set<id_t> vhyperedges = Utils::toSetId(gillesCoeurCoeur->g->hyperedges(v));
        coOcurOrd.insert(make_pair(v, Utils::set_intersection(hyperedges, vhyperedges).size()));
      }

      if (coOcurOrd.size() < laurent)
      {
        remove(f.first, laurentVertices);
      }
      else
      {
        set<pair<vertex_t, id_t>, FreqComparator>::iterator it = prev(coOcurOrd.end(), laurent);
        if (it->second < laurent - 1)
        {
          // cout << "(removed occ) " << flush;
          remove(f.first, laurentVertices);
        }
      }
    }
  }
}

double SweetHBK::Graph::enumerateHyperCliques(void (SweetHBK::Graph::*func)(set<vertex_t, MyComparator> &))
{
  BronKerbosch::nbCallToBK = 0;
  Globals::timedout = false;
  unsigned int laurent = biggestHyperedge;

  CardinalityConstraint::nb = 0;
  CardinalityConstraint::explained = false;
  CardinalityConstraint::alreadyDone.clear();
  CardinalityConstraint::all_constraints.clear();

  if (!Globals::json)
    cout << LINE << endl;
  else
    cout << ", \"cliques\": [";
  chrono::high_resolution_clock::time_point all_start = chrono::high_resolution_clock::now();
  vector<Clique *> all_cliques;
  int previous = 0;

  // cout << "blu" << endl;

  /* Construct the graph which link 'laurent' variables if and only if they all appear negatively in a 'laurent'-clause */
  while (laurent != 1 && !Globals::timedout)
  {
    BronKerbosch::cliques.clear();
    createGilles(laurent, vertices.size());
    set<vertex_t> laurentVertices = laurentToVertices[laurent];

    chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();
    unsigned int n = laurentVertices.size(); // gillesCoeurCoeur->g->vertices().size();
    unsigned int m = gillesCoeurCoeur->g->edgeCount();
    // cout << "n : " << n << ", m : " << m << endl;

    if (m > 0)
    {
      if (m > laurent /* = Utils::binom(laurent + 1, laurent)*/)
      { // There may be a clique of size laurent + 1

        // Add option to choose between the two or not
        if (Globals::nodeFilteringValue == DEGREE_FILTER || Globals::nodeFilteringValue == BOTH_FILTER)
          filterByFreq(laurent, &laurentVertices);
        if (Globals::nodeFilteringValue == COOCCURRENCE_FILTER || Globals::nodeFilteringValue == BOTH_FILTER)
          filterByCoOccurrences(laurent, &laurentVertices);

        orderSeq(laurentVertices);
        set<vertex_t, MyComparator> candidates(comp);
        for (vertex_t v : laurentVertices)
        {
          candidates.insert(v);
        }
        (this->*func)(candidates);
      }
      else
      {
        // Each hyperedge is a clique
        for (const Hyperedge &e : gillesCoeurCoeur->g->hyperedges())
        {
          vector<vertex_t> tmp = e.sortedElements();
          set<vertex_t> hedge;
          for (const vertex_t v : tmp)
            hedge.insert(v);

          BronKerbosch::cliques.insert(hedge);

          /* If each hyperedge is a clique, then the biggest hyperedge is necessary the biggest clique. */
          if (Globals::maxClique && (BronKerbosch::biggestClique.size() < hedge.size()))
          {
            BronKerbosch::biggestClique = hedge;
            break;
          }
        }
      }
      chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
      double extractionTime = ((end - start).count() / 1e9);
      int l10 = ceil(log10(extractionTime));
      l10 = l10 <= 0 ? 1 : l10;

      for (set<vertex_t> clique : BronKerbosch::cliques)
      {
        /* If each hyperedge is a clique, then the biggest hyperedge is necessary the biggest clique. */
        if (Globals::maxClique && (BronKerbosch::biggestClique.size() < clique.size()))
        {
          BronKerbosch::biggestClique = clique;
        }
        all_cliques.push_back(new Clique(laurent, clique, gillesCoeurCoeur->getEdges(clique)));
      }

      if (Globals::print && !Globals::json)
      {
        cout << "c | " << setw(9) << n << " | " << setw(9) << m << " | "
             << setw(9) << BronKerbosch::cliques.size() << " | " << setw(9)
             << laurent << " | " << setprecision(9) << extractionTime
             << " seconds" << setw(LENGTH - 67 - l10) << "|" << endl;
        cout << "c | nb calls to BK: " << setw(10) << BronKerbosch::nbCallToBK - previous << setw(LENGTH - 27) << "|" << endl;
        previous = BronKerbosch::nbCallToBK;
      }
      if (Globals::json)
      {
        if (laurent < biggestHyperedge)
          cout << ", ";
        cout << "{\"nbnodes\": " << n << ", \"nbedges\": " << m << ", \"nbcliques\": " << BronKerbosch::cliques.size()
             << ", \"rank\": " << laurent << ", \"time\": " << extractionTime << ", \"nbcalls\": "
             << BronKerbosch::nbCallToBK - previous;
        if (Globals::debug)
        {
          cout << ", \"cl\": ";
          BronKerbosch::json();
        }
        cout << "}";
      }

      if (Globals::statsc)
      {
        Statc stat(BronKerbosch::cliques);
        stat.print();
      }

      if (Globals::printedge)
        BronKerbosch::printEdge(this);

      if (!Globals::maxClique && Globals::debug && !Globals::json)
        BronKerbosch::print();

      if (Globals::maxClique)
      {
        BronKerbosch::printMaxClique();
        BronKerbosch::cleanBiggest();
      }
    }
    --laurent;
  }
  if (Globals::json)
  {
    cout << "], \"totalcliques\": " << all_cliques.size() << ", \"totalcalls\": " << BronKerbosch::nbCallToBK << ", \"timedout\": " << Globals::timedout;
  }
  else
  {
    cout << "c | total cliques found: " << setw(11) << all_cliques.size() << setw(LENGTH - 33) << "|" << endl;
    cout << "c | total nb calls to BK: " << setw(10) << BronKerbosch::nbCallToBK << setw(LENGTH - 33) << "|" << endl;
    cout << "c | timedout: " << setw(22) << Globals::timedout << setw(LENGTH - 33) << "|" << endl;
  }

  if (Globals::cardinality)
  {
    if (Globals::verbose)
    {
      cout << LINE << endl;
      cout << "c | post-processing... " << setw(LENGTH - 20) << "|" << endl;
    }
    set<CardinalityConstraint *> cardinalities;
    set<CardinalityConstraint *> unitscardinalities;
    map<id_t, vertex_t> units;
    map<vertex_t, CardinalityConstraint *> tounitscardinalities;
    ConstCollection<Hyperedge> hyperedges = gillesCoeurCoeur->g->hyperedges();
    pair<vector<Hyperedge>, vector<id_t>> edg = Utils::toVectors(hyperedges);
    for (auto e : edg.second)
      gillesCoeurCoeur->g->removeEdge(e);
    for (auto edge : edges)
    {
      id_t givenId = gillesCoeurCoeur->g->addEdge(edge.second);
      edgesMap[givenId] = edge.first;
      if (edge.second.size() == 1)
      {
        units[edge.first] = edge.second[0];
      }
    }

    if (Globals::verbose)
      cout << "c | found cliques... " << setw(LENGTH - 18) << "|" << endl;
    for (unsigned long i = 0; i < all_cliques.size(); i++)
    {
      set<CardinalityConstraint *> tmp = CardinalityConstraint::toCardinalityConstraint(all_cliques[i], edgesMap);
      for (CardinalityConstraint *card : tmp)
      {
        if (Globals::verbose)
        {
          card->print();
          all_cliques[i]->print(edgesMap);
        }
        cardinalities.insert(card);
      }
    }

    if (Globals::verbose)
      cout << "c | unit clauses... " << setw(LENGTH - 17) << "|" << endl;
    for (auto literals : units)
    {
      if (Globals::verbose)
        cout << "c | { " << literals.second << " }" << endl
             << "c | [ " << literals.first << " ]" << endl;
      set<id_t> clause;
      clause.insert(literals.first);
      set<CardinalityConstraint *> tmp = CardinalityConstraint::toCardinalityConstraint(literals.second, clause);
      for (CardinalityConstraint *unit : tmp)
      {
        if (Globals::verbose)
          unit->print();
        unitscardinalities.insert(unit);
        tounitscardinalities[*(unit->variables.begin())] = unit;
      }
    }
    set<vertex_t> unitvariables = Utils::keys(tounitscardinalities);
    if (Globals::verbose)
      cout << LINE << endl
           << "c | deletes unit clauses from cardinality constraints..." << setw(LENGTH - 53) << "|" << endl;

    set<CardinalityConstraint *> newCard = CardinalityConstraint::simplify(cardinalities, tounitscardinalities, unitvariables);
    if (Globals::verbose)
    {
      for (CardinalityConstraint *card : newCard)
      {
        card->print();
      }
    }

    // do {
    if (Globals::verbose)
      cout << LINE << endl;
    cardinalities = newCard;
    newCard = CardinalityConstraint::merge(cardinalities);
    if (Globals::verbose)
    {
      for (CardinalityConstraint *card : newCard)
      {
        card->print();
      }
    }
    //} while (newCard.size() != cardinalities.size());
    cardinalities = newCard;

    if (Globals::print || Globals::verbose || Globals::debug)
    {
      cout << "c | " << setw(5) << cardinalities.size() << " constraint(s)" << setw(LENGTH - 20) << "|" << endl;
      vector<id_t> inferedBy;
      for (CardinalityConstraint *card : cardinalities)
      {
        if (Globals::verbose || Globals::debug || CardinalityConstraint::explained)
          card->print();
        if (!card->clauses && CardinalityConstraint::explained)
          for (id_t id : card->inferedBy)
            inferedBy.push_back(id);
      }

      if (CardinalityConstraint::explained)
      {
        cout << "c | Explanation found using constraint(s)" << setw(LENGTH - 38) << "|" << endl;
        while (!inferedBy.empty())
        {
          id_t id = inferedBy.back();
          inferedBy.pop_back();
          CardinalityConstraint::all_constraints[id]->print();
          if (!CardinalityConstraint::all_constraints[id]->clauses)
            for (id_t id : CardinalityConstraint::all_constraints[id]->inferedBy)
              inferedBy.push_back(id);
        }
      }
    }
  }

  chrono::high_resolution_clock::time_point all_end = chrono::high_resolution_clock::now();
  double all_extractionTime = ((all_end - all_start).count() / 1e9);
  int l10 = ceil(log10(all_extractionTime));
  l10 = l10 <= 0 ? 1 : l10;
  if (Globals::json)
    cout << ", \"bktotaltime\": " << all_extractionTime;
  else
    cout << "c | BK total time: " << setprecision(9) << all_extractionTime << " seconds" << setw(LENGTH - 34 - l10) << "|" << endl;

  return (all_extractionTime);
}

set<vertex_t> SweetHBK::Graph::cliqueNeighbourhood(set<vertex_t> &clique, set<vertex_t, MyComparator> &suburbs)
{
  set<vertex_t> result;
  int n = clique.size();

  for (const vertex_t v : suburbs)
  {
    if (gillesCoeurCoeur->isNeighbour(clique, v))
    {
      set<vertex_t> singleton;
      singleton.insert(v);

      set<vertex_t> possibleClique = Utils::set_union(clique, singleton);

      set<id_t> possibleCliqueEdges = gillesCoeurCoeur->getEdges(possibleClique);

      if (possibleCliqueEdges.size() >= Utils::binom(n + 1, gillesCoeurCoeur->laurent))
      {
        result.insert(v);
      }
    }
  }
  return result;
}

set<vertex_t, MyComparator> SweetHBK::Graph::ordCliqueNeighbourhood(set<vertex_t> &clique, set<vertex_t, MyComparator> &suburbs)
{
  set<vertex_t, MyComparator> result(comp);
  int n = clique.size();

  for (const vertex_t v : suburbs)
  {
    if (gillesCoeurCoeur->isNeighbour(clique, v))
    {
      set<vertex_t> singleton;
      singleton.insert(v);

      set<vertex_t> possibleClique = Utils::set_union(clique, singleton);

      set<id_t> possibleCliqueEdges = gillesCoeurCoeur->getEdges(possibleClique);

      if (possibleCliqueEdges.size() >= Utils::binom(n + 1, gillesCoeurCoeur->laurent))
      {
        result.insert(v);
      }
    }
  }
  return result;
}

set<vertex_t> SweetHBK::Graph::nonUniformNeighbourhood(set<vertex_t> &clique, set<vertex_t> &suburbs)
{
  set<vertex_t> result;

  for (const vertex_t v : suburbs)
  {
    if (gillesCoeurCoeur->isNeighbour(clique, v))
    {
      set<vertex_t> singleton;
      singleton.insert(v);

      set<vertex_t> possibleClique = Utils::set_union(clique, singleton);

      if (gillesCoeurCoeur->isNonUniformClique(possibleClique))
      {
        result.insert(v);
      }
    }
  }
  return result;
}

void SweetHBK::Graph::candidateBloomFilter(set<vertex_t> &clique, set<vertex_t, MyComparator> &suburbs)
{
  set<vertex_t> tmp(suburbs.begin(), suburbs.end());
  suburbs.clear();
  int n = clique.size();

  for (const vertex_t v : tmp)
  {
    if (gillesCoeurCoeur->isNeighbour(clique, v))
    {
      set<vertex_t> possibleClique(clique);
      possibleClique.insert(v);

      size_t nb = bloom->getNb(possibleClique, gillesCoeurCoeur->laurent);

      if (nb >= Utils::binom(n + 1, gillesCoeurCoeur->laurent))
      {
        suburbs.insert(v);
      }
    }
  }
}

void SweetHBK::Graph::candidateIncrementalFilter(set<vertex_t> &clique, set<vertex_t, MyComparator> &suburbs, vertex_t lastAdded)
{
  if (!clique.size())
  {
    suburbs = Utils::set_intersection(gillesCoeurCoeur->g->neighbors(lastAdded), suburbs, comp);
  }
  else if (clique.size() < gillesCoeurCoeur->laurent - 2)
  {
    set<vertex_t> vclique(clique.begin(), clique.end());
    vclique.insert(lastAdded);
    suburbs = ordCliqueNeighbourhood(vclique, suburbs);
  }
  else if (clique.size() + suburbs.size() + 1 == gillesCoeurCoeur->laurent)
  {
    set<vertex_t> tmp(clique);
    tmp.insert(suburbs.begin(), suburbs.end());
    tmp.insert(lastAdded);
    if (gillesCoeurCoeur->getEdges(tmp).size() > 0)
      return;
    else
      suburbs.clear();
  }
  else
  {
    set<vertex_t> neighbor_of_v;

    vector<vertex_t> a(clique.begin(), clique.end());

    set<vertex_t> subset;
    vector<set<vertex_t>> res;

    /* Let us generate all the subset of size rank-1 from clique.
     * With this we will add, the lastAdded and each subsurbs to check if the hyperedge exists.
     */
    Utils::generateSubSets(a, gillesCoeurCoeur->laurent - 2, 0, subset, res);

    for (auto s : suburbs)
    {
      bool toAdd = true;
      for (auto test : res)
      {
        test.insert(s);
        test.insert(lastAdded);

        if (gillesCoeurCoeur->getEdges(test).size() == 0)
        {
          toAdd = false;
          break;
        }
      }

      if (toAdd)
        neighbor_of_v.insert(s);
    }

    suburbs = Utils::set_intersection(neighbor_of_v, suburbs, comp);
  }
}

/**
 * Computes the clique expansion graph and uses Bron & Kerbosch (BronKerbosch::findCliques)
 * to approximate the set of candidates.
 * We assume that we are in a r-uniform hypergraph and that |clique| + |suburbs| >= r.
 */
void SweetHBK::Graph::candidateCliqueFilter(set<vertex_t> &clique, set<vertex_t, MyComparator> &suburbs)
{
  set<vertex_t> s;
  for (vertex_t v : suburbs)
    s.insert(v);
  set<vertex_t> all = Utils::set_union(clique, s);
  set<vertex_t, MyComparator> ordall = Utils::set_union(clique, suburbs, comp);

  set<vertex_t> R, X;
  set<vertex_t, MyComparator> P(comp);

  for (const Hyperedge &hedge : gillesCoeurCoeur->g->hyperedges())
  {

    const vector<vertex_t> vertices = hedge.sortedElements();
    vector<vertex_t> tmp(vertices.size());
    vector<vertex_t>::iterator it;

    it = set_intersection(vertices.begin(), vertices.end(), all.begin(), all.end(), tmp.begin());

    tmp.resize(it - tmp.begin());
    if (tmp.size() == vertices.size()) // no vertex not in (clique U suburbs)
      for (const vertex_t v : vertices)
        P.insert(v);
  }

  int tmpCalls = BronKerbosch::nbCallToBK;
  set<set<vertex_t>> tmpCliques(BronKerbosch::cliques);

  BronKerbosch::nbCallToBK = 0;
  BronKerbosch::cliques.clear();
  BronKerbosch::findCliques(R, P, X, this);

  set<vertex_t> all_node_in_cliques;
  for (const set<vertex_t> &clq : BronKerbosch::cliques)
  {
    all_node_in_cliques = Utils::set_union(clq, all_node_in_cliques);
  }

  suburbs = Utils::set_intersection(suburbs, all_node_in_cliques, comp);

  BronKerbosch::nbCallToBK = tmpCalls;
  BronKerbosch::cliques = tmpCliques;
}

void SweetHBK::Graph::candidateNeighbourhoodFilter(set<vertex_t, MyComparator> &suburbs, vertex_t lastAdded)
{
  ConstCollection<vertex_t> v_neighbors = gillesCoeurCoeur->g->neighbors(lastAdded);
  suburbs = Utils::set_intersection(v_neighbors, suburbs, comp);
}

void SweetHBK::Graph::enumerateNonUniformCliques()
{

  BronKerbosch::nbCallToBK = 0;
  BronKerbosch::cliques.clear();

  unsigned int n = gillesCoeurCoeur->g->vertices().size();
  gillesCoeurCoeur->laurent = biggestHyperedge;

  if (gillesCoeurCoeur->g->edgeCount() == 0)
    return;

  chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();

  set<vertex_t> R, X;
  set<vertex_t, MyComparator> P(comp);

  for (const vertex_t i : gillesCoeurCoeur->g->vertices())
  {
    P.insert(i);
  }
  if (Globals::verbose)
    cout << "c | ... ordering done ..." << setw(LENGTH - 22) << "|" << endl;
  if (Globals::verbose)
    cout << "c | ... computing clique ..." << setw(LENGTH - 25) << "|" << endl;

  BronKerbosch::findNonUniformCliques(R, P, X, this);

  chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();

  double extractionTime = ((end - start).count() / 1e9);
  int l10 = ceil(log10(extractionTime));
  l10 = l10 < 0 ? 1 : l10;

  if (Globals::print)
  {
    cout << "c | ";
    cout << setw(9) << n << " | " << setw(9) << gillesCoeurCoeur->g->edgeCount() << " | "
         << setw(9) << BronKerbosch::cliques.size() << " | " << setw(9)
         << gillesCoeurCoeur->laurent << " | " << setprecision(9) << extractionTime
         << " seconds" << setw(LENGTH - 67 - l10) << "|" << endl;
  }

  if (Globals::statsc)
  {
    Statc stat(BronKerbosch::cliques);
    stat.print();
  }

  if (Globals::debug)
    BronKerbosch::print();

  if (Globals::printedge)
    BronKerbosch::printEdge(this);

  cout << "c | nb calls to BK: " << setw(10) << BronKerbosch::nbCallToBK << setw(LENGTH - 27) << "|" << endl;
  return;
}
