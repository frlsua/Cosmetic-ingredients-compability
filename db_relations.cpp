#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>
#include <set>
#include <map>
#include <iomanip>
#include <cmath>
#include "classes.h"
#include "db_function.h"
#include <chrono>

using namespace std;
using namespace chrono;

bool find_conflict(map <int, vector<pair<int, int>>> lst_conflict, int cat1, int cat2){
    auto is_there = lst_conflict.find(cat1);
    if (is_there != lst_conflict.end()){
        auto vect = is_there->second;
        for (int i = 0; i < vect.size(); i++){
            if (vect[i].first == cat2) return true;
        }
    }
    return false;
}

void write_rel_db(vector <relation> relations){
    ofstream fout("relation.txt");
    int i = 1;
    string separator = "; ";

    if (fout.is_open()){
        fout << "ID Ingredient1_ID Ingredient2_ID Relation_Type (Exactly, Restricted, Banned)" << endl;
        for (int ind = 0; ind < relations.size(); ind++){
            relation rel = relations[ind];
            fout << i << separator << rel.get_ing1_id() << separator << rel.get_ing2_id() << separator << rel.get_type() << endl;
            i++;
            fout << i << separator << rel.get_ing2_id() << separator << rel.get_ing1_id() << separator << rel.get_type() << endl;
            i++;
        }
    }
    relations.clear();
    fout.close();
}

int main(){
    vector<ingredient> lst_ingredients = read_ing_db("ingredients.txt", ";");
    vector<category> lst_categories = read_cat_db("category.txt", ";");
    map <int, vector<pair<int, int>>> lst_conflict = read_conf_db("conflict.txt", ";");
    vector <relation> lst_relations;
    relation rel;
    double r0 = 6.5, ra, RED, hsr1_d, hsr1_p, hsr1_h, hsr2_d, hsr2_p, hsr2_h;
    int cat1_id, cat2_id, ing1_id, ing2_id, rel_id = 1;

    ingredient ing1, ing2; 
    for (int ind = 0; ind < lst_ingredients.size() - 1; ind++){
        ing1 = lst_ingredients[ind];
        if (ing1.get_toxicity() == 3) continue;
        ing1.get_hsr(hsr1_d, hsr1_p, hsr1_h);
        cat1_id = ing1.get_cat();
        ing1_id = ing1.get_id();

        for (int ind1 = ind + 1; ind1 < lst_ingredients.size(); ind1++){
            ing2 = lst_ingredients[ind1];
            if (ing2.get_toxicity() == 3) continue;
            ing2.get_hsr(hsr2_d, hsr2_p, hsr2_h);
            cat2_id = ing2.get_cat();
            ing2_id = ing2.get_id();

            ra = sqrt(4 * pow(hsr1_d - hsr2_d, 2) + pow(hsr1_p - hsr2_p, 2) + pow(hsr1_h - hsr2_h, 2));
            RED = ra / r0;

            if (RED > 1.05) rel.set(rel_id, ing1_id, ing2_id, "Banned");
            else if (find_conflict(lst_conflict, cat1_id, cat2_id)) rel.set(rel_id, ing1_id, ing2_id, "Banned");
            else if (RED >= 0 && RED <= 0.95) rel.set(rel_id, ing1_id, ing2_id, "Exactly");
            else rel.set(rel_id, ing1_id, ing2_id, "Restricted");

            lst_relations.push_back(rel);
            rel_id++;
        }
    }
    write_rel_db(lst_relations);
    cout << "DB with relations has done!";
    return 0;
}