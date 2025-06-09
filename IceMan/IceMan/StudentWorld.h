#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GraphObject.h"

#include "GameWorld.h"
#include "GameConstants.h"
#include "GameController.h"

#include <vector>
#include <string>
#include <algorithm>
#include <list>


class Actor;
class Ice;
class Iceman;
class Protestor;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

// This file basically calls all the functions listed in StudentWorld.cpp
// public: lists them
// private: saves the data	
class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir), _iceman(nullptr) {
		for (int x = 0; x < 64; ++x) {
			for (int y = 0; y < 64; ++y) {
				_actorPositions[x][y] = 0;
			}
		}
	}

	~StudentWorld(); // Destructor to clean up memory

	virtual int init() override;
	virtual int move() override;
	virtual void cleanUp() override;

	//  --- Actor Management ---
	bool Can_Face();
	bool Can_Add_Protester();
	bool Can_Add_Waterpool(int x, int y);
	void Find_Protester(int x, int y, std::vector<Actor*>& foundProtesters);
	void Remove_Dead_Game_Objects();
	void Pickup_Oil(int x, int y);
	void New_Direction();
	
	// --- Collision and Movement ---
	bool Is_Boulder(int x, int y, GraphObject::Direction dir) const;
	bool Is_Ice(int x, int y, GraphObject::Direction dir) const;
	bool No_Ice_Or_Boulder(int x, int y, GraphObject::Direction dir) const;
	bool Can_Fall(int x, int y) const;
	bool Can_Shout() const;

	//  --- Game State & Level Progression ---

	bool Finished_Level();
	bool Player_Died();
	bool isPlayerStunned() const;

	//  --- Gameplay & Interactions ---
	// [ None of these inteactions work ]

	Iceman* Iceman_ptr() { return _iceman; }

	bool Near_Iceman(int x, int y, int actortype);
	void Boulder_Annoyed(int x, int y);
	bool Protester_Annoyed(int x, int y,int dmg);
	bool Set_Position(int x, int y, char actortype);

	// --- Other/Misc functions ---
	void SpawnIce();
	bool removeIceAt(int x, int y);
	int getRestTime() const;


private:
	static const int _NUMIce = 3600;
	std::vector<Ice*> _ptrIce;
	Iceman* _iceman;

	char _actorPositions[64][64] = {};
	std::list<Actor*> _actors; // List of actors in the game world
};

#endif // STUDENTWORLD_H_