#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <list>
#include <vector>
using namespace std;
/*-----------------CLASS INVENTORY-------------------------*/
class Inventory
{
public:
	vector<Weapon> Weapons; // weapons in the inventory
	vector<Armor> Armors; // armors in the inventory
	vector<Potion> Potions; // potions in the inventory
	void checkInventory(); // prints items in the inventory
	void equip(Grid *Gr); // takes an item from the inventory and gives it to a hero to wear it
	void use(Grid *Gr); // takes a potion from the inventory and gives it to a hero to use it
};
