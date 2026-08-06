
#include "lib_menu.h"
#include <iostream>

MenuItem::MenuItem(const std::string& name, int ind) : name(name), ind(ind) {}
MenuItem::~MenuItem() {}

void MenuItem::set_id(const int id){
    this->id = id;
}

void MenuItem::set_id_m(const int id_m){
    this->id_m = id_m;
}

void MenuItem::set_id_mx(const int id_mx){
    this->id_mx = id_mx;
}

void MenuItem::set_ind(const int ind){
    this->ind = ind;
}

int MenuItem::get_id_m() const {
    return this->id_m;
}

Menu::~Menu() = default;
void Menu::addItem(const MenuItem& item) {
    items.push_back(item);
}
void Menu::setTitle(const std::string title){
    this->title = title;
}

void Menu::display() const {} 

