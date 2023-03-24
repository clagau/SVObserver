#pragma once

#include <memory>
#include <string>
#include <functional>

#include <boost/asio.hpp>
#include <boost/asio/basic_deadline_timer.hpp>
#include <boost/asio/time_traits.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>

struct lock_acquisition_stream_t;

constexpr size_t maxUsernameSize = 32;
constexpr size_t maxHostSize = 16;

enum class EntityType : std::uint8_t
{
	Empty = 0,
	SVOGateway = 1,
	SVObserver = 2,
};

struct LockEntity
{
	LockEntity();

	EntityType type;
	char username[maxUsernameSize];
	char host[maxHostSize];

	void operator=(const LockEntity& entity);
};

struct LockState
{
	LockEntity owner;
	LockEntity requester;

	void operator=(const LockState& state);
};

struct SharedMemory
{
	SharedMemory();

	LockState mLockState;
	boost::interprocess::interprocess_mutex mMutex;

	boost::interprocess::interprocess_condition mGatewayNotification;
	bool mGatewayNotificationReady;
	bool mGatewayNotificationsDone;

	boost::interprocess::interprocess_condition mObserverNotification;
	bool mObserverNotificationReady;
	bool mObserverNotificationsDone;
};

using lock_state_changed_callback_t = std::function<void(LockState)>;

class SharedMemoryLock
{
public:
	SharedMemoryLock(
		boost::interprocess::open_or_create_t openOrCreate,
		const lock_state_changed_callback_t& onLockStateChangedCb,
		const std::string& name);
	SharedMemoryLock(
		boost::interprocess::open_only_t openOnly,
		const lock_state_changed_callback_t& onLockStateChangedCb,
		const std::string& name);

	bool Acquire(EntityType type, const std::string& username, const std::string& host);
	bool Takeover(EntityType type, const std::string& username, const std::string& host);
	bool RequestTakeover(EntityType type, const std::string& username, const std::string& host);
	bool Release();

protected:
	lock_state_changed_callback_t mOnLockStateChangedCb;
	boost::interprocess::windows_shared_memory mSharedMemoryHandler;
	boost::interprocess::mapped_region mMappedRegion;
	SharedMemory* mSharedMemory;
};

class SVOGatewaySharedMemoryLock : public SharedMemoryLock
{
public:
	explicit SVOGatewaySharedMemoryLock(
		const lock_state_changed_callback_t& onLockStateChangedCb,
		const std::string& name = "Global\\sv_default_shared_memory");
	~SVOGatewaySharedMemoryLock();

	bool Acquire(const std::string& username, const std::string& host);
	bool Takeover(const std::string& username, const std::string& host);
	bool RequestTakeover(const std::string& username, const std::string& host);
	bool Release();

	LockState GetLockState() const;

private:
	std::thread mNotificationsThread;
};

class SVObserverSharedMemoryLock : public SharedMemoryLock
{
public:
	explicit SVObserverSharedMemoryLock(
		const lock_state_changed_callback_t& onLockStateChangedCb,
		const std::string& name = "Global\\sv_default_shared_memory");
	~SVObserverSharedMemoryLock();

	bool Acquire(const std::string& username, const std::string& host);
	bool Takeover(const std::string& username, const std::string& host);
	bool RequestTakeover(const std::string& username, const std::string& host);
	bool Release();

	LockState GetLockState() const;

private:
	std::thread mNotificationsThread;
};

bool operator==(const LockEntity& lhs, const LockEntity& rhs);
bool operator!=(const LockEntity& lhs, const LockEntity& rhs);
