#include "Actor.h"
#include "StudentWorld.h"
#include <random>
using namespace std;

int randInt(int min, int max) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

// --- Actor ---
Actor::Actor(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, StudentWorld* world)
    : GraphObject(imageID, startX, startY, dir, size, depth), _world(world), _swAlive(true)
{
    setVisible(true);
    _world = world;
    _swAlive = true;
}

ActorType Actor::getType() const { return ActorType::Unknown; } // Default implementation, can be overridden

// --- HasHP ---
HasHP::HasHP(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, StudentWorld* world, int initialHealth)
    : Actor(imageID, startX, startY, dir, size, depth, world), _health(initialHealth) { }
HasHP:: ~HasHP() { }
// Decrease health by int amount
void HasHP::decreaseHealth(int amount)
{
    if (!isAlive()) return;
    _health -= amount;
    if (_health <= 0) {
        setDead();
    }
} 

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
            else if (getX() >= 1 && !getWorld()->Is_Boulder(getX() - 1, getY(), left))
            { moveTo(getX() - 1, getY()); }

            //else if (getX() > 0) moveTo(getX() - 1, getY());
            break;
        case KEY_PRESS_RIGHT:
        case 'd':
            if (getDirection() != right) setDirection(right);
            else if (getX() <= 59 && !getWorld()->Is_Boulder(getX() + 1, getY(), right))
            { moveTo(getX() + 1, getY()); }

            //else if (getX() < 60) moveTo(getX() + 1, getY());
            break;
        case KEY_PRESS_UP:
        case 'w':
            if (getDirection() != up) setDirection(up);
            else if (getY() <= 59 && !getWorld()->Is_Boulder(getX(), getY() + 1, up))
            { moveTo(getX(), getY() + 1); }

            //else if (getY() < 60) moveTo(getX(), getY() + 1);
            break;
        case KEY_PRESS_DOWN:
        case 's':
            if (getDirection() != down) setDirection(down);
            else if (getY() >= 1 && !getWorld()->Is_Boulder(getX(), getY() - 1, down))
            { moveTo(getX(), getY() - 1); }

           // else if (getY() > 0) moveTo(getX(), getY() - 1);
            break;

        // Reset Map
        case KEY_PRESS_ESCAPE:
            setDead();
            break;

        // Squirt
        case KEY_PRESS_SPACE:
            if (_waterAmmo > 0){

                if (getDirection() == right && getX() < 60){
                    getWorld()->Squirt_Water(getX() + 4, getY(), right);
                    _waterAmmo--;
                }

                else if (getDirection() == left && getX() > 0){
                    getWorld()->Squirt_Water(getX() - 4, getY(), left);
                    _waterAmmo--;
                }

                else if (getDirection() == up && getY() < 60){
                    getWorld()->Squirt_Water(getX(), getY() + 4, up);
                    _waterAmmo--;
                }

                else if (getDirection() == down && getY() > 0){
                    getWorld()->Squirt_Water(getX(), getY() - 4, down);
                    _waterAmmo--;
                }
            }
            break;

        // Ping!
        case 'Z':
        case 'z':
            if (_sonarAmmo > 0){
                _sonarAmmo--;
                getWorld()->Sonar_Used(getX(), getY());
            }
            break;

        // Place Gold
        case KEY_PRESS_TAB:
            if (_goldAmmo > 0){
                getWorld()->dropGold(getX(), getY());
                _goldAmmo--;
            }
            break;
        }
    }
	//this is the code for Iceman breaking ice
    int icemanX = getX();
    int icemanY = getY();

    if (getWorld()->Remove_Ice_At(getX(), getY()))
    { GameController::getInstance().playSound(SOUND_DIG); }
}

void Iceman::die() {
    setDead();
    GameController::getInstance().playSound(SOUND_PLAYER_GIVE_UP);
}

bool Iceman::canTakeDamage() const
{
    if(getWorld()->isPlayerStunned())
    {
        return false; // Iceman is stunned, cannot take damage
	}
	return true; // Iceman can take damage
}

void Iceman::loseLife()
{
    getWorld()->decLives();
}
void Iceman::annoy(int amount)
{
    decreaseHealth(amount);

    if (!isAlive()) {
        getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
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
	_numStepsInCurrentDirection = 0;

    chooseNewDirection();
}
Protester::~Protester() 
{
    _exitPath.clear();
    _exitPath.shrink_to_fit();
}
bool Protester::isAlive() const
{
    return HasHP::isAlive() || isLeavingField();
}
void Protester::doSomething()
{
    if (!isAlive()) return;

    // --- Resting Logic ---
    if (_restingTime > 0) {
        _restingTime--;
        if (_restingTime == 0)
            _stunned = false;

        if (!isLeavingField()) {
            return;
        }
    }

    if (_restingTime == 0 && _stunned)
    {
        _stunned = false;
    }

    reduceShoutCooldown(); // Reduce shout cooldown each tick

    // --- Leaving Field Logic ---

    // Get path to exit
    if (isLeavingField())
    {
        if (getX() == 60 && getY() == 60)
        {
            setVisible(false);
            setReachedExit(true);
            setDead();
            return;
        }

        // Only compute the path once
        if (_exitPath.empty())
            _exitPath = getWorld()->getPathToExit(getX(), getY());

        if (_exitPath.empty()) {
            _restingTime = getWorld()->getRestTime();
            return;
        }

        // Get next step in path
        auto [nextX, nextY] = _exitPath.front();
        _exitPath.erase(_exitPath.begin());  // Move to next step next tick

        // Set direction to face movement
        int dx = nextX - getX();
        int dy = nextY - getY();
        if (dx > 0) setDirection(right);
        else if (dx < 0) setDirection(left);
        else if (dy > 0) setDirection(up);
        else if (dy < 0) setDirection(down);

        moveTo(nextX, nextY);
        _restingTime = getWorld()->getRestTime();
        return;
    }

    // --- Iceman Detection ---
    int iceX = getWorld()->getIceman()->getX();
    int iceY = getWorld()->getIceman()->getY();
    int dX = getX() - iceX;
    int dY = getY() - iceY;
    double distance = sqrt(dX * dX + dY * dY);

    GraphObject::Direction dir;

    // --- Shout Logic ---
    if (distance <= 4.0 && getWorld()->inLineOfSightToPlayer(getX(), getY(), dir))
    {
        if (canShout())
        {
            if (getDirection() != dir)
                setDirection(dir);

            getWorld()->playSound(SOUND_PROTESTER_YELL);
            getWorld()->getIceman()->annoy(2);
            resetShoutCooldown();
            _restingTime = getWorld()->getRestTime();
            return;
        }

        // If can't shout yet, don't move through Iceman
        return;
    }

    // --- Line-of-Sight Movement (toward Iceman) ---
    if (getWorld()->inLineOfSightToPlayer(getX(), getY(), dir))
    {
        setDirection(dir);
        if (getWorld()->canMoveTo(getX(), getY(), dir))
        {
            moveTo(getX() + dxForDir(dir), getY() + dyForDir(dir));
            _restingTime = getWorld()->getRestTime();
            return;
        }
    }

    // --- Random Movement ---
    if (_numStepsInCurrentDirection <= 0)
    {
        chooseNewDirection(); // sets _currentDirection and _numStepsInCurrentDirection
    }

    if (getWorld()->canMoveTo(getX(), getY(), _currentDirection))
    {
        setDirection(_currentDirection);
        moveTo(getX() + dxForDir(_currentDirection), getY() + dyForDir(_currentDirection));
        _numStepsInCurrentDirection--;
        _restingTime = getWorld()->getRestTime();
    }
    else
    {
        _numStepsInCurrentDirection = 0;
    }
}

// Helper function to get delta x for a direction
int Protester::dxForDir(Direction dir) const {
    switch (dir) {
    case left: return -1;
    case right: return 1;
    default: return 0;
    }
}
// Helper function to get delta y for a direction
int Protester::dyForDir(Direction dir) const {
    switch (dir) {
    case up: return 1;
    case down: return -1;
    default: return 0;
    }
}
void Protester::chooseNewDirection() 
{
    GraphObject::Direction dirs[] = { up, down, left, right };

	//random generator for shuffling directions
	static std::random_device rd; // Random device for seeding
	static std::mt19937 gen(rd()); // Mersenne Twister RNG no idea how this actually works just found it online

    std::shuffle(std::begin(dirs), std::end(dirs), gen);

    for (auto d : dirs) {
        if (getWorld()->canMoveTo(getX(), getY(), d)) {
            _currentDirection = d;
            setDirection(d);
            _numStepsInCurrentDirection = 8 + rand() % 53;
            return;
        }
    }
    _numStepsInCurrentDirection = 0;
}

void Protester::moveInDirection(GraphObject::Direction dir) {
    switch (dir) {
    case left:  moveTo(getX() - 1, getY()); break;
    case right: moveTo(getX() + 1, getY()); break;
    case up:    moveTo(getX(), getY() + 1); break;
    case down:  moveTo(getX(), getY() - 1); break;
    }
}

void Protester::annoy(int damage)
{
    if (isLeavingField() || isStunned()) return;

    _health -= damage;

    if (_health <= 0)
    {
        die(); // Calls overridden die()
    }
    else
    {
        getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
        _restingTime = std::max<unsigned int>(50u, static_cast<unsigned int>(100 - getWorld()->getLevel() * 10));
        _stunned = true;
    }
}

void Protester::bribe() {
    getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
    setLeaveField(true);
    _stunned = false;
    _restingTime = 0;
}

void Protester::die()
{
    if (!isLeavingField())
    {
        setLeaveField(true);
        _stunned = false;
        getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
    }
}

// --- RegularProtestor ---
RegularProtester::~RegularProtester() { }

void RegularProtester::doSomething()
{
    Protester::doSomething(); // Extend or override behavior
}

void RegularProtester::bribe() 
{
	Protester::bribe(); // Call base class bribe logic
}

void RegularProtester::die()
{
    if (!isLeavingField()) {
        switch (getLastDamage()) {
        case DamageSource::Boulder:
            getWorld()->increaseScore(500);
            break;
        case DamageSource::Squirt:
        default:
            getWorld()->increaseScore(100);
            break;
        }
    }

    Protester::die();  // Always call to trigger exit behavior
}

// --- HardcoreProtestor ---
HardcoreProtester::~HardcoreProtester() 
{
    _cachedPath.clear(); // Clear pathfinding cache memory
    _cachedPath.shrink_to_fit();
}
void HardcoreProtester::doSomething()
{
    if (!isAlive()) return;

    // --- Resting ---
    if (getRestingTime() > 0) {
        setRestingTime(getRestingTime() - 1);
        if (getRestingTime() == 0) {
            setStunned(false);
			_cachedPath.clear(); // Clear cached path when resting ends
        }
        return;
    }

    int currTick = getWorld()->getTicks();
	reduceShoutCooldown(); // Reduce shout cooldown each tick
    // --- Leaving ---
    if (isLeavingField())
    {
        if (getX() == 60 && getY() == 60)
        {
            setReachedExit(true);
            setVisible(false);
            setDead();
            return;
        }

        // Only compute the path once
        if (_exitPath.empty())
            _exitPath = getWorld()->getPathToExit(getX(), getY());

        if (_exitPath.empty()) {
            setRestingTime(getWorld()->getRestTime());
            return;
        }

        // Get next step in path
        auto [nextX, nextY] = _exitPath.front();
        _exitPath.erase(_exitPath.begin());  // Move to next step next tick

        // Set direction to face movement
        int dx = nextX - getX();
        int dy = nextY - getY();
        if (dx > 0) setDirection(right);
        else if (dx < 0) setDirection(left);
        else if (dy > 0) setDirection(up);
        else if (dy < 0) setDirection(down);

        moveTo(nextX, nextY);
        setRestingTime(getWorld()->getRestTime());
        return;
    }

    // --- Iceman Detection ---
    int iceX = getWorld()->getIceman()->getX();
    int iceY = getWorld()->getIceman()->getY();
    int dX = getX() - iceX;
    int dY = getY() - iceY;
    double distance = sqrt(dX * dX + dY * dY);
    GraphObject::Direction dir;

    // --- Shout Logic ---
    if (distance <= 4.0 && getWorld()->inLineOfSightToPlayer(getX(), getY(), dir))
    {
        if (canShout())
        {
            if (getDirection() != dir)
                setDirection(dir);

            getWorld()->playSound(SOUND_PROTESTER_YELL);
            getWorld()->getIceman()->annoy(2);
            resetShoutCooldown();
            setRestingTime(getWorld()->getRestTime());
            return;
        }

        // If can't shout yet, don't move through Iceman
        return;
    }

	// --- Path to Iceman ---
    int M = 16 + getWorld()->getLevel() * 2;
    // Recalculate if empty or older than 8 ticks
    if (_cachedPath.empty() || currTick - _lastPathUpdateTick >= 8) {
        _cachedPath = getWorld()->getPathToTarget(getX(), getY(), iceX, iceY);
        _cachedPath.shrink_to_fit();
        _lastPathUpdateTick = currTick;
    }

    // Reject too-long paths
    if (_cachedPath.size() > M) {
        _cachedPath.clear();  // Ignore path if too far
    }

    if (!_cachedPath.empty()) {
        auto [nextX, nextY] = _cachedPath.front();
        _cachedPath.erase(_cachedPath.begin());

        // Determine direction
        GraphObject::Direction moveDir;
        if (nextX > getX()) moveDir = right;
        else if (nextX < getX()) moveDir = left;
        else if (nextY > getY()) moveDir = up;
        else moveDir = down;

        if (getWorld()->canMoveTo(getX(), getY(), moveDir)) {
            setDirection(moveDir);
            moveTo(nextX, nextY);
            setRestingTime(getWorld()->getRestTime()); //This enforces 1 move per tick
            return;
        }
        else {
            _cachedPath.clear(); // Clear if blocked
        }
    }

    // --- Random Movement ---
    if (_numStepsInCurrentDirection <= 0)
    {
        chooseNewDirection();
    }

	// -- Perpendicular Movement --
    if (currTick - _lastPerpendicularTurnTick >= 200) {
        std::vector<GraphObject::Direction> viableDirs;
        GraphObject::Direction currDir = getDirection();

        if (currDir == left || currDir == right) {
            if (getWorld()->canMoveTo(getX(), getY(), up)) viableDirs.push_back(up);
            if (getWorld()->canMoveTo(getX(), getY(), down)) viableDirs.push_back(down);
        }
        else if (currDir == up || currDir == down) {
            if (getWorld()->canMoveTo(getX(), getY(), left)) viableDirs.push_back(left);
            if (getWorld()->canMoveTo(getX(), getY(), right)) viableDirs.push_back(right);
        }

        if (!viableDirs.empty()) {
            int r = randInt(0, (int)viableDirs.size() - 1);
            GraphObject::Direction newDir = viableDirs[r];
            setDirection(newDir);
            _currentDirection = newDir;
            _numStepsInCurrentDirection = randInt(8, 60);
            _lastPerpendicularTurnTick = currTick;

            if (getWorld()->canMoveTo(getX(), getY(), newDir)) {
                moveTo(getX() + dxForDir(newDir), getY() + dyForDir(newDir));
                _numStepsInCurrentDirection--;
                setRestingTime(getWorld()->getRestTime());
                return;
            }
        }
    }

	// --- Move in current direction one step ---
    if (getWorld()->canMoveTo(getX(), getY(), _currentDirection)) 
    {
        setDirection(_currentDirection);
        moveTo(getX() + dxForDir(_currentDirection), getY() + dyForDir(_currentDirection));
        _numStepsInCurrentDirection--;
        setRestingTime(getWorld()->getRestTime());
    }
	// --- If can't move in current direction, reset step count ---
    else
    {
        _numStepsInCurrentDirection = 0;
    }
}

void HardcoreProtester::bribe() 
{
    setStunned(true);
    _cachedPath.clear();
    setRestingTime(50);
    getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
}

void HardcoreProtester::die()
{
    if (!isLeavingField()) {
        switch (getLastDamage()) {
        case DamageSource::Boulder:
            getWorld()->increaseScore(1000);
            break;
        case DamageSource::Gold:
            getWorld()->increaseScore(250);
            _stunned = true;
            _restingTime = std::max(50u, static_cast<unsigned int>(100 - getWorld()->getLevel() * 10));
            break;
        case DamageSource::Squirt:
        default:
            getWorld()->increaseScore(250);
            break;
        }
    }

    Protester::die();  // Always call to trigger exit behavior
}

// --- Ice ---
Ice::Ice(int startX, int startY, StudentWorld* world)
    : Actor(IID_ICE, startX, startY, right, 0.25, 3, world)
{ setVisible(true); }

Ice::~Ice() { setVisible(false); }

void Ice::doSomething() { }

// --- Boulder ---  
Boulder::Boulder(int startX, int startY, StudentWorld* world)
    : Actor(IID_BOULDER, startX, startY, down, 1.0, 1, world)
{
    setType(ActorType::Boulder);
    setVisible(true);
}

void Boulder::doSomething() {
    if (!isAlive())
        return;

    if (_bState == 0) {
        // Check if there�s no ice under it anymore
        if (!getWorld()->Is_Ice(getX(), getY() - 1, GraphObject::down)) {
            _bState = 1;
            _tick = 0;
        }
    }
    else if (_bState == 1) {
        // Wait 30 ticks before falling
        if (_tick < 30) {
            _tick++;
        }
        else {
            _bState = 2;
            GameController::getInstance().playSound(SOUND_FALLING_ROCK);
        }
    }
    else if (_bState == 2) {
        int belowY = getY() - 1;

        if (getWorld()->Can_Fall(getX(), getY())) {
            getWorld()->Set_Position(getX(), getY(), 0);  // Clear current pos
            if (getY() <= 0) { setDead(); }
            moveTo(getX(), belowY);  // Move down
            getWorld()->Boulder_Annoyed(getX(), belowY);  // Check for protester damage
            getWorld()->Set_Position(getX(), belowY, 'B');  // Mark new pos
        }
        else {
            // Can�t fall anymore, go poof
            setDead();
        }
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

    if (getWorld()->Protester_Annoyed(getX(), getY(), 2, static_cast<int>(HasHP::DamageSource::Squirt))) { setDead(); }
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
 
// --- Pickups (Oil, Gold, Sonar, WaterPool) ---
Oil::Oil(int startX, int startY, StudentWorld* world)
    : PickUp(IID_BARREL, startX, startY, right, 1.0, 2, world) {
    setVisible(false);
}

void Oil::doSomething() {  
    if (!isAlive()) { return; }

	// --- Reveal Oil Barrel Logic ---
    if (!isVisible() && getWorld()->Near_Iceman(getX(), getY(), 4)) 
    {
        setVisible(true);
        return;
    }

    if (isVisible() && getWorld()->Near_Iceman(getX(), getY(), 3)) 
    {
        setDead();
        getWorld()->increaseScore(1000);
        getWorld()->Pickup_Oil(getX(), getY());
        GameController::getInstance().playSound(SOUND_FOUND_OIL);
    }
}

Gold::Gold(int startX, int startY, StudentWorld* world, bool isVisible, bool canpick, bool temporary)
    : PickUp(IID_GOLD, startX, startY, right, 1.0, 2, world), _temporary(temporary)
{
    setVisible(isVisible);
    setPickup(canpick);
    if (_temporary)
        setTick(100); // 100 ticks for temporary gold
    else
        setTick(-1);  // not used for permanent
}

void Gold::doSomething() {
    if (!isAlive()) return;

    // Make visible if Iceman is nearby
    if (!isVisible() && getWorld()->Near_Iceman(getX(), getY(), 4)) 
    {
        setVisible(true);
        return;
    }

    if (!_temporary) {
        // Permanent gold: Iceman picks it up
        if (isVisible() && getWorld()->Near_Iceman(getX(), getY(), 3)) 
        {
            setDead();
            getWorld()->increaseScore(10);
            getWorld()->Iceman_ptr()->goldAmmoIncrease();
            GameController::getInstance().playSound(SOUND_GOT_GOODIE);
        }
    }
    else {
        // Temporary gold logic
        if (getTick() <= 0) {
            setDead();
            return;
        }

        Protester* p = getWorld()->Bribe_Nearby_Protester(getX(), getY());
        if (p) {
            if (p->getType() == ActorType::RegularProtester) {
                p->bribe();  // Regular protester leaves field
                getWorld()->increaseScore(25);
                setDead();
            }
            else if (p->getType() == ActorType::HardcoreProtester) {
                p->bribe();  // Hardcore protester pauses temporarily
                getWorld()->increaseScore(50);
                setDead();
            }
            return;
        }

        reduceTick();
    }
}

Sonar::Sonar(StudentWorld* world)
    : PickUp(IID_SONAR, 0, 60, right, 1.0, 2, world) {
    setVisible(true);
    setPickup(true);
    setTick(std::max(100u, static_cast<unsigned int>(300 - 10 * getWorld()->getLevel())));
}

void Sonar::doSomething() {
    if (!isAlive()) return;

    if (getWorld()->Near_Iceman(getX(), getY(), 3)) {
        setDead();
        getWorld()->increaseScore(75);
        getWorld()->Iceman_ptr()->sonarAmmoIncrease();
        GameController::getInstance().playSound(SOUND_GOT_GOODIE);
        return;
    }

    reduceTick();

    if (getTick() <= 0) {
        setDead();
        return;
    }
}

WaterPool::WaterPool(int startX, int startY, StudentWorld* world)
    : PickUp(IID_WATER_POOL, startX, startY, right, 1.0, 2, world) {
    setVisible(true);
    setPickup(true);
    setTick(std::max(100u, static_cast<unsigned int>(300 - 10 * getWorld()->getLevel())));
}

void WaterPool::doSomething() { 
    /* WaterPool logic */
    if (!isAlive()) { return; }

    if (getWorld()->Near_Iceman(getX(), getY(), 3)) {
        setDead();
        getWorld()->increaseScore(100);
        getWorld()->Iceman_ptr()->waterAmmoIncrease();
        GameController::getInstance().playSound(SOUND_GOT_GOODIE);
    }

    else if (getTick() == 0) { setDead(); }
    reduceTick();
}