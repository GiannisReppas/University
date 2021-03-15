#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <list>
#include <vector>
using namespace std;
/*--------------ITEMS---------------------------*/
/*-------------CLASS ITEM----------------------------*/
class Item
{
	string name;
	int buy_value;
	int minimum_level;
public:
	//constructor-sets values
	Item(string,int,int);
	//getters
	string get_name();
	int get_bv();
	int get_ml();
};
/*------------CLASS WEAPON-----------------------------*/
class Weapon:public Item
{
	int damage;
	bool one_hand;
public:
	//constructor-sets values
	Weapon(string,int,int,int,bool);
	//getters
	int get_dmg();
	string get_h();
};
/*-----------CLASS ARMOR------------------------------*/
class Armor:public Item
{
	int armor;
public:
	//constructor
	Armor(string,int,int,int);
	//getter
	int get_ar();
};
/*----------CLASS POTION-------------------------------*/
class Potion:public Item
{
	//how much the stat will increase
	int plus;
	//whats the stat that will be increased
	string stat;
public:
	//constructor-sets values
	Potion(string,int,int,int,string);
	//getters
	string get_stat();
	int get_plus();
};
/*-----------------------------------------*/
