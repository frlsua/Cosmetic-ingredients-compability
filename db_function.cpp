#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <set>
#include <iomanip>
#include "classes.h"

using namespace std;

string to_lower(string str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), [](unsigned char c){ return tolower(c); });
    return result;
}

bool cmd (ingredient ing1, ingredient ing2){
    return ing1.get_name() < ing2.get_name();
}

vector <string> divide_str(string str, string divider){
    int prev = 0, next, delta = divider.size() + 1;
    vector <string> lst;

    while ((next = str.find(divider, prev)) != string::npos){
        string sub = str.substr(prev, next - prev);
        prev = next + delta;
        lst.push_back(sub);
    }
    lst.push_back(str.substr(prev, str.size() - prev));
    return lst;
}

vector <ingredient> read_ing_db(string filename, string divider){
    ingredient ing;
    string ing_name, cas; int ing_id, cat_id, toxicity; double hsr_d, hsr_p, hsr_h;
    string line;
    vector <ingredient> list_ing;

    ifstream fin(filename);
    getline(fin, line);
    while (getline(fin, line)){
        vector <string> inf_ing = divide_str(line, divider);
        if (inf_ing.size() != 8) continue;
        ing_id = stoi(inf_ing[0]);
        ing_name = inf_ing[1];
        cas = inf_ing[2];
        cat_id = stoi(inf_ing[3]);
        toxicity = stoi(inf_ing[4]);
        hsr_d = stod(inf_ing[5]);
        hsr_p = stod(inf_ing[6]);
        hsr_h = stod(inf_ing[7]);
        ing.set(ing_id, ing_name, cas, cat_id, toxicity, hsr_d, hsr_p, hsr_h);
        if (ing.check()) list_ing.push_back(ing);
    }
    fin.close();
    sort(list_ing.begin(), list_ing.end(), cmd);
    return list_ing;
}

vector <category> read_cat_db(string filename, string divider){
    category cat;
    string cat_name; int cat_id; bool has_conflict = false;
    string line;
    vector <category> list_cat;

    ifstream fin(filename);
    getline(fin, line);
    while (getline(fin, line)){
        vector <string> inf_cat = divide_str(line, divider);
        if (inf_cat.size() != 3) continue;
        cat_id = stoi(inf_cat[0]);
        cat_name = inf_cat[1];
        if (to_lower(inf_cat[2]) == "true") has_conflict = true;
        cat.set(cat_id, cat_name, has_conflict);
        if (cat.check()) list_cat.push_back(cat);
    }
    fin.close();
    return list_cat;
}

map <int, vector<pair<int, int>>> read_conf_db(string filename, string divider){
    conflict conf;
    int conf_id, cat1_id, cat2_id, conf_type;
    string line;
    map <int, vector<pair <int, int>>> list_conf;

    ifstream fin(filename);
    getline(fin, line);
    while (getline(fin, line)){
        vector <string> inf_conf = divide_str(line, divider);
        if (inf_conf.size() != 4) continue;
        conf_id = stoi(inf_conf[0]);
        cat1_id = stoi(inf_conf[1]);
        cat2_id = stoi(inf_conf[2]);
        conf_type = stoi(inf_conf[3]);
        conf.set(conf_id, cat1_id, cat2_id, conf_type);
        if (conf.check()){
            list_conf[cat1_id].push_back({cat2_id, conf_type});
            list_conf[cat2_id].push_back({cat1_id, conf_type});
        } 
    }
    fin.close();
    return list_conf;
}

map<int, vector<pair<int, string>>> read_rel_db(string filename, string divider, vector <ingredient> ingredients){
    relation rel;
    int rel_id, ing1, ing2; string rel_type;
    string line;
    map <int, vector<pair<int, string>>> list_rel;

    ifstream fin(filename);
    getline(fin, line);
    while (getline(fin, line)){
        vector <string> inf_rel = divide_str(line, divider);
        if (inf_rel.size() != 4) continue;
        rel_id = stoi(inf_rel[0]);
        ing1 = stoi(inf_rel[1]);
        ing2 = stoi(inf_rel[2]);
        rel_type = inf_rel[3];
        rel.set(rel_id, ing1, ing2, rel_type);
        if (rel.check()){
            list_rel[ing1].push_back({ing2, rel_type});
            list_rel[ing2].push_back({ing1, rel_type});
        }
    }
    fin.close();
    return list_rel;
}

void set_queue_in_db (set <string> &queue){
    ofstream fout("check_and_insert_in_db.txt", ios::app);
    if (fout.is_open()){
        for (auto elem : queue){
            fout << elem << endl;
        }
    }
    queue.clear();
    fout.close();
}