#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

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
	}

	~StudentWorld(); // Destructor to clean up memory

	virtual int init() override;
	virtual int move() override;
	virtual void cleanUp() override;

	// Other/Misc functions
	void SpawnIce();
	bool removeIceAt(int x, int y);

private:
	static const int _NUMIce = 3600;
	std::vector<Ice*> _ptrIce;
	Iceman* _iceman;
	std::list<Actor*> _actors; // List of actors in the game world
};

#endif // STUDENTWORLD_H_