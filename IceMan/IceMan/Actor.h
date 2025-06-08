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
	~Ice() override;
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
    ~Iceman();
    virtual void doSomething() override;
    virtual void die() override;

    void sonarAmmoIncrease();
    void goldAmmoIncrease();
    void waterAmmoIncrease();
    void oilGet();
    int getScore() const;
    void loseLife();
    bool canTakeDamage() const;

private:
    int _oilCount, _waterAmmo, _sonarAmmo, _goldAmmo, _playerScore, _lives;
    bool _canTakeDamage = true;
};

// --- Protester Base Class ---
class Protester : public HasHP
{
public:
    Protester(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, StudentWorld* world);

    virtual ~Protester() = default;
    virtual void doSomething() override;
    virtual void die() override;

    void setLeaveField(bool leave = true) { _leavingField = leave; }
    bool isLeavingField() const { return _leavingField; }

    void setStunned(bool stunned = true) { _stunned = stunned; }
    bool isStunned() const { return _stunned; }

    void setRestingTime(int time) { _restingTime = time; }
    int getRestingTime() const { return _restingTime; }

protected:
    bool _leavingField = false;
    bool _stunned = false;
    int _restingTime = 0;
};

// --- Regular Protester ---
class RegularProtester : public Protester
{
public:
    RegularProtester(StudentWorld* world)
        : Protester(IID_PROTESTER, 60, 60, left, 1.0, 0, world)
    {
        // Customize starting position or behavior as needed
    }

    virtual void doSomething() override;
    virtual void die() override;
};

// --- Hardcore Protester ---
class HardcoreProtester : public Protester
{
public:
    HardcoreProtester(StudentWorld* world)
        : Protester(IID_HARD_CORE_PROTESTER, 60, 60, left, 1.0, 0, world)
    {
        // Customize starting position or behavior as needed
    }

    virtual void doSomething() override;
    virtual void die() override;
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
