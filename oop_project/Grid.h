#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <list>
#include <vector>
using namespace std;
/*----------------CLASS POINT---------------*/
/* this class  is about every point of the grid, it contains its type and its coordinates */
class Point
{
	int x;
	int y;
	string type;
public:
	//getters-setters
	void set_x(int a)
	{
		this->x=a;
	}
	int get_x(void)
	{
		return this->x;
	}
	void set_y(int b)
	{
		this->y=b;
	}
	int get_y(void)
	{
		return this->y;
	}
	void set_type(string t)
	{
		this->type=t;
	}
	string get_type(void)
	{
		return this->type;
	}
};
/*------------------------------------------*/

/*---------------CLASS GRID-----------------*/
class Grid
{
	// the grid is made by a lot of points
	Point **Array;
	// current position of the hero(es) in the grid
	Point current;
public:
	// a vector of all the heroes in the game
	vector<Hero> Heros;
	// a vector for the monster in case of a battle
	vector<Monster> mon;
	// constructor
	Grid();
	// destructor
	~Grid();
	// it prints the grid
	void PrintGrid();
	// it moves the hero(es) to a different point
	string move();
	// prints stats for all the heroes
	void PrintStats();
	// function for the battle
	bool Battle();
	// in case all heroes die, this function will be called
	void Revive();
};
