#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <list>
#include <vector>
#include "Items.h"
#include "Spells.h"
#include "Living.h"
#include "Grid.h"
#include "Inventory.h"
#include "Market.h"

using namespace std;
/*--------------ITEMS---------------------------*/
/*-------------CLASS ITEM----------------------------*/
Item::Item(string n,int b_v,int m_l)
{
	this->name=n;
	this->buy_value=b_v;
	this->minimum_level=m_l;
}
string Item::get_name(void)
{
	return this->name;
}
int Item::get_bv(void)
{
	return this->buy_value;
}
int Item::get_ml(void)
{
	return this->minimum_level;
}
/*------------CLASS WEAPON-----------------------------*/
Weapon::Weapon(string n,int b_v,int m_l,int d,bool o_h):Item(n,b_v,m_l)
{
	this->damage=d;
	this->one_hand=o_h;
}
int Weapon::get_dmg(void)
{
	return this->damage;
}
string Weapon::get_h(void)
{
	if(this->one_hand) return "One";
	else return "Two";
}
/*-----------CLASS ARMOR------------------------------*/
Armor::Armor(string n,int b_v,int m_l,int a):Item(n,b_v,m_l)
{
	this->armor=a;
}
int Armor::get_ar(void)
{
	return this->armor;
}
/*----------CLASS POTION-------------------------------*/
Potion::Potion(string n,int b_v,int m_l,int p,string s):Item(n,b_v,m_l)
{
	this->plus=p;
	this->stat=s;
}
string Potion::get_stat(void)
{
	return this->stat;
}
int Potion::get_plus(void)
{
	return this->plus;
}
/*-----------------------------------------*/
