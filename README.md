# ICTAI 2025

An interactive view of all the data presented here in the sqlite database is available at
https://mpelleau.shinyapps.io/Hyperclique/


## Building the Docker image

First of all, you need a recent installation of `docker` and you need to build the docker image. For this, clone this repository, then:
```
cd docker
docker build -t sea .
```
This will build a docker image with all the data, benchs and binaries needed. The `-t sea` tag gives the container a name (used below in the examples).

The container can open a `shell` by
```
docker run -it sea /bin/bash
```

## Direct access to the database

Once the docker is set up, it is possible to access to the database of runs by
``` 
docker run -it sea sqlite3 /home/hyperclique/tuwien.sqlite
```

Then a number of queries are possible:
- Counts the number of jobs in the database : 
```sql 
SELECT COUNT(*) FROM runs;
```

- Prints the benchmarks with their category:
```sql
SELECT interestingbenchs.bench, category FROM interestingbenchs, categories
WHERE interestingbenchs.bench = categories.bench
ORDER BY category, categories.bench;
```

## Run hyperclique

hyperclique is available at `/home/hyperclique/hyperclique`. You may want to run it with the help argument to get informations about running it
```
docker run -it sea /home/hyperclique/hyperclique -help
```

All the problems are under `/home/hyperclique/tuwien-htd`. You can, for instance, run hyperclique with the first method (a) with the default values from outside the docker image with
```bash
docker run -it sea /home/hyperclique/hyperclique -a -i tuwien-htd/Dubois-030.xml.htd
```

Or, if you opened an interactive shell with the following command:
```bash
>> docker run -it sea /bin/bash
root@91a79eae4e67:/home/hyperclique# ./hyperclique -a -i tuwien-htd/Dubois-030.xml.htd     
c +============================================================================+
c | Hyperclique enumerations                                                   |
c | Main Developers:                                                           |
c | - Marie Pelleau (marie.pelleau@univ-cotedazur.fr)                          |
c | - Laurent Simon (lsimon@labri.fr)                                          |
c | Contributors:                                                              |
c | - Florian Régin (florian.regin@etu.univ-cotedazur.fr)                      |
c |----------------------------------------------------------------------------|
c |                                                                            |
c | Debug:                                OFF                                  |
c | Display:                               ON                                  |
c | Hyper-Bron & Kerbosch enumeration:     ON                                  |
c | Hybrid-Bron & Kerbosch enumeration:   OFF                                  |
c | Clique-CE_HBK enumeration:            OFF                                  |
c | Non-uniform clique enumeration:       OFF                                  |
c | Filter:                               OFF                                  |
c | Ordering:                          random                                  |
c | Node filter:                          OFF                                  |
c |                                                                            |
c +============================================================================+
c | *                      tuwien-htd/Dubois-030.xml.htd                       |
c | *        60 hyperedges                                                     |
c | *        90 nodes                                                          |
c | *         3 max rank                                                       |
c | parsing time: 0.000259192 seconds                                          |
c +============================================================================+
c | Hyper-Bron & Kerbosch enumeration:                                         |
c |----------------------------------------------------------------------------|
c |        90 |        60 |        60 |         3 | 0.011028915 seconds        |
c | nb calls to BK:        308                                                 |
c | total cliques found:          60                                           |
c | total nb calls to BK:        308                                           |
c | timedout:                      0                                           |
c | BK total time: 0.011840922 seconds                                         |
c | total time: 0.012100114 seconds                                            |
c +============================================================================+
```

However, for parsing the results, you may want to use the `-j` argument to output a JSON line:

```bash
root@91a79eae4e67:/home/hyperclique# ./hyperclique -a -j -i tuwien-htd/Dubois-030.xml.htd 
{"bench": "tuwien-htd/Dubois-030.xml.htd", "o": 0, "f": 0, "n": 0, "hyperedges": 60, "nodes": 90, "maxrank": 3, "parsingtime": 0.000228, "m": "a", "cliques": [{"nbnodes": 90, "nbedges": 60, "nbcliques": 60, "rank": 3, "time": 0.00983233, "nbcalls": 308}], "totalcliques": 60, "totalcalls": 308, "timedout": 0, "bktotaltime": 0.0104824, "totaltime": 0.0107108}
```

To help reading/parsing the output, `jq` is installed on the docker. You can pretty print the output with
``` bash
root@91a79eae4e67:/home/hyperclique# ./hyperclique -a -j -i tuwien-htd/Dubois-030.xml.htd | jq
```
Which gives you the JSON output:
```json
{
  "bench": "tuwien-htd/Dubois-030.xml.htd",
  "o": 0,
  "f": 0,
  "n": 0,
  "hyperedges": 60,
  "nodes": 90,
  "maxrank": 3,
  "parsingtime": 0.00019,
  "m": "a",
  "cliques": [
    {
      "nbnodes": 90,
      "nbedges": 60,
      "nbcliques": 60,
      "rank": 3,
      "time": 0.0103165,
      "nbcalls": 308
    }
  ],
  "totalcliques": 60,
  "totalcalls": 308,
  "timedout": 0,
  "bktotaltime": 0.0109974,
  "totaltime": 0.0111877
}
```



let's say you want to run again a configuration in the database. The main component in the traces is the table `runs`. 
```
sqlite> .schema runs 
CREATE TABLE runs
     (runid Integer primary key AUTOINCREMENT, 
     bench varchar(250), 
     o int, 
     f int,
     n int,
     m CHAR(1) CHECK( m IN ('a','b','c') ),
     maxrank int,
     timedout int(1),
     timeout float,
     hyperedges int,
     nodes int,
     totalcliques int,
     parsingtime float,
     totaltime float,
     bktotaltime float,
     totalcalls int);
```
It is possible to see the main configurations and results  by
```sql
SELECT runs.bench, m, n, f, o, totaltime, timedout
FROM runs, interestingbenchs 
WHERE runs.bench=interestingbenchs.bench
LIMIT 10;
```
which gives (the `LIMIT` keyword is of course optional)
```
./tuwien-htd/rand-3-24-24-76-632-50.xml.htd|c|2|1|4|0.0077765|0
./tuwien-htd/Nonogram-074-table.xml.htd|c|2|1|4|0.0296264|0
./tuwien-htd/rand-3-28-28-93-632-48.xml.htd|c|2|1|4|0.0201116|0
./tuwien-htd/reg-s20-p03-c20-d10-n10-l5-97.xml.htd|c|2|1|4|0.00680803|0
./tuwien-htd/reg-s20-p03-c20-d10-n10-l5-96.xml.htd|c|2|1|4|0.00688477|0
./tuwien-htd/rand-7-40-8-40-02500-0.xml.htd|c|2|1|4|0.00638341|0
./tuwien-htd/rand-3-24-24-76-632f-32.xml.htd|c|2|1|4|0.00356882|0
./tuwien-htd/Nonogram-151-table.xml.htd|c|2|1|4|0.44246|0
./tuwien-htd/mdd-7-25-5-ps05-psh07-7.xml.htd|c|2|1|4|0.0065936|0
./tuwien-htd/Nonogram-155-table.xml.htd|c|2|1|4|0.0301679|0
```
So, let's say you want to re-run the second entry
```
./tuwien-htd/Nonogram-074-table.xml.htd|c|2|1|4|0.0296264|0
```

You can launch it with (notice that the method is given by `-a`, `-b` or `-c` and not with `-m=a` or `-m=b` or `-m=c`

``` 
docker run sea /home/hyperclique/hyperclique -c -m=2 -n=1 -f4 -o0 -j -i ./tuwien-htd/Nonogram-074-table.xml.htd
```
which gives you the JSON
```
{"bench": "./tuwien-htd/Nonogram-074-table.xml.htd", "o": 0, "f": 4, "n": 0, "hyperedges": 40, "nodes": 384, "maxrank": 24, "parsingtime": 0.000521, "m": "c", "cliques": [{"nbnodes": 384, "nbedges": 16, "nbcliques": 16, "rank": 24, "time": 4.3197e-05, "nbcalls": 0}, {"nbnodes": 384, "nbedges": 24, "nbcliques": 24, "rank": 16, "time": 0.0838645, "nbcalls": 3265}], "totalcliques": 40, "totalcalls": 3265, "timedout": 0, "bktotaltime": 0.0853867, "totaltime": 0.0859073}
```

## Interesting queries

The table `interestingbenchs` contains all the selected benchs from the paper:
```sql
SELECT count(*) FROM interestingbenchs:
```
Will give you 1037. You can get the names by `SELECT bench from interestingbenchs;`

Other queries
- Jobs in DB : `SELECT COUNT(*) FROM runs;`
- Total CPU Time (in days): `SELECT ROUND(SUM(totaltime)/3600/24) FROM runs;`
- Number of timed out jobs:`SELECT COUNT(*) FROM runs WHERE timedout=1;`,
- Number of trivial jobs (<0.1s): `SELECT COUNT(*) FROM runs WHERE totaltime < 0.1;`
- Number of interesting jobs (>1s): `SELECT COUNT(*) FROM runs WHERE totaltime > 1;`
- Number of interesting benchs: `SELECT COUNT(*) FROM interestingbenchs;`

Generates the data for the scatter plot (note that we use 2000s in case of a timeout just for a clear distinction on the scatter. This is not related to the Par2 scoring)
```sql
    SELECT (CASE WHEN runsA.timedout==1 THEN 2000 ELSE runsA.totaltime END) AS timeA, 
              (CASE WHEN runsB.timedout==1 THEN 2000 ELSE runsB.totaltime END) AS timeB,
              category
       FROM runs AS runsA, runs AS runsB, interestingbenchs, categories 
       WHERE runsA.bench=interestingbenchs.bench AND 
             runsB.bench=interestingbenchs.bench AND
             categories.bench=interestingbenchs.bench AND
             runsA.m='a' AND runsA.n=0 AND runsA.f=0 AND runsA.o=3 AND 
             runsB.m='a' AND runsB.n=2 AND runsB.f=1 AND runsB.o=3 
```

Generates the Par2 Ranking for all the configurations:
```sql
        SELECT m, n, f, o, SUM(timedout) AS '#timeout',
        SUM(CASE WHEN timedout==1 THEN 2600 ELSE totaltime END)/COUNT(*) AS Par2Score 
        FROM runs, interestingbenchs
        WHERE runs.bench=interestingbenchs.bench
        GROUP by m, n, f, o
        ORDER BY Par2Score ASC
 ```
