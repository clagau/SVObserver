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

SharedMemory::SharedMemory()
	: mLockState()
	, mMutex()
{}

SharedMemoryLock::SharedMemoryLock(
	boost::interprocess::open_or_create_t openOrCreate,
	boost::asio::deadline_timer::duration_type expiryTime,
	const lock_state_changed_callback_t& onLockStateChangedCb,
	const std::string& name)
	: mOnLockStateChangedCb(onLockStateChangedCb)
	, mSharedMemoryHandler(openOrCreate, name.c_str(), read_write, sizeof(SharedMemory))
	, mMappedRegion(mSharedMemoryHandler, read_write)
	, mSharedMemory(static_cast<SharedMemory*>(mMappedRegion.get_address()))
	, mIoService()
	, mIoWork(mIoService)
	, mIoThread([&] {mIoService.run(); })
	, mLockStateCheckTimer(mIoService)
{
	{
		mLastLockState = GetLockState();
	}
	scheduleLockStateCheck(expiryTime);
}

SharedMemoryLock::SharedMemoryLock(
	boost::interprocess::open_only_t openOnly,
	boost::asio::deadline_timer::duration_type expiryTime,
	const lock_state_changed_callback_t& onLockStateChangedCb,
	const std::string& name)
	: mOnLockStateChangedCb(onLockStateChangedCb)
	, mSharedMemoryHandler(openOnly, name.c_str(), read_write)
	, mMappedRegion(mSharedMemoryHandler, read_write)
	, mSharedMemory(static_cast<SharedMemory*>(mMappedRegion.get_address()))
	, mIoService()
	, mIoWork(mIoService)
	, mIoThread([&] {mIoService.run(); })
	, mLockStateCheckTimer(mIoService)
{
	{
		mLastLockState = GetLockState();
	}
	scheduleLockStateCheck(expiryTime);
}

SharedMemoryLock::~SharedMemoryLock()
{
	mLockStateCheckTimer.cancel();
	mIoService.stop();
	if (mIoThread.joinable())
	{
		mIoThread.join();
	}
}

void SharedMemoryLock::scheduleLockStateCheck(boost::asio::deadline_timer::duration_type expiryTime)
{
	mLockStateCheckTimer.expires_from_now(expiryTime);
	auto waitFunctor = [this, expiryTime](const boost::system::error_code& rError) {
		return onLockStateCheckTimerExpired(expiryTime, rError);
	};
	mLockStateCheckTimer.async_wait(waitFunctor);
}

void SharedMemoryLock::onLockStateCheckTimerExpired(
	boost::asio::deadline_timer::duration_type expiryTime,
	const boost::system::error_code& error)
{
	if (error)
	{
		SV_LOG_GLOBAL(warning) << "Configuration lock state polling error: " << error.message();
	}

	LockState currentLockState;
	{
		scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
		currentLockState = mSharedMemory->mLockState;
	}
	if (mLastLockState.owner != currentLockState.owner || mLastLockState.requester != currentLockState.requester)
	{
		mIoService.dispatch([this, currentLockState]()
		{
			mOnLockStateChangedCb(currentLockState);
		});

		{
			scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
			if (mLastLockState.requester.type != currentLockState.requester.type &&
				currentLockState.requester.type != EntityType::Empty)
			{
				mSharedMemory->mLockState.requester = LockEntity();
				mLastLockState.requester = LockEntity();
			}
			else
			{
				mLastLockState = currentLockState;
			}
		}
	}
	scheduleLockStateCheck(expiryTime);
}

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

	scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
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

	scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
	mSharedMemory->mLockState.owner.type = type;
	std::strcpy(mSharedMemory->mLockState.owner.username, username.c_str());
	std::strcpy(mSharedMemory->mLockState.owner.host, host.c_str());

	return true;
}

bool SharedMemoryLock::RequestTakeover(EntityType type, const std::string& username, const std::string& host)
{
	if (mSharedMemory == nullptr)
	{
		SV_LOG_GLOBAL(error) << "RequestTakeover(): Pointer to shared memory is not initialized!";
		return false;
	}
	if (type == EntityType::Empty)
	{
		SV_LOG_GLOBAL(error) << "RequestTakeover(): Invalid argument!";
		return false;
	}

	scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
	if (mSharedMemory->mLockState.requester.type == EntityType::Empty)
	{
		mSharedMemory->mLockState.requester.type = type;
		std::strcpy(mSharedMemory->mLockState.requester.username, username.c_str());
		std::strcpy(mSharedMemory->mLockState.requester.host, host.c_str());
		SV_LOG_GLOBAL(debug) << "RequestTakeover(): Shared memory lock takeover requested!";
		return true;
	}

	SV_LOG_GLOBAL(debug) << "RequestTakeover(): Probably other client already requested a takeover";
	return false;
}

void SharedMemoryLock::Release()
{
	if (mSharedMemory == nullptr)
	{
		SV_LOG_GLOBAL(error) << "Release(): Pointer to shared memory is not initialized!";
		return;
	}

	scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
	mSharedMemory->mLockState.owner.type = EntityType::Empty;
	mSharedMemory->mLockState.requester.type = EntityType::Empty;
	std::memset(mSharedMemory->mLockState.owner.username, 0, maxUsernameSize);
	std::memset(mSharedMemory->mLockState.owner.host, 0, maxHostSize);
	std::memset(mSharedMemory->mLockState.requester.username, 0, maxUsernameSize);
	std::memset(mSharedMemory->mLockState.requester.host, 0, maxHostSize);
	SV_LOG_GLOBAL(debug) << "Release(): Shared memory lock released!";
}

void SharedMemoryLock::PushBackStream(const std::shared_ptr<lock_acquisition_stream_t>& streamPtr)
{
	scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
	mLockAcquisitionStreams.push_back(streamPtr);
}

std::shared_ptr<lock_acquisition_stream_t> SharedMemoryLock::GetLockOwnerStream() const
{
	scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
	auto lockOwnerStream = std::find_if(
		mLockAcquisitionStreams.begin(),
		mLockAcquisitionStreams.end(),
		[](const std::shared_ptr<lock_acquisition_stream_t>& streamPtr)
	{
		return streamPtr->isLockOwner;
	});

	if (lockOwnerStream == mLockAcquisitionStreams.end() ||
		(*lockOwnerStream)->streamContext->isCancelled())
	{
		return nullptr;
	}

	return *lockOwnerStream;
}

std::shared_ptr<lock_acquisition_stream_t> SharedMemoryLock::GetStreamById(const std::uint32_t id) const
{
	scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
	auto stream = std::find_if(
		mLockAcquisitionStreams.begin(),
		mLockAcquisitionStreams.end(),
		[id](const std::shared_ptr<lock_acquisition_stream_t>& streamPtr)
	{
		return id == streamPtr->id;
	});

	if (stream == mLockAcquisitionStreams.end() ||
		(*stream)->streamContext->isCancelled())
	{
		return nullptr;
	}

	return *stream;
}

std::vector<std::shared_ptr<lock_acquisition_stream_t>>& SharedMemoryLock::GetStreams()
{
	return mLockAcquisitionStreams;
}

LockState SharedMemoryLock::GetLockState() const
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

void LockState::operator=(const LockState& state)
{
	owner = state.owner;
	requester = state.requester;
}
