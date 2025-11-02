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

#ifndef CONSTANTS_H
#define CONSTANTS_H

using namespace std;

#define ALLLENGTH 80
#define LENGTH ALLLENGTH - 3
#define LINE "c |" << setfill('-') << setw(LENGTH) << "|" << setfill(' ')
#define DLINE "c +" << setfill('=') << setw(LENGTH) << "+" << setfill(' ')

#define NO_FILTER 0
#define BLOOM_FILTER 1
#define CLIQUE_FILTER 2
#define NEIGHBOURHOOD_FILTER 3
#define INCREMENTAL_FILTER 4
#define NB_FILTER 5

#define RANDOM_ORDERING 0
#define MIN_DEGREE_ORDERING 1
#define MIN_FILL_ORDERING 2
#define NATURAL_ORDERING 3
#define DEGENERACY_ORDERING 4
#define MAX_DEGREE_ORDERING 5
#define NB_ORDER 6

#define DEGREE_FILTER 1
#define COOCCURRENCE_FILTER 2
#define BOTH_FILTER 3
#define NB_NODE_FILTER 4

#endif // CONSTANTS_H