#pragma once
#include <iostream>
#include <string>

using namespace std;

class ingredient{
private:
    int ingredient_id;
    string name;
    string cas_code;
    int category_id;
    int toxicity = 0;
    int hsr_d, hsr_p, hsr_h;
    
public:
    bool check(){
        if (ingredient_id <= 0 || name == "" || cas_code == "" || category_id <= 0 || toxicity <= 0 || toxicity >= 4 || hsr_d < 0 || hsr_p < 0 || hsr_h < 0) return false;
        return true;
    }
    void set(int ing_id, string ing_name, string cas, int cat_id, int toxic, int hsrd, int hsrp, int hsrh){
        ingredient_id = ing_id; name = ing_name; cas_code = cas;
        category_id = cat_id; toxicity = toxic; hsr_d = hsrd; hsr_p = hsrp; hsr_h = hsrh;
    }
    void print(){
        cout << "Ingredient " << ingredient_id << ": " << name << endl;
        cout << "CAS: " << cas_code << endl;
        cout << "Category: " << category_id << endl;
        cout << "Toxicity: " << toxicity << endl;
        cout << "HSR: " << hsr_d << " " << hsr_p << " " << hsr_h << endl;
    }

    string get_name(){ return name; }
    int get_toxicity() {return toxicity; }
    int get_id() { return ingredient_id; }
    int get_cat() { return category_id; }
    void get_hsr(double &hsrd, double &hsrp, double &hsrh) { hsrd = hsr_d; hsrp = hsr_p; hsrh = hsr_h; }
};

class relation{
private:
    int relation_id;
    int ingredient_1_id;
    int ingredient_2_id;
    string relation_type;
public:
    void set(int rel_id, int ing_1_id, int ing_2_id, string rel_type){
        relation_id = rel_id; ingredient_1_id = ing_1_id;
        ingredient_2_id = ing_2_id; relation_type = rel_type;
    }
    void print(){
        cout << "Relation " << relation_id << endl;
        cout << "Ingredient 1: " << ingredient_1_id << endl;
        cout << "Ingredient 1: " << ingredient_2_id << endl;
        cout << "Relation type: " << relation_type << endl;
    }
    bool check(){
        if (relation_id <= 0 || ingredient_1_id <= 0 || ingredient_2_id <= 0 || relation_type == ""){ return false;}
        return true;
    }
    int get_id(){ return relation_id; }
    int get_ing1_id() { return ingredient_1_id; }
    int get_ing2_id() { return ingredient_2_id; }
    string get_type() { return relation_type; }
};

class category{
private:
    int category_id;
    string category_name;
    bool has_conflict;
public:
    void set(int cat_id, string cat_name, bool conf){
        category_id = cat_id; category_name = cat_name; has_conflict = conf;
    }
    void print(){
        cout << "Category " << category_id << ": " << category_name << endl;
        cout << "Has conflict: " << has_conflict << endl;
    }
    bool check(){
        if (category_id <= 0 || category_name == ""){
            return false;
        }
        return true;
    }
    bool get_has_conflict(){ return has_conflict; }
};

class conflict{
private:
    int conflict_id;
    int categ1_id;
    int categ2_id;
    int conflict_type;
public:
    void set(int id, int cat1, int cat2, int conf_type){
        conflict_id = id; categ1_id = cat1; categ2_id = cat2; conflict_type = conf_type;
    }
    bool check(){
        if (conflict_id <= 0 || categ1_id <= 0 || categ2_id <= 0 || conflict_type <= 0 || conflict_type >= 8){
            return false;
        }
        return true;
    }
    int get_conflict_type(){
        return conflict_type;
    }
};