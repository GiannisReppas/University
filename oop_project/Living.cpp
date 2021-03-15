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
/*---------------CLASS LIVING--------------------------*/
Living::Living(string n,int l,int hP)
{
	this->name=n;
	this->level=l;
	this->healthPower=hP;
	this->maxhealthPower=hP;
}
string Living::get_name(void)
{
	return this->name;
}
int Living::get_level(void)
{
	return this->level;
}
int Living::get_hP(void)
{
	return this->healthPower;
}
void Living::set_hP(int a)
{
	this->healthPower=a;
}
int Living::get_maxhP(void)
{
	return this->maxhealthPower;
}
/*-----------------------------------------*/

/*--------------------HEROS---------------------*/
/*------------------CLASS HERO-----------------------*/
Hero::Hero(string n):Living(n,1,500)
{
	this->cl="hero";
	this->magicPower=200;
	this->maxmagicPower=200;
	this->strength=10;
	this->dexterity=10;
	this->agility=10;
	this->money=1000;
	this->experience=0;
	this->max_experience=100;
}
int Hero::get_money(void)
{
	return this->money;
}
void Hero::set_money(int a)
{
	this->money=a;
}
int Hero::get_mP(void)
{
	return this->magicPower;
}
void Hero::set_mP(int a)
{
	this->magicPower=a;
}
float Hero::get_strength(void)
{
	return this->strength;
}
void Hero::set_strength(int a)
{
	this->strength=a;
}
float Hero::get_agility(void)
{
	return this->agility;
}
void Hero::set_agility(int a)
{
	this->agility=a;
}
float Hero::get_dexterity(void)
{
	return this->dexterity;
}
void Hero::set_dexterity(int a)
{
	this->dexterity=a;
}
int Hero::get_maxmP(void)
{
	return this->maxmagicPower;
}
int Hero::get_exp(void)
{
	return this->experience;
}
void Hero::set_exp(int a)
{
	this->experience=a;
}
int Hero::get_maxexp(void)
{
	return this->max_experience;
}
void Hero::levelUp(void)
{
	if(this->cl=="warrior")
	{
		this->level++;
		this->experience=0;
		this->max_experience*=2;
		this->strength+=(this->strength)*(7.5/100.0);
		this->agility+=(this->agility)*(7.5/100.0);
		this->dexterity+=(this->dexterity)*(5/100.0);
		this->healthPower+=250;
		this->maxhealthPower+=250;
		this->magicPower+=100;
		this->maxmagicPower+=100;
	}
	else if(this->cl=="sorcerer")
	{
		this->level++;
		this->experience=0;
		this->max_experience*=2;
		this->strength+=(this->strength)*(5/100.0);
		this->agility+=(this->agility)*(7.5/100.0);
		this->dexterity+=(this->dexterity)*(7.5/100.0);
		this->healthPower+=250;
		this->maxhealthPower+=250;
		this->magicPower+=100;
		this->maxmagicPower+=100;
	}
	else if(this->cl=="paladin")
	{
		this->level++;
		this->experience=0;
		this->max_experience*=2;
		this->strength+=(this->strength)*(7.5/100.0);
		this->agility+=(this->agility)*(5/100.0);
		this->dexterity+=(this->dexterity)*(7.5/100.0);
		this->healthPower+=250;
		this->maxhealthPower+=250;
		this->magicPower+=100;
		this->maxmagicPower+=100;
	}
}
int Hero::get_dmg_of_weapons(void)
{
	int i;
	int sz = this->using_W.size();
	int total=0;
	
	for(i=0; i < sz; i++)
	{
		total+=this->using_W.at(i).get_dmg();
	}

	return total;
}
int Hero::get_arm_of_armors(void)
{
	int i;
	int sz = this->using_A.size();
	int total=0;
	
	for(i=0; i < sz; i++)
	{
		total+=this->using_A.at(i).get_ar();
	}

	return total;
}

/*----------------CLASS WARRIOR-------------------------*/
Warrior::Warrior(string n):Hero(n)
{
	this->cl="warrior";
	this->strength+=5;
	this->agility+=5;
}
/*---------------CLASS SORCERER--------------------------*/
Sorcerer::Sorcerer(string n):Hero(n)
{
	this->cl="sorcerer";
	this->dexterity+=5;
	this->agility+=5;
}
/*---------------CLASS PALADIN--------------------------*/
Paladin::Paladin(string n):Hero(n)
{
	this->cl="paladin";
	this->strength+=5;
	this->dexterity+=5;
}
/*-----------------------------------------*/

/*-----------------MONSTERS------------------------*/
/*---------------CLASS MONSTER--------------------------*/
Monster::Monster(string n,int l,int hP):Living(n,l,hP)
{
	this->damage=this->level * 10;
	this->defense=this->level * 10;
	this->escape=1;
}
/*---------------CLASS DRAGON--------------------------*/
Dragon::Dragon(string n,int l,int hP):Monster(n,l,hP)
{
	this->affected_for[0]=0;
	this->affected_for[1]=0;
	this->affected_for[2]=0;
	this->change[0]=1.0;
	this->change[1]=1.0;
	this->change[2]=1.0;
	this->damage+=this->level * 5;
}
/*----------------EXOSKELETON-------------------------*/
Exoskeleton::Exoskeleton(string n,int l,int hP):Monster(n,l,hP)
{
	this->affected_for[0]=0;
	this->affected_for[1]=0;
	this->affected_for[2]=0;
	this->change[0]=1.0;
	this->change[1]=1.0;
	this->change[2]=1.0;
	this->defense+=this->level * 5;
}
/*---------------SPIRIT--------------------------*/
Spirit::Spirit(string n,int l,int hP):Monster(n,l,hP)
{
	this->affected_for[0]=0;
	this->affected_for[1]=0;
	this->affected_for[2]=0;
	this->change[0]=1.0;
	this->change[1]=1.0;
	this->change[2]=1.0;
	this->escape=5;
}
/*-----------------------------------------*/
