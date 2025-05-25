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
		: GameWorld(assetDir), _iceman(nullptr), _numIce(3600){}

	virtual int init() override;
	virtual int move() override;
	virtual void cleanUp() override;
	
	// Other/Misc functions
	void SpawnIce();
private:
	int _numIce = 3600;
	Ice* _ptrIce[3600] = {};
	Iceman* _iceman;
};


#endif // STUDENTWORLD_H_