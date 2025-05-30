#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <queue>

//variables
// private : _nameOfThing
// public : nameOfThing

// Functions :
// private : _NameOfThing()
// public : NameOfThing()

class StudentWorld;  // Forward declaration

// --- Base Actor Class ---
class Actor : public GraphObject
{
public:
    Actor(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, StudentWorld* world);
    virtual ~Actor() = default;

    virtual void doSomething() = 0;

protected:
    StudentWorld* getWorld() const { return _world; }

private:
    StudentWorld* _world;
};

// --- Neutral Actors ---
class Ice : public Actor
{
public:
    Ice(int startX, int startY, StudentWorld* world);
    virtual void doSomething() override;
};

class Boulder : public Actor
{
public:
    Boulder(int startX, int startY, StudentWorld* world);
    virtual void doSomething() override;
};

class Squirt : public Actor
{
public:
    Squirt(int startX, int startY, Direction dir, StudentWorld* world);
    virtual void doSomething() override;
};

// --- Actors with Health ---
class HasHP : public Actor
{
public:
    HasHP(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, StudentWorld* world, int initialHealth);
    virtual ~HasHP() = default;

    void decreaseHealth();
    void setHealth(int health);
    bool isAlive() const;

protected:
    virtual void die() = 0;
    int _health;
};

class Iceman : public HasHP
{
public:
    Iceman(StudentWorld* world);
    virtual void doSomething() override;
    virtual void die() override;

    void sonarAmmoIncrease();
    void goldAmmoIncrease();
    void waterAmmoIncrease();
    void oilGet();
    int getScore() const;
    void loseLife();

private:
    int _oilCount, _waterAmmo, _sonarAmmo, _goldAmmo, _playerScore, _lives;
};

// --- Pickups ---
class PickUp : public Actor
{
public:
    PickUp(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, StudentWorld* world);
    virtual void doSomething() override;

protected:
    void showPickup();
    void sonarPickup();
    void goldPickup();
    void waterPickup();
private:
    bool _pickUpExists = false;
};

class Oil : public PickUp
{
public:
    Oil(int startX, int startY, StudentWorld* world);
    virtual void doSomething() override;
};

class Gold : public PickUp
{
public:
    Gold(int startX, int startY, StudentWorld* world);
    virtual void doSomething() override;
};

class Sonar : public PickUp
{
public:
    Sonar(int startX, int startY, StudentWorld* world);
    virtual void doSomething() override;
};

class WaterPool : public PickUp
{
public:
    WaterPool(int startX, int startY, StudentWorld* world);
    virtual void doSomething() override;
};

#endif // ACTOR_H_
