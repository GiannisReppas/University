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
/*---------------CLASS MARKET---------------------------*/
Market::Market(void)
{
	/*Weapons*/
	Weapon *W;
	W = new Weapon("Doran's_blade",100,1,150,true);
	this->Weapons.push_back(*W);
	delete W;
	W = new Weapon("Long_Sword",200,3,300,false);
	this->Weapons.push_back(*W);
	delete W;
	W = new Weapon("B.F._Sword",200,4,300,true);
	this->Weapons.push_back(*W);
	delete W;
	W = new Weapon("Statik_Shiv",400,6,500,true);
	this->Weapons.push_back(*W);
	delete W;
	W = new Weapon("Phantom_Dancer",600,7,900,false);
	this->Weapons.push_back(*W);
	delete W;
	W = new Weapon("Runa's_Hurricane'",700,9,1000,true);
	this->Weapons.push_back(*W);
	delete W;
	W = new Weapon("Infinity_Blade",1000,11,1500,true);
	this->Weapons.push_back(*W);
	delete W;
	/*Armors*/
	Armor *A;
	A = new Armor("Cloth_Armor",50,1,25);
	this->Armors.push_back(*A);
	delete A;
	A = new Armor("Iron_Armor",100,2,50);
	this->Armors.push_back(*A);
	delete A;
	A = new Armor("Super_Armor",150,3,80);
	this->Armors.push_back(*A);
	delete A;
	A = new Armor("Thornmail",300,6,200);
	this->Armors.push_back(*A);
	delete A;
	A = new Armor("Guardian_Angel",500,8,300);
	this->Armors.push_back(*A);
	delete A;
	A = new Armor("Frozen_Heart",650,10,450);
	this->Armors.push_back(*A);
	delete A;
	/*Potions*/
	Potion *P;
	P = new Potion("Small_Healing_Potion",50,1,25,"healthPower");
	this->Potions.push_back(*P);
	delete P;
	P = new Potion("Small_Mana_Potion",50,1,10,"magicPower");
	this->Potions.push_back(*P);
	delete P;
	P = new Potion("Small_Strength_Potion",100,2,5,"strength");
	this->Potions.push_back(*P);
	delete P;
	P = new Potion("Small_Agility_Potion",100,2,5,"agility");
	this->Potions.push_back(*P);
	delete P;
	P = new Potion("Small_Dexterity_Potion",100,2,5,"dexterity");
	this->Potions.push_back(*P);
	delete P;
	P = new Potion("Big_Healing_Potion",250,6,100,"healthPower");
	this->Potions.push_back(*P);
	delete P;
	P = new Potion("Big_Mana_Potion",250,6,50,"magicPower");
	this->Potions.push_back(*P);
	delete P;
	P = new Potion("Big_Strength_Potion",350,7,10,"strength");
	this->Potions.push_back(*P);
	delete P;
	P = new Potion("Big_Agility_Potion",350,7,10,"agility");
	this->Potions.push_back(*P);
	delete P;
	P = new Potion("Big_Dexterity_Potion",350,7,10,"dexterity");
	this->Potions.push_back(*P);
	delete P;
	/*Ice Spells*/
	IceSpell *IS;
	IS = new IceSpell("Cold_ball",200,1,100,50,2,5);
	this->IceSpells.push_back(*IS);
	delete IS;
	IS = new IceSpell("Super_Ice_Bomb",400,5,300,150,4,7);
	this->IceSpells.push_back(*IS);
	delete IS;
	/*Fire Spells*/
	FireSpell *FS;
	FS = new FireSpell("Fire_ball",200,1,100,50,2,2);
	this->FireSpells.push_back(*FS);
	delete FS;
	FS = new FireSpell("Overheat",400,5,300,150,4,5);
	this->FireSpells.push_back(*FS);
	delete FS;
	/*Lightning Spells*/
	LightningSpell *LS;
	LS = new LightningSpell("Thundershock",200,1,100,0,2,2);
	this->LightningSpells.push_back(*LS);
	delete LS;
	LS = new LightningSpell("Thunderbolt",400,5,300,150,4,5);
	this->LightningSpells.push_back(*LS);
	delete LS;	
}
void Market::Print_Market(void)
{
	int i;
	cout << "----------------------" << endl << "Available Weapons:" << endl << endl;
	for(i=0;i<this->Weapons.size();i++)
	{
		cout << "Weapon's name: " << this->Weapons.at(i).get_name() << endl;
		cout << " Buy value: " << this->Weapons.at(i).get_bv() << endl;
		cout << " Minimum level: " << this->Weapons.at(i).get_ml() << endl;
		cout << " Damage: " << this->Weapons.at(i).get_dmg() << endl;
		cout << " Hands: " << this->Weapons.at(i).get_h() << endl;
		cout << endl << endl;
	}
	cout << "----------------------" << endl << "Available Armors:" << endl << endl;
	for(i=0;i<this->Armors.size();i++)
	{
		cout << "Armor's name: " << this->Armors.at(i).get_name() << endl;
		cout << " Buy value: " << this->Armors.at(i).get_bv() << endl;
		cout << " Minimum level: " << this->Armors.at(i).get_ml() << endl;
		cout << " Armor: " << this->Armors.at(i).get_ar() << endl;
		cout << endl << endl;
	}
	cout << "----------------------" << endl << "Available Potions:" << endl << endl;
	for(i=0;i<this->Potions.size();i++)
	{
		cout << "Potion's name: " << this->Potions.at(i).get_name() << endl;
		cout << " Buy value: " << this->Potions.at(i).get_bv() << endl;
		cout << " Minimum level: " << this->Potions.at(i).get_ml() << endl;
		cout << " Increasing: " << this->Potions.at(i).get_stat() << " for " << this->Potions.at(i).get_plus() << endl;
		cout << endl << endl;
	}
	cout << "----------------------" << endl << "Available Ice Spells:"  << endl << endl;
	for(i=0;i<this->IceSpells.size();i++)
	{
		cout << "Ice Spell's name: " << this->IceSpells.at(i).get_name() << endl;
		cout << " Buy value: " << this->IceSpells.at(i).get_bv() << endl;
		cout << " Minimum level: " << this->IceSpells.at(i).get_ml() << endl;
		cout << " Damage: " << this->IceSpells.at(i).get_dmg() << endl;
		cout << " Mana cost: " << this->IceSpells.at(i).get_manac() << endl;
		cout << " Effects lasting for: " << this->IceSpells.at(i).get_rounds() << " rounds" << endl;
		cout << " Reducing the stat for: " << this->IceSpells.at(i).get_p() << "%" << endl;
		cout << endl << endl;
	}
	cout << "----------------------" << endl << "Available Fire Spells:"  << endl << endl;
	for(i=0;i<this->FireSpells.size();i++)
	{
		cout << "Fire Spell's name: " << this->FireSpells.at(i).get_name() << endl;
		cout << " Buy value: " << this->FireSpells.at(i).get_bv() << endl;
		cout << " Minimum level: " << this->FireSpells.at(i).get_ml() << endl;
		cout << " Damage: " << this->FireSpells.at(i).get_dmg() << endl;
		cout << " Mana cost: " << this->FireSpells.at(i).get_manac() << endl;
		cout << " Effects lasting for: " << this->FireSpells.at(i).get_rounds() << " rounds" << endl;
		cout << " Reducing the stat for: " << this->FireSpells.at(i).get_p() << "%" << endl;
		cout << endl << endl;
	}
	cout << "----------------------" << endl << "Available Lightning Spells:"  << endl << endl;
	for(i=0;i<this->LightningSpells.size();i++)
	{
		cout << "Lightning Spell's name: " << this->LightningSpells.at(i).get_name() << endl;
		cout << " Buy value: " << this->LightningSpells.at(i).get_bv() << endl;
		cout << " Minimum level: " << this->LightningSpells.at(i).get_ml() << endl;
		cout << " Damage: " << this->LightningSpells.at(i).get_dmg() << endl;
		cout << " Mana cost: " << this->LightningSpells.at(i).get_manac() << endl;
		cout << " Effects lasting for: " << this->LightningSpells.at(i).get_rounds() << " rounds" << endl;
		cout << " Reducing the stat for: " << this->LightningSpells.at(i).get_p() << "%" << endl;
		cout << endl << endl;
	}
}
void Market::Buy(Inventory *Inv, Grid *Gr)
{
	this->Print_Market();
	string thing,human;
	cout << "Give the name of the Item or Spell you want to buy: ";
	cin >> thing;
	cout << "Give the name of the Hero that will pay: ";
	cin >> human;
	
	int i,j; bool check=false;
	for(i=0;i<Gr->Heros.size();i++)
	{
		if(Gr->Heros.at(i).get_name()==human)
		{
			check=true;
			j=i;
			break;
		}
	}
	if(check==false)
	{
		cout << "There is no such Hero in the Grid!";
		return;
	}
	
	// takes the weapon the players want and puts it in the inventory
	for(i=0;i<this->Weapons.size();i++)
	{
		if(thing==this->Weapons.at(i).get_name())
		{
			if((Gr->Heros.at(j).get_money()>=this->Weapons.at(i).get_bv())&&(Gr->Heros.at(j).get_level()>=this->Weapons.at(i).get_ml())) // if the hero has enough money && has the minimum level
			{
				Gr->Heros.at(j).set_money(Gr->Heros.at(j).get_money() - this->Weapons.at(i).get_bv());
				Inv->Weapons.push_back(this->Weapons.at(i));
				return;
			}
			else
			{
				cout << "You do not meet the prerequirements!";
				return;
			}
		}
	}
	// takes the armor the players want and puts it in the inventory
	for(i=0;i<this->Armors.size();i++)
	{
		if(thing==this->Armors.at(i).get_name())
		{
			if((Gr->Heros.at(j).get_money()>=this->Armors.at(i).get_bv())&&(Gr->Heros.at(j).get_level()>=this->Armors.at(i).get_ml())) // if the hero has enough money && has the minimum level
			{
				Gr->Heros.at(j).set_money(Gr->Heros.at(j).get_money() - this->Armors.at(i).get_bv());
				Inv->Armors.push_back(this->Armors.at(i));
				return;
			}
			else
			{
				cout << "You do not meet the prerequirements!";
				return;
			}
		}
	}
	// takes the potion the players want and puts it in the inventory
	for(i=0;i<this->Potions.size();i++)
	{
		if(thing==this->Potions.at(i).get_name())
		{
			if((Gr->Heros.at(j).get_money()>=this->Potions.at(i).get_bv())&&(Gr->Heros.at(j).get_level()>=this->Potions.at(i).get_ml())) // if the hero has enough money && has the minimum level
			{
				Gr->Heros.at(j).set_money(Gr->Heros.at(j).get_money() - this->Potions.at(i).get_bv());
				Inv->Potions.push_back(this->Potions.at(i));
				return;
			}
			else
			{
				cout << "You do not meet the prerequirements!";
				return;
			}
		}
	}
	// takes the spell the players want and gives it to the hero
	int other;
	for(i=0;i<this->LightningSpells.size();i++)
	{
		if(thing==this->LightningSpells.at(i).get_name())
		{
			for(other=0;other<Gr->Heros.at(j).using_LS.size();other++) // if the hero does not have that spell
			{
				if(Gr->Heros.at(j).using_LS.at(other).get_name()==thing)
				{
					cout << "You already own that spell!" << endl;
					return;
				}
			}
			if((Gr->Heros.at(j).get_money()>=this->LightningSpells.at(i).get_bv())&&(Gr->Heros.at(j).get_level()>=this->LightningSpells.at(i).get_ml()))// if the hero has enough money && has the minimum level
			{
				Gr->Heros.at(j).set_money(Gr->Heros.at(j).get_money() - this->LightningSpells.at(i).get_bv());
				Gr->Heros.at(j).using_LS.push_back(this->LightningSpells.at(i));
				return;
			}
			else
			{
				cout << "You do not meet the prerequirements!" << endl;
				return;
			}
		}
	}
	for(i=0;i<this->IceSpells.size();i++)
	{
		if(thing==this->IceSpells.at(i).get_name())
		{
			for(other=0;other<Gr->Heros.at(j).using_IS.size();other++)
			{
				if(Gr->Heros.at(j).using_IS.at(other).get_name()==thing)
				{
					cout << "You already own that spell!" << endl;
					return;
				}
			}
			if((Gr->Heros.at(j).get_money()>=this->IceSpells.at(i).get_bv())&&(Gr->Heros.at(j).get_level()>=this->IceSpells.at(i).get_ml()))
			{
				Gr->Heros.at(j).set_money(Gr->Heros.at(j).get_money() - this->IceSpells.at(i).get_bv());
				Gr->Heros.at(j).using_IS.push_back(this->IceSpells.at(i));
				return;
			}
			else
			{
				cout << "You do not meet the prerequirements!";
				return;
			}
		}
	}
	for(i=0;i<this->FireSpells.size();i++)
	{
		if(thing==this->FireSpells.at(i).get_name())
		{
			for(other=0;other<Gr->Heros.at(j).using_FS.size();other++)
			{
				if(Gr->Heros.at(j).using_FS.at(other).get_name()==thing)
				{
					cout << "You already own that spell!" << endl;
					return;
				}
			}
			if((Gr->Heros.at(j).get_money()>=this->FireSpells.at(i).get_bv())&&(Gr->Heros.at(j).get_level()>=this->FireSpells.at(i).get_ml()))
			{
				Gr->Heros.at(j).set_money(Gr->Heros.at(j).get_money() - this->FireSpells.at(i).get_bv());
				Gr->Heros.at(j).using_FS.push_back(this->FireSpells.at(i));
				return;
			}
			else
			{
				cout << "You do not meet the prerequirements!";
				return;
			}
		}
	}
	cout << "There is no such Item or Spell in the Market!" << endl;
}
void Market::Sell(Inventory *Inv,Grid *Gr)
{
	Inv->checkInventory();
	string thing,human;
	cout << "Give the name of the Item you want to sell: ";
	cin >> thing;
	cout << "Give the name of the Hero that will take the money: ";
	cin >> human;
	
	int i,j; bool check=false;
	for(i=0;i<Gr->Heros.size();i++)
	{
		if(Gr->Heros.at(i).get_name()==human)
		{
			check=true;
			j=i;
			break;
		}
	}
	if(check==false)
	{
		cout << "There is no such Hero in the Grid!";
		return;
	}
	
	for(i=0;i<Inv->Weapons.size();i++)
	{
		if(thing==Inv->Weapons.at(i).get_name())
		{
			Gr->Heros.at(j).set_money(Gr->Heros.at(j).get_money() + (Inv->Weapons.at(i).get_bv()/2));
			Inv->Weapons.erase(Inv->Weapons.begin() + i);
			return;
		}
	}
	for(i=0;i<Inv->Armors.size();i++)
	{
		if(thing==Inv->Armors.at(i).get_name())
		{
			Gr->Heros.at(j).set_money(Gr->Heros.at(j).get_money() + (Inv->Armors.at(i).get_bv()/2));
			Inv->Armors.erase(Inv->Armors.begin() + i);
			return;
		}
	}
	for(i=0;i<Inv->Potions.size();i++)
	{
		if(thing==Inv->Potions.at(i).get_name())
		{
			Gr->Heros.at(j).set_money(Gr->Heros.at(j).get_money() + (Inv->Potions.at(i).get_bv()/2));
			Inv->Potions.erase(Inv->Potions.begin() + i);
			return;
		}
	}
	cout << "You dont own such Item!" << endl;
}
/*------------------------------------------*/
