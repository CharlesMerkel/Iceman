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
}

void Iceman::die()
{
    setVisible(false);
    setHealth(10);
}

// --- Ice ---
Ice::Ice(int startX, int startY, StudentWorld* world)
    : Actor(IID_ICE, startX, startY, right, 0.25, 3, world)
{
    setVisible(true);
}

void Ice::doSomething() { /* Ice is static; does nothing each tick */ }

// --- Boulder ---
Boulder::Boulder(int startX, int startY, StudentWorld* world)
    : Actor(IID_BOULDER, startX, startY, down, 1.0, 1, world)
{
    setVisible(true);
}

void Boulder::doSomething() { /* Implement Boulder behavior here */ }

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

// --- Squirt ---
Squirt::Squirt(int startX, int startY, Direction dir, StudentWorld* world)
    : Actor(IID_WATER_SPURT, startX, startY, dir, 1.0, 1, world)
{
    setVisible(true);
}

void Squirt::doSomething() { /* Implement Squirt behavior here */ }

// --- Pickups (Oil, Gold, Sonar, WaterPool) ---
Oil::Oil(int startX, int startY, StudentWorld* world)
    : PickUp(IID_BARREL, startX, startY, right, 1.0, 2, world) {
}

void Oil::doSomething() { /* Oil logic */ }

Gold::Gold(int startX, int startY, StudentWorld* world)
    : PickUp(IID_GOLD, startX, startY, right, 1.0, 2, world) {
}

void Gold::doSomething() { /* Gold logic */ }

Sonar::Sonar(int startX, int startY, StudentWorld* world)
    : PickUp(IID_SONAR, startX, startY, right, 1.0, 2, world) {
}

void Sonar::doSomething() { /* Sonar logic */ }

WaterPool::WaterPool(int startX, int startY, StudentWorld* world)
    : PickUp(IID_WATER_POOL, startX, startY, right, 1.0, 2, world) {
}

void WaterPool::doSomething() { /* WaterPool logic */ }