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

#ifndef CARDINALITY_H
#define CARDINALITY_H

#include "Utils.h"
#include "Clique.h"
#include <math.h>

enum cardType
{
  atleast,
  equals
};

class CardinalityConstraint
{
public:
  static id_t nb;
  static bool explained;
  static map<id_t, set<id_t>> alreadyDone;
  static map<id_t, CardinalityConstraint *> all_constraints;

  id_t numId;
  set<vertex_t> variables;
  int bound;
  cardType type;
  set<id_t> inferedBy;
  bool infer;
  bool merged;
  bool clauses;

  CardinalityConstraint(set<vertex_t> variables, int bound, cardType type) : variables(variables), bound(bound), type(type)
  {
    nb++;
    numId = nb;
    infer = false;
    merged = false;
    clauses = false;
    all_constraints[numId] = this;
  }

  CardinalityConstraint(set<vertex_t> variables, int bound, cardType type, set<id_t> inferedBy,
                        bool c = false) : variables(variables), bound(bound), type(type), inferedBy(inferedBy),
                                          clauses(c)
  {
    nb++;
    numId = nb;
    infer = false;
    merged = false;
    all_constraints[numId] = this;
  }

  ~CardinalityConstraint() {}

  void print();

  bool equals(CardinalityConstraint *other) const
  {
    bool eq = false;
    if (bound == other->bound && type == other->type)
    {
      set<vertex_t>::iterator it = variables.begin();
      set<vertex_t>::iterator jt = other->variables.begin();
      eq = true;
      while (eq && it != variables.end() && jt != other->variables.end())
      {
        if (*it != *jt)
          eq = false;
        else
        {
          it++;
          jt++;
        }
      }
      if (eq)
        eq = it == variables.end() && jt == other->variables.end();
    }
    return eq;
  }

  static set<vertex_t> reduce(set<vertex_t> variables);
  static set<vertex_t> inverse(set<vertex_t> variables);
  static set<CardinalityConstraint *> toCardinalityConstraint(Clique *clique,
                                                              map<id_t, id_t> edgesMap);
  static set<CardinalityConstraint *> toCardinalityConstraint(vertex_t literal,
                                                              set<id_t> inferedBy);
  static bool add(set<CardinalityConstraint *> &cardinalities, CardinalityConstraint *newcard);
  static bool add(set<CardinalityConstraint *> &cardinalities, set<vertex_t> variables,
                  int bound, cardType type, set<id_t> inferedBy);

  static set<CardinalityConstraint *> merge(set<CardinalityConstraint *> cardinalities);

  static set<CardinalityConstraint *> simplify(set<CardinalityConstraint *> cardinalities,
                                               map<vertex_t, CardinalityConstraint *> unitcardinalities, set<vertex_t> unitvariables);
};

#endif // CARDINALITY_H