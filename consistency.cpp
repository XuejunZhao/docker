//
// Created by workshop on 11/24/2018.
//

#include "consistency.h"

void consistency::work ()
{
    FamilyRelation.resize (solve.TotNode);
    CalcFamilyPosition (FamilyRelation);
    CalcMargin ();
    CalcError ();
    SortGraph ();
    CalcError();
    cout << "adjacent !!!!!!!!!!==========\n";
    adjust_FamilyConsistency ();
    CalcError ();
    /*for (auto i : margin)
    {
        PrintLL(i.choose);
        cout << "family num  " << CalcFamilyNum(i.choose) << "    " << (*i.count.begin ()).first.size () << endl;
    }*/
}

void consistency::CalcError ()
{
    cout << "Calc Error  !!  ::::   ";
    assert (margin.size () == OriginMargin.size ());
    double sum = 0.0;
    int tot = 0;
    for (int i = 0; i < margin.size (); i++)
    {
        double error = 0;
//        assert (margin[i].choose == OriginMargin[i].choose);
        //cout << margin[i].count.size () << "   " << OriginMargin[i].count.size () << endl;
        assert (margin[i].count.size () == OriginMargin[i].count.size ());
        for (auto j : margin[i].count)
            error += (j.second - OriginMargin[i].count[j.first]) * (j.second - OriginMargin[i].count[j.first]);
        //error /= 1.0 * margin[i].count.size ();
        sum += error;
        tot += margin[i].count.size ();
    }
    cout << sum / tot << endl;
}

ll consistency::FindCommon ()
{
    rep (i, 0, MarginSize - 1)
        rep (j, i + 1, MarginSize - 1)
        {
            auto cur = margin[i].choose & margin[j].choose;
            if (cur == 0)
                continue;
            CommonMargin.push_back (cur);
            if (Debug)
                rep (k, 0, solve.TotNode - 1)
                    if (solve.Exist (cur, k))
                        assert (solve.Exist(margin[i].choose, k)),
                        assert (solve.Exist(margin[j].choose, k));
                    else
                        assert (0 == solve.Exist(margin[i].choose, k) || 0 == solve.Exist(margin[j].choose, k));
        }
}

int consistency::CalcFamilyDomainSize (const ll x)
{
    int ret = 1;
    for (auto i : AllFamilyMember)
    {
        for (int j = i.size () - 1; j >= 0; j--)
            if (solve.Exist(x, i[j]))
            {
                ret *= solve.node[i[j]].domain;
                break;
            }
    }
    return ret;
}

void consistency::CalcMargin ()
{
    Margin_size_part = solve.part.size ();
    int TotNum = TestMarginNum, now = 0;
    for (auto Origin_i : solve.part)
    {
        now++;
        if (now > TotNum)
            break;
        ll i = Origin_i;
       // AddAllParent (i);

        Margin cur (i);
        cur.CellValue = solve.sigma3 * solve.sigma3;
        for (const auto j : solve.data)
        {
            auto test = SelectSubData(j, i);
//            for (auto k : test)
//                cout << k << "  ";
//            cout << endl;
//            cout << EncodeVector(test, i) << " === \n";
            if (Debug)
            {
//                auto Encode = EncodeVector(test, i);
//                auto P = DecodeToVec(Encode, i);
//                assert (P == test);
            }
            cur.count[test] += 1.0;
        }
        if (Debug)
        {
            OriginMargin.push_back (cur);
            vector<int> temp;
            Dfs (*OriginMargin.rbegin (), 0, temp, 0LL, -1.0);
            int size = OriginMargin.size ();
            AddAllParent(OriginMargin[size - 1].choose);
        }
        vis = 0;
        AddNoise3 (cur);
        assert (vis == CalcFamilyDomainSize (i));
        assert (cur.count.size () == vis);
        AddAllParent(cur.choose);
        margin.push_back (cur);
    }
}

vector<int> consistency::ClearVecParent (const vector<int> &X_withParent, const ll ChooseWithParent)
{
    int LastFamilyNumber = -1, index = 0;
    vector<int> ret;
    rep (i, 0, solve.TotNode - 1)
        if (solve.Exist(ChooseWithParent, i))
        {
            if (LastFamilyNumber != solve.OriginPos[i])
                ret.push_back (X_withParent[index]), LastFamilyNumber = solve.OriginPos[i];
            index++;
        }
    return ret;
}

ll consistency::ClearAllParent (const ll x)
{
    ll ret = 0;
    rep (i, 0, 31)
    {
        for (int j = (int)AllFamilyMember[i].size () - 1; j >= 0; j--)
//        rep (j, 0, AllFamilyMember[i].size () - 1)
            if (solve.Exist(x, AllFamilyMember[i][j]))
            {
                solve.ChooseOneMore(ret, AllFamilyMember[i][j]);
                break;
            }
    }
    return ret;
}

vector<int> consistency::SelectSubData (const vector<int> &data, const ll x)
{
    vector<int> ret;
    rep (i, 0, solve.TotNode - 1)
    {
        int Origin = solve.OriginPos[i];
        //check origin pos
        int succe = 0;
        for (auto j : AllFamilyMember[Origin])
            if (j == i)
                succe = 1;
        assert (succe);
        if (solve.Exist(x, i))
            ret.push_back (data[Origin] / solve.node[i].base);
    }
    assert (CalcDimension(x) == ret.size ());
    return ret;
}

void consistency::AddAllParent (ll &x)
{
    rep (i, 0, 31)
    {
        for (int j = AllFamilyMember[i].size () - 1; j >= 0; j--)
            if (solve.Exist (x, AllFamilyMember[i][j]))
            {
                for (int k = j - 1; k >= 0; k--)
                    if (solve.Exist(x, AllFamilyMember[i][k]) == 0)
                        solve.ChooseOneMore(x, AllFamilyMember[i][k]);
                break;
            }
    }
}

void consistency::AddNoise3 (Margin &x)
{
    vector<int> temp;
    Dfs (x, 0, temp, 0LL, solve.sigma3);
}

void consistency::Dfs (Margin &x, int pos, vector<int> &now, ll choose, const double sigma)
{
    while (pos < 52 && solve.Exist(x.choose, pos) == 0)
        pos++;
    if (pos == 52)
    {
        vis++;
        if (sigma >= 0.0)
            x.count[now] += NOISE * solve.gauss_rand(sigma); //add noise
        else
            x.count[now] += 0.0; //add noise
        return;
    }
    if (Debug)
        assert (pos < 52 && solve.Exist(x.choose, pos));

    int index = solve.node[pos].son, son = -1;
    while (index != -1)
    {
        if (solve.Exist(x.choose, index))
        {
            son = index;
            break;
        }
        index = solve.node[index].son;
    }

    if (son != -1) //already exist this family
    {
        int last = *now.rbegin ();
        last /= solve.node[pos].base / solve.node[son].base;
        now.push_back (last);
        solve.ChooseOneMore(choose, pos);
        Dfs (x, pos + 1, now, choose, sigma);
        solve.Erase(choose, pos);
        now.pop_back ();
    }
    else
    {
        rep (i, 0, solve.node[pos].domain - 1)
        {
            now.push_back (i);
            solve.ChooseOneMore(choose, pos);
            Dfs (x, pos + 1, now, choose, sigma);
            solve.Erase(choose, pos);
            now.pop_back ();
        }
    }
}

void consistency::SortGraph ()
{
    MarginSize = margin.size ();
    FindCommon();
    sort (CommonMargin.begin (), CommonMargin.end ());
    CommonMargin.erase (unique (CommonMargin.begin (), CommonMargin.end ()), CommonMargin.end ());
    common_MarginSize = CommonMargin.size ();
    edge.resize (common_MarginSize);
    in.resize (common_MarginSize);
    rep (i, 0, common_MarginSize - 1)
        rep (j, i + 1, common_MarginSize - 1)
            CompareCommonMargin (i, j);
    TopSort ();
}

void consistency::CalcFamilyPosition (vector<pii > &ret)
{
    int cur = 32;
    AllFamilyMember.resize (32);
    for (int i = solve.TotNode - 1; i >= 0; i--)
    {
        if (solve.node[i].father == -1) // is the biggest
            ret[i] = pii (--cur, 0);
        else
        {
            ret[i] = ret[i + 1], ret[i].second++;
            if (Debug)
                assert (i + 1 < ret.size ()), assert (ret[i].first == cur);
        }
        AllFamilyMember[ret[i].first].push_back(i);
    }
    assert (cur == 0);
}

void consistency::CompareCommonMargin (const int x, const int y)
{
    if ((CommonMargin[x] & CommonMargin[y]) == CommonMargin[x]) // x belong to y
    {
        AddEdge(x, y);
        return;
    }
    if ((CommonMargin[x] & CommonMargin[y]) == CommonMargin[y]) // y belong to x
    {
        AddEdge(y, x);
        return;
    }

    int big_x = 0, big_y = 0;
    rep (i, 0, 31)
    {
        if (!HaveFmaily (x, i) || !HaveFmaily(y, i)) //only one have Family[i]
            continue;
        for (auto j : AllFamilyMember[i])
        {
            bool x_exist = solve.Exist(CommonMargin[x], j);
            bool y_exist = solve.Exist(CommonMargin[y], j);
            if (x_exist != y_exist) // not equal
            {
                if (x_exist) // x have, y not have
                    big_y = 1;
                else
                    big_x = 1;
            }
        }
    }
    //cout << big_x << " ?? " << big_y << endl;
    if (big_x ^ big_y)
    {
//        cout << "add edge !!!!!!!!!!!    " << big_x << big_y << endl;
//        PrintLL(CommonMargin[x]), PrintLL (CommonMargin[y]);
        if (big_x)
            AddEdge(y, x);
        else
            AddEdge (x, y);
    }
}

bool consistency::HaveFmaily (int x, int Family)
{
    int num =0;
    for (auto i : AllFamilyMember[Family])
        if (solve.Exist(CommonMargin[x], i))
            num++;
    return num;
}

void consistency::AddEdge (int x, int y)
{
    assert (x < common_MarginSize);
    assert(y < common_MarginSize);
//    cout << "add edge " << x << "  " << y << endl;
//    PrintLL (CommonMargin[x]);
//    PrintLL (CommonMargin[y]);
    edge[x].push_back (y);
    in[y]++;
}

void consistency::TopSort ()
{
    cout << "Top Sort\n";
    queue <int> q;
    rep (i, 0, common_MarginSize - 1)
        if (in[i] == 0)
            q.push (i);
    while (!q.empty ())
    {
        int now = q.front ();
        q.pop ();
        PrintLL(CommonMargin[now]),
        AdjustConsistentAccordingToCommon(CommonMargin[now]);
        for (auto i : edge[now])
        {
            in[i]--;
            if (in[i] == 0)
                q.push (i);
        }
    }
}

void consistency::PrintLL (ll x)
{
    rep (i, 0, solve.TotNode - 1)
        if (solve.Exist(x, i))
            cout << i << "(" << FamilyRelation[i].first << ", " << FamilyRelation[i].second << ")   ";
    cout << endl;
}

vector<double> consistency::CalcWeightValue (const vector<int> &need, const ll x, const vector<map <vector<int>,int>> Time)
{
    vector<double> ret;
    double PreSigma = margin[need[0]].CellValue * (*Time[0].begin()).second;
    rep (i, 1, need.size () - 1)
    {
        double CurSigma = margin[need[i]].CellValue * (*Time[i].begin ()).second;
        double PreWeight = (CurSigma / (CurSigma + PreSigma));
        double CurWeight = (PreSigma / (CurSigma + PreSigma));
        if (i == 1)
            ret.push_back (PreWeight), ret.push_back(CurWeight);
        else
        {
            assert (ret.size () == i);
            for (auto &j : ret)
                j *= PreWeight;
            ret.push_back (CurWeight);
        }
        PreSigma = PreSigma * PreWeight * PreWeight + CurSigma * CurWeight * CurWeight;
    }
    assert (ret.size () == need.size ());
    double sum = 0.0;
    for (auto i : ret)
        sum += i;
    assert (fabs (sum - 1.0) < 1e-8);
    return ret;
}

void consistency::AdjustConsistentAccordingToCommon (const ll x)
{
    cout << "common adjust  !!  ";
    vector<int> need;
    rep (i, 0, MarginSize - 1)
        if ((margin[i].choose & x) == x) //i contains x
            need.push_back (i), cout << i << "  ";
    cout << endl;
//    for (auto i : need)
//        PrintLL(margin[i].choose);
    map <vector<int>, double> TotalSumAverage;
    vector<map<vector<int>, double> > EachListSum;
    int ColDomain = CalcFamilyDomainSize(x);

    vector<map <vector<int>,int>> Time (need.size (), map<vector<int>,int> ());
    rep (i, 0, need.size () - 1)
        for (auto j : margin[need[i]].count)
        {
            vector<int> Ex = Extract (j.first, margin[need[i]].choose, x);
            Time[i][Ex]++;
        }
//        rep (i, 0, need.size () - 1)
//        {
//            cout << "Time i " << i << "                ";
//            for (auto j : Time[i])
//                cout << j.second << "  ";
//            cout << endl;
//        }

    auto WeightValue = CalcWeightValue (need, x, Time);
    int index = 0;
    for (auto List : need)
    {
        map <vector<int>, double> ListSum;
        for (auto EachItem : margin[List].count)
        {
            vector<int> ColumnItem = Extract (EachItem.first, margin[List].choose, x);
            ListSum[ColumnItem] += EachItem.second;
        }

        //add to the total sum
        for (auto each : ListSum)
            TotalSumAverage[each.first] += each.second * WeightValue[index];
        EachListSum.push_back (ListSum);
        index++;
    }
    assert (index == WeightValue.size ());

    vector<int> MostRowNumber;
    //begin to adjust each table

    rep (i, 0, need.size () - 1)
    {
        map<vector<int>, double> ChangeNum;
        //int RowDomain = CalcFamilyDomainSize(margin[need[i]].choose) / ColDomain;

        MostRowNumber.push_back ((*Time[i].begin ()).second);

        for (auto j : EachListSum[i])
        {
            ChangeNum[j.first] = (j.second - TotalSumAverage[j.first]) / (1.0 * Time[i][j.first]);
            if (j == *EachListSum[i].begin ())
                cout << "before change, the sum of margin " << need[i] << "    of "  << "  is : " << j.second << "   " << TotalSumAverage[j.first] << "      time : " << Time[i][j.first] << endl;
        }

        map <vector<int>,double> debug_map;
        for (auto &j : margin[need[i]].count)
        {
            vector<int> Ex = Extract (j.first, margin[need[i]].choose, x);
            j.second -= ChangeNum[Ex];

            debug_map[Ex] += j.second;
        }
        //print debug
        map<vector<int>,double> origin_map;
        for (auto j : OriginMargin[need[i]].count)
            origin_map[Extract(j.first, margin[need[i]].choose, x)] += j.second;
//        cout << "Change num  " << need[i] << "    " << (*ChangeNum.begin ()).second << endl;
        cout << "after change, the sum of margin " << need[i] << "    of "  << "  is :                                                   " << (*debug_map.begin()).second << endl;
        cout << "Origin map, the sum of margin " << need[i] << "    of " << "  is : " << (*origin_map.begin ()).second << endl;
        /*for (auto j : ChangeNum)
            cout << "Change num  " << need[i] << "    " << j.second << endl;
        for (auto j : debug_map)
            cout << "after change, the sum of margin " << need[i] << "    of "  << "  is : " << j.second << endl;*/
//        for (auto j : origin_map)
//            cout << "Origin map, the sum of margin " << need[i] << "    of " << "  is : " << j.second << endl;

    }
    assert (MostRowNumber.size () == need.size ());
    rep (i, 0, need.size () - 1)
        UpdateCellValue (need, i, WeightValue, MostRowNumber[i]);
    CalcError();
}

void consistency::UpdateCellValue (const vector<int> &need, const int pos, const vector<double> WeightValue, const int RowNumber)
{
    double count = 1.0 * RowNumber;
    double w = WeightValue[pos];
    double Third = 0.0;
    rep (i, 0, need.size () - 1)
        if (i != pos)
            Third += WeightValue[i] * WeightValue[i] * margin[need[i]].CellValue;
    margin[need[pos]].CellValue = (1 - w / count) * (1 - w / count) * margin[need[pos]].CellValue
                            + w * w * (count - 1.0) / count / count * margin[need[pos]].CellValue + Third / count / count;
}

int consistency::CalcDomainSize (const ll x)
{
    int ret = 1;
    rep (i, 0, solve.TotNode - 1)
        if (solve.Exist(x, i))
            ret *= solve.node[i].domain;
    return ret;
}

int consistency::CalcDimension (const ll x)
{
    int ret = 0;
    rep (i, 0, solve.TotNode - 1)
        if (solve.Exist(x, i))
            ret++;
    return ret;
}

vector<int> consistency::Extract (const vector<int> OriginVec, const ll Origin, const ll WantExtract)
{
    /*ll OriginChoose = 0;
    rep (i, 0, 31)
        for (int j = AllFamilyMember[i].size () - 1; j >= 0; j--)
            if (solve.Exist(Origin, AllFamilyMember[i][j]))
            {
                solve.ChooseOneMore(OriginChoose, AllFamilyMember[i][j]);
                break;
            }
    ll WantExtract = 0;
    rep (i, 0, 31)
        for (int j = AllFamilyMember[i].size () - 1; j >= 0; j--)
            if (solve.Exist(WantExtract_, AllFamilyMember[i][j]))
            {
                solve.ChooseOneMore(WantExtract, AllFamilyMember[i][j]);
                break;
            }
    assert(OriginVec.size () == CalcDimension(OriginChoose));
    int index = 0;
    vector<int> ret;
    rep (i, 0, solve.TotNode - 1)
        if (solve.Exist(OriginChoose, i))
        {
            if (solve.Exist(WantExtract, i))
                ret.push_back (OriginVec[index]);
            index++;
        }
    assert (index == CalcDimension(OriginChoose));*/
    int index = -1;
    vector<int> ret;
    rep (i, 0, 31)
    {
        //test if exist
        int pos = -1, base;
        for (int j = AllFamilyMember[i].size () - 1; j >= 0; j--)
            if (solve.Exist(Origin, AllFamilyMember[i][j]))
            {
//                cout << "Exist  !  " << AllFamilyMember[i][j] << endl;
                index++;
                pos = j;
                base = solve.node[AllFamilyMember[i][j]].base;
                break;
            }
        if (pos != -1) //exist !
        {
            for (int j = pos; j >= 0; j--)
                if (solve.Exist(WantExtract, AllFamilyMember[i][j]))
                {
                    ret.push_back (OriginVec[index] * base / solve.node[AllFamilyMember[i][j]].base);
                    break;
                }
        }
    }
    assert (index == OriginVec.size () - 1);
    return ret;
}

void consistency::adjust_FamilyConsistency ()
{
    for (auto i : margin) //adjust margin=i
        for (auto j : AllFamilyMember) //adjust family-j  of  margin-i
            for (auto k : j)
                if (solve.Exist(i.choose, k))
                {
                    int son = FindFamilySon (i.choose, k);
                    if (son != -1)
                        AdjacentAdjust (i, k, son);
                }
}

void consistency::AdjacentAdjust (Margin &x, const int fa, const int son)
{
    cout << "adjacent  adjust ::  " << fa << "   " << son << endl;
    PrintLL(x.choose);
    assert (solve.node[fa].base / solve.node[son].base == 10);
    if (Debug)
        assert (solve.Exist(x.choose, fa) && solve.Exist(x.choose, son));
    ll TheOtherPart = x.choose ^ (1LL << fa) ^ (1LL << son);
    map<pair<vector<int>, int>, double> SonSumAndChange, FatherCount;
    for (auto i : x.count)
    {
        auto the_other = Extract(i.first, x.choose, TheOtherPart);
        auto FaPart = Extract(i.first, x.choose, (1LL << fa));
        auto SonPart = Extract(i.first, x.choose, (1LL << son));
        if (Debug)
            assert (FaPart.size () == 1 && SonPart.size () == 1);
        FatherCount[make_pair (the_other, FaPart[0])] += i.second;
        SonSumAndChange[make_pair (the_other, SonPart[0] / 10)] += i.second;
    }
    //begin to change
    for (auto &i : x.count)
    {
        auto the_other = Extract(i.first, x.choose, TheOtherPart);
        auto FaPart = Extract(i.first, x.choose, (1LL << fa));
        auto SonPart = Extract(i.first, x.choose, (1LL << son));
        if (Debug)
            assert (FaPart.size () == 1 && SonPart.size () == 1);
        i.second += (FatherCount[make_pair (the_other, FaPart[0])] - SonSumAndChange[make_pair(the_other, SonPart[0] / 10)]) / 10.0;
    }
}

int consistency::FindFamilySon (const ll x, const int pos)
{
    if (solve.node[pos].son != -1)
    {
        if (solve.Exist(x, solve.node[pos].son))
            return solve.node[pos].son;
        else
            return -1;
    }
    else
        return -1;
}

vector<vector<int> > consistency::Genarate ()
{
    CurMargin = margin;
    PreSimpling();
    double num = 0.0;
    for (auto i : margin[0].count)
        num += i.second;
    vector<vector<int> > ret;
    int Dec = 1;
    rep (i, 1, (int)num) //Dec
    {
        if (i % 1000 == 0)
            cout << i <<endl;
        auto now = Simpling(Dec);
        int t = 0;
        while (now.first == 0 && (Dec == 0 || t <= 10))
            now = Simpling(Dec), t++;
        if (now.first == 0)
        {
            assert (Dec);
            Dec = 0;
            CurMargin = margin;
            i--;
            continue;
        }
        assert (now.first);
        ret.push_back(now.second);
    }
    return ret;
    /*rep (i, 0, 1)
    {
        ret.clear ();
        CurMargin = margin;
        if (TryGenarate(ret, tot))
            break;
        cout << "Try Genaration***********@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n";
    }
    rep (i, 1, tot - ret.size ())
    {
        auto now = Simpling();
        while (now.first == 0)
            now = Simpling(0.0);
        ret.push_back (now.second);
    }
    return ret;*/
}

bool consistency::TryGenarate (vector<vector<int> > &result, int size)
{
    rep (i, 0, size - 1)
    {
        cout << i << endl;
        int ti = 0, have = 0;

        while (ti < 10)
        {
            auto now = Simpling ();
            if (now.first == 0)
            {
                ti++;
                cout << "now fail !!!&&&&&&&&    " << result.size () << endl;
            }
            else
            {
                result.push_back (now.second);
                have = 1;
                break;
                //cout << "succe !!!!!!!!!!!!\n";
            }
        }
        if (!have)
            return 0;
    }
    return 1;
}

pair<bool, vector<int> > consistency::Simpling (int Dec)
{
    auto ret = simpling_first ();
    auto Ret = ret;

    vector<int> SubTot;
    vector<pair<int,int> > SubCountNum;

    for (int i = 1; i < CurMargin.size (); i++)
    {
        CommonVec = Extract(ret, choose[i - 1], common[i]);
        double Tot = tot[tot_index[make_pair(i, CommonVec)]];
        if (Tot <= 0)
            return make_pair(0, vector<int> ());
        double sum = 0.0, x = RandDouble(Tot);
        int can = 0;
        int now_index = CountNumIndex[make_pair(CommonVec, i)];
        assert (CountNum[now_index].size () == solve.node[extra[i]].domain);
        rep (j, 0, solve.node[extra[i]].domain - 1)
        {
            //double now = CountNum[make_pair (CommonVec, make_pair (i, j))];
            double now = CountNum[now_index][j];
            if (sum <= x && x <= sum + now) //choose it
            {
                ret = ChangeOne(ret, choose[i - 1], extra[i], j);
                can = 1;
                SubCountNum.push_back(make_pair(now_index, j));
                SubTot.push_back(tot_index[make_pair(i, CommonVec)]);
                break;
            }
            sum += now;
        }
        assert (can);
    }
    if (Dec) // sub it
    {
        //for zero
        if (SubNumber (tot[tot_index[make_pair (0, vector<int>())]]) == 0 || SubNumber(CurMargin[0].count[Ret]) == 0)
            return make_pair (0, ret);
        //for other
        assert (SubCountNum.size () == SubTot.size ());
        for (int i = 0; i < (int)SubCountNum.size (); i++)
            if (SubNumber(tot[SubTot[i]]) == 0 || SubNumber(CountNum[ SubCountNum[i].first ][ SubCountNum[i].second ]) == 0)
                return make_pair (0, ret);
    }
    return make_pair(1, ret);
}

void consistency::PreSimpling ()
{
    extra.resize(CurMargin.size ());
    common.resize(CurMargin.size ());
    choose.resize(CurMargin.size ());

    choose[0] = CurMargin[0].choose;
    AddAllParent(choose[0]);
    for (int i = 1; i < CurMargin.size (); i++)
    {
        extra[i] = FindExtraOne (CurMargin[i].choose, choose[i - 1]);
        common[i] = choose[i - 1] & CurMargin[i].choose;
        choose[i] = choose[i - 1] | CurMargin[i].choose;
        AddAllParent(choose[i]);
    }

    for (int i = 0; i < CurMargin.size (); i++)
    {
        for (auto j : CurMargin[i].count)
        {
            vector<int> Common = Extract (j.first, CurMargin[i].choose, common[i]);
            auto temp = make_pair (i, Common);
            if (tot_index.find (temp) == tot_index.end ())
                tot_index[temp] = tot.size (), tot.push_back(0.0);
            tot[tot_index[temp]] += j.second;
        }
    }
    for (int i = 1; i < CurMargin.size (); i++)
    {
        for (auto j : CurMargin[i].count)
        {
            vector<int> Common = Extract (j.first, CurMargin[i].choose, common[i]);
            vector<int> TheOther = Extract (j.first, CurMargin[i].choose, (1LL << extra[i]));
            assert (TheOther.size () == 1);
            //CountNum[make_pair (Common, make_pair (i, TheOther[0]))] = j.second;
            auto Pi = make_pair (Common, i);
            if (CountNumIndex.find (Pi) == CountNumIndex.end ())
            {
                CountNumIndex[Pi] = CountNum.size ();
                vector<double>temp (solve.node[extra[i]].domain, -1.0);
                CountNum.push_back (temp);
            }
            CountNum[CountNumIndex[Pi]][TheOther[0]] = j.second;
        }
    }
    cout << "finish PreSimple\n";
}

vector<int> consistency::simpling_first ()
{
    double x = RandDouble (tot[tot_index[make_pair (0, vector<int>())]]);

    double sum = 0.0;
    for (auto i : CurMargin[0].count)
    {
        if (sum <= x && x <= sum + i.second)
            return i.first;
        sum += i.second;
    }
    assert (0);
}

int consistency::FindExtraOne (const ll x, unsigned long long int y)
{
    int num = 0, ret = -1;
    rep (i, 0, solve.TotNode - 1)
        if (solve.Exist(x, i) && !solve.Exist(y, i))
            ret = i, num++;// cout << "same   !! " << i << endl;
    assert (num == 1);
    return ret;
}

vector<int> consistency::CommonVecAddOne (const vector<int> &x, const ll choose, const int pos, const int value)
{
    int index = 0;
    vector<int> ret;
    assert (!solve.Exist (choose, pos));
    rep (i, 0, 31)
    {
        int exist = 0;
        for (int j = AllFamilyMember[i].size () - 1; j >= 0; j--)
            if (solve.Exist(choose, AllFamilyMember[i][j]))
            {
                exist = 1;
                if (i == solve.FindOrigin(pos))
                    ret.push_back (value);
                else
                    ret.push_back (x[index]);
                index++;
                break;
            }
        if (!exist && i == solve.FindOrigin(pos))
            ret.push_back(value);
    }
    /*rep (i, 0, solve.TotNode - 1)
        if (solve.Exist(choose, i))
        {
            assert (pos != i);
            ret.push_back (x[index++]);
        }
        else
            if (i == pos)
                ret.push_back (value);*/
    assert (index == x.size ());
    //assert (index == CalcDimension(choose));
    return ret;
}

double consistency::RandDouble (double right)
{
    return right * ((double)rand()/RAND_MAX);
}

void consistency::Normalize ()
{
    double sum = -1;
    for (auto &i : margin)
    {
        double cur = 0.0, nxt = 0.0;
        for (auto &j : i.count)
        {
            cur += j.second;
            if (j.second < 0.0)
                j.second = 0;
            nxt += j.second;
        }
        if (sum < 0)
            sum = cur;
        for (auto &j : i.count)
            j.second *= sum / nxt;
        assert((*i.count.begin ()).first.size () == CalcFamilyNum (i.choose));
        i.choose = ClearAllParent(i.choose);
    }
}

vector<int> consistency::FamilyExtract (const vector<int> OriginVec, const ll OriginChoose, const ll WantExtract)
{
    int index = 0, FamilyNum = 0;
    vector<int> ret;
    PrintLL(OriginChoose);
    rep (i, 0, solve.TotNode - 1)
        if (solve.Exist(OriginChoose, i) && (solve.node[i].son == -1 || solve.Exist(OriginChoose, solve.node[i].son) == 0)) //the last of current family
        {
            cout << "  ! !!   " << solve.FindOrigin(i) << endl;
            FamilyNum++;
            for (auto j : AllFamilyMember[solve.FindOrigin(i)])
                if (solve.Exist(WantExtract, j))
                    ret.push_back (OriginVec[index] / solve.node[j].base * solve.node[i].base);
            index++;
        }
    cout << "fmaily num   777 Line :: " << FamilyNum << "    " << OriginVec.size () << endl;
    assert (FamilyNum == OriginVec.size ());//todo
    return ret;
}

vector<int> consistency::EraseFamilyMember (const vector<int> &VecOrigin, const ll ChooseOrigin, const int FamilyNumber)
{
    int pos = -1, count = 0;
    for (auto i : AllFamilyMember[FamilyNumber])
        if (solve.Exist (ChooseOrigin, i))
            pos = i, count++;
    if (count == 0 && pos == -1) // no this family member
        return VecOrigin;
    assert (count == 1);
    assert (pos != -1);
    //assert (VecOrigin.size () == CalcDimension(ChooseOrigin));
    vector<int> ret;
    int index = 0;
    rep (i, 0, solve.TotNode - 1)
        if (solve.Exist(ChooseOrigin, i))
        {
            if (i != pos)
                ret.push_back (VecOrigin[index]);
            index++;
        }
    return ret;
}

int consistency::CalcFamilyNum (const ll x)
{
    int ret = 0;
    rep (i, 0, 31)
    {
        for (auto j : AllFamilyMember[i])
            if (solve.Exist(x, j))
            {
                ret ++;
                break;
            }
    }
    return ret;
}

vector<int> consistency::ChangeOne (const vector<int> &VecOrigin, const ll ChooseOrigin, const int pos, const int value)
{
    vector<int> ret;
    int fa = solve.FindOrigin(pos), index = 0, flag = -1;
    rep (i, 0, 31)
    {
        for (int j = AllFamilyMember[i].size () - 1; j >= 0; j--)
            if (solve.Exist(ChooseOrigin, AllFamilyMember[i][j]))
            {
                if (i == fa)
                    flag = index, ret.push_back(value);
                else
                    ret.push_back(VecOrigin[index]);
                index++;
                break;
            }
        if (flag == -1 && i == fa)
            ret.push_back(value);
    }
    assert (index == VecOrigin.size ());
    return ret;
}

vector<int> consistency::AddFamilyMember (const vector<int> x)
{
    vector<int> ret;
    rep (i, 0, 31)
    {
        for (int j = AllFamilyMember[i].size () - 1; j >= 0; j--)
            ret.push_back (x[i] / solve.node[j].base);
    }
    return ret;
}

void consistency::CountError (const vector<vector<int> > &NewData)
{
    ErrorMargin = margin;
    for (auto &i : ErrorMargin)
        for (auto &j : i.count)
            j.second = 0;
    vector<vector<int> > ExtractNewData;
    for (auto i : NewData)
    {
        //auto BigFamily = AddFamilyMember(i);
        for (auto &j : ErrorMargin)
        {
            auto cur = Extract(i, (1LL << 52) - 1, j.choose);
            j.count[cur] += 1.0;
        }
    }

    cout << "Calc result Error  !!  ::::   ";
    assert (OriginMargin.size () == ErrorMargin.size ());
    double sum = 0.0;
    int tot = 0;
    for (int i = 0; i < OriginMargin.size (); i++)
    {
        double error = 0;
        assert (OriginMargin[i].count.size () == ErrorMargin[i].count.size ());
        for (auto j : OriginMargin[i].count)
            error += (j.second - ErrorMargin[i].count[j.first]) * (j.second - ErrorMargin[i].count[j.first]);
        sum += error;
        tot += OriginMargin[i].count.size ();
    }
    cout << sum / tot << endl;
}

bool consistency::SubNumber (double &x)
{
    if (x == 0.0)
        return 0;
    x = max (x - 1.0, 0.0);
    return 1;
}

