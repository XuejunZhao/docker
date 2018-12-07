#include <iostream>
#include "attribute.h"
#include "tool.h"
#include "consistency.h"
#include <ctime>
#include <cstdlib>
//----------------------------------------------------add xj
#include "inoutf.h"
#include <stdio.h> 
//----------------------------------------------------add xj
#define epsilon 0.1
#define k 10
#define y 100


using namespace std;

In in;
Solve solve;
int main ()
{
    srand ((unsigned)time (NULL));
    //----------------------------------------------------add xj
    in.epilson = epsilon;
    in.hierarchy_k = k;
    in.hierarchy_y = y;
    in.ReadData2("fire-data-specs.json", "fire-data.csv");

    //----------------------------------------------------add xj add F
    solve.F = in.F;

    solve.epilson = epsilon;
    solve.theta = 5;
    solve.ReadAttribute ("data.domain");
    solve.ReadData();
    solve.BeforeCalc();
    solve.Calc ();
    cout << "finish calc\n";
    //solve.AddNoise (solve.epilson);
    solve.BuildGraph();
    consistency con (solve);
    con.work ();
    rep (i, 0, con.margin.size () - 1)
    {
        double t = 0.0;
        for (auto j : con.margin[i].count)
            t += j.second;
        cout << t << endl;
    }
    con.Normalize();
    vector<vector<int> > result = con.Genarate ();  //change auto
    in.out_result(result);
    remove ("data.domain");
    remove ("data.dat");
    con.CountError (result);
    return 0;
}
