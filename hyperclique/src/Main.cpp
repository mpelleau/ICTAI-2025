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

#include "Main.h"

bool Globals::print = true;
bool Globals::printedge = false;
bool Globals::debug = false;
bool Globals::verbose = false;
bool Globals::json = false;
bool Globals::statsc = false;
bool Globals::statsh = false;
bool Globals::cardinality = false;
bool Globals::incremental = false;
bool Globals::maxClique = false;
unsigned int Globals::filteringValue = NO_FILTER;
unsigned int Globals::orderingValue = RANDOM_ORDERING;
unsigned int Globals::nodeFilteringValue = NO_FILTER;
double Globals::timer = 0;
bool Globals::timedout = false;
string Globals::jsonLine = "";

bool hbk, ce, ce_hbk, nu_hbk;

void printHelp()
{
  cout << "usage: ./bin/hyperclique [<option> ...] [< INPUT]" << endl;
  cout << "where <option> is one of the following:" << endl
       << endl;
  cout << "  -h        prints this command line option summary." << endl;
  cout << "  -d        prints the cliques found." << endl;
  cout << "  -l        prints the cliques found and the corresponding edges." << endl;
  cout << "  -v        very verbose mode." << endl;
  cout << "  -p        disable printing the summary of the cliques found." << endl;
  cout << "  -j        prints the output in a json format." << endl;
  cout << "  -a        uses the Hyper Bron-Kerbosch algorithm." << endl;
  cout << "  -b        uses the Hybrid Bron-Kerbosch algorithm." << endl;
  cout << "  -c        uses the Hybrid CE-HBK algorithm." << endl;
  cout << "  -sh       prints some stats on the hypergraph." << endl;
  cout << "  -sc       prints some stats on the cliques found <BETA>." << endl;
  cout << "  -t <time> sets a timeout to `time` seconds for each method." << endl;
  cout << "  -x        search the maximal hyperclique." << endl;
  cout << "  -u        uses Hybrid Bron-Kerbosch algorithm to find hypercliques in non-uniform hypergraphs <BETA>." << endl;
  cout << "  -m        transforms the clique into cardinality constraints and tries to merge them <BETA>." << endl;
  cout << "  -n<i>     uses node filtering on candidates set." << endl;
  cout << "    <i>=0   [default] no filtering." << endl;
  cout << "    <i>=1             degree." << endl;
  cout << "    <i>=2             cooccurrences." << endl;
  cout << "    <i>=3             both." << endl;
  cout << "  -f<i>     uses filtering on candidates set." << endl;
  cout << "    <i>=0   [default] no filtering." << endl;
  cout << "    <i>=1             Bloom filter." << endl;
  cout << "    <i>=2             clique filter." << endl;
  cout << "    <i>=3             neighbourhood filter." << endl;
  cout << "    <i>=4             Incremental 'filter'." << endl;
  cout << "  -o<i>     uses ordering on candidates set." << endl;
  cout << "    <i>=0   [default] random." << endl;
  cout << "    <i>=1             min-degree." << endl;
  cout << "    <i>=2             min-fill." << endl;
  cout << "    <i>=3             natural." << endl;
  cout << "    <i>=4             degeneracy-graph." << endl;
  cout << "    <i>=5             max-degree." << endl;
  cout << "  -i <n>    specifies the input file." << endl;
  cout << "            If the `n` is a directory then the program is run for every file in `n`." << endl;

  cout << "and input is specified as follows" << endl;
  cout << "  INPUT       input file in PACE 2019 format" << endl
       << endl;

  cout << "Example of usage:" << endl;
  cout << "  ./bin/hyperclique -b -d -f1 < instances/myinstance.hgr" << endl;
  cout << "or" << endl;
  cout << "  ./bin/hyperclique -b -d -f1 -i instances/myinstance.hgr" << endl;
  cout << "Uses the Hybrid Bron-Kerbosch algorithm with the Bloom filter and prints the resulting hypercliques" << endl
       << endl;
  cout << "  ./bin/hyperclique -a -b -c -d -f1 -o2 < instances/myinstance.hgr" << endl;
  cout << "Uses the Hyper Bron-Kerbosch algorithm with the Bloom filter and the min-fill ordering" << endl
       << "and prints the resulting hypercliques." << endl
       << "It also uses the Hybrid Bron-Kerbosch and the Hybrid CE-HBK algorithms" << endl
       << "and prints the results." << endl;

  cout << DLINE << endl;
  exit(0);
}

void printTime(double time)
{
  if (!Globals::json)
  {
    int l10 = ceil(log10(time));
    l10 = l10 <= 0 ? 1 : l10;
    cout << "c | total time: " << setprecision(9) << time << " seconds" << setw(LENGTH - 31 - l10) << "|" << endl;
  }
  else
    cout << ", \"totaltime\": " << time << "}" << endl;
}

SweetHBK::Graph *parse(streambuf *std_in, streambuf *file_buf, double *parsingTime)
{
  // Redirect standard input
  cin.rdbuf(file_buf);
  chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();
  SweetHBK::Graph *graph = new SweetHBK::Graph();
  chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
  *parsingTime = ((end - start).count() / 1e9);
  if (!Globals::json && !Globals::statsh)
  {
    int l10 = ceil(log10(*parsingTime));
    l10 = l10 <= 0 ? 1 : l10;
    cout << "c | parsing time: " << setprecision(9) << *parsingTime << " seconds" << setw(LENGTH - 33 - l10) << "|" << endl;
  }
  if (Globals::json)
    Globals::jsonLine += ", \"parsingtime\": " + to_string(*parsingTime);

  if (Globals::verbose && !Globals::statsh)
  {
    cout << "c | ... parsing done ..." << setw(LENGTH - 21) << "|" << endl;
    cout << graph->edges.size() << endl;
    for (auto e : graph->edges)
    {
      cout << "c | " << e.first << ":";
      for (vertex_t v : e.second)
        cout << " " << v;
      cout << endl;
    }
    for (auto le : graph->laurentToEdges)
    {
      cout << "c | " << le.first << " :";
      for (id_t i : le.second)
      {
        cout << " " << i;
      }
      cout << endl;
    }
    for (auto lv : graph->laurentToVertices)
    {
      cout << "c | " << lv.first << " :";
      for (vertex_t v : lv.second)
      {
        cout << " " << v;
      }
      cout << endl;
    }
  }
  return graph;
}

void doIt(streambuf *std_in, streambuf *file_buf)
{
  double processTime, totalTime, parsingTime;
  SweetHBK::Graph *graph = parse(std_in, file_buf, &parsingTime);

  double timeLeft = Globals::timer - parsingTime;

  if (hbk)
  {
    if (Globals::json)
      cout << Globals::jsonLine << ", \"m\": \"a\"";
    else if (!Globals::statsh)
      cout << DLINE << endl
           << "c | Hyper-Bron & Kerbosch enumeration:" << setw(LENGTH - 35) << "|" << endl;
    if (Globals::timer > 0)
    {
      future<void> future = std::async(std::launch::async, [graph, &processTime]()
                                       { 
                processTime = graph->enumerateHyperCliques(&SweetHBK::Graph::getHyperClique);
                return; });
      future_status status = future.wait_for(chrono::milliseconds((int)(timeLeft * 1000)));
      Globals::timedout = (status == future_status::timeout);
    }
    else
      processTime = graph->enumerateHyperCliques(&SweetHBK::Graph::getHyperClique);
    BronKerbosch::cleanBiggest();

    totalTime = parsingTime + processTime;
    printTime(totalTime);
  }

  if (ce)
  {
    if (Globals::json)
      cout << Globals::jsonLine << ", \"m\": \"b\"";
    else if (!Globals::statsh)
      cout << DLINE << endl
           << "c | Hybrid-Bron & Kerbosch enumeration:" << setw(LENGTH - 36) << "|" << endl;
    if (Globals::timer > 0)
    {
      future<void> future = std::async(std::launch::async, [graph, &processTime]()
                                       { 
                processTime = graph->enumerateHyperCliques(&SweetHBK::Graph::getHyperCliqueCE);
                return; });
      future_status status = future.wait_for(chrono::milliseconds((int)Globals::timer * 1000));
      Globals::timedout = (status == future_status::timeout);
    }
    else
      processTime = graph->enumerateHyperCliques(&SweetHBK::Graph::getHyperCliqueCE);
    BronKerbosch::cleanBiggest();

    totalTime = parsingTime + processTime;
    printTime(totalTime);
  }

  if (ce_hbk)
  {
    if (Globals::json)
      cout << Globals::jsonLine << ", \"m\": \"c\"";
    else if (!Globals::statsh)
      cout << DLINE << endl
           << "c | Clique-CE_HBK enumeration:" << setw(LENGTH - 27) << "|" << endl;
    if (Globals::timer > 0)
    {
      future<void> future = std::async(std::launch::async, [&graph, &processTime]()
                                       { 
                processTime = graph->enumerateHyperCliques(&SweetHBK::Graph::getHyperCliqueCE_HBK);
                return; });
      future_status status = future.wait_for(chrono::milliseconds((int)Globals::timer * 1000));
      Globals::timedout = (status == future_status::timeout);
    }
    else
      processTime = graph->enumerateHyperCliques(&SweetHBK::Graph::getHyperCliqueCE_HBK);

    totalTime = parsingTime + processTime;
    printTime(totalTime);
  }

  if (nu_hbk)
  {
    if (Globals::json)
      cout << Globals::jsonLine << ", \"m\": \"u\"";
    else if (!Globals::statsh)
      cout << DLINE << endl
           << "c | Non-uniform clique enumeration:" << setw(LENGTH - 32) << "|" << endl;
    graph->enumerateNonUniformCliques();
  }

  if (!Globals::statsh && !Globals::json)
    cout << DLINE << endl;

  cin.rdbuf(std_in);
}

/**
 * @brief This is the main function, we will start to verify a proof for a given input.
 * The goal of this program is to be able to provide a human-readable explaination if the proof is valid.
 * @param[in] argc the number of arguments given to the main.
 * @param[in] argv the different arguments in the char* format.
 * @return 0 if the proof is valid, 1 otherwise.
 */
int main(int argc, char **argv)
{
  hbk = false;
  ce = false;
  ce_hbk = false;
  nu_hbk = false;

  streambuf *std_in = cin.rdbuf();
  streambuf *file_buf = std_in;
  fstream file;
  int filei = 0;

  for (int i = 1; i < argc; i++)
  {
    if (argv[i][0] == '-')
    {
      switch (argv[i][1])
      {
      case 'h':
        printHelp();
        return 0;
      case 'd':
        Globals::debug = true;
        break;
      case 'p':
        Globals::print = false;
        break;
      case 'l':
        Globals::printedge = true;
        break;
      case 's':
        if (argv[i][2] == 'h')
          Globals::statsh = true;
        else if (argv[i][2] == 'c')
          Globals::statsc = true;
        break;
      case 't':
        if (++i < argc)
        {
          Globals::timer = atof(argv[i]);
        }
        else
        {
          cout << "Option -t must be followed by a time value." << endl;
          exit(-1);
        }
        break;
      case 'v':
        Globals::verbose = true;
        break;
      case 'j':
        Globals::json = true;
        break;
      case 'a':
        hbk = true;
        break;
      case 'b':
        ce = true;
        break;
      case 'c':
        ce_hbk = true;
        break;
      case 'u':
        nu_hbk = true;
        break;
      case 'x':
        Globals::maxClique = true;
        break;
      case 'm':
        Globals::cardinality = true;
        break;
      case 'f':
        Globals::filteringValue = atoi(argv[i] + 2);
        break;
      case 'o':
        Globals::orderingValue = atoi(argv[i] + 2);
        break;
      case 'n':
        Globals::nodeFilteringValue = atoi(argv[i] + 2);
        break;
      case 'i':
        if (++i < argc)
        {
          filei = i;
        }
        else
        {
          cout << "Option -i must be followed by a file name." << endl;
          exit(-1);
        }
        break;
      default:
        cout << "Option \"" << argv[i] << "\" not taken into account" << endl;
        exit(-1);
        break;
      }
    }
  }

  if (!Globals::statsh && !Globals::json)
  {
    cout << fixed << DLINE << endl;
    cout << "c | Hyperclique enumerations" << setw(LENGTH - 25) << "|" << endl;
    cout << "c | Main Developer:" << setw(LENGTH - 17) << "|" << endl;
    cout << "c | - Marie Pelleau (marie.pelleau@univ-cotedazur.fr)" << setw(LENGTH - 50) << "|" << endl;
    cout << "c | - Laurent Simon (lsimon@labri.fr)" << setw(LENGTH - 34) << "|" << endl;
    cout << "c | Contributors:" << setw(LENGTH - 14) << "|" << endl;
    cout << "c | - Florian Régin (florian.regin@etu.univ-cotedazur.fr)" << setw(LENGTH - 54) << "|" << endl;
    cout << LINE << endl;
    cout << "c |" << setw(LENGTH) << "|" << endl;

    int remain = LENGTH - 42;
    if (Globals::debug)
      cout << "c | Debug:" << setw(35) << "ON" << setw(remain) << "|" << endl;
    else
      cout << "c | Debug:" << setw(35) << "OFF" << setw(remain) << "|" << endl;

    if (Globals::print)
      cout << "c | Display:" << setw(33) << "ON" << setw(remain) << "|" << endl;
    else
      cout << "c | Display:" << setw(33) << "OFF" << setw(remain) << "|" << endl;

    if (hbk)
      cout << "c | Hyper-Bron & Kerbosch enumeration:" << setw(7) << "ON" << setw(remain) << "|" << endl;
    else
      cout << "c | Hyper-Bron & Kerbosch enumeration:" << setw(7) << "OFF" << setw(remain) << "|" << endl;

    if (ce)
      cout << "c | Hybrid-Bron & Kerbosch enumeration:" << setw(6) << "ON" << setw(remain) << "|" << endl;
    else
      cout << "c | Hybrid-Bron & Kerbosch enumeration:" << setw(6) << "OFF" << setw(remain) << "|" << endl;

    if (ce_hbk)
      cout << "c | Clique-CE_HBK enumeration:" << setw(15) << "ON" << setw(remain) << "|" << endl;
    else
      cout << "c | Clique-CE_HBK enumeration:" << setw(15) << "OFF" << setw(remain) << "|" << endl;

    if (nu_hbk)
      cout << "c | Non-uniform clique enumeration:" << setw(10) << "ON" << setw(remain) << "|" << endl;
    else
      cout << "c | Non-uniform clique enumeration:" << setw(10) << "OFF" << setw(remain) << "|" << endl;

    if (Globals::timer)
      cout << "c | Timeout:" << setw(32) << Globals::timer << "s" << setw(remain) << "|" << endl;

    Utils::printFiltering();
    Utils::printOrdering();
    Utils::printNodeFiltering();

    cout << "c |" << setw(LENGTH) << "|" << endl;
    cout << DLINE << endl;
  }

  if (filei > 0)
  {
    DIR *dir;
    struct dirent *ent;
    struct stat st;
    int inst = 50;
    int remain = LENGTH - inst - 3;
    if ((dir = opendir(argv[filei])) != NULL)
    {
      while ((ent = readdir(dir)) != NULL)
      {
        const string directory(argv[filei]);
        const string file_name(ent->d_name);
        const string full_file_name = directory + file_name;

        if (file_name[0] == '.')
          continue;
        if (stat(full_file_name.c_str(), &st) == -1)
          continue;
        if ((st.st_mode & S_IFDIR) != 0)
          continue;

        if (!Globals::statsh && !Globals::json)
          cout << "c | * " << setw(inst) << full_file_name << setw(remain) << "|" << endl;
        else if (Globals::statsh && Globals::json)
          Globals::jsonLine = "{\"bench\": \"" + full_file_name + "\"";
        else if (Globals::statsh)
          cout << full_file_name << ",";
        else
        {
          Globals::jsonLine = "{\"bench\": \"" + full_file_name + "\", \"o\": " + to_string(Globals::orderingValue) + ", \"f\": " + to_string(Globals::filteringValue) + ", \"n\": " + to_string(Globals::nodeFilteringValue);
          if (Globals::timer)
            Globals::jsonLine += ", \"Timeout\": " + to_string(Globals::timer);
        }

        file.open(full_file_name, ios::in);
        file_buf = file.rdbuf();

        doIt(std_in, file_buf);
        file.close();
      }
      closedir(dir);
    }
    else
    {
      if (!Globals::statsh && !Globals::json)
        cout << "c | * " << setw(inst) << argv[filei] << setw(remain) << "|" << endl;
      else if (Globals::statsh && Globals::json)
        Globals::jsonLine = "{\"bench\": \"" + string(argv[filei]) + "\"";
      else if (Globals::statsh)
        cout << argv[filei] << ",";
      else
      {
        Globals::jsonLine = "{\"bench\": \"" + string(argv[filei]) + "\", \"o\": " + to_string(Globals::orderingValue) + ", \"f\": " + to_string(Globals::filteringValue) + ", \"n\": " + to_string(Globals::nodeFilteringValue);
        if (Globals::timer)
          Globals::jsonLine += ", \"Timeout\": " + to_string(Globals::timer);
      }

      file.open(argv[filei], ios::in);
      if (file.good())
      {
        file_buf = file.rdbuf();

        doIt(std_in, file_buf);
        file.close();
      }
      else
      {
        if (!Globals::statsh && !Globals::json)
          cout << "c | * " << setw(inst) << "FILE NOT FOUND" << setw(remain) << "|" << endl;
        else if (Globals::statsh && !Globals::json)
          cout << "FILE NOT FOUND" << endl;
        else
          cout << Globals::jsonLine << ", \"err\": \"fileNotFound\"}" << endl;
      }
    }
  }
  else
  {
    doIt(std_in, std_in);
  }

  return 0;
}
