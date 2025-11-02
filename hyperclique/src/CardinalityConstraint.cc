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

#include "CardinalityConstraint.h"

id_t CardinalityConstraint::nb = 0;
bool CardinalityConstraint::explained = false;
map<id_t, set<id_t>> CardinalityConstraint::alreadyDone;
map<id_t, CardinalityConstraint *> CardinalityConstraint::all_constraints;

set<vertex_t> CardinalityConstraint::reduce(set<vertex_t> variables)
{
  set<vertex_t> res;

  set<vertex_t>::iterator it = variables.begin();
  for (; it != variables.end(); it++)
  {
    vertex_t i = *it;

    set<vertex_t>::iterator jt = it;
    jt++;
    vertex_t j = jt != variables.end() ? *jt : 0;
    if (i >> 1 != j >> 1)
    { // not the same variable
      res.insert(i);
    }
    else
    { // ignore the two literals
      it++;
    }
  }
  return res;
}

set<vertex_t> CardinalityConstraint::inverse(set<vertex_t> variables)
{
  set<vertex_t> res;
  for (vertex_t v : variables)
  {
    if (v % 2 == 0)
      res.insert(v + 1);
    else
      res.insert(v - 1);
  }
  return res;
}

set<CardinalityConstraint *> CardinalityConstraint::toCardinalityConstraint(Clique *clique,
                                                                            map<id_t, id_t> edgesMap)
{
  unsigned long nb_neg = 0, nb_pos = 0;
  set<CardinalityConstraint *> constraints;
  set<vertex_t> variables;
  int reduceBound = 0;

  set<vertex_t>::iterator it = clique->vertices.begin();
  for (; it != clique->vertices.end(); it++)
  {
    vertex_t i = *it;

    set<vertex_t>::iterator jt = it;
    jt++;
    vertex_t j = jt != clique->vertices.end() ? *jt : 0;
    if (i >> 1 != j >> 1)
    { // not the same variable
      variables.insert(i);
      if (i % 2 == 0)
        nb_pos++;
      else
        nb_neg++;
    }
    else
    { // ignore the two literals
      it++;
      reduceBound++;
    }
  }
  unsigned long nb = variables.size();
  if (nb > 0)
  {
    if (nb_neg == nb)
    { // only negative literals => atMost(laurent - 1) constraint => atLeast(1 - laurent)
      constraints.insert(new CardinalityConstraint(variables, 1 - clique->laurent + reduceBound,
                                                   cardType::atleast, clique->getClauses(edgesMap), true));
    }
    else if (nb_pos == nb)
    { // only positive literals => atLeast(nb - laurent + 1) constraint
      constraints.insert(new CardinalityConstraint(variables, nb - clique->laurent + 1 + reduceBound,
                                                   cardType::atleast, clique->getClauses(edgesMap), true));
    }
    else
    { // mixed clique => atLeast(nb - laurent + 1) constraint
      constraints.insert(new CardinalityConstraint(variables, nb - clique->laurent + 1 - nb_neg + reduceBound,
                                                   cardType::atleast, clique->getClauses(edgesMap), true));
    }
  }
  return constraints;
}

set<CardinalityConstraint *> CardinalityConstraint::toCardinalityConstraint(vertex_t literal, set<id_t> inferedBy)
{
  set<CardinalityConstraint *> constraints;
  set<vertex_t> variables;
  variables.insert(literal);
  if (literal % 2 == 0)
  {
    constraints.insert(new CardinalityConstraint(variables, 1, cardType::equals, inferedBy, true));
    constraints.insert(new CardinalityConstraint(inverse(variables), -1, cardType::equals, inferedBy, true));
  }
  else
  {
    constraints.insert(new CardinalityConstraint(variables, 0, cardType::equals, inferedBy, true));
    constraints.insert(new CardinalityConstraint(inverse(variables), 0, cardType::equals, inferedBy, true));
  }
  return constraints;
}

bool CardinalityConstraint::add(set<CardinalityConstraint *> &cardinalities, CardinalityConstraint *newcard)
{
  for (const CardinalityConstraint *card : cardinalities)
  {
    if (card->equals(newcard))
    {
      nb--;
      return false;
    }
  }
  cardinalities.insert(newcard);
  return true;
}

void CardinalityConstraint::print()
{
  cout << "c | " << numId << ":";
  for (vertex_t v : variables)
  {
    if (v % 2 == 0)
      cout << " + x" << v / 2;
    else
      cout << " - x" << (v - 1) / 2;
  }
  if (type == cardType::atleast)
    cout << " >= ";
  else if (type == cardType::equals)
    cout << " = ";
  cout << bound << endl;

  if (inferedBy.size() > 0)
  {
    cout << "c | ";
    if (clauses)
      cout << "c";
    else
      cout << "m";
    for (id_t i : inferedBy)
      cout << " " << i;
    cout << endl;
  }
}

bool CardinalityConstraint::add(set<CardinalityConstraint *> &cardinalities, set<vertex_t> variables, int bound,
                                cardType type, set<id_t> inferedBy)
{
  bool added = false;
  if (variables.size() == 1)
  {
    vertex_t v = *(variables.begin());
    if ((v % 2 == 0 && bound > 1) || (v % 2 != 0 && bound > 0))
    {
      // cardinality explained
      cout << LINE << endl
           << "c | EXPLAINED" << setw(LENGTH - 10) << "|" << endl;
      explained = true;
      cardinalities.clear();
      cardinalities.insert(new CardinalityConstraint(variables, bound, type, inferedBy));
      added = true;
    }
    else if ((v % 2 == 0 && bound < 1) || (v % 2 != 0 && bound < 0))
    {
      if (Globals::verbose)
      {
        cout << "c | d ";
        if (v % 2 == 0)
          cout << "+ x" << v / 2;
        else
          cout << "- x" << (v - 1) / 2;
        if (type == cardType::atleast)
          cout << " >= ";
        else
          cout << " = ";
        cout << bound << endl
             << "c | m";
        for (id_t id : inferedBy)
          cout << " " << id;
        cout << endl;
      }
      added = false;
    }
    else
    {
      added = add(cardinalities, new CardinalityConstraint(variables, bound, type, inferedBy));
      added |= add(cardinalities, new CardinalityConstraint(inverse(variables), -bound, type, inferedBy));
    }
  }
  else if (variables.size() > 0)
  {
    int nb_pos = 0;
    for (vertex_t v : variables)
    {
      if (v % 2 == 0)
        nb_pos++;
    }
    if (bound > nb_pos)
    {
      // cardinality explained
      cout << LINE << endl
           << "c | EXPLAINED" << setw(LENGTH - 10) << "|" << endl;
      explained = true;
      cardinalities.clear();
      cardinalities.insert(new CardinalityConstraint(variables, bound, type, inferedBy));
      added = true;
    }
    else
    {
      int nb_neg = variables.size() - nb_pos;
      bound = max(bound, -nb_neg);
      added = add(cardinalities, new CardinalityConstraint(variables, bound, type, inferedBy));
    }
  }
  else if (bound > 0)
  {
    cout << "c | WEAK EXPLAINATION 0 >= " << bound << endl;
    explained = true;
    cardinalities.clear();
    cout << "c | m";
    vector<id_t> toadd(inferedBy.begin(), inferedBy.end());
    while (!toadd.empty())
    {
      id_t id = toadd.back();
      toadd.pop_back();
      cout << " " << id;
      cardinalities.insert(all_constraints[id]);
      if (!all_constraints[id]->clauses)
        for (id_t id2 : all_constraints[id]->inferedBy)
        {
          toadd.push_back(id2);
        }
    }
    cout << endl;
    added = true;
  }
  return added;
}

set<CardinalityConstraint *> CardinalityConstraint::simplify(set<CardinalityConstraint *> cardinalities,
                                                             map<vertex_t, CardinalityConstraint *> unitcardinalities, set<vertex_t> unitvariables)
{
  set<CardinalityConstraint *> res;

  for (CardinalityConstraint *c : cardinalities)
  {
    set<vertex_t> todelete = Utils::set_intersection(c->variables, unitvariables);
    if (todelete.size() > 0)
    { // deletes the unit clauses
      set<vertex_t> variables = Utils::set_difference(c->variables, todelete);
      set<id_t> inferedBy;
      inferedBy.insert(c->numId);
      int bound = c->bound;
      todelete = inverse(todelete);
      for (vertex_t v : todelete)
      {
        CardinalityConstraint *unit = unitcardinalities[v];
        bound += unit->bound;
        inferedBy.insert(unit->numId);
      }
      add(res, variables, bound, c->type, inferedBy);
      if (explained)
        return res;
    }
    else
    {
      add(res, c);
    }
  }
  return res;
}

set<CardinalityConstraint *> CardinalityConstraint::merge(set<CardinalityConstraint *> cardinalities)
{
  set<CardinalityConstraint *> res;
  set<CardinalityConstraint *>::iterator it = cardinalities.begin();

  if (explained)
    return cardinalities;

  for (; it != cardinalities.end() && !explained; it++)
  {
    set<CardinalityConstraint *>::iterator jt = it;
    jt++;
    if (!(*it)->infer)
    {
      for (; jt != cardinalities.end() && !explained; jt++)
      {
        if (!(*jt)->infer && !(*it)->merged && !(*jt)->merged && alreadyDone[(*it)->numId].find((*jt)->numId) == alreadyDone[(*it)->numId].end())
        {
          if ((*it)->type == (*jt)->type)
          { // Same constraint type
            if (Utils::set_intersection((*it)->variables, (*jt)->variables).size() == (*it)->variables.size() && (*it)->variables.size() == (*jt)->variables.size())
            {
              // same variables
              // Equalities => k <> k' => Cardinality explained
              if ((*it)->type == cardType::equals)
              {
                int k1 = (*it)->bound;
                int k2 = (*jt)->bound;
                if (k1 == k2)
                {
                  (*jt)->infer = true;
                }
                else
                {
                  // cardinality explained
                  if (!explained)
                    cout << LINE << endl
                         << "c | EXPLAINED" << setw(LENGTH - 10) << "|" << endl;
                  explained = true;
                  res.clear();
                  res.insert(*it);
                  res.insert(*jt);
                  return res;
                }
              }
              // Inequalities => Keep de the most constrainted one
              else
              {
                alreadyDone[(*it)->numId].insert((*jt)->numId);
                alreadyDone[(*jt)->numId].insert((*it)->numId);
                int new_bound = (*it)->bound > (*jt)->bound ? (*it)->bound : (*jt)->bound;
                set<id_t> inferedBy;
                inferedBy.insert((*it)->numId);
                inferedBy.insert((*jt)->numId);
                add(res, new CardinalityConstraint((*it)->variables, new_bound, cardType::atleast, inferedBy));
                (*it)->infer = true;
                (*jt)->infer = true;
                (*it)->merged = true;
                (*jt)->merged = true;
                if (explained)
                  return res;
              }
            }
            else if ((*it)->variables.size() == (*jt)->variables.size() && Utils::set_intersection((*it)->variables, inverse((*jt)->variables)).size() == (*it)->variables.size())
            {
              // same variables, inverse sign
              int lower = (*it)->bound;
              int upper = -(*jt)->bound;
              if (lower == upper)
              {
                if ((*it)->type == cardType::atleast)
                {
                  alreadyDone[(*it)->numId].insert((*jt)->numId);
                  alreadyDone[(*jt)->numId].insert((*it)->numId);
                  set<id_t> inferedBy;
                  inferedBy.insert((*it)->numId);
                  inferedBy.insert((*jt)->numId);
                  add(res, new CardinalityConstraint((*it)->variables, lower, cardType::equals, inferedBy));
                  (*it)->infer = true;
                  (*jt)->infer = true;
                  (*it)->merged = true;
                  (*jt)->merged = true;
                }
                else
                {
                  (*jt)->infer = true; // deletes the redundancy
                }
              }
              else if ((*it)->type == cardType::equals || lower > upper)
              {
                // cardinality explained
                if (!explained)
                  cout << LINE << endl
                       << "c | EXPLAINED" << setw(LENGTH - 10) << "|" << endl;
                explained = true;
                res.clear();
                res.insert(*it);
                res.insert(*jt);
                return res;
              }
            }
            else if (Utils::set_intersection((*it)->variables, (*jt)->variables).empty())
            {
              // no variables in common => merge
              alreadyDone[(*it)->numId].insert((*jt)->numId);
              alreadyDone[(*jt)->numId].insert((*it)->numId);
              set<id_t> inferedBy;
              inferedBy.insert((*it)->numId);
              inferedBy.insert((*jt)->numId);
              set<vertex_t> variables = reduce(Utils::set_union((*it)->variables, (*jt)->variables));
              add(res, variables, (*it)->bound + (*jt)->bound, (*it)->type, inferedBy);
              (*it)->merged = true;
              (*jt)->merged = true;
              if (explained)
                return res;
            }
          }
          else
          { // One constraint >= one constraint =
            set<CardinalityConstraint *>::iterator eq = (*it)->type == cardType::equals ? it : jt;
            set<CardinalityConstraint *>::iterator geq = (*it)->type == cardType::equals ? jt : it;
            int keq = (*eq)->bound;
            int kgeq = (*geq)->bound;
            if ((*it)->variables.size() == (*jt)->variables.size())
            {
              if (Utils::set_intersection((*it)->variables, inverse((*jt)->variables)).size() == (*it)->variables.size())
              {
                // same variables, inverse sign
                keq = -keq;
              }
              else if (Utils::set_intersection((*it)->variables, (*jt)->variables).size() != (*it)->variables.size())
              {
                // no variable in common => do nothing
                continue;
              }
              if (keq < kgeq)
              {
                if (!explained)
                  cout << LINE << endl
                       << "c | EXPLAINED" << setw(LENGTH - 10) << "|" << endl;
                explained = true;
                res.clear();
                res.insert(*it);
                res.insert(*jt);
                return res;
              }
              else
              {
                alreadyDone[(*it)->numId].insert((*jt)->numId);
                alreadyDone[(*jt)->numId].insert((*it)->numId);
                set<id_t> inferedBy;
                inferedBy.insert((*it)->numId);
                inferedBy.insert((*jt)->numId);
                add(res, new CardinalityConstraint((*eq)->variables, (*eq)->bound, cardType::equals, inferedBy));
                (*it)->infer = true;
                (*jt)->infer = true;
                (*it)->merged = true;
                (*jt)->merged = true;
                if (explained)
                  return res;
              }
            }
          }
        }
      }
      if (!(*it)->infer)
        add(res, *it);
    }
  }
  return res;
}