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

class Actor : public GraphObject
{
public:
	Actor(int imageID, int startX, int startY, Direction dir = right, double size = 1.0,
		unsigned int depth = 0) : GraphObject(imageID, startX, startY, dir, size, depth)

		//m_brightness(1.0), m_animationNumber(0), m_direction(dir), m_destX(startX), m_destY(startY), 
	{
		setVisible(true);
	}
	void doSomething()
	{

	}

	virtual ~Actor() = default;
private:
	//ID
	//starting direction
	//depth
	//size
	//location (colum, row)
	//setVisible(T)
};

// Neutral Actors
class Ice : public Actor
{
public:

private:
};

//class Boulder : public Actor
//{
//public:
//
//private:
//};

//class Squirt : public Actor
//{
//public:
//
//private:
//};

// Actors that have HP
class HasHP : public Actor
{
public:
	void DecreaseHealth()
	{
		_health--;
		if (_health <= 0)
		{
			//kill player by changing to death state
		}
	}
	void SetHealth(int health)
	{
		_health = health;
	}
private:
	int _health = 1;
};

class Iceman : public HasHP
{
public:
	Iceman()
	{
		//ID = IID_PLAYER
		//starting location is x = 30, y = 60
		//facing right
		//depth = 0
		//size = 1.0
		//setalth(10); //starting health


		//starting health = 10
		//water ammo = 5
		//sonar ammo = 1
		//gold ammo = 0

		//setVisible(true);
	}

	void SonarAmmoIncrease()
	{
		_playerScore = (_playerScore + 75);
		_sonarAmmo++;
	}
	void GoldAmmoIncrease()
	{
		_playerScore = (_playerScore + 10);
		_goldAmmo++;
	}
	void WaterAmmoIncrease()
	{
		_playerScore = (_playerScore + 100);
		_waterAmmo = (_waterAmmo + 5);
	}
	void OilGet()
	{
		_playerScore = (_playerScore + 1000);
		_oilcount++;
	}
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
	int GetScore()
	{
		return _playerScore;
	}
	void LoseLife()
	{
		_lives--;
	}
	virtual void doSomething()
	{
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
private:
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
