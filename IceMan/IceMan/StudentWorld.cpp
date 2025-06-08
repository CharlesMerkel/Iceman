#include "StudentWorld.h"
#include "Actor.h"
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <iomanip>
#include <algorithm>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Add StudentWorld's:
//  - Display & Cleanup:
// 
// Update_Display_Text - Updates the display with the current statistics. 
// No_Overlap - Checks if an actor can be placed in the map without being overlapped with
//              other actors/objects.

// Init - Initializes the game world: ice, boulders, gold, oil, the Iceman thenmself.
//        and also determines the number of actors based off of the current level.
int StudentWorld::init() {

	SpawnIce();
	_iceman = new Iceman(this);


	// Spawn a RegularProtester
	_actors.push_back(new RegularProtester(this));

	return GWSTATUS_CONTINUE_GAME;
}

// Move - The Main game logic, updates the display, spawns most of the actors, has the
//        probablity of spawning hardcore protesters, spawns most of the Pickups & finally 
//        checks if the level is completed or if the player died to start the next map.
int StudentWorld::move() {
	if(_iceman)
	_iceman->doSomething(); //let the iceman act this tick

	return GWSTATUS_CONTINUE_GAME;
}

// Clean_Up - Self-explanatory, deletes every actor & ice and resets their positions.
void StudentWorld::cleanUp() {
	delete _iceman; //deletes the iceman
	_iceman = nullptr;

	//deletes all actors
	for (Actor* actor : _actors) {
		delete actor;
	}
	_actors.clear();

	//deletes ice
	for( Ice* ice : _ptrIce) 
	{
		delete ice;
	}
	_ptrIce.clear();
}

//  --- Actor Management --
 
//Checks if the Actor can face the Iceman based off of its coordinates &
//current direction, returns false if unable.
bool StudentWorld::Can_Face()
{
	return false; // Placeholder implementation, should be replaced with actual logic
}        

// Checks if a new protester can spawn based off of the level and tick.
bool StudentWorld::Can_Add_Protester()
{
	return true; // Placeholder implementation, should be replaced with actual logic
}

// Can_Add_Waterpool - Checks if a waterpool can be added at a coordinate, ensuring that ice isn't
// blocking the area.
bool StudentWorld::Can_Add_Waterpool(int x, int y)
{
	return true; // Placeholder implementation, should be replaced with actual logic
}
// Find_Protester - Searches and returns a protester within a 3x3 area around a point.
void StudentWorld::Find_Protester(int x, int y, vector<Actor*>& foundProtesters)
{
	//for (Actor* actor : _actors) 
	//{
	//	if (actor->isProtester() && abs(actor->getX() - x) <= 3 && abs(actor->getY() - y) <= 3) 
	//	{
	//		foundProtesters.push_back(actor);
	//	}
	//}
}

// Remove_Dead_Game_Objects - Removes all dead actors & updates the actor's position.
void StudentWorld::Remove_Dead_Game_Objects()
{
	//for (auto it = _actors.begin(); it != _actors.end();) 
	//{
	//	if ((*it)->isDead()) 
	//	{
	//		delete *it; // Delete the dead actor
	//		it = _actors.erase(it); // Remove from the vector
	//	} 
	//	else 
	//	{
	//		++it; // Move to the next actor
	//	}
	//}
}

//  --- Game State & Level Progression ---

// Finished_Level - Returns true if the player picked up all the oil.
bool StudentWorld::Finished_Level()
{
	//// Check if all oil barrels have been picked up
	//for (Actor* actor : _actors) 
	//{
	//	if (actor->isOilBarrel() && !actor->isPickedUp()) 
	//	{
	//		return false; // Not all oil barrels have been picked up
	//	}
	//}
	return true; // All oil barrels have been picked up
}

// Player_Died - Checks if the player died, if so, then decrement lives and returns true.
bool StudentWorld::Player_Died()
{
	//if(_iceman._health <= 0) // Check if the Iceman's health is zero or less
	//{
	//	// Decrement lives and return true to indicate player death
	//	_iceman->decrementLives();
	//	return true;
	//}
}

// New_Direction - Generates a random direction for an actor to move, as long as it's not blocked.
void StudentWorld::New_Direction()
{
	// Generate a random direction (0-3) for the actor to move
	int direction = rand() % 4; // 0: up, 1: right, 2: down, 3: left
	// Set the actor's direction based on the generated value
	switch (direction) {
	case 0:
		// Move up
		break;
	case 1:
		// Move right
		break;
	case 2:
		// Move down
		break;
	case 3:
		// Move left
		break;
	default:
		break; // Should never happen
	}
}

// Pickup_Oil - Marks an Oil barrel as being picked up and increments Oil Pickup counter.
void StudentWorld::Pickup_Oil()
{
		//for (Actor* actor : _actors) 
	//{
	//	if (actor->isOilBarrel() && !actor->isPickedUp()) 
	//	{
	//		actor->setPickedUp(true); // Mark the oil barrel as picked up
	//		incrementOilCount(); // Increment the oil count
	//		return; // Exit after picking up one oil barrel
	//	}
	//}
}

//  --- Collision & Movement Checks ---

// Is_Boulder - Checks if there is a boulder in the specified direction from the given coordinates.
bool StudentWorld::Is_Boulder(int x, int y, GraphObject::Direction dir) const
{
	// Calculate the adjacent tile based on direction
	switch (dir) {
	case GraphObject::left:  x -= 1; break;
	case GraphObject::right: x += 1; break;
	case GraphObject::up:    y += 1; break;
	case GraphObject::down:  y -= 1; break;
	default: break;
	}

	//// Now check if any actor at the new (x, y) is a Boulder
	//for (Actor* actor : _actors) {
	//	Boulder* b = dynamic_cast<Boulder*>(actor);
	//	if (b && b->isAlive() && b->getX() == x && b->getY() == y) {
	//		return true;
	//	}
	//}

	return false;
}

// Is_Ice - Checks if there is ice at the given coordinates in a specified direction.
bool StudentWorld::Is_Ice(int x, int y, GraphObject::Direction dir) const
{
	// Calculate the adjacent tile based on direction
	switch (dir) {
	case GraphObject::left:  x -= 1; break;
	case GraphObject::right: x += 1; break;
	case GraphObject::up:    y += 1; break;
	case GraphObject::down:  y -= 1; break;
	default: break;
	}
	// Check if there is ice at the new (x, y)
	for (Ice* ice : _ptrIce) {
		if (ice->getX() == x && ice->getY() == y) {
			return true; // Ice found at the specified coordinates
		}
	}
	return false; // No ice found
}

// No_Ice_Or_Boulder - Checks if there is neither ice nor a boulder in a specified direction from given coordinates.
bool StudentWorld::No_Ice_Or_Boulder(int x, int y, GraphObject::Direction dir) const
{
	// Calculate the adjacent tile based on direction
	switch (dir) {
	case GraphObject::left:  x -= 1; break;
	case GraphObject::right: x += 1; break;
	case GraphObject::up:    y += 1; break;
	case GraphObject::down:  y -= 1; break;
	default: break;
	}
	// Check for ice at the new (x, y)
	if (Is_Ice(x, y, dir)) {
		return false; // Ice found
	}
	// Check for boulders at the new (x, y)
	if (Is_Boulder(x, y, dir)) {
		return false; // Boulder found
	}
	return true; // Neither ice nor boulder found
}

// Can_Fall - Checks if an object at given coords can fall downward (Check if no ice or boulder below it).
bool StudentWorld::Can_Fall(int x, int y) const
{
	// Check if there is ice or a boulder directly below the given coordinates
	if (Is_Ice(x, y - 1, GraphObject::down) || Is_Boulder(x, y - 1, GraphObject::down)) {
		return false; // Cannot fall if there is ice or a boulder below
	}
	return true; // Can fall if no ice or boulder below
}

// Can_Shout - Checks if the Iceman can be shouted at again.
bool StudentWorld::Can_Shout() const
{
	// Check if the Iceman can get shouted at again based on the current game state
	if (_iceman && _iceman->canTakeDamage()) {
		return true; // Iceman can get shouted at
	}
	return false; // Iceman cannot be shouted at
}


int StudentWorld::getRestTime() const
{
	return std::max(0, static_cast<int>(3 - getLevel() / 4));
}

//  - Gameplay & Interactions - [ None of these inteactions work ]
// 
// Near_Iceman - Checks if the Iceman is within a certain distance from a given coordinate.

// Boulder_Annoyed - Allows the boulder to 'damage' any actor.
void StudentWorld::Boulder_Annoyed(int x, int y)
{
	if (_iceman->getX() >= x - 3 && _iceman->getX() <= x + 3 && _iceman->getY() >= y - 3 && _iceman->getY() <= y + 3)
	{ _iceman->die(); }

	Protester_Annoyed(x, y, 100);
}

// Protester_Annoyed - Allows the squirt object to damage protesters.
bool StudentWorld::Protester_Annoyed(int x, int y, int dmg)
{
// Input Protester Damage 
}
// Set_Position - Sets a 4x4 actor in a specified coordinate.
void StudentWorld::Set_Position(int x, int y, char actortype)
{
	for (int row = x; row != x + 4; row++)
		for (int col = y; col != y + 4; col++)
			_actorPositions[row][col] = actortype;
}
// Squirt_Water - Creates a squirt object from the player
// Sonar_Used - Makes all actors within a 12 unit radius from the player visible.

//  - Misc Functions

// Ice Functions
// Spawn_Ice - Spawns ice in the grid, ensuring that the tunnel space is not filled with ice.
void StudentWorld::SpawnIce() 
{
	_ptrIce.clear();
	for (int x = 0; x < 64; x++)
	{
		for (int y = 0; y < 60; y++)
		{
			if (x >= 30 && x <= 33 && y > 3)
				continue; // Skip tunnel space
			_ptrIce.push_back(new Ice(x, y, this));
		}
	}
}

// Remove_Ice - Attempts to remove ice from the grid at the specified coordinates. If successful, it returns true.
bool StudentWorld::removeIceAt(int x, int y)
{
	for (int i = 0; i < _ptrIce.size(); ++i)
	{
		if (_ptrIce[i] && _ptrIce[i]->getX() == x && _ptrIce[i]->getY() == y)
		{
			delete _ptrIce[i];
			_ptrIce[i] = nullptr;
			// play sound effect for ice removal
			return true;
		}
	}
	return false; // No ice was removed at this location
}

//Destructor - Cleans up all actors and ice when the game world is destroyed. including forced closes.
StudentWorld::~StudentWorld() {
	cleanUp(); // Clean up all actors and ice
}
