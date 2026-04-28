#pragma once
#include <vector>
#include <string>
#include <map>
#include <iomanip>
#include "classes.h"

using namespace std;

string to_lower(string);

vector <string> divide_str(string, string);

vector <ingredient> read_ing_db(string, string);

vector <category> read_cat_db(string, string);

map <int, vector<pair<int, int>>> read_conf_db(string, string);

map<int, vector<pair<int, string>>> read_rel_db(string, string, vector <ingredient>);

void set_queue_in_db (set <string> &);