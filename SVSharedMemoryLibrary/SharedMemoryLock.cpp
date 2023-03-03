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

LockState::LockState()
	: acquired(false)
	, owner(LockOwner::SVOGateway)
{
	std::memset(username, 0, maxUsernameSize);
	std::memset(host, 0, maxHostSize);
}

SharedMemory::SharedMemory()
	: mLockState()
	, mMutex()
{}

SharedMemoryLock::SharedMemoryLock(
	boost::asio::deadline_timer::duration_type expiryTime,
	const lock_state_changed_callback_t& onLockStateChangedCb,
	const std::string& name)
	: mOnLockStateChangedCb(onLockStateChangedCb)
	, mSharedMemoryHandler(open_or_create, name.c_str(), read_write, sizeof(SharedMemory))
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
		SV_LOG_GLOBAL(error) << "Configuration lock state polling error: " << error.message();
		return;
	}

	LockState currentLockState;
	{
		scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
		currentLockState = mSharedMemory->mLockState;
	}
	if (mLastLockState.acquired != currentLockState.acquired ||
		mLastLockState.owner != currentLockState.owner ||
		std::strcmp(mLastLockState.username, currentLockState.username) != 0 ||
		std::strcmp(mLastLockState.host, currentLockState.host) != 0)
	{
		mLastLockState = currentLockState;
		mIoService.dispatch([this]()
		{
			mOnLockStateChangedCb(mLastLockState);
		});
	}
	scheduleLockStateCheck(expiryTime);
}

bool SharedMemoryLock::Acquire(LockOwner owner, const std::string& username, const std::string& host)
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
	if (!mSharedMemory->mLockState.acquired)
	{
		mSharedMemory->mLockState.acquired = true;
		mSharedMemory->mLockState.owner = owner;
		std::strcpy(mSharedMemory->mLockState.username, username.c_str());
		std::strcpy(mSharedMemory->mLockState.host, host.c_str());
		SV_LOG_GLOBAL(debug) << "Acquire(): Shared memory lock acquired!";
		return true;
	}

	return false;
}

bool SharedMemoryLock::Takeover(LockOwner owner, const std::string& username, const std::string& host)
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
	mSharedMemory->mLockState.owner = owner;
	std::strcpy(mSharedMemory->mLockState.username, username.c_str());
	std::strcpy(mSharedMemory->mLockState.host, host.c_str());

	return true;
}

void SharedMemoryLock::Release()
{
	if (mSharedMemory == nullptr)
	{
		SV_LOG_GLOBAL(error) << "Release(): Pointer to shared memory is not initialized!";
		return;
	}

	scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
	mSharedMemory->mLockState.acquired = false;
	std::memset(mSharedMemory->mLockState.username, 0, maxUsernameSize);
	std::memset(mSharedMemory->mLockState.host, 0, maxHostSize);
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
