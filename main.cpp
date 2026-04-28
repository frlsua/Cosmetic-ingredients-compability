#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>
#include <set>
#include <map>
#include <iomanip>
#include "classes.h"
#include "db_function.h"
#include <chrono>

using namespace std;
using namespace chrono;

int menu(){
    char choice_x;
    cout << "Welcome to the Program!\n";
    cout << "0. Exit\n" << "1. Examine one list of ingredients\n" << "2. Examine two lists of ingredients\n";
    cout << "Choose the function's number: ";
    cin >> choice_x;
    int choice = choice_x - '0';

    while (choice < 0 || choice > 2){
        cout << "\nWrong number. Try again\n\nChoose the function's number: ";
        cin >> choice_x;
        choice = choice_x - '0';
    }
    return choice;
}

int binary_search(string name, vector<ingredient> db){
    int left = 0, right = db.size() - 1;
    name = to_lower(name);

    while (left < right){
        int middle = (right + left)/2;
        string ing_name = to_lower(db[middle].get_name());

        if (name < ing_name){
            right = middle - 1;
        } else if (name > ing_name){
            left = middle + 1;
        } else {
            return middle;
        }
    }
    if (name == to_lower(db[right].get_name())) return right;
    else return -1;
}

void output_analyze_toxicity(string name_vector, set <string> set_ing){
    if (set_ing.empty()) return;

    cout << name_vector << ": ";
    for (auto i = set_ing.begin(); i != set_ing.end(); i++){
        cout << *i << "; ";
    }
    cout << endl;
    return;
}

void analyze_ingredients(string line, vector<ingredient> db, set <string> &queue, set <string> &unknown, set <string> &safe,
    set <string> &restrict, set <string> &forbid, map <int, string> &full_list_ing){
    
    vector <string> lst = divide_str(line, ",");
    int position, toxicity;

    for (int i = 0; i < lst.size(); i++){
        if (lst[i] == "") continue;
        position = binary_search(lst[i], db);
        if (position == -1){
            queue.insert(lst[i]);
            continue;
        }
        switch (db[position].get_toxicity()){
            case 0:
                unknown.insert(lst[i]);
                break;
            case 1:
                safe.insert(db[position].get_name());
                full_list_ing[db[position].get_id()] = db[position].get_name();
                break;
            case 2:
                restrict.insert(db[position].get_name());
                full_list_ing[db[position].get_id()] = db[position].get_name();
                break;
            case 3:
                forbid.insert(db[position].get_name());
                break;
            default:
                cout << "Error in analyze function. Wrong num in toxicity\n";
                break;
        }
    }

    output_analyze_toxicity("Safe", safe);
    output_analyze_toxicity("Restricted", restrict);
    output_analyze_toxicity("Forbidden", forbid);
    output_analyze_toxicity("In need to clarify", unknown);
    output_analyze_toxicity("Unknown", queue);
    return;
}

void inner_analyze (map <int, string> id_to_name, map <int, vector<pair<int, string>>> rel, 
    vector <pair<string, string>> &worst_rel, vector <pair<string, string>> &caut_rel){
    
    set<int> ing_set;
    set<pair<int, int>> added;
    for (auto i = id_to_name.begin(); i != id_to_name.end(); i++){
        ing_set.insert(i->first);
    }

    for (auto i = id_to_name.begin(); i != id_to_name.end(); i++){
        int ing1_id = i->first; string name = i->second;
        vector <pair<int, string>> next_ing = rel.at(ing1_id);
        for (int j = 0; j < next_ing.size(); j++){
            int ing2_id = next_ing[j].first; string rel_type = next_ing[j].second;
            if (ing1_id >= ing2_id) continue;
            if (ing_set.count(ing2_id) && added.count({ing1_id, ing2_id}) == 0) {
                if (rel_type == "Banned") {
                    worst_rel.push_back({name, id_to_name[ing2_id]});
                    added.insert({ing1_id, ing2_id});
                    added.insert({ing2_id, ing1_id});
                } else if (rel_type == "Restricted"){
                    caut_rel.push_back({name, id_to_name[ing2_id]});
                    added.insert({ing1_id, ing2_id});
                    added.insert({ing2_id, ing1_id});
                }
            }
        }
        added.clear();
    }
    return;
}

void inner_outer_analyze_output(vector <pair<string, string>> worst_rel, vector <pair<string, string>> caut_rel){
    if (worst_rel.size() != 0){
        cout << "The worst combination: ";
        for (int i = 0; i < worst_rel.size(); i++){
            cout << worst_rel[i].first << " + " << worst_rel[i].second;
            if (i != worst_rel.size() - 1) cout << ", ";
        }
        cout << endl;
    }
    if (caut_rel.size() != 0){
        cout << "\"Be cautious\" combination: ";
        for (int i = 0; i < caut_rel.size(); i++){
            cout << caut_rel[i].first << " + " << caut_rel[i].second;
            if (i != caut_rel.size() - 1) cout << ", ";
        }
        cout << endl;
    }
    return;
}

void remove_duplicates(map <int, string> &ing_list1, map <int, string> &ing_list2){
    if (ing_list1.size() < ing_list2.size()){
        for(auto i = ing_list1.begin(); i != ing_list1.end(); i++){
            ing_list2.erase(i->first);
        }
    } else {
        auto elem = ing_list2.begin();
        while (elem != ing_list2.end()){
            if (ing_list1.find(elem->first) != ing_list1.end()){
                elem = ing_list2.erase(elem);
            } else {
                elem++;
            }
        }
    }
}

void outer_analyze(map <int, string> &ing_list1, map <int, string> &ing_list2, map <int, vector<pair<int, string>>> rel,
    vector <pair<string, string>> &worst_rel, vector <pair<string, string>> &caut_rel){
    set<pair<int, int>> added;

    for (auto i = ing_list1.begin(); i != ing_list1.end(); i++){
        if (rel.find(i->first) == rel.end()) continue;
        vector <pair<int, string>> next_ing = rel.at(i->first);
        for (int j = 0; j < next_ing.size(); j++){
            if (ing_list2.find(next_ing[j].first) == ing_list2.end()) continue;
            if (added.count({i->first, next_ing[j].first})) continue;
            if (next_ing[j].second == "Banned"){
                worst_rel.push_back({i->second, ing_list2.at(next_ing[j].first)});
            } else if (next_ing[j].second == "Restricted") {
                caut_rel.push_back({i->second, ing_list2.at(next_ing[j].first)});
            }
        }
    }
    return;
}

bool enter_list(string count, string &user_enter, string answer){
    cout << "Enter the" << count << "list of ingredients (divide with the symbol - \",\"): ";
    if (count == " first " || count == " ") cin.ignore();
    getline(cin, user_enter);
    if (user_enter.find(",") == string::npos) {
        cout << "Have you entered only one ingredient? [yes/no] ";
        cin >> answer;
        if (answer == "no" || answer != "yes"){
            cout << "Wrong enter. Try again\n\n";
            return false;
        }
    }
    return true;
}

int main(){
    string user_enter, user_enter1, answer;
    vector <ingredient> ingredients = read_ing_db("ingredients.txt", ";");
    map<int, vector<pair<int, string>>> relations = read_rel_db("relation.txt", ";", ingredients);

    set <string> queue_to_db, cur_queue, unknown, safe, restrict, forbid;
    set <string> whole_list_ing;
    map <int, string> full_list_ing_1, full_list_ing_2;
    vector <pair<string, string>> worst_relations, cautious_relations;

    while (true){
        switch (menu()){
            case 0:
                cout << "\nGoodbye!";
                if (queue_to_db.size() != 0) set_queue_in_db(queue_to_db);
                return 0;
            case 1:
                if (!enter_list(" ", user_enter, answer)){
                    break;
                }
                analyze_ingredients(user_enter, ingredients, queue_to_db, unknown, safe, restrict, forbid, full_list_ing_1);
                inner_analyze(full_list_ing_1, relations, worst_relations, cautious_relations);
                if (worst_relations.size() == 0 && cautious_relations.size() == 0 && (safe.size() + restrict.size() > 1)){
                    cout << "All ingredient combinations are safe!\n";
                } else {
                    inner_outer_analyze_output(worst_relations, cautious_relations);
                }
                cout << endl;
                
                unknown = {}; safe = {}; restrict = {}; forbid = {}, whole_list_ing = {};
                full_list_ing_1 = {}; worst_relations = {}; cautious_relations = {};
                break;
            case 2:
                if (!enter_list(" first ", user_enter, answer)) break;
                if (!enter_list(" second ", user_enter1, answer)) break;
                
                cout << "-- Information about List 1 --\n";
                analyze_ingredients(user_enter, ingredients, cur_queue, unknown, safe, restrict, forbid, full_list_ing_1);
                inner_analyze(full_list_ing_1, relations, worst_relations, cautious_relations);
                if (worst_relations.size() == 0 && cautious_relations.size() == 0 && (safe.size() > 1 || restrict.size() > 1)){
                    cout << "All ingredient combinations are safe!\n";
                } else {
                    inner_outer_analyze_output(worst_relations, cautious_relations);
                }
                queue_to_db.insert(cur_queue.begin(), cur_queue.end()); cur_queue = {};
                unknown = {}; safe = {}; restrict = {}; forbid = {}, whole_list_ing = {};
                worst_relations = {}; cautious_relations = {};

                cout << "\n\n-- Information about List 2 --\n";
                analyze_ingredients(user_enter1, ingredients, cur_queue, unknown, safe, restrict, forbid, full_list_ing_2);
                inner_analyze(full_list_ing_2, relations, worst_relations, cautious_relations);
                if (worst_relations.size() == 0 && cautious_relations.size() == 0 && (safe.size() > 1 || restrict.size() > 1)){
                    cout << "All ingredient combinations are safe!\n";
                } else {
                    inner_outer_analyze_output(worst_relations, cautious_relations);
                }
                queue_to_db.insert(cur_queue.begin(), cur_queue.end()); cur_queue = {};
                unknown = {}; safe = {}; restrict = {}; forbid = {}, whole_list_ing = {};
                worst_relations = {}; cautious_relations = {};
                
                cout << "\n\n-- Information about List 1 & List 2 --\n";
                remove_duplicates(full_list_ing_1, full_list_ing_2);
                if (full_list_ing_2.size() == 0){
                    cout << "List 2 has the same ingredients as the List 1\n";
                    break;
                }
                outer_analyze(full_list_ing_1, full_list_ing_2, relations, worst_relations, cautious_relations);
                if (worst_relations.size() == 0 && cautious_relations.size() == 0 && (safe.size() > 1 || restrict.size() > 1)){
                    cout << "All ingredient combinations are safe!\n";
                } else {
                    inner_outer_analyze_output(worst_relations, cautious_relations);
                }

                full_list_ing_1 = {}; full_list_ing_2 = {}; worst_relations = {}; cautious_relations = {};
                break;
            default:
                cout << "Error in menu function\n";
                break;
        }
        cout << "\nDo you want to continue? [yes/no] ";
        cin >> answer;
        if (to_lower(answer) != "yes" || to_lower(answer) == "no"){
            if (queue_to_db.size() != 0) set_queue_in_db(queue_to_db);
            cout << "\nGoodbye!";
            return 0;
        }
    }
    return 0;
}