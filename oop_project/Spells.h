#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <list>
#include <vector>
using namespace std;
/*-----------------SPELLS------------------------*/
/*------------CLASS SPELL-----------------------------*/
class Spell
{
	string name;
	int buy_value;
	int minimum_level;
	int damage; // depends on the dexterity of the hero
	int mana_cost; // the amount of mana that will be lost from the hero
public:
	//constructor-sets value
	Spell(string,int,int,int,int);
	//getters
	string get_name();
	int get_bv();
	int get_ml();
	int get_dmg();
	int get_manac();
};
/*------------CLASS ICE SPELL-----------------------------*/
class IceSpell:public Spell
{
	//low damage for some rounds
	int affects; //rounds
	float percent;
public:
	//contrustor-sets values
	IceSpell(string,int,int,int,int,int,float);
	//getters
	int get_rounds();
	float get_p();
};
/*------------CLASS FIRE SPELL-----------------------------*/
class FireSpell:public Spell
{
	//low defense for some rounds
	int affects;// rounds
	float percent;
public:
	//construcor-sets values
	FireSpell(string,int,int,int,int,int,float);
	//getters
	int get_rounds();
	float get_p();
};
/*------------CLASS LIGHTNING SPELL-----------------------------*/
class LightningSpell:public Spell
{
	//low percent to dodge an attack
	int affects; //round
	float percent;
public:
	//constructor-sets values
	LightningSpell(string,int,int,int,int,int,float);
	//getters
	int get_rounds();
	float get_p();
};
