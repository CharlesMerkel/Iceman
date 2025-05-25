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

class StudentWorld; // Forward declaration to avoid circular dependency

class Actor : public GraphObject
{
public:
	Actor(int imageID, int startX, int startY, Direction dir = right, double size = 1.0,
		unsigned int depth = 0, StudentWorld* ptrStudWrld)
		: GraphObject(imageID, startX, startY, dir, size, depth), _ptrStudentWorld(ptrStudWrld)

		//m_brightness(1.0), m_animationNumber(0), m_direction(dir), m_destX(startX), m_destY(startY), 
	{
		setVisible(true);
	}
	//pure virtual function
	virtual void doSomething() = 0;
	//virtual destructor
	virtual ~Actor() = default;

protected:
	StudentWorld* getWorld() const{	return _ptrStudentWorld;}
private:
	//ID
	//starting direction
	//depth
	//size
	//location (colum, row)
	//setVisible(T)

	StudentWorld* _ptrStudentWorld;
};

// Neutral Actors
class Ice : public Actor
{
public:
	Ice(int imageID, int startX, int startY, Direction dir = right, double size = 1.0,
		unsigned int depth = 3, StudentWorld* ptrStudWrld) : Actor(IID_ICE, startX, startY, dir, size, depth, ptrStudWrld)
	{ setVisible(true); }

	virtual void doSomething() { ; }

	virtual ~Ice() { ; }
private:
};

//class Boulder : public Actor
//{
//public:
// Boulder() : Actor(IID_ICE, startX, startY, dir, size, depth, ptrStudWrld)
//private:
//};

//class Squirt : public Actor
//{
//public:
// Squirt() : Actor(IID_ICE, startX, startY, dir, size, depth, ptrStudWrld)
//private:
//};

// Actors that have HP
class HasHP : public Actor
{
public:
	HasHP(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, StudentWorld* ptrStudWrld, int initialHealth)
		: Actor(imageID, startX, startY, dir, size, depth, ptrStudWrld), _health(initialHealth)
	{

	}
	virtual ~HasHP() = default;

	void DecreaseHealth()
	{
		_health--;
		if (_health <= 0)
		{
			//calls the actor's die function
		}
	}
	void SetHealth(int health) { _health = health; }
	//Returns true if ths actor has more than 0 health
	bool isAlive() const { return _health > 0; }
protected:
	virtual void die() = 0; // pure virtual function to be implemented by derived classes
	int _health = 1;
};

class Iceman : public HasHP
{
public:
	Iceman(StudentWorld* world)
		: HasHP(IID_PLAYER, 30, 60, right, 1.0, 0, world, 10),
		_waterAmmo(5), _sonarAmmo(1), _goldAmmo(0), _playerScore(0), _oilCount(0), _lives(3)
	{
		setVisible(true);
	}
	void SonarAmmoIncrease() { _playerScore += 75; _sonarAmmo++; }
	void GoldAmmoIncrease() { _playerScore += 10; _goldAmmo++; }
	void WaterAmmoIncrease(){ _playerScore += 100; _waterAmmo += 5; }
	void OilGet() { _playerScore += 1000; _oilcount++; }
	void SpawnEnemyGold()
	{
		//check count of current gold, must be greater than 0
		//choose location for spawning (under player)
		//create gold gameobject
		//set id = IID_GOLD
		//setVisible(F)
		//goldAmmoDecrease();
		//toggle on collision with enemy
	}
	int GetScore() { return _playerScore; }
	void LoseLife() { _lives--;	}
	virtual void doSomething() override
	{
		if (!isAlive()) { return; } // If the player is dead, return immediately
		//check for user input
		//move in direction
		//check for collision with boulder
		//check for collision with ice
		//check for collision with water pool
		//check for collision with gold
		//check for collision with sonar
		//check for collision with oil barrel

		//this also handles the spawning of gold at the current player location
		//spawn water attacks facing the direction the player is facing
	}
	virtual void die() override
	{
		// Handle player death logic here, such as resetting the game or showing a game over screen
		// For now, we will just set the player to not visible and reset health
		setVisible(false);
		SetHealth(10); // Reset health for next life
	}
private:
	int _oilCount = 0; // number of oil barrels collected
	int _waterAmmo = 5;
	int _sonarAmmo = 1;
	int _goldAmmo = 0;
	int _playerScore = 0;
	int _oilcount = 0;
	int _lives = 3;
};

//class Protesters : public HasHP
//{
//public:
//
//private:
//};

// Differentiating Protester Stats
//class RegularProtesters : public Protesters {
//public:
//
//private:
//};

//class HardcoreProtesters : public Protesters
//{
//public:
//
//private:
//};

// PickUp Actors
//class PickUp : public Actor
//{
//public:
//	PickUp()
//	{
//		//ID = determine on spawn
//		//starting location is determined on spawn
//		//facing right
//		//depth = determined on spawn
//		//size = 1.0
//
//		//setVisible(true);
//		//playerCollision(true);
//	}
//	void ShowPickup()
//	{
//		Actor :: depth = 0;
//		setVisible(true);
//	}
//	void SonarPickup()
//	{
//		//call Iceman sonarAmmoIncrease();
//		//delete this gameobject
//	}
//	void GoldPickup()
//	{
//		//call Iceman goldAmmoIncrease();
//		//delete this gameobject
//	}
//	void WaterPickup()
//	{
//		//call Iceman waterAmmoIncrease();
//		//delete this gameobject
//	}
//	void SpawnGold()
//	{
//		//choose location for spawning
//		//create this gameobject
//		//set id = IID_GOLD
//		//setVisible(false);
//		//toggle on collision with player
//	}
//	void SpawnWater()
//	{
//		//choose location for spawning
//		//create this gameobject
//		//set id = IID_WATER_POOL
//		//setVisible(true);
//		//toggle on collision with player
//	}
//	void SpawnSonar()
//	{
//		//check to see if sonar is already there
//		//spawn in preset location
//		//id = IID_SONAR
//		//setVisible(ture)
//	}
//	void SpawnOil()
//	{
//		//check to see if oil is already there
//		//spawn in preset location
//		//id = IID_BARREL
//		//setVisible(ture)
//	}
//private:
//	bool _pickUpExists = false;
//};
//
//class Oil : public PickUp
//{
//public:
//
//private:
//};
//
//class Gold : public PickUp
//{
//public:
//
//private:
//};
//
//class Sonar : public PickUp
//{
//public:
//
//private:
//};
//
//class WaterPool : public PickUp
//{
//public:
//
//private:
//};

#endif // ACTOR_H_
