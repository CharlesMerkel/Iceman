#include "StudentWorld.h"
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

// All function names aren't set in stone.
// 
// Add StudentWorld's:
//  - Display & Cleanup:
// 
// Update_Display_Text - Updates the display with the current statistics. 
// No_Overlap - Checks if an actor can be placed in the map without being overlapped with
//              other actors/objects.

// Init - Initializes the game world: ice, boulders, gold, oil, the Iceman thenmself.
//        and also determines the number of actors based off of the current level.
/*
int StudentWorld::init() {
	_numIce = 3600;
	int i = 0;
	while (i < _numIce) {
		for (int x = 0; x < 60; x++) {
			for (int y = 0; y < 60; y++) {
				if (x >= 30 && x <= 33 && y > 3) { ; }
				else {
					_ptrIce[i] = new Ice(x, y, this);
					i++;
				}
			}
		}

	}
}
*/

// Move - The Main game logic, updates the display, spawns most of the actors, has the
//        probablity of spawning hardcore protesters, spawns most of the Pickups
//        & finally checks if the level is completed or if the player died to 
//        start the next map.
// Clean_Up - Self-explanatory, deletes every actor & ice and resets their positions.
// 
//  - Actor Management
// 
// Can_Face - Checks if the Actor can face the Iceman based off of its coordinates &
//            current direction, returns false if unable.
// Can_Add_Protester - Checks if a new protester can spawn based off of the level
//                     and tick.
// Can_Add_Waterpool - Checks if a waterpool can be added at a coordinate, ensuring that ice isn't
//		           blocking the area.
// Find_Protester - Searches and returns a protester within a 3x3 area around a point.
// Remove_Dead_Game_Objects - Removes all dead actors & updates the actor's position.
// 
//  - Game State & Level Progression
// 
// Finished_Level - Returns true if the player picked up all the oil.
// Player_Died - Checks if the player died, if so, then decrement lives and returns true.
// New_Direction - Generates a random direction for an actor to move, as long as it's not blocked.
// Pickup_Oil - Marks an Oil barrel as being picked up and increments Oil Pickup counter.
// 
//  - Collision & Movement Checks
// 
// Is_Boulder - Checks if there is a boulder in the specified direction from the given coordinates.
// Is_Ice - Checks if there is ice at the given coordinates in a specified direction.
// No_Ice_Or_Boulder - Checks if there is neither ice nor a boulder in a specified direction from given coordinates.
// Can_Fall - Checks if an object at given coords can fall downward (Check if no ice or boulder below it).
// Can_Shout - Checks if the Iceman can be shouted again.
// 
//  - Gameplay & Interactions
// 
// Remove_Ice - Attempts to remove ice from the grid at the specified coordinates. If successful, it returns true.
// Near_Iceman - Checks if the Iceman is within a certain distance from a given coordinate.
// Boulder_Annoyed - Allows the boulder to 'damage' any actor.
// Protester_Annoyed - Allows the squirt object to damage protesters.
// Set_Position - Sets a 4x4 actor in a specified coordinate.
// Squirt_Water - Creates a squirt object from the player
// Sonar_Used - Makes all actors within a 12 unit radius from the player visible.




