#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <list>
#include <vector>
using namespace std;
/*---------------CLASS LIVING--------------------------*/
class Living
{
protected:
	string name;
	int level;
	int healthPower; // current hP
	int maxhealthPower; // maximum hP
public:
	//constructor-sets values
	Living(string,int,int);
	//getters
	string get_name();
	int get_level();
	int get_hP();
	void set_hP(int);
	int get_maxhP();
};
/*-----------------------------------------*/

/*--------------------HEROS---------------------*/
/*------------------CLASS HERO-----------------------*/
class Hero:public Living
{
public:
	vector<Weapon> using_W; //the weapons the hero holds
	vector<Armor> using_A; // the armor he wears
	vector<IceSpell> using_IS; // the ice spells he can use
	vector<FireSpell> using_FS; // the fire spells he can use
	vector<LightningSpell> using_LS; // the lightning spells he can use
protected:
	int magicPower; //current magic Power
	int maxmagicPower; //maximum magic Power
	
	float strength; //damage weapons
	float dexterity; //damage spells 
	float agility; // escape from an attack
	
	int money;
	int experience;
	int max_experience;
	string cl;
public:
	//contructor-sets values
	Hero(string);
	//getters-setters
	int get_money();
	void set_money(int a);
	int get_mP();
	void set_mP(int);
	float get_strength();
	void set_strength(int);
	float get_agility();
	void set_agility(int);
	float get_dexterity();
	void set_dexterity(int);
	int get_maxmP();
	int get_exp();
	void set_exp(int);
	int get_maxexp();
	// function for leveling up-different for everyry this->cl value
	void levelUp();
	// get the damage the hero does with his wepons only
	int get_dmg_of_weapons();
	// get the defense the hero has from his armor
	int get_arm_of_armors();
};
/*----------------CLASS WARRIOR-------------------------*/
class Warrior:public Hero
{
	// better strength and agility
public:
	Warrior(string);
};
/*---------------CLASS SORCERER--------------------------*/
class Sorcerer:public Hero
{
	// better dexterity and agility
public:
	Sorcerer(string);
};
/*---------------CLASS PALADIN--------------------------*/
class Paladin:public Hero
{
	// better strength and dexterity
public:
	Paladin(string);
};
/*-----------------------------------------*/

/*-----------------MONSTERS------------------------*/
/*---------------CLASS MONSTER--------------------------*/
class Monster:public Living
{
protected:
	int damage;
	int defense;
	int escape; // chance to dodge an attack
public:
	// constructor-sets values
	Monster(string,int,int);
	// getters
	int get_escape(){
		return escape;
	}
	int get_dmg(){
		return damage;
	}
	int get_def(){
		return defense;
	}
};
/*---------------CLASS DRAGON--------------------------*/
class Dragon:public Monster
{
	//better damage
	// rounds that it will be affected if a spells damages it (3 spells)
	int affected_for[3];
	// how much it will be affected ( 3 different spells)
	float change[3];
public:
	Dragon(string,int,int);
};
/*----------------EXOSKELETON-------------------------*/
class Exoskeleton:public Monster
{
	//better defense
	// rounds that it will be affected if a spells damages it (3 spells)
	int affected_for[3];
	// how much it will be affected ( 3 different spells)
	float change[3];
public:
	Exoskeleton(string,int,int);
};
/*---------------SPIRIT--------------------------*/
class Spirit:public Monster
{
	//better chance to dodge an attack
	// rounds that it will be affected if a spells damages it (3 spells)
	int affected_for[3];
	// how much it will be affected ( 3 different spells)
	float change[3];
public:
	Spirit(string,int,int);
};
