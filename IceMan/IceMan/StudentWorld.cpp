#include "StudentWorld.h"
#include "Actor.h"

#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <iomanip>
#include <algorithm>
using namespace std;

// Note: A minimal memory leak (~Tree_node<GraphObject*>) persists in snapshots.
// This appears tied to GraphObject or GameController tracking which cannot be modified.
// All dynamic memory under StudentWorld and Actor hierarchy is properly deleted.

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

	// Spawn a RegularProtester [ MOVED to move() ]
	// _actors.push_back(new RegularProtester(this));

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

	// --- Spawn Protesters ---
	int rngHCProtester = min<unsigned int>(90, getLevel() * 10 + 30);
	int G = getLevel() * 25 + 300;

	if (Can_Add_Protester()) {
		int n = rand() % 100 + 1;
		if (n <= rngHCProtester)
		{ _actors.push_back(new HardcoreProtester(this)); }
		else
		{ _actors.push_back(new RegularProtester(this)); }
	}

	// --- Actor Management ---

	for (auto it = _actors.begin(); it != _actors.end(); ++it) 
	{
		Actor* a = *it;

		if (!a || !a->isAlive())
			continue;

		try {
			a->doSomething();
		}
		catch (...) {
			a->setDead();
		}
	}

	// --- Cleanup: Remove dead actors ---
	Remove_Dead_Game_Objects();

	// --- Lose conditions ---
	if (_iceman && !_iceman->isAlive()) {
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}
	// --- Win conditions ---
	if (Finished_Level()) {
		return GWSTATUS_FINISHED_LEVEL;
	}

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

		// If actor is a Protester...
		if (Protester* p = dynamic_cast<Protester*>(actor))
		{
			// CASE 1: Leaving field and has reached the exit � now safe to delete
			if (p->isLeavingField() && p->hasReachedExit())
			{
				delete actor;
				it = _actors.erase(it);
				--_nProtesters;
				continue;
			}

			// CASE 2: Leaving field but not yet at exit � keep alive
			if (p->isLeavingField() && !p->hasReachedExit())
			{
				++it;
				continue;
			}
		}

		// CASE 3: All other actors that are now dead
		if (!actor->isAlive()) {
			Set_Position(actor->getX(), actor->getY(), 0);

			if (dynamic_cast<Protester*>(actor) != nullptr) {
				--_nProtesters;
			}

			delete actor;
			it = _actors.erase(it);
		}
		else {
			++it;
		}
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

void StudentWorld::dropGold(int x, int y) { _actors.push_back(new Gold(x, y, this, true, true, true)); }

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
bool StudentWorld::Is_Boulder(int x, int y, GraphObject::Direction dir) const {
	for (Actor* a : _actors) {
		if (a->getType() == ActorType::Boulder && a->isAlive()) {
			int ax = a->getX();
			int ay = a->getY(); 

			if (abs(ax - x) <= 3 && abs(ay - y) <= 3)
			{ return true; }
		}
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
	int belowY = y - 1;

	if (belowY < 0) return false;  // Can't fall off screen

	// Check for ICE one row below the 4x4 block
	for (int ix = x; ix < x + 4; ix++) {
		for (int iy = belowY; iy < belowY + 1; iy++) {
			for (Ice* ice : _ptrIce) {
				if (!ice || !ice->isAlive()) continue;
				if (ice->getX() == ix && ice->getY() == iy)
					return false;
			}
		}
	}

	// Check for another BOULDER directly below
	for (Actor* actor : _actors) {
		if (!actor || !actor->isAlive()) continue;
		if (actor->getType() != ActorType::Boulder) continue;

		int ax = actor->getX();
		int ay = actor->getY();

		if (ax < x + 4 && ax + 4 > x && ay < y && ay + 4 > belowY)
			return false;
	}

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

	// Clamp to legal bounds � we subtract 3 to ensure 4x4 block stays in bounds
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

// Helper overload to allow position-only checks
bool StudentWorld::canMoveTo(int x, int y) const
{
	// Reuse directional version by simulating "standing still" and moving into tile from above
	// Picks a dummy direction that ends at (x, y)
	// Reverse engineer: start = (x - dx, y - dy), dir = ?
	// Try all 4 directions and see if they lead *to* (x, y)
	for (GraphObject::Direction dir : {GraphObject::left, GraphObject::right, GraphObject::up, GraphObject::down}) {
		int px = x, py = y;
		switch (dir) {
		case GraphObject::left:  px += 1; break;
		case GraphObject::right: px -= 1; break;
		case GraphObject::up:    py -= 1; break;
		case GraphObject::down:  py += 1; break;
		}
		if (canMoveTo(px, py, dir)) return true;
	}
	return false;
}

int StudentWorld::getRestTime() const
{
	return std::max(1, static_cast<int>(3 - getLevel() / 4));
}

//  --- Gameplay & Interactions ---

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

	Protester_Annoyed(x, y, 100, static_cast<int>(HasHP::DamageSource::Boulder));
}

// Protester_Annoyed - Allows the squirt object to damage protesters.
bool StudentWorld::Protester_Annoyed(int x, int y, int dmg, int sourceVal) {
	HasHP::DamageSource source = static_cast<HasHP::DamageSource>(sourceVal);

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
					p->setLastDamage(source);
					p->annoy(dmg); // Deal damage and auto-stun
					return true;
				}
			}
		}
	}
	return false;

}

// --- Pathfinding Functions ---
std::vector<std::pair<int, int>> StudentWorld::getPathToExit(int startX, int startY) {
	const int WIDTH = 64;
	const int HEIGHT = 64;
	std::vector<std::vector<bool>> visited(WIDTH, std::vector<bool>(HEIGHT, false));
	std::vector<std::vector<std::pair<int, int>>> parent(WIDTH, std::vector<std::pair<int, int>>(HEIGHT));
	std::queue<std::pair<int, int>> q;

	q.push({ startX, startY });
	visited[startX][startY] = true;

	const int dirX[4] = { -1, 1, 0, 0 };
	const int dirY[4] = { 0, 0, 1, -1 };

	while (!q.empty()) {
		auto [x, y] = q.front();
		q.pop();
		if (x == 60 && y == 60) break;

		for (int d = 0; d < 4; ++d) {
			int nx = x + dirX[d], ny = y + dirY[d];

			if (nx >= 0 && ny >= 0 && nx < WIDTH && ny < HEIGHT &&
				!visited[nx][ny] &&
				canMoveTo(nx, ny))  //Only go to valid tiles
			{
				visited[nx][ny] = true;
				parent[nx][ny] = { x, y };
				q.push({ nx, ny });
			}
		}
	}

	std::vector<std::pair<int, int>> path;
	int x = 60, y = 60;
	if (!visited[x][y]) return path; // No path

	while (!(x == startX && y == startY)) {
		path.push_back({ x, y });
		auto p = parent[x][y];
		x = p.first;
		y = p.second;
	}
	std::reverse(path.begin(), path.end());
	return path;
}

std::vector<std::pair<int, int>> StudentWorld::getPathToTarget(int startX, int startY, int goalX, int goalY)
{
	std::vector<std::vector<bool>> visited(VIEW_WIDTH, std::vector<bool>(VIEW_HEIGHT, false));
	std::vector<std::vector<std::pair<int, int>>> cameFrom(VIEW_WIDTH, std::vector<std::pair<int, int>>(VIEW_HEIGHT, { -1, -1 }));

	std::queue<std::pair<int, int>> q;
	q.push({ startX, startY });
	visited[startX][startY] = true;

	while (!q.empty()) {
		auto [x, y] = q.front(); q.pop();

		if (x == goalX && y == goalY) break;

		for (auto [dx, dy] : std::vector<std::pair<int, int>>{ {-1,0},{1,0},{0,-1},{0,1} }) {
			int nx = x + dx;
			int ny = y + dy;

			// Bounds check
			if (nx < 0 || ny < 0 || nx >= VIEW_WIDTH || ny >= VIEW_HEIGHT)
				continue;

			// Respect ice and actor collisions
			if (!visited[nx][ny] && canMoveTo(nx, ny)) {
				visited[nx][ny] = true;
				cameFrom[nx][ny] = { x, y };
				q.push({ nx, ny });
			}
		}
	}

	// Reconstruct path
	std::vector<std::pair<int, int>> path;
	if (!visited[goalX][goalY]) return {}; // No path

	int x = goalX, y = goalY;
	while (x != startX || y != startY) {
		path.emplace_back(x, y);
		auto [px, py] = cameFrom[x][y];
		x = px; y = py;
	}

	std::reverse(path.begin(), path.end());
	return path;
}

std::vector<std::pair<int, int>> StudentWorld::computePathFromTo(int startX, int startY, int goalX, int goalY)
{
	std::vector<std::vector<bool>> visited(VIEW_WIDTH, std::vector<bool>(VIEW_HEIGHT, false));
	std::vector<std::vector<std::pair<int, int>>> cameFrom(VIEW_WIDTH, std::vector<std::pair<int, int>>(VIEW_HEIGHT, { -1, -1 }));

	std::queue<std::pair<int, int>> q;
	q.push({ startX, startY });
	visited[startX][startY] = true;

	while (!q.empty()) {
		auto [x, y] = q.front(); q.pop();
		if (x == goalX && y == goalY) break;

		for (auto [dx, dy] : std::vector<std::pair<int, int>>{ {-1,0}, {1,0}, {0,1}, {0,-1} }) {
			int nx = x + dx, ny = y + dy;
			if (canMoveTo(nx, ny) && !visited[nx][ny]) {
				visited[nx][ny] = true;
				cameFrom[nx][ny] = { x, y };
				q.push({ nx, ny });
			}
		}
	}

	// Reconstruct path
	std::vector<std::pair<int, int>> path;
	if (!visited[goalX][goalY]) return {};

	int x = goalX, y = goalY;
	while (x != startX || y != startY) {
		path.emplace_back(x, y);
		std::tie(x, y) = cameFrom[x][y];
	}

	std::reverse(path.begin(), path.end());
	path.shrink_to_fit();

	return path;
}

Protester* StudentWorld::Bribe_Nearby_Protester(int x, int y) {
	for (Actor* actor : _actors) {
		if (!actor || !actor->isAlive()) continue;

		if (actor->getType() == ActorType::RegularProtester || actor->getType() == ActorType::HardcoreProtester) {
			int dx = actor->getX() - x;
			int dy = actor->getY() - y;
			if (dx * dx + dy * dy <= 3 * 3) {
				Protester* p = dynamic_cast<Protester*>(actor);
				if (p && !p->isLeavingField()) {
					return p;  // returns the protester
				}
			}
		}
	}
	return nullptr;
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

//  --- Misc Functions ---

// Ice Functions
// Spawn_Ice - Spawns ice in the grid, ensuring that the tunnel space is not filled with ice.
void StudentWorld::SpawnIce() 
{
	for (Ice* ice : _ptrIce) {
		delete ice;
	}
	_ptrIce.clear();
	_ptrIce.shrink_to_fit();
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
	for (int i = 0; i < _NUMIce; i++) {
		if (_ptrIce[i] != nullptr) {
			int ix = _ptrIce[i]->getX();
			int iy = _ptrIce[i]->getY();

			// Check for overlap with 4x4 block from (x,y)
			if (ix >= x && ix < x + 4 && iy >= y && iy < y + 4) {
				delete _ptrIce[i];
				_ptrIce[i] = nullptr;
				returnVal = true;
			}
		}
	}
	return returnVal;
}

//Destructor - Cleans up all actors and ice when the game world is destroyed. including forced closes.
StudentWorld::~StudentWorld() {
	cleanUp(); // Clean up all actors and ice
}
