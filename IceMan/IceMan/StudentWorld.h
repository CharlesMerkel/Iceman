#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir)
	{
	}

	// Initializes data structures used to keep track of your game's virtual world
	// Construct a new oil field (map) that meets the requirements stated in page 16 & 17
	// Allocate & insert a valid 'Iceman' object into the game world at the proper location
	virtual int init()
	{
		return GWSTATUS_CONTINUE_GAME;
	}

	// Must update the status text on the top of the screen with the latest information
	// (e.g., the user's current score , the remaining bonus score for the level, etc.)
	// - - - 
	// Must ask all of the actors that are currently active in the game world to do something
	// - If an actor does something that causes the Iceman to give up, the move() method should return GWSTATUS_PLAYER_DIED
	// - If the Iceman collects all of the barrels (Win condition) play SOUND_FINISHED_LEVEL and reutrn GWSTATUS_FINISHED_LEVEL
	// - - -
	// Must delete any actors that need to be removed from the game during this tick & remove them from your STL container that tracks them.
	// - Protester run to top right corner after lost all hp
	// - Boulder falls down and breaks on the bottom
	// - Gold Nugget when player picks it up
	// - Waterpool duration
	// - Squirt goes away after reaching maximum distance
	// - Others
	// - - - 
	// Returns GWSTATUS_: PLAYER_DIED, CONTINUE_GAME, FINISHED_LEVEL when it returns at the end of a tick
	// - - - 
	// Give each actor a chance to do something... (pg. 20)
	// Add new Actors during each tick... (pg. 20 & 21)
	// Remove dead actors after each tick... (pg. 21)
	// Updates display text... (pg. 21)
	virtual int move()
	{
		// This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
		// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}

	// Create Classes for All Actors:
	// Iceman, Regular Protesters, Hardcore Protesters, Squirts of water, Barrels of Oil, Boulders, Gold Nuggets, Sonar Kits, Water pools, Ice
	// Define base classes whenever any classes use the same code
	virtual void cleanUp()
	{
	}

private:
};

#endif // STUDENTWORLD_H_
