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
class Protester;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

// This file basically calls all the functions listed in StudentWorld.cpp
// public: lists them
// private: saves the data	
class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir), _iceman(nullptr) 
	{
		for (int x = 0; x < 64; ++x) {
			for (int y = 0; y < 64; ++y) {
				_actorPositions[x][y] = 0;
			}
		}
	}

	~StudentWorld(); // Destructor to clean up memory

	bool No_Overlap(int x, int y);
	virtual int init() override;
	virtual int move() override;
	virtual void cleanUp() override;

	// --- Display ---
	void Update_Display_Text();

	//  --- Actor Management ---
	bool Can_Face(int x, int y, GraphObject::Direction& dir);
	bool Can_Add_Protester();
	bool Can_Add_Waterpool(int x, int y);
	void Find_Protester(int x, int y, std::vector<Actor*>& foundProtesters);
	void Remove_Dead_Game_Objects();
	void Pickup_Oil(int x, int y);
	void New_Direction();
	Iceman* getIceman() const { return _iceman; }
	bool inLineOfSightToPlayer(int x, int y, GraphObject::Direction& outDir) const;
	void spawnPowerUps();

	// --- Collision and Movement ---
	bool canMoveTo(int x, int y, GraphObject::Direction dir) const;
	bool canMoveTo(int x, int y) const; //overloaded helper function
	bool Is_Boulder(int x, int y, GraphObject::Direction dir) const;
	bool Is_Ice(int x, int y, GraphObject::Direction dir) const;
	bool No_Ice_Or_Boulder(int x, int y, GraphObject::Direction dir) const;
	bool Can_Fall(int x, int y) const;
	bool Can_Shout() const;
	std::vector<std::pair<int, int>> getPathToExit(int startX, int startY);
	std::vector<std::pair<int, int>> getPathToTarget(int startX, int startY, int goalX, int goalY);
	std::vector<std::pair<int, int>> computePathFromTo(int startX, int startY, int goalX, int goalY);

	//  --- Game State & Level Progression ---

	bool Finished_Level();
	bool Player_Died();
	bool isPlayerStunned() const;

	//  --- Gameplay & Interactions --- 

	Iceman* Iceman_ptr() { return _iceman; }
	unsigned int getTicks() const { return _ticks; }

	bool Near_Iceman(int x, int y, int actortype);
	void Boulder_Annoyed(int x, int y);
	bool Protester_Annoyed(int x, int y, int dmg, int source);
	Protester* Bribe_Nearby_Protester(int x, int y);
	bool Set_Position(int x, int y, char actortype);
	char Get_Position(int x, int y) const { return _actorPositions[x][y]; }
	void Squirt_Water(int x, int y, GraphObject::Direction dir);
	void Sonar_Used(int x, int y);
	void dropGold(int x, int y);

	// --- Other/Misc functions ---
	void SpawnIce();
	bool Remove_Ice_At(int x, int y);
	int getRestTime() const;


private:
	static const int _NUMIce = 3616;
	std::vector<Ice*> _ptrIce;
	Iceman* _iceman;
	int _Barrels = 0;
	int _pickedBarrels = 0;
	int _Boulders = 0;
	int _Gold = 0;
	int _ticksForProtester;
	int _t_LastProtester;
	int _nProtesters = 0;
	int _ticks = 0; // Ticks for the game world

	char _actorPositions[64][64] = {};
	std::list<Actor*> _actors; // List of actors in the game world
};

#endif // STUDENTWORLD_H_