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

#include "BloomFilter.h"

void BloomFilter::add(vector<vertex_t> edge)
{
  add(createLabel(edge), edge.size());
}

void BloomFilter::add(label_t label, id_t laurent)
{
  laurentToLabels[laurent].insert(label);
}

void BloomFilter::clear()
{
  laurentToLabels.clear();
}

bool BloomFilter::exists(vector<vertex_t> edge)
{
  return exists(createLabel(edge), edge.size());
}

bool BloomFilter::exists(label_t label, id_t laurent)
{
  set<label_t>::iterator it = laurentToLabels[laurent].find(label);
  return (it != laurentToLabels[laurent].end());
}

bool BloomFilter::subExists(vector<vertex_t> edge)
{
  return subExists(createLabel(edge));
}

bool BloomFilter::subExists(label_t label)
{
  for (auto labels : laurentToLabels)
  {
    for (auto it = labels.second.begin(); it != labels.second.end(); it++)
    {
      if ((*it & label) == label)
        return true;
    }
  }
  return false;
}

bool BloomFilter::subExists(vector<vertex_t> edge, id_t laurent)
{
  return subExists(createLabel(edge), laurent);
}

bool BloomFilter::subExists(label_t label, id_t laurent)
{
  for (auto it = laurentToLabels[laurent].begin(); it != laurentToLabels[laurent].end(); it++)
  {
    if ((*it & label) == label)
      return true;
  }
  return false;
}

size_t BloomFilter::getNb(vector<vertex_t> vertices)
{
  size_t nb = 0;
  label_t label = createLabel(vertices);
  for (auto labels : laurentToLabels)
  {
    for (auto it = labels.second.begin(); it != labels.second.end(); it++)
    {
      if ((*it & label) == (*it))
        nb++;
    }
  }
  return nb;
}

size_t BloomFilter::getNb(set<vertex_t> vertices)
{
  size_t nb = 0;
  label_t label = createLabel(vertices);
  for (auto labels : laurentToLabels)
  {
    for (auto it = labels.second.begin(); it != labels.second.end(); it++)
    {
      if ((*it & label) == (*it))
        nb++;
    }
  }
  return nb;
}

size_t BloomFilter::getNb(vector<vertex_t> vertices, id_t laurent)
{
  size_t nb = 0;
  label_t label = createLabel(vertices);
  for (auto it = laurentToLabels[laurent].begin(); it != laurentToLabels[laurent].end(); it++)
  {
    if ((*it & label) == (*it))
      nb++;
  }
  return nb;
}

size_t BloomFilter::getNb(set<vertex_t> vertices, id_t laurent)
{
  size_t nb = 0;
  label_t label = createLabel(vertices);
  for (auto it = laurentToLabels[laurent].begin(); it != laurentToLabels[laurent].end(); it++)
  {
    if ((*it & label) == (*it))
      nb++;
  }
  return nb;
}