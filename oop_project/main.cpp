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
int main(void)
{
	srand( time(NULL) );
	
	Grid Gr;
	Market Mar;	
	Inventory Inv;
	
	char input;
	string m;
	int chance;
	
	cout << "Welcome!" << endl;
	
	while(1)
	{
		do
		{
			cout << "To Display Map, press 1" << endl;
			cout << "To Move, press 2" << endl;
			cout << "To see the Stats of the players, press 3" << endl;
			cout << "To Exit the game, press 0" << endl;
			cout << "Input: ";
			cin >> input;
			if(input=='1') Gr.PrintGrid();
			else if(input=='2') m=Gr.move();
			else if(input=='3') Gr.PrintStats();
			else if(input != '0') cout << "Invalid Input!" << endl << endl; 
		}while((input!='2')&&(input!='0'));
		if(input=='0')  break;
		
		if(m=="Market")
		{
			do
			{
				cout << "Welcome to the Market!" << endl << endl;
				cout << "To Buy stuff, press 1" << endl;
				cout << "To Sell stuff, press 2" << endl;
				cout << "To Exit the Market, press 0" << endl;
				cin >> input;
				if(input=='1') Mar.Buy(&Inv,&Gr);
				else if(input=='2') Mar.Sell(&Inv,&Gr);
				else if(input != '0') cout << "Invalid Input!" << endl << endl;
			}while(input!='0');
		}
		else
		{
			chance=rand()%10 +1;
			// random chance for a battle
			if(chance>3)
			{
				cout << "Hopefully, there is no Monster here." << endl << endl;
				do
				{
					cout << "To check your Invertory, press 1" << endl;
					cout << "To equip a weapon or armor, press 2" << endl;
					cout << "To use a potion, press 3" << endl;
					cout << "If u finished with your Inventory management, press 0" << endl;
					cin >> input;
					if(input=='1') Inv.checkInventory();
					else if(input=='2') Inv.equip(&Gr);
					else if(input=='3') Inv.use(&Gr);
					else if(input!='0') cout << "Invalid Input!" << endl << endl;
				}while(input!='0');
			}
			else
			{
				cout << "Oh my God! A Monster!" << endl << endl;
				if(Gr.Battle())
				{
					cout << "----All Players revived----" << endl;
					Gr.Revive();
				}
			}
		}
	}
	return 0;
}
