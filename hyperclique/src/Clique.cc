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

#include "Clique.h"

void Clique::print(map<id_t, id_t> edgesMap)
{
  cout << "c | {";
  for (const vertex_t v : vertices)
  {
    cout << " " << v;
  }
  cout << " }" << endl;
  cout << "c | [";
  for (const id_t e : edges)
  {
    cout << " " << edgesMap[e];
  }
  cout << " ]" << endl;
}

set<id_t> Clique::getClauses(map<id_t, id_t> edgesMap)
{
  set<id_t> res;
  for (const id_t e : edges)
  {
    res.insert(edgesMap[e]);
  }
  return res;
}