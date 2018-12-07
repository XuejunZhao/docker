//
// Created by workshop on 11/24/2018.
//

#ifndef WORK_CONSISTENCY_H
#define WORK_CONSISTENCY_H

#include "tool.h"
#include "attribute.h"

struct Margin
{
    Margin (ll c_){choose = c_;}
    ll choose;
    double CellValue;
    map<vector<int>,double> count;
};

struct consistency
{
    int vis;
    vector<vector<int> > AllFamilyMember;
    consistency (Solve s_){solve = s_;}
    Solve solve;
    vector<Margin> margin, OriginMargin, CurMargin, ErrorMargin;

    void work ();
    void AddAllParent (ll &x);
    ll ClearAllParent (const ll x);
    vector<int> ClearVecParent (const vector<int> &X_withParent, const ll ChooseWithParent);
    vector<int> SelectSubData (const vector<int> &data, const ll x);
    int CalcFamilyDomainSize (const ll x);
    void CalcMargin ();
    ll FindCommon ();
    void AddNoise3 (Margin &x);
    void Dfs ( Margin &x, int pos, vector<int> &now, ll choose, const double sigma);

    vector<pii > FamilyRelation;

    vector<vector<int> > edge;
    vector<int> in;
    vector<ll> CommonMargin;
    int MarginSize, common_MarginSize;
    int Margin_size_part;

    void SortGraph ();
    void CalcFamilyPosition (vector<pii > & ret);
    void CompareCommonMargin (const int x, const int y);
    bool HaveFmaily (int x, int Family);
    void AddEdge (int x, int y);
    void TopSort ();
    void AdjustConsistentAccordingToCommon (const ll x);
    void UpdateCellValue (const vector<int> &need, const int pos, const vector<double> WeightValue, const int RowNumber);
    vector<double> CalcWeightValue (const vector<int> &need, const ll x, const vector<map<vector<int>, int> > Time);
    int CalcDomainSize (const ll x);
    int CalcDimension (const ll x);
    int CalcFamilyNum (const ll x);
    vector<int> Extract (const vector<int> OriginVec, const ll OriginChoose, const ll WantExtract);
    void adjust_FamilyConsistency ();
    int FindFamilySon (const ll x, const int pos);
    void AdjacentAdjust (Margin & x, const int fa, const int son);

    void PrintLL (ll x);
    void CalcError ();

    void Normalize ();

    vector<vector<int> > Genarate ();
    bool TryGenarate (vector<vector<int> > &result, const int size);
    void PreSimpling();
    pair<bool, vector<int> > Simpling(int Dec = 0);
    vector<int> simpling_first ();
    int FindExtraOne (const ll x, const ll y);
    vector<int> CommonVecAddOne (const vector<int> &x, const ll choose, const int pos, const int value);
    double RandDouble (double right);
    vector<int> FamilyExtract (const vector<int> OriginVec, const ll OriginChoose, const ll WantExtract);
    vector<int> EraseFamilyMember (const vector<int> &VecOrigin, const ll ChooseOrigin, const int FamilyNumber);
    vector<int> ChangeOne (const vector<int> &VecOrigin, const ll ChooseOrigin, const int pos, const int value);
    bool SubNumber (double &x);

    void CountError (const vector<vector<int> > & NewData);
    vector<int> AddFamilyMember (const vector<int> x);

    vector<int> extra;
    vector<ll> common;
    vector<ll> choose;
//    vector<map<vector<int>, double> > tot;
    map<pair<int, vector<int> >, int> tot_index;
    vector<double> tot;
//    map<pair<vector<int>,pair<int,int> > , double> CountNum;
    map<pair<vector<int>, int>, int> CountNumIndex;
    vector<vector<double> > CountNum;

    vector<int> CommonVec, temp;

};


#endif //WORK_CONSISTENCY_H
