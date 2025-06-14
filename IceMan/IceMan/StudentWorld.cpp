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

//  --- Display & Cleanup ---

// Update_Display_Text - Updates the display with the current statistics.
void StudentWorld::Update_Display_Text()
{
	int level = getLevel();
	int lives = getLives();
	int health = _iceman->getHealth(); // returns 0-10; multiply by 10 for percent
	int water_count = _iceman->getWaterAmmo();
	int gold = _iceman->getGoldAmmo();
	int barrelsLeft = _Barrels - _pickedBarrels;
	int sonar = _iceman->getSonarAmmo();
	int score = getScore();

	// Formatting the string for display
	ostringstream oss;
	oss << "Lvl: " << setw(2) << level
		<< "  Lives: " << setw(1) << lives
		<< "  Hlth: " << setw(3) << (health * 10) << "%"
		<< "  Wtr: " << setw(2) << water_count
		<< "  Gld: " << setw(2) << gold
		<< "  Oil Left: " << setw(2) << barrelsLeft
		<< "  Sonar: " << setw(2) << sonar
		<< "  Scr: " << setw(6) << setfill('0') << score;

	// Display on top of screen
	setGameStatText(oss.str());
}
// No_Overlap - Checks if an actor can be placed in the map without being overlapped with
//              other actors/objects.
bool StudentWorld::No_Overlap(int x, int y) {
	const int maxSqrt = 36; 

	for (const auto* actor : _actors) {
		int dx = actor->getX() - x;
		int dy = actor->getY() - y;
		if (dx * dx + dy * dy <= maxSqrt)
			return false;
	}

	if (Get_Position(x, y) != 0 ||
		Get_Position(x + 3, y) != 0 ||
		Get_Position(x, y + 3) != 0 ||
		Get_Position(x + 3, y + 3) != 0)
		return false;

	return true;
}

// Init - Initializes the game world: ice, boulders, gold, oil, the Iceman thenmself.
//        and also determines the number of actors based off of the current level.
int StudentWorld::init() {

	SpawnIce();
	_ticks = 0;
	_ticksForProtester = _t_LastProtester = max<unsigned int>(25, 200 - getLevel());
	_pickedBarrels = 0;
	_nProtesters = 0;

	// Set boundary
	for (int x = 30; x != 34; x++) {
		for (int y = 4; y != 60; y++) {
			_actorPositions[x][y] = 't';
		}
	}

	// --- Spawn Iceman ---
	_iceman = new Iceman(this);

	// --- Barrel Initialization ---
	_Barrels = min<unsigned int>(2 + getLevel(), 21);

	// --- Boulder Initialization ---
	_Boulders = min<int>(6, getLevel() / 2 + 2);

	for (int j = 0; j < _Barrels; j++) {
		int x = rand() % 61;
		int y = rand() % 57;

		if (No_Overlap(x, y)) {
			Oil* oil = new Oil(x, y, this);
			oil->setType(ActorType::OilBarrel);
			_actors.push_back(oil);
			Set_Position(x, y, 'O');
		}
		else {
			j--; // Retry placement
		}
	}

	// Spawn Boulder (?)
	for (int i = 0; i < _Boulders; i++){
		int x = rand() % 61;
		int y = rand() % 37 + 20;

		if (No_Overlap(x, y)){
			_actors.push_back(new Boulder(x, y, this));
			Remove_Ice_At(x, y);
			Set_Position(x, y, 'B');
		}

		else { i--; }
	}

	// Spawn Gold (?)
	_Gold = max<unsigned int>(5 - getLevel() / 2, 2);

	for (int i = 0; i < _Gold; i++){
		int x = rand() % 61;
		int y = rand() % 37 + 20;

		if (No_Overlap(x, y)) {
			Gold* gold = new Gold(x, y, this, false, true);
			// gold->setType(ActorType::Gold);
			_actors.push_back(gold);
			Set_Position(x, y, 'G');
		}

		else { i--; }
	}

	// Spawn a RegularProtester
	_actors.push_back(new RegularProtester(this));

	return GWSTATUS_CONTINUE_GAME;
}

// Move - The Main game logic, updates the display, spawns most of the actors, has the
//        probablity of spawning hardcore protesters, spawns most of the Pickups & finally 
//        checks if the level is completed or if the player died to start the next map.
int StudentWorld::move() {
	Update_Display_Text();
	_ticks++;
	// --- Iceman Management ---
	if(_iceman && _iceman->isAlive())
	_iceman->doSomething(); //let the iceman act this tick

	// --- Power Up Management ---
	spawnPowerUps();

	// --- Actor Management ---
	for (auto it = _actors.begin(); it != _actors.end(); ++it) 
	{
		Actor* a = *it;

		if (!a || !a->isAlive())
			continue;

		// Catch any unexpected issues in actor logic
		try {
			a->doSomething();
		}
		catch (...) {
			// Safely kill problematic actor and avoid repeated crash
			a->setDead();

			// Optional: Set game stat text for debugging (since you can't see console)
			setGameStatText("Error: Actor crash in doSomething()");
		}
	}

	// --- Lose conditions ---
	if (_iceman && !_iceman->isAlive()) {
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}
	// --- Win conditions ---
	if (Finished_Level()) {
		return GWSTATUS_FINISHED_LEVEL;
	}
	Remove_Dead_Game_Objects();

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

//  --- Actor Management ---
 
void StudentWorld::spawnPowerUps() {
	int dropChance = getLevel() * 25 + 300;
	if (rand() % dropChance == 0) {
		int chance = rand() % 5;
		if (chance == 0)
			_actors.push_back(new Sonar(this));
		else {
			int x = rand() % 61;
			int y = rand() % 61;
			int tries = 100;
			while (!Can_Add_Waterpool(x, y) && tries-- > 0) {
				x = rand() % 61;
				y = rand() % 61;
			}
			if (tries > 0)
				_actors.push_back(new WaterPool(x, y, this));
		}
	}

	//orignal code
		//int dropChance = getLevel() * 25 + 300;
	//int n = rand() % dropChance + 1;

	//if (n <= 1)
	//{
	//	int chance = rand() % 5 + 1;

	//	if (chance <= 1)
	//	{ _actors.push_back(new Sonar(this)); }

	//	else if (chance > 1){
	//		int x = rand() % 61;
	//		int y = rand() % 61;

	//		while (!Can_Add_Waterpool(x, y)){
	//			x = rand() % 61;
	//			y = rand() % 61;
	//		}

	//		_actors.push_back(new WaterPool(x, y, this));
	//	}
	//}
}
// Can_Face - Checks if the Actor can face the Iceman based off of its coordinates &
//current direction, returns false if unable.
bool StudentWorld::Can_Face(int x, int y, GraphObject::Direction& dir){
		bool rv = false;

		if (x >= _iceman->getX() - 63 && x < _iceman->getX() - 4 && y < _iceman->getY() + 4 && y > _iceman->getY() - 4 && !Near_Iceman(x, y, 4)){
			rv = true;
			dir = GraphObject::right;

			for (int i = x; i != _iceman->getX(); i++) {
				if (!No_Ice_Or_Boulder(i, y, GraphObject::right)) { rv = false; }
			}
		}

		else if (x <= _iceman->getX() + 63 && x > _iceman->getX() + 4 && y < _iceman->getY() + 4 && y > _iceman->getY() - 4 && !Near_Iceman(x, y, 4)){
			rv = true;
			dir = GraphObject::left;

			for (int i = x; i != _iceman->getX(); i--) {
				if (!No_Ice_Or_Boulder(i, y, GraphObject::left)) { rv = false; }
			}
		}

		else if (y >= _iceman->getY() - 63 && y < _iceman->getY() - 4 && x < _iceman->getX() + 4 && x > _iceman->getX() - 4 && !Near_Iceman(x, y, 4)){
			rv = true;
			dir = GraphObject::up;

			for (int i = y; i != _iceman->getY(); i++) {
				if (!No_Ice_Or_Boulder(x, i, GraphObject::up)) { rv = false; }
			}
		}

		else if (y <= _iceman->getY() + 63 && y > _iceman->getY() + 4 && x < _iceman->getX() + 4 && x > _iceman->getX() - 4 && !Near_Iceman(x, y, 4)){
			rv = true;
			dir = GraphObject::down;

			for (int i = y; i != _iceman->getY(); i--) {
				if (!No_Ice_Or_Boulder(x, i, GraphObject::down)) { rv = false; }
			}
		}

		return rv;
	}

// Checks if a new protester can spawn based off of the level and tick.
bool StudentWorld::Can_Add_Protester(){
    int maxProtesters = std::min<unsigned int>(15, 2 + getLevel() * 1.5);

	if (_t_LastProtester >= _ticksForProtester && _nProtesters < maxProtesters) {
		_t_LastProtester = 0;
		_nProtesters++;
		return true;
	}

	_t_LastProtester++;
	return false;
}

// Can_Add_Waterpool - Checks if a waterpool can be added at a coordinate, ensuring that ice isn't
// blocking the area.
bool StudentWorld::Can_Add_Waterpool(int x, int y) {
	for (int i = 0; i != _NUMIce; i++) {
		if (_ptrIce[i] != nullptr) {
			if (_ptrIce[i]->getX() >= x && _ptrIce[i]->getX() <= x + 3 && _ptrIce[i]->getY() >= y && _ptrIce[i]->getY() <= y + 3)
			{ return false; }
		}
	}
	return true;
}

//Find_Protester - Searches and returns a protester within a 3x3 area around a point.
void StudentWorld::Find_Protester(int x, int y, vector<Actor*> & foundProtesters)
{
	for (Actor* actor : _actors) 
	{
		ActorType type = actor->getType();
		if ((type == ActorType::RegularProtester || type == ActorType::HardcoreProtester) && abs(actor->getX() - x) <= 3 && abs(actor->getY() - y) <= 3) 
		{
			foundProtesters.push_back(actor);
		}
	}
}

bool StudentWorld::isPlayerStunned() const
{
	if (_iceman)
	{
		return _iceman->isStunned(); // Check if the Iceman is stunned
	}
	return false; // If Iceman is not initialized, return false
}

// Remove_Dead_Game_Objects - Removes all dead actors & updates the actor's position.
void StudentWorld::Remove_Dead_Game_Objects() 
{
	for (auto it = _actors.begin(); it != _actors.end(); ) {
		Actor* actor = *it;

		// Don't delete Iceman if somehow in the actor list
		if (actor == _iceman) {
			++it;
			continue;
		}

		if (!actor->isAlive()) {
			Set_Position(actor->getX(), actor->getY(), 0);
			if (dynamic_cast<Protester*>(actor) != nullptr) { --_nProtesters; }

			delete actor;
			it = _actors.erase(it); 
		}
		else { ++it; }
	}
}

bool StudentWorld::inLineOfSightToPlayer(int x, int y, GraphObject::Direction& outDir) const
{
	int px = _iceman->getX();
	int py = _iceman->getY();

	// Same column?
	if (x == px) {
		outDir = (py > y) ? GraphObject::up : GraphObject::down;
		for (int iy = std::min(y, py) + 1; iy < std::max(y, py); ++iy) {
			if (Is_Boulder(x, iy, outDir) || Is_Ice(x, iy, outDir))
				return false;
		}
		return true;
	}

	// Same row?
	if (y == py) {
		outDir = (px > x) ? GraphObject::right : GraphObject::left;
		for (int ix = std::min(x, px) + 1; ix < std::max(x, px); ++ix) {
			if (Is_Boulder(ix, y, outDir) || Is_Ice(ix, y, outDir))
				return false;
		}
		return true;
	}

	return false;
}

void StudentWorld::dropGold(int x, int y) { _actors.push_back(new Gold(x, y, this, true, true)); }

//  --- Game State & Level Progression ---

// Finished_Level - Returns true if the player picked up all the oil.
bool StudentWorld::Finished_Level()
{
	for (Actor* actor : _actors)
	{
		if (!actor) continue;

		// Optional: safer type check
		ActorType type;
		try {
			type = actor->getType();
		}
		catch (...) {
			continue;
		}

		if (type == ActorType::OilBarrel && !actor->isPickedUp()) {
			return false;
		}
	}
	return true;
}

// Player_Died - Checks if the player died, if so, then decrement lives and returns true.
bool StudentWorld::Player_Died()
{
	if(!_iceman->isAlive()) // Check if the Iceman's health is zero or less
	{
		// Decrement lives and return true to indicate player death
		_iceman->loseLife();
		return true;
	}
	return false; // Player is still alive
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
void StudentWorld::Pickup_Oil(int x, int y)
{
	Set_Position(x, y, 0);
	_pickedBarrels++;
}

//  --- Collision & Movement Checks ---

// Is_Boulder - Checks if there is a boulder in the specified direction from the given coordinates.
bool StudentWorld::Is_Boulder(int x, int y, GraphObject::Direction dir) const
{
	// Calculate the adjacent tile based on direction
	switch (dir) {
	case GraphObject::right:
		if (Get_Position(x + 3, y) == 'B' || Get_Position(x + 3, y + 3) == 'B')
		{ return true; }
		break;
	case GraphObject::left:
		if (Get_Position(x, y) == 'B' || Get_Position(x, y + 3) == 'B')
		{ return true; }
		break;
	case GraphObject::up:
		if (Get_Position(x, y + 3) == 'B' || Get_Position(x + 3, y + 3) == 'B')
		{ return true; }
		break;
	case GraphObject::down:
		if (Get_Position(x, y) == 'B' || Get_Position(x + 3, y) == 'B')
		{ return true; }
		break;
	}

	return false;
}

// Is_Ice - Checks if there is ice at the given coordinates in a specified direction.
bool StudentWorld::Is_Ice(int x, int y, GraphObject::Direction dir) const {
	switch (dir)
	{
	case GraphObject::down:
		for (int i = 0; i < _NUMIce; i++)
			if (_ptrIce[i] != nullptr){
				if (_ptrIce[i]->getX() >= x && _ptrIce[i]->getX() <= x + 3 && _ptrIce[i]->getY() == y)
				{ return true; }	
			}
		break;

	case GraphObject::up:
		for (int i = 0; i < _NUMIce; i++)
			if (_ptrIce[i] != nullptr){
				if (_ptrIce[i]->getX() >= x && _ptrIce[i]->getX() <= x + 3 && _ptrIce[i]->getY() == y + 3)
				{ return true; }
			}
		break;

	case GraphObject::right:
		for (int i = 0; i < _NUMIce; i++)
			if (_ptrIce[i] != nullptr){
				if (_ptrIce[i]->getY() >= y && _ptrIce[i]->getY() <= y + 3 && _ptrIce[i]->getX() == x + 3)
				{ return true; }
			}
		break;

	case GraphObject::left:
		for (int i = 0; i < _NUMIce; i++)
			if (_ptrIce[i] != nullptr){
				if (_ptrIce[i]->getY() >= y && _ptrIce[i]->getY() <= y + 3 && _ptrIce[i]->getX() == x)
				{ return true; }
			}
		break;
	}

	return false;
}

// No_Ice_Or_Boulder - Checks if there is neither ice nor a boulder in a specified direction from given coordinates.
bool StudentWorld::No_Ice_Or_Boulder(int x, int y, GraphObject::Direction dir) const {
	if (!Is_Ice(x, y, dir) && !Is_Boulder(x, y, dir)) { return true; }

	return false;
}

// Can_Fall - Checks if an object at given coords can fall downward (Check if no ice or boulder below it).
bool StudentWorld::Can_Fall(int x, int y) const {
	if (y < 0) { return false; }
	if (!No_Ice_Or_Boulder(x, y, GraphObject::down))
	{ return false;}
	return true;
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

bool StudentWorld::canMoveTo(int x, int y, GraphObject::Direction dir) const
{
	// Calculate new position after moving
	switch (dir) {
	case GraphObject::left:  x -= 1; break;
	case GraphObject::right: x += 1; break;
	case GraphObject::up:    y += 1; break;
	case GraphObject::down:  y -= 1; break;
	}

	// Clamp to legal bounds — we subtract 3 to ensure 4x4 block stays in bounds
	if (x < 0 || x > 60 || y < 0 || y > 60)
		return false;

	// Don't allow movement through tunnel shaft
	if (x >= 30 && x <= 33 && y >= 4 && y <= 59)
		return false;

	// Block movement if any tile in the 4x4 destination block contains ice
	for (Ice* ice : _ptrIce) {
		if (ice == nullptr ||!ice->isAlive())
			continue;

		int ix = ice->getX();
		int iy = ice->getY();

		if (ix >= x && ix < x + 4 && iy >= y && iy < y + 4)
			return false;
	}

	// Check for boulder blocking destination
	for (Actor* actor : _actors) {
		if (actor == nullptr || !actor->isAlive()) continue;
		if (actor->getType() != ActorType::Boulder) continue;

		int bx = actor->getX();
		int by = actor->getY();
		if (bx >= x && bx < x + 4 && by >= y && by < y + 4)
			return false;
	}

	return true;
}

int StudentWorld::getRestTime() const
{
	return std::max(0, static_cast<int>(3 - getLevel() / 4));
}

//  --- Gameplay & Interactions ---
// [ None of these inteactions work ]

// Near_Iceman - Checks if the Iceman is within a certain distance from a given coordinate.
bool StudentWorld::Near_Iceman(int x, int y, int actortype) {
	int dx = _iceman->getX() - x;
	int dy = _iceman->getY() - y;

	int distSqrt = dx * dx + dy * dy;
	return distSqrt <= actortype * actortype;
}
// Boulder_Annoyed - Allows the boulder to 'damage' any actor.
void StudentWorld::Boulder_Annoyed(int x, int y)
{
	if (_iceman->getX() >= x - 3 && _iceman->getX() <= x + 3 && _iceman->getY() >= y - 3 && _iceman->getY() <= y + 3)
	{ _iceman->die(); }

	Protester_Annoyed(x, y, 100);
}

// Protester_Annoyed - Allows the squirt object to damage protesters.
bool StudentWorld::Protester_Annoyed(int x, int y, int dmg) {
	for (Actor* actor : _actors)
	{
		if (!actor || !actor->isAlive())
			continue;

		ActorType type = actor->getType();
		if (type == ActorType::RegularProtester || type == ActorType::HardcoreProtester)
		{
			int dx = actor->getX() - x;
			int dy = actor->getY() - y;
			if (dx * dx + dy * dy <= 3 * 3)
			{
				Protester* p = dynamic_cast<Protester*>(actor);
				if (p && !p->isStunned())
				{
					p->annoy(dmg); // Deal damage and auto-stun
					return true;
				}
			}
		}
	}
	return false;

}

// Set_Position - Sets a 4x4 actor in a specified coordinate.
bool StudentWorld::Set_Position(int x, int y, char actortype)
{
	for (int row = x; row != x + 4; row++) 
		for (int col = y; col != y + 4; col++)
			_actorPositions[row][col] = actortype;

	return true;
}

// Squirt_Water - Creates a squirt object from the player
void StudentWorld::Squirt_Water(int x, int y, GraphObject::Direction dir){
	_actors.push_back(new Squirt(x, y, dir, this));
	GameController::getInstance().playSound(SOUND_PLAYER_SQUIRT);
}
// Sonar_Used - Makes all actors within a 12 unit radius from the player visible.
void StudentWorld::Sonar_Used(int x, int y) {
	const int range = 144;

	for (Actor* actor : _actors) {
		int dx = actor->getX() - x;
		int dy = actor->getY() - y;
		int distSquared = dx * dx + dy * dy;

		if (distSquared <= range) { actor->setVisible(true); }
	}

	GameController::getInstance().playSound(SOUND_SONAR);
}

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

// Remove_Ice_At - Attempts to remove ice from the grid at the specified coordinates. If successful, it returns true.
bool StudentWorld::Remove_Ice_At(int x, int y)
{
	bool returnVal = false;
	for (int i = 0; i < _NUMIce; i++)
		if (_ptrIce[i] != nullptr) {
			if (_ptrIce[i]->getX() >= x && _ptrIce[i]->getX() <= x + 3 && _ptrIce[i]->getY() >= y && _ptrIce[i]->getY() <= y + 3)
			{
				delete _ptrIce[i];
				_ptrIce[i] = nullptr;
				returnVal = true;
			}
		}
	return returnVal; // No Ice returned
}

//Destructor - Cleans up all actors and ice when the game world is destroyed. including forced closes.
StudentWorld::~StudentWorld() {
	cleanUp(); // Clean up all actors and ice
}
