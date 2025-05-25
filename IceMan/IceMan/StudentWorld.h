#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include "GameController.h"
#include <vector>
#include <string>
#include <algorithm>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

// This file basically calls all the functions listed in StudentWorld.cpp
// public: lists them
// private: saves the data	
class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir){}

	virtual int init()
	{
		_iceman = new Iceman(this);
		return GWSTATUS_CONTINUE_GAME;
	}

	virtual int move()
	{
		_iceman->doSomething(); // Calls the doSomething function of the iceman
		// This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
		// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}

	// Self-Explanatory, deletes every entity and makes new ones
	virtual void cleanUp()
	{
		delete _iceman;
		_iceman = nullptr;
	}

private:
	int _numIce;
	Ice* _ptrIce [3600];
	Iceman* _iceman;
};

#endif // STUDENTWORLD_H_