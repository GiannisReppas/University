// grid dimentions
#define M 10
#define N 10

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
/*---------------CLASS GRID-----------------*/
Grid::Grid(void)
{
	int i,j;
	
	/* this part builds all the heroes that will participate in the game(number, names, classes etc) */
	string name,cls;
	int number;
	while(1)
	{
		cout << "Give the number of the Heros that will play:";
		cin >> number;
		if((number>=1)&&(number<=3)) break;
		else cout << "The number of Heros must be between 1 and 3" << endl << endl;
	}

	Warrior *W;
	Sorcerer *S;
	Paladin *P;
	for(i=0;i<number;i++)
	{
		cout << "Give the name of Hero number " << i+1 << ": ";
		cin >> name;
		cout << "Give his class too: ";
		while(1)
		{
			cin >> cls;
			if((cls=="Warrior")||(cls=="warrior")||(cls=="Sorcerer")||(cls=="sorcerer")||(cls=="Paladin")||(cls=="paladin")) break;
			else cout << "This is not a class of a hero, try again please:";
		}
		if((cls=="Warrior")||(cls=="warrior"))
		{
			W = new Warrior(name);
			this->Heros.push_back(*W);
			delete W;
		}
		else if((cls=="Sorcerer")||(cls=="sorcerer"))
		{
			S = new Sorcerer(name);
			this->Heros.push_back(*S);
			delete S;
		}
		else if((cls=="Paladin")||(cls=="paladin"))
		{
			P = new Paladin(name);
			this->Heros.push_back(*P);
			delete P;
		}
	}
	
	/* this part creates the grid-around of the grid there are non accessible points by the heros and */
	/* we have some markets at random places */
	this->Array= new Point*[N];
	for(i=0;i<N;i++) Array[i]= new Point[M];
	
	bool check;
	int one;
	// first row with non accessible points
	for(j=0;j<M;j++)
	{
		this->Array[0][j].set_type("NonAccessible");
		this->Array[0][j].set_x(0);
		this->Array[0][j].set_y(j);
	}
	// from the second till one row before the end of thye grid, only the first and the last column will have non accessible points
	for(i=1;i<N-1;i++)
	{
		check=false;
		for(j=0;j<M;j++)
		{
			if((j==0)||(j==M-1))
			{
				this->Array[i][j].set_type("NonAccessible");
				this->Array[i][j].set_x(i);
				this->Array[i][j].set_y(j);
			}
			else
			{
				// the points with market will be random
				one=rand()%10 + 1;
				if((one==1)&&(i%2==0)&&(check==false))
				{
					this->Array[i][j].set_type("Market");
					this->Array[i][j].set_x(i);
					this->Array[i][j].set_y(j);
					check=true;
				}
				else
				{
					this->Array[i][j].set_type("Accessible");
					this->Array[i][j].set_x(i);
					this->Array[i][j].set_y(j);
				}
			}
		}
	}
	// last row with non accessible points
	for(j=0;j<M;j++)
	{
		this->Array[N-1][j].set_type("NonAccessible");
		this->Array[N-1][j].set_x(i);
		this->Array[N-1][j].set_y(j);
	}
	
	// if there are no markets, we build one
	check=false;
	for(i=0;i<N;i++) for(j=0;j<M;j++) if(this->Array[i][j].get_type()=="Market") check=true;
	if(check==false) this->Array[1][2].set_type("Market");
	
	//we put the Heroes on the first Accessible point on the Grid
	check=false;
	for(i=0;i<N;i++)
	{
		if(check==true) break;
		for(j=0;j<M;j++)
		{
			if(this->Array[i][j].get_type()=="Accessible")
			{
				this->current=this->Array[i][j];
				check=true;
				break;
			}
		}
	}
}
Grid::~Grid()
{
	int i;
	for(i=0;i<N;i++) delete []Array[i];
	delete []Array;
}
string Grid::move(void)
{
	cout << endl << "Make your next move: W->Up S->Down D->Right A->Left 0->Exit" << endl;
	do
	{
		char a;
		cout << "Move:";
		cin >> a;
	
		if((a=='W')||(a=='w'))
		{
			if(this->Array[this->current.get_x()-1][this->current.get_y()].get_type()!="NonAccessible")
			{
				this->current = this->Array[this->current.get_x()-1][this->current.get_y()];
				return this->Array[this->current.get_x()][this->current.get_y()].get_type();
			}
			else cout << "You can't go there!" << endl;
		}
		else if((a=='S')||(a=='s'))
		{
			if(this->Array[this->current.get_x()+1][this->current.get_y()].get_type()!="NonAccessible")
			{
				this->current = this->Array[this->current.get_x()+1][this->current.get_y()];
				return this->Array[this->current.get_x()][this->current.get_y()].get_type();
			}
			else cout << "You can't go there!" << endl;
		}
		else if((a=='D')||(a=='d'))
		{
			if(this->Array[this->current.get_x()][this->current.get_y()+1].get_type()!="NonAccessible")
			{
				this->current = this->Array[this->current.get_x()][this->current.get_y()+1];
				return this->Array[this->current.get_x()][this->current.get_y()].get_type();
			}
			else cout << "You can't go there!" << endl;
		}
		else if((a=='A')||(a=='a'))
		{
			if(this->Array[this->current.get_x()][this->current.get_y()-1].get_type()!="NonAccessible")
			{
				this->current = this->Array[this->current.get_x()][this->current.get_y()-1];
				return this->Array[this->current.get_x()][this->current.get_y()].get_type();
			}
			else cout << "You can't go there!" << endl;
		}
		else if(a=='0') return "exit";
		else cout << "Invalid input!" << endl;
		cout << endl;
	}while(1);
}
void Grid::PrintGrid(void)
{
	//we print the grid: "."-->Accessible , "*"-->NonAccessible , "M"->Market , "S"--> current position of the hero
	int i,j;
	for(i=0;i<N;i++) 
	{
		for(j=0;j<M;j++)
		{
			if(this->Array[i][j].get_type()=="NonAccessible")
			{
				if((this->current.get_x()==i)&&(this->current.get_y()==j)) cout << '$' << ' ';
				else cout << '*' << ' ';
			}
			else if(this->Array[i][j].get_type()=="Market")
			{
				if((this->current.get_x()==i)&&(this->current.get_y()==j)) cout << '$' << ' ';
				else cout << 'M' << ' ';
			}
			else if(this->Array[i][j].get_type()=="Accessible")
			{
				if((this->current.get_x()==i)&&(this->current.get_y()==j)) cout << '$' << ' ';
				else cout << '.' << ' ';
			}
			else cout << '?';
		}
		cout << endl;
	}
	
	//we tell the players what their position is too
	cout << endl << "You are currently at the position in the Grid with x:" << this->current.get_x() << " and y:" << this->current.get_y() << endl;
	if(this->current.get_type()=="Accessible") cout << "This position is a common position" << endl;
	else if(this->current.get_type()=="Market") cout << "This postion is a Market" << endl;
	cout << endl;
}
void Grid::PrintStats(void)
{
	int i;
	cout << "Statistics for the players:" << endl;
	for(i=0;i<this->Heros.size();i++)
	{
		cout << "Player " << i+1 << ":" << endl;
		cout << "Name: " << this->Heros.at(i).get_name() << endl;
		cout << "Level: " << this->Heros.at(i).get_level() << endl;
		cout << "healthPower: " << this->Heros.at(i).get_hP() << endl;
		cout << "magicPower: " << this->Heros.at(i).get_mP() << endl;
		cout << "Strength: " << this->Heros.at(i).get_strength() << endl;
		cout << "Agility: " << this->Heros.at(i).get_agility() << endl;
		cout << "Dexterity: " << this->Heros.at(i).get_dexterity() << endl;
		cout << "Money: " << this->Heros.at(i).get_money() << endl << endl;
	}
}
bool Grid::Battle(void)
{
	int which_mon=rand()%3;
	//int choise;
	string hero_name,spell_name,choise;
	int i,j,blow=0,which_hero,miss_chanse,sum,defsp=4,atksp=0,escsp=0,defcount=-1,atkcount=-1,esccount=-1;
	bool check=false,player_end=false,fspell=false,ispell=false,lspell=false,check2=false,tpk=false;
	int mon_level=Heros.at(0).get_level();
	if (which_mon==0) // ta terata!
	{
		Dragon *mons;
		mons = new Dragon("Dragon", mon_level, mon_level*250);
		cout<<"A "<< mons->get_name()<< " appeared!"<<endl;
		mon.push_back(*mons);
		delete mons;
	}
	else if (which_mon==1)
	{
		Exoskeleton *mons;
		mons = new Exoskeleton("Exoskeleton", mon_level, mon_level*250);
		cout<<"A Exoskeleton appeared!"<<endl;
		mon.push_back(*mons);
		delete mons;
	}
	else if (which_mon==2)
	{
		Spirit *mons;
		mons= new Spirit("Spirit", mon_level, mon_level*250); 
		cout<<"A Spirit appeared!"<<endl;
		mon.push_back(*mons);
		delete mons;
	}
	// na kanoume ena rand gia to an varaei prwta to teras h panta o paixtis? 
	for(;;)
	{
		sum=0;
		check=false;
		player_end=false;
		blow=0;
		fspell=0;
		ispell=0;
		lspell=0;
		check2=0;
		cout<< mon.at(0).get_name()<<" has "<< mon.at(0).get_hP()<< "/"<< mon.at(0).get_maxhP()<<" hP."<<endl; //prints ana girw
		for(i=0;i<this->Heros.size();i++)
		{
			cout<< this->Heros.at(i).get_name()<<" has "<< this->Heros.at(i).get_hP()<< "/"<< this->Heros.at(i).get_maxhP()<<" hP."<<endl;
			cout<< this->Heros.at(i).get_name()<<" has "<< this->Heros.at(i).get_mP()<< "/"<< this->Heros.at(i).get_maxmP()<<" mP."<<endl;
		}
		for(;;)
		{
			cout<<"What do you want to do?"<<endl<<"To attack press: 1"<<endl<<"To cast a spell press: 2"<<endl;
			cin>>choise;
			for(;;)
			{
				if(choise!= "1" && choise!= "2") cin>>choise;
				else break;
			}
			if(choise=="1")
			{
				for(;;)
				{
					cout<<"Select the hero you want to attack!"<<endl;
					cin>>hero_name;		
					for(i=0;i<this->Heros.size();i++)
					{
						if(this->Heros.at(i).get_hP()>0)
						if(this->Heros.at(i).get_name()==hero_name )  //edw pernoume to blow apo to melee atk
						{
					  		blow= this->Heros.at(i).get_strength() + this->Heros.at(i).get_dmg_of_weapons(); 
							check=true;
						}
					}
					if(check) break;
					cout<<"No Hero named like that!"<<endl;
				}
				miss_chanse=rand()%10+1;
				if(miss_chanse > (mon.at(0).get_escape()-escsp)) //edw bazoume to def kai to esc chanse gia na doume an tha varesei kai poso
				{
					blow=blow - ((mon.at(0).get_def())/defsp);	
					cout<<"You dealt "<<blow <<" dmg!"<<endl;		
					mon.at(0).set_hP(mon.at(0).get_hP()-blow);
				}
				else
				{
					cout<<"You missed!"<<endl;
				}
				player_end=true;
			}
			if(player_end) break;
			if(choise=="2")
			{
				cout<<"Select the hero you want to attack!"<<endl;
				cin>>hero_name;
				for(i=0;i<this->Heros.size();i++)
			    {
					if(this->Heros.at(i).get_hP()>0)
					{
						if(this->Heros.at(i).get_name()==hero_name )
						{
							if(this->Heros.at(i).using_IS.size()!=0 || this->Heros.at(i).using_FS.size()!=0 || this->Heros.at(i).using_LS.size()!=0)
							for(j=0;j<this->Heros.at(i).using_FS.size();j++)
							{
								cout<<this->Heros.at(i).using_FS.at(j).get_name()<<endl;
							}
							for(j=0;j<this->Heros.at(i).using_LS.size();j++) // autes edw h epenalispsis kanoun print ta spell poy kserei o irwas!wste meta na dialeksei
							{
								cout<<this->Heros.at(i).using_LS.at(j).get_name()<<endl;
							}
							for(j=0;j<this->Heros.at(i).using_IS.size();j++)
							{
								cout<<this->Heros.at(i).using_IS.at(j).get_name()<<endl;
							}
							for(;;)
							{
								cout<<"Choose a spell to cast!"<<endl;
								cin>>spell_name;
								for(j=0;j<this->Heros.at(i).using_FS.size();j++)   //edw einai an petixoun ta spell poso dmg kanoun kai posous girous einai ta debuff tous
								{
									if(this->Heros.at(i).using_FS.at(j).get_name()==spell_name)
									{
										if(this->Heros.at(i).get_mP()>this->Heros.at(i).using_FS.at(j).get_manac())
										{
											check2=true;
											fspell=true;
											atksp=this->Heros.at(i).using_FS.at(j).get_p();
											if(atkcount<=0) atkcount=this->Heros.at(i).using_FS.at(j).get_rounds();
											blow=this->Heros.at(i).get_dexterity() + this->Heros.at(i).using_FS.at(j).get_dmg();
											this->Heros.at(i).set_mP(this->Heros.at(i).get_mP() - this->Heros.at(i).using_FS.at(j).get_manac());
										}
										if(check2) break;			
									}					
								}
								for(j=0;j<this->Heros.at(i).using_IS.size();j++)
								{
									if(this->Heros.at(i).using_IS.at(j).get_name()==spell_name)
									{
										if(this->Heros.at(i).get_mP()>this->Heros.at(i).using_IS.at(j).get_manac())
										{
											check2=true;
											ispell=true;
											defsp=this->Heros.at(i).using_IS.at(j).get_p();
											if(defcount<=0)	defcount=this->Heros.at(i).using_IS.at(j).get_rounds();
											blow=this->Heros.at(i).get_dexterity() + this->Heros.at(i).using_IS.at(j).get_dmg();
											this->Heros.at(i).set_mP(this->Heros.at(i).get_mP() - this->Heros.at(i).using_IS.at(j).get_manac());
										}
										if(check2) break;
									}		
								}
								for(j=0;j<this->Heros.at(i).using_LS.size();j++)
								{
									if(this->Heros.at(i).using_LS.at(j).get_name()==spell_name)
									{
										if(this->Heros.at(i).get_mP()>this->Heros.at(i).using_LS.at(j).get_manac())
										{					
											check2=true;
											lspell=true;
											escsp=this->Heros.at(i).using_LS.at(j).get_p();
											if(esccount<=0)	esccount=this->Heros.at(i).using_LS.at(j).get_rounds();
											blow=this->Heros.at(i).get_dexterity() + this->Heros.at(i).using_LS.at(j).get_dmg();
											this->Heros.at(i).set_mP(this->Heros.at(i).get_mP() - this->Heros.at(i).using_LS.at(j).get_manac());
										}
										if(check2) break;		
									}	
								}
								if(check2) break;
								if(fspell==false) break;
							}
							if(check2)
							{
								miss_chanse=rand()%10+1;
								if(miss_chanse > (mon.at(0).get_escape()-escsp))
								{
									blow=blow + defsp;
									cout<<"You dealt "<<blow <<" dmg!"<<endl;	
									mon.at(0).set_hP(mon.at(0).get_hP()-blow);
									if(fspell)
									{
										cout<< mon.at(0).get_name()<<" has less dmg for "<<atkcount<<" rounds "<<endl;
									}
									if(ispell)
									{
										cout<< mon.at(0).get_name()<<" has less def for "<<defcount<<" rounds "<<endl;
									}
									if(lspell)
									{
										cout<< mon.at(0).get_name()<<" has less esc chance for "<<esccount<<" rounds "<<endl;
									}	
								}
								else // an misarei to spell alla prin eixa petixi gia na einai kanonika oi counters!
								{
									if(atkcount==this->Heros.at(i).using_FS.at(j).get_rounds())
									{
										atkcount=-1;
										atksp=0;
									}	
									if(atkcount==this->Heros.at(i).using_IS.at(j).get_rounds())
									{
										defcount=-1;
										defsp=4;
									}
									if(atkcount==this->Heros.at(i).using_FS.at(j).get_rounds())
									{
										esccount=-1;
										escsp=0;
									}
									cout<<"You missed!"<<endl;
								}
								player_end=true;
							}	 
						}
					}
				}
				if(check2) break;
				else cout<<"No Hero named like that! or not enough mana!"<<endl;
			}
			if(player_end) break;
		}
		blow=0;
		if(mon.at(0).get_hP()>0) // to atk tou monster!
		{
			for(;;)
			{
				if(this->Heros.size()!=1) which_hero=rand()%Heros.size();
				else which_hero=0;
				if(this->Heros.at(which_hero).get_hP()>0) break;
			}
			miss_chanse=rand()%10+1;
			if(miss_chanse > (this->Heros.at(which_hero).get_agility()-12))
			{
			
				int hhhh=this->Heros.at(which_hero).get_arm_of_armors();
				blow=mon.at(0).get_dmg()-atksp - hhhh;
				if(blow<0)
					blow=1;
				this->Heros.at(which_hero).set_hP(this->Heros.at(which_hero).get_hP()-blow);
				cout<<this->Heros.at(which_hero).get_name()<<" took "<< blow<<" dmg!"<<endl;	
			}
			else cout<<mon.at(0).get_name()<<" missed!"<<endl;		
  		}	
		if (atkcount>0)
		{
			atkcount--;
			cout<<"fire debuff rounds: "<<atkcount<<endl;
		}
		if (atkcount==0)
		{
			atkcount--;
		  	atksp=0;
		  	cout<<"fire debuff over!"<<endl;
		}
		if (defcount>0)
		{
			defcount--;
			cout<<"Ice debuff rounds: "<<defcount<<endl;
		}
		if (defcount==0)
		{
			defcount--;
		  	defsp=4;
		  	cout<<"Ice debuff over!"<<endl;
		}
		if (esccount>0)
		{
			esccount--;
			cout<<"lighting debuff rounds: "<<esccount<<endl;
		}
		if (esccount==0)
		{
			esccount--;
		  	escsp=0;
		  	cout<<"lighting debuff over!"<<endl;
		}
		if(mon.at(0).get_hP()<=0)
		{
			cout<<"You slayed the "<< mon.at(0).get_name()<<"."<<endl;
			break;
		}
		for(i=0;i<this->Heros.size();i++) //an pethanei to party
		{
			if(this->Heros.at(i).get_hP()>0)
				sum = sum + this->Heros.at(i).get_hP();
			else 
				sum= sum + 0;
		}
		if(sum<=0)
		{
			cout<<"TPK"<<endl;
			tpk=true;
			break;
		}
	}
	if(tpk) 
		return tpk;
	for(i=0;i<this->Heros.size();i++) // to level up
	{
		this->Heros.at(i).set_exp(this->Heros.at(i).get_exp() + (mon_level*50));
		if(this->Heros.at(i).get_exp()>=this->Heros.at(i).get_maxexp())
		{
			this->Heros.at(i).levelUp();
			cout<<"DING!"<< this->Heros.at(i).get_name()<<" leveled up!!"<<endl;
		}
	}
	mon.clear();
	return tpk;
}
void Grid::Revive(void)
{
	int i;
	for(i=0;i<this->Heros.size();i++)
	{
		this->Heros.at(i).set_hP(this->Heros.at(i).get_maxhP()/2);
	}
}
/*------------------------------------------*/
