#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <list>
#include <vector>
using namespace std;
/*---------------CLASS MARKET---------------------------*/
class Market
{
	vector<Weapon> Weapons; // weapons in the market
	vector<Armor> Armors; // armors in the market
	vector<Potion> Potions; // potions in the market
	vector<IceSpell> IceSpells; // ice spells in the market
	vector<FireSpell> FireSpells; // fire spells int the market
	vector<LightningSpell> LightningSpells; // lighting spells in the market
public:
	// constructor-puts all the items/spells in the market
	Market();
	void Print_Market(); // prints the items and the potions of the market
	void Buy(Inventory *Inv,Grid *Gr);
	void Sell(Inventory *Inv,Grid *Gr);
};
