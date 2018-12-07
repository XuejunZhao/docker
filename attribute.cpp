//
// Created by workshop on 11/21/2018.
//

#include "attribute.h"
#include <sstream>
#define Debug 1


int CountLeft (const string &s)
{
    int ret = 0;
    for (auto i : s)
        if (i == '{')
            ret++;
        else
            return ret;
}

vector<Attri> Solve::ReadAttribute(string S)
{
    vector<Attri> ret;
    ifstream fdomain(S);
    string s;
    while (getline(fdomain, s))
    {
        vector<int> tmp, relation;
        auto cur = FromString2vec(s, 1);
        for (auto i : cur)
            for (auto j = 0; j <= CountLeft(i); j++)
            {
                while (tmp.size () <= j)
                    tmp.push_back(0);
                tmp[j]++;
            }
        for (auto i : tmp)
            node.push_back(Attri (node.size(), i)), relation.push_back(node.size() - 1);
        for (int i = 0; i < relation.size (); i++)
        {
            if (i + 1 < relation.size ())
                node[relation[i]].father = relation[i + 1];
            if (i - 1 >= 0)
                node[relation[i]].son = relation[i - 1];
        }
        *relation.begin().base() = 1;
        for (int i = 1; i < relation.size (); i++)
            node[relation[i]].base = node[relation[i - 1]].base * 10;
    }
    fdomain.close ();
    TotNode = node.size();
    rep (i, 0, TotNode - 1)
        if (node[i].son != -1)
            node[i].domain = (node[node[i].son].domain + 9) / 10;
    for (auto i : node)
    {
        cout << i.id << "   " << i.domain << "     ====   " << i.father << "   " << i.son << "    " << i.base;
        cout << endl;
    }
    return node;
}

void Solve::ReadData ()
{
    ifstream fdata("data.dat"); //read data
    data.resize (N);
    int tt;
    rep (i, 0, N - 1)
        rep (j, 0, M - 1)
            fdata >> tt, data[i].push_back(tt);
    fdata.close ();
}

void Solve::ReadRecord ()
{
    ifstream in ("score.txt");
    ScoreItem cur;
    while (in >> cur.x >> cur.y >> cur.weight)
        score.push_back(cur);
    in.close();
    ifstream Edge ("Edge.txt");
    rep (i, 0, 51)
        rep (j, 0, 51)
            Edge >> edge[i][j];
}

vector<string> Solve::FromString2vec(const string &s, int Del)
{
//    cout <<"from  =="<< s << endl;
    stringstream ss;
    ss.clear();
    ss.str (s);
    vector <string> cur;
    string tmp;
    bool fir = 1;
    while (!ss.fail())
    {
        ss >> tmp;
        if (fir && Del)
            fir = 0;
        else
            cur.push_back (tmp);
    }

    cur.pop_back();
    return cur;
}

void Solve::BeforeCalc ()
{
    //Calc OriginPos
    OriginPos.resize(TotNode);
    int now = -1;
    rep (i, 0, TotNode - 1)
        now += (node[i].son == -1), OriginPos[i] = now;
    CalcSigma2();
    CalcSigma3();
}

void Solve::Calc()
{
    //count time(x)
    TimeAttri.resize(TotNode, vector<int> ()); //init time_count
    rep (i, 0, TotNode - 1)
        TimeAttri[i].resize(node[i].domain, 0);
    //calc the array index of each attribute
    vector <int> AttriNum;
    rep (i, 0, TotNode - 1)
        if (node[i].son == -1)
            AttriNum.push_back(i);

    //count the time of each attri
    rep (i, 0, N - 1)
        rep (j, 0, M - 1)
        {
            int now = AttriNum[j];
            TimeAttri[now][ data[i][j] ]++;
            while (node[now].father != -1)
            {
                now = node[now].father;
                TimeAttri[now][ (data[i][j]) / node[now].base]++;
            }
        }

    //begin calc the weight
    for (int CurAtt = TotNode - 1; CurAtt >= 1; CurAtt--)
        //if (node[CurAtt].son == -1) //calc i to {0 -> (i - 1) }
        {
            int x_origin = FindOrigin(CurAtt);
            vector <vector <vector<int> > > now ( node[CurAtt].domain, vector< vector<int> >());
            rep (i, 0, N - 1)
            {
                vector<int> tmp;
                tmp.assign(data[i].begin(), data[i].begin() + FindOrigin(CurAtt - 1) + 1);
                if (Debug)
                {
                    assert (now.size () == node[CurAtt].domain);
                    assert (data[i][x_origin] / node[CurAtt].base < node[CurAtt].domain);
                }
                now[ data[i][x_origin] / node[CurAtt].base ].push_back(tmp);
            }
            calc_weight (CurAtt, now);
        }
    cout << "finish calc the weight\n";
    score_table_size = score.size ();

    //print edge, score
    ofstream Edge("Edge.txt");
    rep (i, 0, 51)
    {
        rep (j, 0, 51)
            Edge << edge[i][j] << ' ';
        cout << endl;
    }
    Edge.close ();
    ofstream Score ("score.txt");
    for (auto i : score)
        Score << i.x << ' ' << i.y << ' ' << i.weight << endl;
    Score.close ();
}

int Solve::FindOrigin(int x)
{
   return OriginPos[x];
}

void Solve::calc_weight(int CurAtt, const vector<vector<vector<int> > >& CountData)
{
    cout << "calc weight  " << CurAtt << endl;
    vector<double> weight (CurAtt, 0.0);
    rep (x_value, 0, node[CurAtt].domain - 1) // the value of attribute X
    {
        rep (y, 0, CurAtt - 1) //attribute Y
        {
            //int Y_time[node[y].domain] = {0};
            int Y_time[node[y].domain];
            fill_n (Y_time, node[y].domain, 0);
            Y_time[0] = 0;
            int OriginY = FindOrigin (y);
            int L_x_value = x_value * node[CurAtt].base, R_x_value = L_x_value + node[CurAtt].base - 1;
            rep (mid_x_value, L_x_value, R_x_value)
                if (CountData.size () > mid_x_value)
                {
                    for (auto j : CountData[mid_x_value])
                        Y_time[j[OriginY] / node[y].base]++; //p(xy)
                }
                else
                    break;
            rep (y_value, 0, node[y].domain - 1)
            {
                if (Debug)
                {
                    assert (weight.size () > y);
                    assert (node[y].domain > y_value);
                    assert (TimeAttri.size () > CurAtt);
                    assert (TimeAttri[CurAtt].size () > x_value);
                }
//                if (Y_time[y_value]) cout << CurAtt << "  : " << x_value << " : " << TimeAttri[CurAtt][x_value] << "   " << y << "(" << FindOrigin(y) << ") : " << y_value << " : " << TimeAttri[y][y_value] << " ==== " << Y_time[y_value] << endl;
                weight[y] += abs (1.0 * Y_time[y_value] - TimeAttri[CurAtt][x_value] * TimeAttri[y][y_value] / (1.0 * N + NOISE * CalcNoise2 ())) * 0.5 + NOISE * CalcNoise();
            }
        }
    }
    rep (i, 0, CurAtt - 1)
        if (i != CurAtt)
        {
            score.push_back (ScoreItem(min (i, CurAtt), max (CurAtt, i), weight[i]));
            edge[i][CurAtt] = edge[CurAtt][i] = weight[i];
        }
}

double Solve::CalcNoise ()
{
    // double F;
    // if (epilson == 0.1)
    //     F = 0.011403467309464246;
    // else
    //     if (epilson == 1)
    //         F = 0.10153596547895295;
    //     else
    //         if (epilson == 10)
    //             F = 1.1983052175843252;
    //         else
    //             assert (0);

    double sigma = sqrt (52.0 * 51.0 / 2.0) * 1.0 / sqrt (beta) / F;
    return gauss_rand(sigma);
}

double Solve::CalcNoise2 ()
{
   CalcSigma2();
   return gauss_rand(sigma2);
}

void Solve::NeverChooseFather(int index)
{
    while (node[index].father != -1)
    {
        index = node[index].father;
        if (!Exist(NeverChoose, index))
            ChooseOneMore (NeverChoose, index);
    }
}

void Solve::NeverChooseSon(int index)
{
    while (node[index].son != -1)
    {
        index = node[index].son;
        if (!Exist(NeverChoose, index))
            ChooseOneMore (NeverChoose, index);
    }
}

double LambdaFunc (double x, const double & epsilon)
{
    return erfc (x) + erfc (sqrt (x * x + epsilon)) - 2 * delta;
}

double Solve::gauss_rand(const double sigma)
{
    static double V1, V2, S;
    static int phase = 0;
    double X;
    if ( phase == 0 )
    {
        do {
            double U1 = (double)rand() / RAND_MAX;
            double U2 = (double)rand() / RAND_MAX;

            V1 = 2 * U1 - 1;
            V2 = 2 * U2 - 1;
            S = V1 * V1 + V2 * V2;
        } while(S >= 1 || S == 0);

        X = V1 * sqrt(-2 * log(S) / S);
    }
    else
        X = V2 * sqrt(-2 * log(S) / S);
    phase = 1 - phase;
    return X * sigma * sigma;
}

double Solve::CalcEdgeScore(const ScoreItem now)
{
    return now.weight / (1.0 * node[now.x].domain * node[now.y].domain);
}

ScoreItem Solve::AskMaxScore()
{
    double Max = CalcEdgeScore(score[0]);
    int index = 0;
    for (int i = 0; i < score.size (); i++)
        if (CalcEdgeScore (score[i]) > Max)
            Max = CalcEdgeScore (score[i]), index = i;
    return score[index];
}

void Solve::ChooseOneMore(ll &choose, int x)
{
    assert (x < 52);
    assert (!Exist(choose, x));
    choose |= (1LL << x);
}

ll Solve::BuildOriginGraph()
{
    auto OriginEdge = AskMaxScore();
    set<int> OriginSet;
    ll ret = 0;
    OriginSet.insert (OriginEdge.x), OriginSet.insert (OriginEdge.y);
    for (auto i : OriginSet)
        NeverChooseSon(i), NeverChooseFather (i);
    double CurJudge = ThetaUseful / (1.0 * node[OriginEdge.x].domain * node[OriginEdge.y].domain);
    while (AddOneEdge(OriginSet, CurJudge));
    for (auto i : OriginSet)
        ChooseOneMore (ret, i);
    return ret;
}

bool Solve::AddOneEdge(set<int> &now, double &CurJudge)
{
    double Max;
    int index = -1;

    rep (i, 0, TotNode - 1)
        if (!Exist(NeverChoose, i) && now.find (i) == now.end ()) // can choose
        {
            if (CurJudge / (1.0 * node[i].domain) < 1.0) //not theta-useful
                continue;
            double CurScore = 0;
            for (auto j : now)
                CurScore += edge[i][j] / (1.0 * node[i].domain * node[j].domain);
            if (index == -1 || CurScore > Max)
                Max = CurScore, index = i;
        }

    if (index == -1) // can not choose any more
        return 0;
    now.insert (index);
    //else choose
    CurJudge /= (1.0 * node[index].domain);
    now.insert (index);
    // father and son can not choose
    NeverChooseFather(index);
    NeverChooseSon(index);
    return 1;
}

void Solve::BuildGraph()
{
    ThetaUseful = (1.0 * N + gauss_rand(sigma2)) / theta / sigma3;
    ll Origin = BuildOriginGraph();
    cout << "finish origin graph :";
    rep (i, 0, TotNode - 1)
        if (Exist (Origin, i))
            cout << i << "  ";
    cout << endl;
    part.push_back (Origin);
    AddAllParent(Origin);
    NeverChoose = 0;
    int MarginNum = 1;
    while (AddPoint (Origin))
    {
        MarginNum++;
        if (MarginNum > TestMarginNum)
            break;
    }
}

bool Solve::FamilyRelation(int x, int y)
{
    int fa = x;
    while (node[fa].father != -1)
    {
        fa = node[fa].father;
        if (fa == y)
            return 1;
    }
    int son = x;
    while (node[son].son != -1)
    {
        son = node[son].son;
        if (son == y)
            return 1;
    }
    return 0;
}

bool Solve::AddPoint(ll &now)
{
    int index = -1;
    double Max;
    ll BestSet = 0;
    rep (i, 0, TotNode - 1)
        if (!Exist(NeverChoose, i) && !Exist(now, i))
        {
            auto temp = FindParentSet (now, i);
            if (index == -1 || temp.first > Max)
            {
                Max = temp.first;
                index = i;
                BestSet = temp.second;
            }
        }
    if (index == -1)
        return 0;
    ChooseOneMore(now, index);
    NeverChooseFather(index);
    cout << "    add one point : " << index << "  ::: ";
    rep (i, 0, TotNode - 1)
        if (Exist(BestSet, i))
            cout << i << " ";
    cout << endl;
    if (Debug)
    {
        rep (i, 0, TotNode - 1)
            rep (j, 0, TotNode - 1)
                if (Exist(BestSet, i) && Exist(BestSet, j) && i != j)
                    assert (!FamilyRelation (i, j));
        rep (i, 0, TotNode - 1)
            if (Exist(BestSet, i))
                assert (!FamilyRelation(i, index));
    }
    ChooseOneMore(BestSet, index);
    part.push_back (BestSet);
    AddAllParent(now);
    return 1;
}

bool Solve::BelongFamily(const int x, const ll &choose)
{
    int fa = x;
    while (node[fa].father != -1)
    {
        fa = node[fa].father;
        if (Exist(choose, fa))
            return 1;
    }
    int son = x;
    while (node[son].son != -1)
    {
        son = node[son].son;
        if (Exist(choose, son))
            return 1;
    }
    return 0;
}

pair<double, ll > Solve::FindParentSet(const ll &CanChoose, const int x)
{
    unordered_map <ll, bool>vis;
    unordered_map<ll, vector<int> > mp;

    double judge = ThetaUseful / (1.0 * node[x].domain);
    assert (theta >= 1.0); //can choose the first point
    queue <QueueItem> q;
    rep (i, 0, TotNode - 1)
        if (Exist(CanChoose, i) && !FamilyRelation(x, i)) //add only one
            if (judge / (1.0 * node[i].domain) >= 1.0)
            {
                mp[0].push_back (i);
                auto tmp = QueueItem (judge / (1.0 * node[i].domain), i);
                tmp.score = edge[x][i];
                q.push (tmp);
                ll cur = 0;
                cur |= (1LL << i);
                vis[cur] = 1;
            }
    ll ret = 0;
    double Max;
    while (!q.empty ())
    {
        auto now = q.front ();
        q.pop ();
//        assert (!vis[now.choose]);
//        cout << "now  ++++  ";
//        cout << now.judge << "   " << now.score << "  : " ;
//        for (auto i : now.choose)
//            cout << i << ' ';
//        cout << endl;
//        if (Debug)
//            for (auto i : now.choose)
//                assert (BelongFamily(i, now.choose) == 0); // check no family relation

        if (ret == 0 || now.score >= Max) //update the ret value
            Max = now.score, ret = now.choose;
        //find new node
        rep (i, 0, TotNode - 1)
            if (Exist(now.choose, i))
            {
                ll sub = now.choose;
                Erase (sub, i);
                for (auto j : mp[sub]) //add j to now
                    if (now.judge / (1.0 * node[j].domain) >= 1.0) // can add j to now
                    {
                        if (BelongFamily (j, now.choose) || Exist (now.choose, j))
                            continue;
                        mp[now.choose].push_back (j); //update the map

                        //push the new node
                        auto New = now;
                        ChooseOneMore(New.choose, j);
                        New.judge /= 1.0 * node[j].domain;
                        New.score += edge[x][j];
                        if (vis[New.choose] == 0)
                        {
                            q.push (New), vis[New.choose] = 1;
    //                        cout << "         extend  ";
    //                        for (auto k : New.choose)
    //                            cout << k << "  ";
    //                        cout << endl;
                        }
                    }
            }
    }
    return make_pair (Max, ret);
}

bool Solve::Exist(const ll choose, int x)
{
    return choose & (1LL << x);
}

void Solve::Erase(ll &choose, int x)
{
    assert (Exist(choose, x));
    choose ^= (1LL << x);
    assert (!Exist(choose, x));
}

void Solve::AddAllParent (ll &x)
{
    rep (i, 0, TotNode - 1)
        if (Exist(x, i))
        {
            int index = node[i].father;
            while (index != -1)
            {
                if (!Exist (x, index))
                    ChooseOneMore(x, index), cout << "               add additional point  " << index << endl;
                index = node[index].father;
            }
        }
}

void Solve::CalcSigma3 ()
{
    // double F;
    // if (epilson == 0.1)
    //     F = 0.011403467309464246;
    // else
    //     if (epilson == 1)
    //         F = 0.10153596547895295;
    //     else
    //         if (epilson == 10)
    //             F = 1.1983052175843252;
    //         else
    //             assert (0);
    sigma3 = sqrt (52.0) / sqrt (1.0 - beta - beta2)/F; //sqrt (Margin_size_part) * 1.0 / sqrt(1.0 - beta - beta2) / F;
}

void Solve::CalcSigma2 ()
{
    // double F;
    // if (epilson == 0.1)
    //     F = 0.011403467309464246;
    // else
    //     if (epilson == 1)
    //         F = 0.10153596547895295;
    //     else
    //         if (epilson == 10)
    //             F = 1.1983052175843252;
    //         else
    //             assert (0);
    sigma2 = 1.0 / sqrt (beta2)/F;
}

