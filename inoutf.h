#include <set>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <iostream>
#include <map>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "tool.h"
using namespace std;

struct In
{
	double F, del, epilson;
	int total, dim, hierarchy_k, hierarchy_y;
	vector<string>  all_name;
	vector<int> small;
	set<string> integer_small;
	map<string, int> categorical;
	map<string, pair<double, double> > numeric;
	map<int,pair<string, int>> json1;
	map<string, pair<double, double>> json2;

    void processDomain(string domain, map<string, int>& categorical, map<string, pair<double, double>>& numeric);
    // map<int,pair<string, pair<double, double>>>&  gather_json();
	string splitHierachyCat(int count, int k, char type, int y);
    
    
    void ReadData2 (string R, string S);   
    void CalculateF();
    void out_result(vector<vector<int> > & matrix);

};
