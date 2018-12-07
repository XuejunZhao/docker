//
// Created by workshop on 11/21/2018.
//

#ifndef WORK_ATTRIBUTE_H
#define WORK_ATTRIBUTE_H

#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include "tool.h"

using namespace std;


struct Attri
{
    Attri (int Id){id = Id;domain = 0; son = father = -1; base = 1;}
    Attri (int Id, int n_){id = Id;domain = n_; son = father = -1; base = 1;}
    int domain, father, id, son, base = 1;
};

struct ScoreItem
{
    int x, y;
    double weight;
    ScoreItem (){}
    ScoreItem (int x_, int y_, double weight_)
    {
        x = x_, y = y_, weight = weight_;
    }
};

struct QueueItem
{
    ll choose;
    double judge, score;
    QueueItem (int Judge, int Begin)
    {
        judge = Judge;
        choose = (1LL << Begin);
    }
};

struct Solve
{
    vector <int> OriginPos;
    vector<vector<int > > data;
    vector <Attri>node;
    int TotNode, score_table_size;
    double edge[52][52] = {{0}};

    vector<vector<int> > TimeAttri;
    vector <ScoreItem> score;
    void ReadRecord ();
    void ReadData ();
    void BeforeCalc ();
    void Calc ();
    vector<string> FromString2vec (const string &s, int Del = 0);
    vector<Attri> ReadAttribute (string s);
    int FindOrigin (int x);
    void calc_weight (int CurAtt, const vector<vector< vector< int> > > &data);

    double CalcNoise ();
    double CalcNoise2 ();
    double sigma2, sigma3;
    void CalcSigma2 ();
    void CalcSigma3 ();
    double gauss_rand(const double sigma);

    ll NeverChoose;
    double theta, ThetaUseful, epilson;
    vector<ll>part;

    void ChooseOneMore (ll &choose, int x);
    bool Exist (const ll choose, int x);
    void Erase (ll &choose, int x);

    void BuildGraph ();
    void AddAllParent (ll &x);
    double CalcEdgeScore (const ScoreItem now);
    ll BuildOriginGraph ();
    bool FamilyRelation (int x, int y);
    ScoreItem AskMaxScore ();
    bool AddOneEdge (set<int> &now, double &CurScore);
    void NeverChooseFather (int index);
    void NeverChooseSon (int index);
    bool BelongFamily (const int x, const ll &choose);
    bool AddPoint (ll & now);
    pair<double, ll > FindParentSet (const ll & CanChoose, const int x);
    //___________________________________________________________________add xj
    double F;
};
#endif //WORK_ATTRIBUTE_H