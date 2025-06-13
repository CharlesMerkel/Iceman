#include "Actor.h"
#include "StudentWorld.h"
using namespace std;


// --- Actor ---
Actor::Actor(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, StudentWorld* world)
    : GraphObject(imageID, startX, startY, dir, size, depth), _world(world), _swAlive(true)
{
    setVisible(true);
}

ActorType Actor::getType() const { return ActorType::Unknown; } // Default implementation, can be overridden

// --- HasHP ---
HasHP::HasHP(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, StudentWorld* world, int initialHealth)
    : Actor(imageID, startX, startY, dir, size, depth, world), _health(initialHealth) { }

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
void HasHP::setHealth(int health) { _health = health; }

// Check if still alive
bool HasHP::isAlive() const { return _health > 0; }

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
            getWorld()->Remove_Ice_At(icemanX + dx, icemanY + dy);
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
	return true; // Iceman can take damage
}

bool Iceman::isStunned() const { return _isStunned; }
void Iceman::loseLife()
{
    getWorld()->decLives();
}
//void Iceman::setStunned(bool stunned) { _isStunned = stunned; } This was already defined in Actor.h
void setStunned(bool stunned){}

// --- Protestor --
Protester::Protester(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, StudentWorld* world)
    : HasHP(imageID, startX, startY, dir, size, depth, world, 5) // 5 HP for Regular Protester, override for Hardcore
{
    setVisible(true);
    _leavingField = false;
    _stunned = false;
    _restingTime;
}

void Protester::doSomething()
{
    if (!isAlive()) return;

	reduceShoutCooldown(); // Reduce shout cooldown each tick

	//stores the Iceman's position
    int iceX = getWorld()->getIceman()->getX();
    int iceY = getWorld()->getIceman()->getY();

    GraphObject::Direction dir;
    if(getWorld()->inLineOfSightToPlayer(getX(),getY(), dir) && canShout())
    {
        setDirection(dir);
        // Shout at the player
        getWorld()->playSound(SOUND_PROTESTER_YELL);
        resetShoutCooldown(); // Reset shout cooldown
		_restingTime = getWorld()->getRestTime(); // Set resting time after shouting
        return;
	}

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
    setVisible(true);
}

void Boulder::doSomething() { 
    /* Implement Boulder behavior here */
    if (!isAlive()) { return; }

    if (_bState == 0) 
    {
        if (!getWorld()->Is_Ice(getX(), getY(), GraphObject::down)) 
        {
            _bState = 1;
            _tick = 0;
        }
    }

    else if (_bState == 1 && _tick < 30) { _tick++; }

    else if (_bState == 1 && _tick == 30) {
        _bState = 2;
        // start playing sfx
    }

    else if (_bState == 2) {
        if (getWorld()->Can_Fall(getX(), getY() - 1)) {
            getWorld()->Set_Position(getX(), getY(), 0);
            moveTo(getX(), getY() - 1);
            getWorld()->Boulder_Annoyed(getX(), getY());
            getWorld()->Set_Position(getX(), getY(), 'B');
        }
        else if (!getWorld()->Can_Fall(getX(), getY() - 1)) { setDead(); }
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
    if (getWorld()->Protester_Annoyed(getX(), getY(), 2)) { setDead();; }
    if (_sDistance == 4) { setDead();; }

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
    _tickSpan = max<unsigned int>(100, 300 - 10 * (getWorld()->getLevel()));
}

/* PickUp doesn't need a doSomething()
void PickUp::doSomething()
{
    // Placeholder logic for PickUp actions
    // This can be customized to check collisions, player interactions, etc.
}
*/
// --- Pickups (Oil, Gold, Sonar, WaterPool) ---
Oil::Oil(int startX, int startY, StudentWorld* world)
    : PickUp(IID_BARREL, startX, startY, right, 1.0, 2, world) {
    setVisible(false);
}

void Oil::doSomething() { 
    /* Oil logic */
    if (!isAlive()) { return; }

    if (!isVisible() && getWorld()->Near_Iceman(getX(), getY(), 4)) {
        setVisible(true);
        return;
    }

    else if (getWorld()->Near_Iceman(getX(), getY(), 3)) {
        setDead();
        //play sfx
        getWorld()->increaseScore(1000);
        getWorld()->Pickup_Oil(getX(), getY());
    }

}

Gold::Gold(int startX, int startY, StudentWorld* world, bool isVisible, bool canpick)
    : PickUp(IID_GOLD, startX, startY, right, 1.0, 2, world) {
    setVisible(true);
    setPickup(true);
    setTick(100);
}

void Gold::doSomething() { 
    /* Gold logic */
    if (!isAlive()) { return; }

    if (!isVisible() && getWorld()->Near_Iceman(getX(), getY(), 3)) {
        setDead();
        //play sfx
        getWorld()->increaseScore(10);
        getWorld()->Iceman_ptr()->goldAmmoIncrease();
    }

    else if (!isPickedUp()) {
        std::vector<Actor*> protesters;
        if (!protesters.empty()) {
            if (getTick() == 0) { setDead(); }

            else { reduceTick(); }
        }
        else {
            setDead();
            // bribe the actor
        }
    }
}

Sonar::Sonar(StudentWorld* world)
    : PickUp(IID_SONAR, 0, 60, right, 1.0, 2, world) {
    setVisible(true);
    setPickup(true);
}

void Sonar::doSomething() { 
    /* Sonar logic */ 
    if (!isAlive()) { return; }

    if (getWorld()->Near_Iceman(getX(), getY(), 3)) {
        setDead();
        //play sfx
        getWorld()->increaseScore(75);
        getWorld()->Iceman_ptr()->sonarAmmoIncrease();
    }

    else if (getTick() == 0) { setDead(); }
    reduceTick();
}

WaterPool::WaterPool(int startX, int startY, StudentWorld* world)
    : PickUp(IID_WATER_POOL, startX, startY, right, 1.0, 2, world) {
    setVisible(true);
    setPickup(true);
}

void WaterPool::doSomething() { 
    /* WaterPool logic */
    if (!isAlive()) { return; }

    if (getWorld()->Near_Iceman(getX(), getY(), 3)) {
        setDead();
        //play sfx
        getWorld()->increaseScore(100);
        getWorld()->Iceman_ptr()->waterAmmoIncrease();
    }

    else if (getTick() == 0) { setDead(); }
    reduceTick();
}