#include "inoutf.h"

void In::processDomain(string domain, map<string, int>& categorical, map<string, pair<double, double>>& numeric) {
    fstream f;
    f.open(domain);
    string s;
    getline(f, s);
    int count = 0;
    double min = 0;
    double max = 0;
    string type;
    string name;
    int json = 0;

    while (getline(f, s)) {

        if (s.size() == 1) break;//the line "}"
        if (s[2] == '}') {//finish reading one attribute
            if (type == "enum") {
                categorical.insert(make_pair(name, count));
//                cout <<json <<name << count << endl;
                json1.insert(make_pair(json, pair<string, int>(name, count)));
                json = json + 1;
            }
            else {
                if (type == "integer" &&  (max - min) < hierarchy_k * hierarchy_k && max > 0) {
                    count = max - min + 1 ;
                    categorical.insert(make_pair(name, count));
                    cout <<json <<name << count << endl;
                    json1.insert(make_pair(json, pair<string, int>(name, count)));
                    integer_small.insert(name);
                    //small.push_back(json);
                    json = json + 1;
                }
                else{
                    numeric.insert(make_pair(name, pair<double, double>(min, max)));
                    cout <<json <<name <<setprecision(12)<< min << max << endl;
                    json2.insert(make_pair(name, pair<double, double>(min, max)));
                    json = json + 1;
                }


            }
        }
        else {
            int loc = s.find('"', 0);
            loc = loc + 1;
            int loc1 = s.find('"', loc);
            string usevalue = s.substr(loc, loc1 - loc);
            if (usevalue == "type") {
                loc = s.find('"', loc1 + 1);
                loc++;
                loc1 = s.find('"', loc);
                type = s.substr(loc, loc1 - loc);
            }
            else if (usevalue == "count") {
                loc1 = loc1 + 3;
                count = atoi(s.substr(loc1).c_str());
            }
            else if (usevalue == "min") {
                loc1 = loc1 + 3;
                min = atof(s.substr(loc1).c_str());
            }
            else if (usevalue == "max") {
                loc1 = loc1 + 3;
                max = atof(s.substr(loc1).c_str());
            }
            else if (usevalue != "optional") {
                name = usevalue;
            }
        }
    }
    f.close();

}





string In::splitHierachyCat (int count, int k, char type, int y)
{
    map<int, pair<int, int>> kuohao;
    int depth = 0;
    double t = count;

    while (t >= y) {
        t = t/ ((double) k);
        depth = depth+1;
    }

    //int depth = floor(log((double)count) / log((double)k)) - 1;
    for (int t = depth; t > 0; t--) {
        int step = pow(k, t);
        int nodenum = ceil((double)count / step);
        for (int i = 0; i < nodenum; i++) {
            int start = i * step;
            int end = (i + 1)*step;//[start, end)
            if (end > count) {
                int x = ceil((count - 1 - start + 1) / pow(k, t - 1));
                if (x == 1) continue;
                else end = count;
            }
            if (kuohao.find(start) == kuohao.end()) {
                pair<int, int> x;
                x.first = 1; x.second = 0;
                kuohao.insert(make_pair(start, x));
            }
            else {
                kuohao.find(start)->second.first++;
            }

            if (kuohao.find(end - 1) == kuohao.end()) {
                pair<int, int> x;
                x.first = 0; x.second = 1;
                kuohao.insert(make_pair(end - 1, x));
            }
            else {
                kuohao.find(end - 1)->second.second++;
            }
        }
    }
    string s;
    for (int i = 0; i < count; i++) {
        if (kuohao.find(i) == kuohao.end()) {
            if(type == 'D')
                s+=(to_string(i));
            else
                s += (to_string(i + 1));//0 is the empty
        }
        else {
            int before = kuohao.find(i)->second.first;
            int after = kuohao.find(i)->second.second;
            for (int ii = 0; ii < before; ii++)
                s.push_back('{');
            if (type == 'D')
                s += (to_string(i));
            else
                s += (to_string(i + 1));
            for (int ii = 0; ii < after; ii++)
                s.push_back('}');
        }
        if (i != count - 1)
            s.push_back(' ');
    }
    return s;
}

//double epsilon;
//double N;

void In::ReadData2 (string R, string S)
{
    int badtuple = 0;
    total = 0;
    fstream f;
    string s;
    map<int, string> attributeInf;
    map<string, int> categorical;
    map<string, pair<double, double>> numeric;
    In::processDomain(R, categorical, numeric);//read attribute information

    dim = categorical.size() + numeric.size();
    cout<<"dim size"<<dim;
    int grid = hierarchy_k*hierarchy_k;//# of values we want to discretize the numeric attribute into
//    int k = 10;
//    int y = 100;

    f.open(S);
    getline(f, s);
    f.close();
    int loc = 0;
    f.open("data.domain", ios::app);
    for (int t = 0; t < dim; t++) {

        string value;
        if (t == dim - 1) {
            value = s.substr(loc);
        }
        else {
            int loc1 = s.find(',', loc);
            value = s.substr(loc, loc1 - loc);
            loc = loc1 + 1;
        }
        attributeInf.insert(make_pair(t, value));
        if (categorical.find(value) != categorical.end()) {
            f << "D" << " ";
            int count = categorical.find(value)->second;
            if (count > 100) {
                string s = splitHierachyCat(count, hierarchy_k, 'D', hierarchy_y);
                for (int si = 0; si < s.size(); si++)
                    f << s[si];
                f << endl;
            }
            else {
                for (int ci = 0; ci < count - 1; ci++)
                    f << ci << " ";
                f << count - 1 << endl;
            }
        }
        else {
            f << "C" << " "<<"{0}"<<" ";
            string s = splitHierachyCat(grid, hierarchy_k, 'C', hierarchy_y);
            for (int si = 0; si < s.size(); si++)
                f << s[si];
            f << endl;
        }
    }
    f.close();


    vector<vector<int>> tuples;
    f.open(S);
    getline(f, s);
    //data.resize (N);
    int count = 0 ;
    while (getline(f, s)) {
        if (s.size() == 0) continue;
        vector<int> tuple;
        loc = 0;
        for (int t = 0; t < dim; t++) {
            int loc1;
            string value;
            if (t < dim - 1) {
                loc1 = s.find(',', loc);
                value = s.substr(loc, loc1 - loc);
                loc = loc1 + 1;
            }
            else {
                value = s.substr(loc);
            }

            string name = attributeInf.find(t)->second;
            if (value.size() == 0) {//read empty value
                tuple.push_back(0);
                continue;
            }
            
            if (categorical.find(name) != categorical.end()) {
                if (integer_small.find(name) != integer_small.end()) {
                    tuple.push_back(atoi(value.c_str())-1);
                }
                else
                    tuple.push_back(atoi(value.c_str()));
                if (tuple.back() >= categorical.find(name)->second) {
                    badtuple++;
                    break;
                }
            }
            else {
                
                double min = numeric.find(name)->second.first;
                double max = numeric.find(name)->second.second;
                double x = atof(value.c_str());
                double step = (double)(max - min) / grid;
                int newvalue = floor((x - min) / step) + 1;//"+1", since "0" indicates empty
                if (newvalue == grid + 1)
                    newvalue = newvalue - 1;
                if (x < min || x > max) {
                    badtuple++;
                    break;
                }
                tuple.push_back(newvalue);
            }
        }
        if (tuple.size() == dim) {
            //data.push_back(tuple);
            total = total + 1;
            tuples.push_back(tuple);
//            data[count]=(tuple);
        }
        count += 1;
    }
    //cout << badtuple <<" badtuple "<< data.size() << endl;
    f.close();
    //epsilon = Solve::epilson;
    f.open("data.dat", ios::app);
    for (int i = 0; i < tuples.size(); i++) {
        for (int j = 0; j < tuples[i].size() - 1; j++)
            f << tuples[i][j] << " ";
        f << tuples[i][tuples[i].size() - 1] << endl;
    }
    f.close();

    del = 1.0 / total / total ;
    CalculateF();
    //cout << "final F"<<F;
}


// map<int,pair<string, pair<double, double>>>&  In::gather_json () {
//     return json2;
// }



//-------------------------used for calculate F in calcNoise(and others)
// delete epilson ==> F
//    if (epilson == 0.1)
//        F = 0.011403467309464246;
//    else
//        if (epilson == 1)
//            F = 0.10153596547895295;
//        else
//            if (epilson == 10)
//                F = 1.1983052175843252;
//            else
//                assert (0);
double Phi(double t) {
    return 0.5*(1.0 + erf(double(t)/sqrt(2.0)));
}

double caseA(double epsilon, double s){
    return Phi(sqrt(epsilon*s)) - exp(epsilon)*Phi(-sqrt(epsilon*(s+2.0)));    //x is a captured variable, y is an argument
};

double caseB(double epsilon, double s){
    return Phi(-sqrt(epsilon*s)) - exp(epsilon)*Phi(-sqrt(epsilon*(s+2.0)));   //x is a captured variable, y is an argument
};


void In::CalculateF()
{
    double GS = 1.0;
    double tol = 1.e-15;
    cout << "epilson"<<epilson;
    double delta_thr = caseA(epilson, 0.0);
    cout << "delta_thr"<<delta_thr;
    double alpha;
    double s_inf = 0.0;
    double s_sup = 1.0;
    double k ;
    if (del == delta_thr) alpha = 1.0;
    else {
        if(del > delta_thr){
            while(caseA(epilson, s_sup) < del){
                s_inf = s_sup;
                s_sup = 2.0*s_inf;
            }
            double s_mid = s_inf + (s_sup-s_inf)/2.0;
            while(abs(caseA(epilson, s_mid) - del)>= tol){
                if (caseA(epilson, s_mid) > del) s_sup = s_mid;
                else s_inf = s_mid;
                s_mid = s_inf + (s_sup-s_inf)/2.0;
            }
            double s_final = s_mid;
            alpha = sqrt(1.0 + s_final/2.0) + sqrt(s_final/2.0);

        }
        else{

            while(caseB(epilson, s_sup) > del){

                s_inf = s_sup;
                s_sup = 2.0*s_inf;
            }
            double s_mid = s_inf + (s_sup-s_inf)/2.0;
            while(abs(caseB(epilson, s_mid) - del) >= tol){
                if (caseB(epilson, s_mid) < del) s_sup = s_mid;
                else s_inf = s_mid;
                s_mid = s_inf + (s_sup-s_inf)/2.0;
            }
            double s_final = s_mid;
            alpha = sqrt(1.0 + s_final/2.0) + sqrt(s_final/2.0);
        }
    }

    double sigma = alpha*GS/sqrt(2.0*epilson);
    F = 1/sigma;
    cout<< "F value" <<F;
}


void In::out_result(vector<vector<int> >& matrix){
	stringstream ss;
    string str;
    ss << fixed << setprecision(1) << epilson;
    ss >> str;
    replace(str.begin(),str.end(),'.', '_');
    ifstream head("fire-data.csv");
    string ins;
    getline(head,ins);
    head.close();

    stringstream all(ins);
    string token;
    while(getline(all, token, ',')) {
        all_name.push_back(token);
    }

    ofstream numout ( str + ".csv");
    numout << ins << endl;
    set<string> loc_minmax;
    for(map<string, pair<double, double> >::iterator it = json2.begin(); it != json2.end(); ++it) {
        loc_minmax.insert(it->first);
    }

    for (int i=0; i<matrix.size(); i++){
        vector<int> temp = matrix[i];
        for (int t = 0; t < temp.size(); t++){
            string temp_name;
            temp_name = all_name[t];
            set<string>::iterator find_n = find(loc_minmax.begin(), loc_minmax.end(), temp_name);
            set<string>::iterator find_small = find(integer_small.begin(), integer_small.end(), temp_name);
            if (find_n == loc_minmax.end() && find_small == integer_small.end()) {
                numout << int (temp[t]);
            }
            else {                
                if (find_n == loc_minmax.end() && find_small != integer_small.end()) {
                    numout << int (temp[t]+1);
                }
                else {

                    pair<double, double> pair_mm;
                    for(map<string, pair<double, double> >::iterator it = json2.begin(); it != json2.end(); ++it) {
                        if (it->first == temp_name) pair_mm = it->second;
                    }
                    if (pair_mm.first-int(pair_mm.first) == 0){
                        
                        if (temp[t] == 0) numout << "";
                        else {
                            int min, max, final;
                            double gap, gap_i;
                            min = int (pair_mm.first);
                            max = int (pair_mm.second);
                            gap = ((double)(max-min))/(double) hierarchy_y;
                            gap_i = gap * ((double)rand() / RAND_MAX);
                            
                            if (gap_i != 0){
                                final = int(min + gap * temp[t] + gap_i);
                            }
                            else final = int(min + gap * temp[t]);
                            numout << final;
                            if (final<0) {
                                cout<<"find_int_minmax"<<temp_name<<endl;
                                cout<<"min"<<min<<endl;
                                cout<<"max"<<max<<endl;
                            }
                        }   
                    }
                    else{

                        
                        double min, max, final, gap;
                        double gap_i;
                        if (pair_mm.first >0){
                            min = pair_mm.first;
                            max = pair_mm.second;
                            gap = (max-min)/ hierarchy_y;
                            gap_i = (gap * ((double)rand() / RAND_MAX)) ;
                            
                            if (gap_i != 0){
                                final = min + gap * temp[t]+ gap_i;
                            }
                            else {
                                final = min + gap * temp[t];
                            }
                            numout <<fixed << setprecision(13)<< final;
                        }
                        else {
                            min = pair_mm.first;
                            max = pair_mm.second;
                            gap = (max-min)/ hierarchy_y;
                            gap_i = (gap * ((double)rand() / RAND_MAX)) ;
                            if (gap_i != 0){
                                final = min + gap * temp[t]+ gap_i;
                            }
                            else {
                                final = min + gap * temp[t];
                            }
                            numout <<fixed << setprecision(12)<< final;
                        }
                    }   
                }
            }
            if (t < temp.size()-1) numout<<',';
        }
        numout<<endl;
}
    numout.close();
}

