#pragma region Includes
#include "stdafx.h"
#include "SharedMemoryLock.h"
#include "SVLogLibrary/Logging.h"
#include "SVSharedMemoryLibrary/LockAcquisitionStream.h"

#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/log/attributes/mutable_constant.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind/bind.hpp>
#pragma endregion Includes

using namespace boost::interprocess;

LockEntity::LockEntity()
	: type(EntityType::Empty)
{
	std::memset(username, 0, maxUsernameSize);
	std::memset(host, 0, maxHostSize);
}

void LockEntity::operator=(const LockEntity& entity)
{
	type = entity.type;
	std::strcpy(username, entity.username);
	std::strcpy(host, entity.host);
}

void LockState::operator=(const LockState& state)
{
	owner = state.owner;
	requester = state.requester;
}

SharedMemory::SharedMemory()
	: mLockState()
	, mMutex()
	, mGatewayNotification()
	, mGatewayNotificationReady(false)
	, mGatewayNotificationsDone(false)
	, mObserverNotification()
	, mObserverNotificationReady(false)
	, mObserverNotificationsDone(false)
{}

SharedMemoryLock::SharedMemoryLock(
	boost::interprocess::open_or_create_t openOrCreate,
	const lock_state_changed_callback_t& onLockStateChangedCb,
	const std::string& name)
	: mOnLockStateChangedCb(onLockStateChangedCb)
	, mSharedMemoryHandler(openOrCreate, name.c_str(), read_write, sizeof(SharedMemory))
	, mMappedRegion(mSharedMemoryHandler, read_write)
	, mSharedMemory(static_cast<SharedMemory*>(mMappedRegion.get_address()))
{}

SharedMemoryLock::SharedMemoryLock(
	boost::interprocess::open_only_t openOnly,
	const lock_state_changed_callback_t& onLockStateChangedCb,
	const std::string& name)
	: mOnLockStateChangedCb(onLockStateChangedCb)
	, mSharedMemoryHandler(openOnly, name.c_str(), read_write)
	, mMappedRegion(mSharedMemoryHandler, read_write)
	, mSharedMemory(static_cast<SharedMemory*>(mMappedRegion.get_address()))
{}

bool SharedMemoryLock::Acquire(EntityType type, const std::string& username, const std::string& host)
{
	if (mSharedMemory == nullptr)
	{
		SV_LOG_GLOBAL(error) << "Acquire(): Pointer to shared memory is not initialized!";
		return false;
	}
	if (username.size() >= maxUsernameSize)
	{
		SV_LOG_GLOBAL(error) << "Acquire(): provided username it too long to store it in shared memory";
		return false;
	}
	if (host.size() >= maxHostSize)
	{
		SV_LOG_GLOBAL(error) << "Acquire(): provided host it too long to store it in shared memory";
		return false;
	}

	if (mSharedMemory->mLockState.owner.type == EntityType::Empty)
	{
		mSharedMemory->mLockState.owner.type = type;
		std::strcpy(mSharedMemory->mLockState.owner.username, username.c_str());
		std::strcpy(mSharedMemory->mLockState.owner.host, host.c_str());
		SV_LOG_GLOBAL(debug) << "Acquire(): Shared memory lock acquired!";
		return true;
	}

	return false;
}

bool SharedMemoryLock::Takeover(EntityType type, const std::string& username, const std::string& host)
{
	if (mSharedMemory == nullptr)
	{
		SV_LOG_GLOBAL(error) << "Takeover(): Pointer to shared memory is not initialized!";
		return false;
	}
	if (username.size() >= maxUsernameSize)
	{
		SV_LOG_GLOBAL(error) << "Takeover(): provided username it too long to store it in shared memory";
		return false;
	}
	if (host.size() >= maxHostSize)
	{
		SV_LOG_GLOBAL(error) << "Takeover(): provided host it too long to store it in shared memory";
		return false;
	}

	mSharedMemory->mLockState.owner.type = type;
	std::strcpy(mSharedMemory->mLockState.owner.username, username.c_str());
	std::strcpy(mSharedMemory->mLockState.owner.host, host.c_str());
	SV_LOG_GLOBAL(debug) << "Takeover(): Shared memory lock taken over!";
	return true;
}

bool SharedMemoryLock::RequestTakeover(EntityType type, const std::string& username, const std::string& host)
{
	if (mSharedMemory == nullptr)
	{
		SV_LOG_GLOBAL(error) << "RequestTakeover(): Pointer to shared memory is not initialized!";
		return false;
	}
	if (username.size() >= maxUsernameSize)
	{
		SV_LOG_GLOBAL(error) << "RequestTakeover(): provided username it too long to store it in shared memory";
		return false;
	}
	if (host.size() >= maxHostSize)
	{
		SV_LOG_GLOBAL(error) << "RequestTakeover(): provided host it too long to store it in shared memory";
		return false;
	}
	if (type == EntityType::Empty)
	{
		SV_LOG_GLOBAL(error) << "RequestTakeover(): Invalid argument!";
		return false;
	}

	mSharedMemory->mLockState.requester.type = type;
	std::strcpy(mSharedMemory->mLockState.requester.username, username.c_str());
	std::strcpy(mSharedMemory->mLockState.requester.host, host.c_str());
	SV_LOG_GLOBAL(debug) << "RequestTakeover(): Shared memory lock takeover requested!";
	return true;
}

bool SharedMemoryLock::Release()
{
	if (mSharedMemory == nullptr)
	{
		SV_LOG_GLOBAL(error) << "Release(): Pointer to shared memory is not initialized!";
		return false;
	}

	mSharedMemory->mLockState.owner.type = EntityType::Empty;
	mSharedMemory->mLockState.requester.type = EntityType::Empty;
	std::memset(mSharedMemory->mLockState.owner.username, 0, maxUsernameSize);
	std::memset(mSharedMemory->mLockState.owner.host, 0, maxHostSize);
	std::memset(mSharedMemory->mLockState.requester.username, 0, maxUsernameSize);
	std::memset(mSharedMemory->mLockState.requester.host, 0, maxHostSize);
	SV_LOG_GLOBAL(debug) << "Release(): Shared memory lock released!";
	return true;
}

SVOGatewaySharedMemoryLock::SVOGatewaySharedMemoryLock(
	const lock_state_changed_callback_t& onLockStateChangedCb,
	const std::string& name)
	: SharedMemoryLock(boost::interprocess::open_only, onLockStateChangedCb, name)
	, mNotificationsThread([this] {
		scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
		while (true)
		{
			mSharedMemory->mGatewayNotification.wait(lock, [this] {
				return mSharedMemory->mGatewayNotificationReady || mSharedMemory->mGatewayNotificationsDone;
			});
			if (mSharedMemory->mGatewayNotificationsDone)
			{
				mSharedMemory->mGatewayNotificationsDone = false;
				break;
			}
			mOnLockStateChangedCb(mSharedMemory->mLockState);
			mSharedMemory->mGatewayNotificationReady = false;
		}
		SV_LOG_GLOBAL(info) << "SVOGatewaySharedMemoryLock: notification thread is finished!";
	})
{}

SVOGatewaySharedMemoryLock::~SVOGatewaySharedMemoryLock()
{
	{
		scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
		mSharedMemory->mGatewayNotificationsDone = true;
	}
	mSharedMemory->mGatewayNotification.notify_one();
	if (mNotificationsThread.joinable())
	{
		mNotificationsThread.join();
	}
}

bool SVOGatewaySharedMemoryLock::Acquire(const std::string& username, const std::string& host)
{
	bool isAcquired = false;
	{
		scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
		isAcquired = SharedMemoryLock::Acquire(EntityType::SVOGateway, username, host);
		mSharedMemory->mObserverNotificationReady = isAcquired;
	}
	if (isAcquired)
	{
		mSharedMemory->mObserverNotification.notify_one();
	}
	
	return isAcquired;
}

bool SVOGatewaySharedMemoryLock::Takeover(const std::string& username, const std::string& host)
{
	bool isTakenOver = false;
	{
		scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
		isTakenOver = SharedMemoryLock::Takeover(EntityType::SVOGateway, username, host);
		mSharedMemory->mObserverNotificationReady = isTakenOver;
	}
	if (isTakenOver)
	{
		mSharedMemory->mObserverNotification.notify_one();
	}
	return isTakenOver;
}

bool SVOGatewaySharedMemoryLock::RequestTakeover(const std::string& username, const std::string& host)
{
	bool isTakeoverRequested = false;
	{
		scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
		isTakeoverRequested = SharedMemoryLock::RequestTakeover(EntityType::SVOGateway, username, host);
		mSharedMemory->mObserverNotificationReady = isTakeoverRequested;
	}
	if (isTakeoverRequested)
	{
		mSharedMemory->mObserverNotification.notify_one();
	}
	return isTakeoverRequested;
}

bool SVOGatewaySharedMemoryLock::Release()
{
	bool isReleased = false;
	{
		scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
		isReleased = SharedMemoryLock::Release();
		mSharedMemory->mObserverNotificationReady = isReleased;
	}
	if (isReleased)
	{
		mSharedMemory->mObserverNotification.notify_one();
	}
	return isReleased;
}

LockState SVOGatewaySharedMemoryLock::GetLockState() const
{
	scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
	return mSharedMemory->mLockState;
}

SVObserverSharedMemoryLock::SVObserverSharedMemoryLock(
	const lock_state_changed_callback_t& onLockStateChangedCb,
	const std::string& name)
	: SharedMemoryLock(boost::interprocess::open_or_create, onLockStateChangedCb, name)
	, mNotificationsThread([this] {
		scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
		while (true)
		{
			mSharedMemory->mObserverNotification.wait(lock, [this] {
				return mSharedMemory->mObserverNotificationReady || mSharedMemory->mObserverNotificationsDone;
			});
			if (mSharedMemory->mObserverNotificationsDone)
			{
				mSharedMemory->mObserverNotificationsDone = false;
				break;
			}
			mOnLockStateChangedCb(mSharedMemory->mLockState);
			mSharedMemory->mObserverNotificationReady = false;
		}
		SV_LOG_GLOBAL(info) << "SVObserverSharedMemoryLock: notification thread is finished!";
	})
{}

SVObserverSharedMemoryLock::~SVObserverSharedMemoryLock()
{
	{
		scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
		mSharedMemory->mObserverNotificationsDone = true;
	}
	mSharedMemory->mObserverNotification.notify_one();
	if (mNotificationsThread.joinable())
	{
		mNotificationsThread.join();
	}
}

bool SVObserverSharedMemoryLock::Acquire(const std::string& username, const std::string& host)
{
	bool isAcquired = false;
	{
		scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
		isAcquired = SharedMemoryLock::Acquire(EntityType::SVObserver, username, host);
		mSharedMemory->mGatewayNotificationReady = isAcquired;
	}
	if (isAcquired)
	{
		mSharedMemory->mGatewayNotification.notify_one();
	}

	return isAcquired;
}

bool SVObserverSharedMemoryLock::Takeover(const std::string& username, const std::string& host)
{
	bool isTakenOver = false;
	{
		scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
		isTakenOver = SharedMemoryLock::Takeover(EntityType::SVObserver, username, host);
		mSharedMemory->mGatewayNotificationReady = isTakenOver;
	}
	if (isTakenOver)
	{
		mSharedMemory->mGatewayNotification.notify_one();
	}
	return isTakenOver;
}

bool SVObserverSharedMemoryLock::RequestTakeover(const std::string& username, const std::string& host)
{
	bool isTakeoverRequested = false;
	{
		scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
		isTakeoverRequested = SharedMemoryLock::RequestTakeover(EntityType::SVObserver, username, host);
		mSharedMemory->mGatewayNotificationReady = isTakeoverRequested;
	}
	if (isTakeoverRequested)
	{
		mSharedMemory->mGatewayNotification.notify_one();
	}
	return isTakeoverRequested;
}

bool SVObserverSharedMemoryLock::Release()
{
	bool isReleased = false;
	{
		scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
		isReleased = SharedMemoryLock::Release();
		mSharedMemory->mGatewayNotificationReady = isReleased;
	}
	if (isReleased)
	{
		mSharedMemory->mGatewayNotification.notify_one();
	}
	return isReleased;
}

LockState SVObserverSharedMemoryLock::GetLockState() const
{
	scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
	return mSharedMemory->mLockState;
}

bool operator==(const LockEntity& lhs, const LockEntity& rhs)
{
	return lhs.type == rhs.type &&
		std::strcmp(lhs.username, rhs.username) == 0 &&
		std::strcmp(lhs.host, rhs.host) == 0;
}

bool operator!=(const LockEntity& lhs, const LockEntity& rhs)
{
	return lhs.type != rhs.type ||
		std::strcmp(lhs.username, rhs.username) != 0 ||
		std::strcmp(lhs.host, rhs.host) != 0;
}
