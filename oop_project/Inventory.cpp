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
/*-----------------CLASS INVENTORY-------------------------*/
void Inventory::checkInventory(void)
{
	int i;
	cout << "----------------------" << endl << "Weapons in the Inventory:" << endl << endl;
	for(i=0;i<this->Weapons.size();i++)
	{
		cout << "Weapon's name: " << this->Weapons.at(i).get_name() << endl;
		cout << " Buy value: " << this->Weapons.at(i).get_bv() << endl;
		cout << " Minimum level: " << this->Weapons.at(i).get_ml() << endl;
		cout << " Damage: " << this->Weapons.at(i).get_dmg() << endl;
		cout << " Hands: " << this->Weapons.at(i).get_h() << endl;
		cout << endl << endl;
	}
	cout << "----------------------" << endl << "Armors in the Inventory: " << endl << endl;
	for(i=0;i<this->Armors.size();i++)
	{
		cout << "Armor's name: " << this->Armors.at(i).get_name() << endl;
		cout << " Buy value: " << this->Armors.at(i).get_bv() << endl;
		cout << " Minimum level: " << this->Armors.at(i).get_ml() << endl;
		cout << " Armor: " << this->Armors.at(i).get_ar() << endl;
		cout << endl << endl;
	}
	cout << "----------------------" << endl << "Potions in the Inventory: " << endl << endl;
	for(i=0;i<this->Potions.size();i++)
	{
		cout << "Potion's name: " << this->Potions.at(i).get_name() << endl;
		cout << " Buy value: " << this->Potions.at(i).get_bv() << endl;
		cout << " Minimum level: " << this->Potions.at(i).get_ml() << endl;
		cout << " Increasing: " << this->Potions.at(i).get_stat() << " for " << this->Potions.at(i).get_plus() << endl;
		cout << endl << endl;
	}
}
void Inventory::equip(Grid *Gr)
{
	this->checkInventory();
	string thing,human;
	cout << "Give the name of the Weapon or Armor that you want to use from the Inventory: ";
	cin >> thing;
	cout << "Give the name of the Hero that will use it: ";
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
	
	// we find the weapon and give it to the Hero the players want to have it and then delete it from the inventory
	for(i=0;i<this->Weapons.size();i++)
	{
		if(thing==this->Weapons.at(i).get_name())
		{
			if(Gr->Heros.at(j).using_W.size()==0)
			{
				Gr->Heros.at(j).using_W.push_back(this->Weapons.at(i));
				this->Weapons.erase(this->Weapons.begin() + i);
			}
			else if(Gr->Heros.at(j).using_W.size()==1)
			{
				if((this->Weapons.at(i).get_h()=="One")&&(Gr->Heros.at(j).using_W.at(0).get_h()=="One"))
				{
					Gr->Heros.at(j).using_W.push_back(this->Weapons.at(i));
					this->Weapons.erase(this->Weapons.begin() + i);
				}
				else
				{
					this->Weapons.push_back(Gr->Heros.at(j).using_W.at(0));
					Gr->Heros.at(j).using_W.erase(Gr->Heros.at(j).using_W.begin() + 0);
					Gr->Heros.at(j).using_W.push_back(this->Weapons.at(i));
					this->Weapons.erase(this->Weapons.begin() + i);
				}
			}
			else
			{
				if(this->Weapons.at(i).get_h()=="One")
				{
					Gr->Heros.at(j).using_W.push_back(this->Weapons.at(i));
					this->Weapons.erase(this->Weapons.begin() + i);
					this->Weapons.push_back(Gr->Heros.at(j).using_W.at(0));
					Gr->Heros.at(j).using_W.erase(Gr->Heros.at(j).using_W.begin() + 0);
				}
				else
				{
					if(Gr->Heros.at(j).using_W.at(0).get_h()=="One")
					{
						this->Weapons.push_back(Gr->Heros.at(j).using_W.at(0));
						Gr->Heros.at(j).using_W.erase(Gr->Heros.at(j).using_W.begin() + 0);
						this->Weapons.push_back(Gr->Heros.at(j).using_W.at(0));
						Gr->Heros.at(j).using_W.erase(Gr->Heros.at(j).using_W.begin() + 0);
						Gr->Heros.at(j).using_W.push_back(this->Weapons.at(i));
						this->Weapons.erase(this->Weapons.begin() + i);
					}
					else
					{
						this->Weapons.push_back(Gr->Heros.at(j).using_W.at(0));
						Gr->Heros.at(j).using_W.erase(Gr->Heros.at(j).using_W.begin() + 0);
						Gr->Heros.at(j).using_W.push_back(this->Weapons.at(i));
						this->Weapons.erase(this->Weapons.begin() + i);
					}
				}
			}
			return;
		}
	}
	// we find the armor and give it to the Hero the players want to have it and then delete it from the inventory
	for(i=0;i<this->Armors.size();i++)
	{
		if(thing==this->Armors.at(i).get_name())
		{
			if(Gr->Heros.at(j).using_A.size()==0)
			{
				Gr->Heros.at(j).using_A.push_back(this->Armors.at(i));
				this->Armors.erase(this->Armors.begin() + i);
			}
			else
			{
				this->Armors.push_back(Gr->Heros.at(j).using_A.at(0));
				Gr->Heros.at(j).using_A.erase(Gr->Heros.at(j).using_A.begin() + 0);
				Gr->Heros.at(j).using_A.push_back(this->Armors.at(i));
				this->Armors.erase(this->Armors.begin() + i);
			}
			return;
		}
	}
	cout << "There is no such Weapon or Armor in the Inventory!" << endl;
}
void Inventory::use(Grid *Gr)
{
	this->checkInventory();
	string thing,human;
	cout << "Give the name of the Potion that you want to use from the Inventory: ";
	cin >> thing;
	cout << "Give the name of the Hero that will use it: ";
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
	
	// we find the potion and give it to the Hero the players want to have it and then delete it from the inventory
	for(i=0;i<this->Potions.size();i++)
	{
		if(thing==this->Potions.at(i).get_name())
		{
			if(this->Potions.at(i).get_stat()=="healthPower")
			{
				Gr->Heros.at(j).set_hP(this->Potions.at(i).get_plus() + Gr->Heros.at(j).get_hP());
				if(Gr->Heros.at(j).get_hP()>Gr->Heros.at(j).get_maxhP()) Gr->Heros.at(j).set_hP(Gr->Heros.at(j).get_maxhP());
				this->Potions.erase(this->Potions.begin() + i);
			}
			else if(this->Potions.at(i).get_stat()=="magicPower")
			{
				Gr->Heros.at(j).set_mP(this->Potions.at(i).get_plus() + Gr->Heros.at(j).get_mP());
				if(Gr->Heros.at(j).get_mP()>Gr->Heros.at(j).get_maxmP()) Gr->Heros.at(j).set_mP(Gr->Heros.at(j).get_maxmP());
				this->Potions.erase(this->Potions.begin() + i);
			}
			else if(this->Potions.at(i).get_stat()=="strength")
			{
				Gr->Heros.at(j).set_strength(this->Potions.at(i).get_plus() + Gr->Heros.at(j).get_strength());
				this->Potions.erase(this->Potions.begin() + i);
			}
			else if(this->Potions.at(i).get_stat()=="agility")
			{
				Gr->Heros.at(j).set_agility(this->Potions.at(i).get_plus() + Gr->Heros.at(j).get_agility());
				this->Potions.erase(this->Potions.begin() + i);
			}
			else if(this->Potions.at(i).get_stat()=="dexterity")
			{
				Gr->Heros.at(j).set_dexterity(this->Potions.at(i).get_plus() + Gr->Heros.at(j).get_dexterity());
				this->Potions.erase(this->Potions.begin() + i);
			}
			return;
		}
	}
	cout << "There is no such name of a Potion in the Invertory!" << endl;
}
/*------------------------------------------*/
