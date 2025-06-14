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

enum class ActorType
{
    Iceman,
    Ice,
    Boulder,
    OilBarrel,
    Gold,
    Sonar,
    WaterPool,
	RegularProtester,
	HardcoreProtester,
	Squirt,
	Unknown //placeholder for unknown actor types
};

// --- Base Actor Class ---
class Actor : public GraphObject
{
public:
    Actor(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, StudentWorld* world);
    virtual ~Actor() = default;

    // recommended v const
    bool isAlive() const { return _swAlive; }
    virtual void setDead() { _swAlive = false; }

    virtual void doSomething() = 0;

    virtual ActorType getType() const;
    void setType(ActorType type) { _type = type; }
    virtual bool isPickedUp() const { return false; } // default for non-pickups

protected:
    StudentWorld* getWorld() const { return _world; }

private:
    StudentWorld* _world;
    ActorType _type = ActorType::Unknown; // Default to avoid undefined behavior
    bool _swAlive;
};

// --- Neutral Actors ---
class Ice : public Actor
{
public:
    Ice(int startX, int startY, StudentWorld* world);
	~Ice() override;
    virtual ActorType getType() const override { return ActorType::Ice; }
    virtual void doSomething() override;
};

class Boulder : public Actor
{
public:
    Boulder(int startX, int startY, StudentWorld* world);
    virtual ActorType getType() const override { return ActorType::Boulder; }
    virtual void doSomething() override;

private:
    int _bState = 0; // Boulder's State
    int _tick = 0;
};

class Squirt : public Actor
{
public:
    Squirt(int startX, int startY, Direction dir, StudentWorld* world);
    virtual ActorType getType() const override { return ActorType::Squirt; }
    virtual void doSomething() override;

private:
    int _sDistance; 
};

// --- Actors with Health ---
class HasHP : public Actor
{
public:
    HasHP(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, StudentWorld* world, int initialHealth);
    virtual ~HasHP() = default;

    void decreaseHealth(int amount);
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

	//amount increase functions
    void sonarAmmoIncrease() { _sonarAmmo++; }
    void goldAmmoIncrease() { _goldAmmo++; }
    void waterAmmoIncrease() { _waterAmmo += 5; }
    void oilGet();

    //score and life
    int getScore() const;
    void loseLife();
	void annoy(int amount);

	//damage and stun handling
    bool canTakeDamage() const;
    bool isStunned() const { return _isStunned; }
	void setStunned(bool stunned = true) { _canTakeDamage = !stunned; }

    //type setting
    virtual ActorType getType() const override { return ActorType::Iceman; }

    //getters for display text
	int getHealth() const { return _health; }
	int getWaterAmmo() const { return _waterAmmo; }
	int getGoldAmmo() const { return _goldAmmo; }
	int getSonarAmmo() const { return _sonarAmmo; }
	int getOilCount() const { return _oilCount; }
	int getLives() const { return _lives; }

private:
    int _oilCount, _waterAmmo, _sonarAmmo, _goldAmmo, _playerScore, _lives;

    bool _canTakeDamage = true;
	bool _isStunned = false;
};

// --- Protester Base Class ---
class Protester : public HasHP
{
public:
    Protester(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, StudentWorld* world);

    virtual ~Protester() = default;
    virtual void doSomething() override;
    virtual void die() override;

    virtual bool is_Protester() { return true; }
    virtual void take_Damage(int dmg) { ; }

    void setLeaveField(bool leave = true) { _leavingField = leave; }
    bool isLeavingField() const { return _leavingField; }
    std::vector<std::pair<int, int>> _exitPath;
    
    void setStunned(bool stunned = true) { _stunned = stunned; }
    bool isStunned() const { return _stunned; }
	void annoy(int amount);

    void setRestingTime(int time) { _restingTime = time; }
    int getRestingTime() const { return _restingTime; }

    void moveInDirection(GraphObject::Direction dir);
    void chooseNewDirection();
    int dyForDir(Direction dir) const;
    int dxForDir(Direction dir) const;

	void reduceShoutCooldown() { if (_shoutCooldown > 0) _shoutCooldown--; }
	bool canShout() const { return _shoutCooldown <= 0; }
	void resetShoutCooldown() { _shoutCooldown = 15; } // Reset to 15 ticks

protected:
    bool _leavingField = false;
    bool _stunned = false;
    int _restingTime = 0;
	int _shoutCooldown = 0; // Cooldown for shouting
	int _numStepsInCurrentDirection = 0; // Steps in the current directionq
	GraphObject::Direction _currentDirection; // Current direction of the protester
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
    virtual ActorType getType() const override { return ActorType::RegularProtester; }
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
    virtual ActorType getType() const override { return ActorType::HardcoreProtester; }
    virtual void doSomething() override;
    virtual void die() override;
};

// --- Pickups ---
class PickUp : public Actor
{
public:
    PickUp(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, StudentWorld* world);
    //virtual void doSomething() override;

    //recommended v const
    bool isVisible() const { return _pickVisible; }

    void setVisible(bool vis) {
        GraphObject::setVisible(vis);
        _pickVisible = vis;
    }

    void setPickup(bool exists) { _pickUpExists = exists; }

    virtual bool isPickedUp() const override { return !_pickUpExists; }

    //recommended v const
    int getTick() const { return _tickSpan; }

    void reduceTick() { _tickSpan--; }

    void setTick(int amt) { _tickSpan = amt; }

protected:
    void showPickup();
    void sonarPickup();
    void goldPickup();
    void waterPickup();

private:
    bool _pickVisible;
    bool _pickUpExists = false;
    int _tickSpan;
};

class Oil : public PickUp
{
public:
    Oil(int startX, int startY, StudentWorld* world);
    virtual ActorType getType() const override { return ActorType::OilBarrel; }
    virtual void doSomething() override;
	bool isPickedUp() const override { return !isAlive(); } // Override to indicate pickup state
};

class Gold : public PickUp
{
public:
    Gold(int startX, int startY, StudentWorld* world, bool isVisible, bool exists);
    virtual ActorType getType() const override { return ActorType::Gold; }
    virtual void doSomething() override;
};

class Sonar : public PickUp
{
public:
    Sonar(StudentWorld* world);
    virtual ActorType getType() const override { return ActorType::Sonar; }
    virtual void doSomething() override;
};

class WaterPool : public PickUp
{
public:
    WaterPool(int startX, int startY, StudentWorld* world);
    virtual ActorType getType() const override { return ActorType::WaterPool; }
    virtual void doSomething() override;
};

#endif // ACTOR_H_
