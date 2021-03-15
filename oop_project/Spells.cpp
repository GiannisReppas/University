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
/*-----------------SPELLS------------------------*/
/*------------CLASS SPELL-----------------------------*/
Spell::Spell(string n,int b_v,int m_l,int d,int m)
{
	this->name=n;
	this->buy_value=b_v;
	this->minimum_level=m_l;
	this->damage=d;
	this->mana_cost=m;
}
string Spell::get_name(void)
{
	return this->name;
}
int Spell::get_bv(void)
{
	return this->buy_value;
}
int Spell::get_ml(void)
{
	return this->minimum_level;
}
int Spell::get_dmg(void)
{
	return this->damage;
}
int Spell::get_manac(void)
{
	return this->mana_cost;
}
/*------------CLASS ICE SPELL-----------------------------*/
IceSpell::IceSpell(string n,int b_v,int m_l,int d,int m,int a,float p):Spell(n,b_v,m_l,d,m)
{
	this->percent=p;
	this->affects=a;
}
int IceSpell::get_rounds(void)
{
	return this->affects;
}
float IceSpell::get_p(void)
{
	return this->percent;
}
/*------------CLASS FIRE SPELL-----------------------------*/
FireSpell::FireSpell(string n,int b_v,int m_l,int d,int m,int a,float p):Spell(n,b_v,m_l,d,m)
{
	this->percent=p;
	this->affects=a;
}

int FireSpell::get_rounds(void)
{
	return this->affects;
}
float FireSpell::get_p(void)
{
	return this->percent;
}
/*------------CLASS LIGHTNING SPELL-----------------------------*/
LightningSpell::LightningSpell(string n,int b_v,int m_l,int d,int m,int a,float p):Spell(n,b_v,m_l,d,m)
{
	this->percent=p;
	this->affects=a;
}
int LightningSpell::get_rounds(void)
{
	return this->affects;
}
float LightningSpell::get_p(void)
{
	return this->percent;
}
/*-----------------------------------------*/
