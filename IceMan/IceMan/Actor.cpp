#include "Actor.h"
#include "StudentWorld.h"
using namespace std;


// --- Actor ---
Actor::Actor(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, StudentWorld* world)
    : GraphObject(imageID, startX, startY, dir, size, depth), _world(world)
{
    setVisible(true);
}

// --- HasHP ---
HasHP::HasHP(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, StudentWorld* world, int initialHealth)
    : Actor(imageID, startX, startY, dir, size, depth, world), _health(initialHealth)
{
}

// Decrease health by 1
void HasHP::decreaseHealth()
{
    _health--;
    if (_health <= 0)
    {
        die();  // Trigger the die behavior if health drops to zero
    }
}

// Set the health explicitly
void HasHP::setHealth(int health)
{
    _health = health;
}

// Check if still alive
bool HasHP::isAlive() const
{
    return _health > 0;
}

// --- Iceman ---
Iceman::Iceman(StudentWorld* world)
    : HasHP(IID_PLAYER, 30, 60, right, 1.0, 0, world, 10),
    _waterAmmo(5), _sonarAmmo(1), _goldAmmo(0), _playerScore(0), _oilCount(0), _lives(3)
{
    setVisible(true);
}
Iceman::~Iceman() 
{
	setVisible(false);
}

void Iceman::doSomething()
{
    if (!isAlive()) return;

    int key;
    if (getWorld()->getKey(key))
    {
        switch (key)
        {
        case KEY_PRESS_LEFT:
        case 'a':
            if (getDirection() != left) setDirection(left);
            else if (getX() > 0) moveTo(getX() - 1, getY());
            break;
        case KEY_PRESS_RIGHT:
        case 'd':
            if (getDirection() != right) setDirection(right);
            else if (getX() < 60) moveTo(getX() + 1, getY());
            break;
        case KEY_PRESS_UP:
        case 'w':
            if (getDirection() != up) setDirection(up);
            else if (getY() < 60) moveTo(getX(), getY() + 1);
            break;
        case KEY_PRESS_DOWN:
        case 's':
            if (getDirection() != down) setDirection(down);
            else if (getY() > 0) moveTo(getX(), getY() - 1);
            break;
        }
    }
	//this is the code for Iceman breaking ice
    int icemanX = getX();
    int icemanY = getY();

    for (int dx = 0; dx < 4; ++dx)
    {
        for (int dy = 0; dy < 4; ++dy)
        {
            getWorld()->removeIceAt(icemanX + dx, icemanY + dy);
        }
    }
}

void Iceman::die() //this is wrong Fixlater
{
    setVisible(false);
    setHealth(10);
}

bool Iceman::canTakeDamage() const
{
    if(getWorld()->isPlayerStunned())
    {
        return false; // Iceman is stunned, cannot take damage
	}
}

// --- Protestor --
Protester::Protester(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, StudentWorld* world)
    : HasHP(imageID, startX, startY, dir, size, depth, world, 5) // 5 HP for Regular Protester, override for Hardcore
{
    setVisible(true);
    _leavingField = false;
    _stunned = false;
    _restingTime = 0;
}

void Protester::doSomething()
{
    if (!isAlive()) return;

    if (isLeavingField())
    {
        if (getX() == 60 && getY() == 60)
        {
			setVisible(false);
            return;
        }
		//move toward exit


        //rest after moving 
        _restingTime = getWorld()->getRestTime();
        return;
    }




    if (_restingTime > 0)
    {
        _restingTime--;
        return;
    }

    // Basic protester logic goes here
    // E.g., shout at player, walk toward player, etc.
}

void Protester::die()
{
    if (!isLeavingField())
    {
        setLeaveField();
		// add pathing logic to exit the field
    }
}

// --- RegularProtestor ---
void RegularProtester::doSomething()
{
    Protester::doSomething(); // Extend or override behavior
    // Additional logic for regular protester
}

void RegularProtester::die()
{
    Protester::die();
    // Maybe play sound, add score, etc.
}

// --- HardcoreProtestor ---
void HardcoreProtester::doSomething()
{
    Protester::doSomething();
    // More aggressive pathfinding or resistance logic here
}

void HardcoreProtester::die()
{
    Protester::die();
    // Different sound/score/etc
}

// --- Ice ---
Ice::Ice(int startX, int startY, StudentWorld* world)
    : Actor(IID_ICE, startX, startY, right, 0.25, 3, world)
{
    setVisible(true);
}

Ice::~Ice()
{
    setVisible(false);
}

void Ice::doSomething() { /* Ice is static; does nothing each tick */ }

// --- Boulder --- [ Everything past this might not work ] 
Boulder::Boulder(int startX, int startY, StudentWorld* world)
    : Actor(IID_BOULDER, startX, startY, down, 1.0, 1, world)
{
    _bState = 0;
    setVisible(true);
}

void Boulder::doSomething() { 
    /* Implement Boulder behavior here */
    if (!isAlive()) { return; }

    if (_bState == 0) {
        if (!getWorld()->is_Ice(getX(), getY() - 1, down) {
            _bState = 1;
            tick = 0;
       }
    }

    else if (_bState == 1 && tick < 30) { tick++; }

    else if (_bState == 1 && tick == 30) {
        _bState = 2;
        // start playing sfx
    }

    else if (_bState == 2) {
        if (getWorld()->Can_Fall(getX(), getY() - 1)) {
            getWorld()->Set_Position(getX(), getY(), 0);
            moveTo(getX(), getY() - 1);
            // Needs both Boulder_Annoyed & Protester_Annoyed to function
            getWorld()->Set_Position(getX(), getY(), 'B');
        }
        else if (!getWorld()->Can_Fall(getX(), getY() - 1)) { IsDead(); }
    }
}

// --- Squirt ---
Squirt::Squirt(int startX, int startY, Direction dir, StudentWorld* world)
    : Actor(IID_WATER_SPURT, startX, startY, dir, 1.0, 1, world)
{
    setVisible(true);
    _sDistance = 0;
}

void Squirt::doSomething() { 
    /* Implement Squirt behavior here */
    if (getWorld()->Protester_Annoyed) { setDead; }
    if (_sDistance == 4) { setDead; }

    else if (getDirection() == up) {
        if (!getWorld()->No_Ice_Or_Boulder(getX(), getY() + 1, up) || getY() >= 60) { setDead(); }

        else {
            moveTo(getX(), getY() + 1);
            _sDistance++;
        }
    }

    else if (getDirection() == down) {
        if (!getWorld()->No_Ice_Or_Boulder(getX(), getY() - 1, down) || getY() <= 0) { setDead(); }

        else {
            moveTo(getX(), getY() - 1);
            _sDistance++;
        }
    }

    else if (getDirection() == left) {
        if (!getWorld()->No_Ice_Or_Boulder(getX() - 1, getY(), left) || getX() <= 0) { setDead(); }

        else {
            moveTo(getX() - 1, getY());
            _sDistance++;
        }
    }

    else if (getDirection() == right) {
        if (!getWorld()->No_Ice_Or_Boulder(getX() + 1, getY(), right) || getX() >= 60) { setDead(); }

        else {
            moveTo(getX() + 1, getY());
            _sDistance++;
        }
    }
}

// --- PickUp ---
PickUp::PickUp(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, StudentWorld* world)
    : Actor(imageID, startX, startY, dir, size, depth, world)
{
    setVisible(true);
}

void PickUp::doSomething()
{
    // Placeholder logic for PickUp actions
    // This can be customized to check collisions, player interactions, etc.
}

// --- Pickups (Oil, Gold, Sonar, WaterPool) ---
Oil::Oil(int startX, int startY, StudentWorld* world)
    : PickUp(IID_BARREL, startX, startY, right, 1.0, 2, world) {
    setVisible(false);
}

void Oil::doSomething() { /* Oil logic */ }

Gold::Gold(int startX, int startY, StudentWorld* world)
    : PickUp(IID_GOLD, startX, startY, right, 1.0, 2, world) {
    setVisible(isVisible);
    setPickup(exists);
    setTick(100);
}

void Gold::doSomething() { /* Gold logic */ }

Sonar::Sonar(StudentWorld* world)
    : PickUp(IID_SONAR, 0, 60, right, 1.0, 2, world) {
    setVisible(true);
    setPickup(true);
}

void Sonar::doSomething() { /* Sonar logic */ }

WaterPool::WaterPool(StudentWorld* world)
    : PickUp(IID_WATER_POOL, startX, startY, right, 1.0, 2, world) {
    setVisible(true);
    setPickup(true);
}

void WaterPool::doSomething() { /* WaterPool logic */ }