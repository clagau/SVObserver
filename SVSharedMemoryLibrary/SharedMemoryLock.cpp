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

SharedMemory::SharedMemory()
	: mLockState{.acquired=false, .owner=LockOwner::SVOGateway, .username="", .host=""}
	, mMutex()
{
}

SharedMemoryLock::SharedMemoryLock(
	boost::asio::io_service& ioService,
	boost::asio::deadline_timer::duration_type expiryTime,
	const lock_state_changed_callback_t& onLockStateChangedCb,
	const std::string& name)
	: mIoService(ioService)
	, mOnLockStateChangedCb(onLockStateChangedCb)
	, mMemoryName(name)
	, mSharedMemoryHandlerPtr(nullptr)
	, mMappedRegionPtr(nullptr)
	, mSharedMemory(nullptr)
	, mLockStateCheckTimer(ioService)
{
	cleanSharedMemory(); // Clean potential garbage
	createOrOpenSharedMemorySegment();
	{
		mLastLockState = GetLockState();
	}
	scheduleLockStateCheck(expiryTime);
}

void SharedMemoryLock::createOrOpenSharedMemorySegment()
{
	bool isSharedMemoryCreated = false;
	try
	{
		mSharedMemoryHandlerPtr = std::make_unique<shared_memory_object>(
			create_only, mMemoryName.c_str(), read_write);
		mSharedMemoryHandlerPtr->truncate(sizeof(SharedMemory));
		isSharedMemoryCreated = true;
	}
	catch (const interprocess_exception& e)
	{
		SV_LOG_GLOBAL(info) << "Couldn't create shared memory: " << e.what();
		try
		{
			mSharedMemoryHandlerPtr = std::make_unique<shared_memory_object>(
				open_only, mMemoryName.c_str(), read_write);
		}
		catch (const interprocess_exception& ex)
		{
			SV_LOG_GLOBAL(error) << "Couldn't open shared memory named " << mMemoryName << ": " << ex.what();
			throw std::runtime_error("Shared memory error");
		}
	}

	mMappedRegionPtr = std::make_unique<mapped_region>(*mSharedMemoryHandlerPtr, read_write);
	void* const address = mMappedRegionPtr->get_address();
	if (isSharedMemoryCreated)
	{
		mSharedMemory = new (address) SharedMemory;
		SV_LOG_GLOBAL(info) << "Shared memory " << mMemoryName << " created!";
	}
	else
	{
		mSharedMemory = static_cast<SharedMemory*>(address);
		SV_LOG_GLOBAL(info) << "Shared memory " << mMemoryName << " opened!";
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

	mSharedMemory->mMutex.lock();
	auto currentLockState = mSharedMemory->mLockState;
	mSharedMemory->mMutex.unlock();

	if (mLastLockState.acquired != currentLockState.acquired ||
		mLastLockState.owner != currentLockState.owner ||
		mLastLockState.username != currentLockState.username ||
		mLastLockState.host != currentLockState.host)
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

	scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
	if (!mSharedMemory->mLockState.acquired)
	{
		mSharedMemory->mLockState.acquired = true;
		mSharedMemory->mLockState.owner = owner;
		mSharedMemory->mLockState.username = username;
		mSharedMemory->mLockState.host = host;
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

	scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
	mSharedMemory->mLockState.owner = owner;
	mSharedMemory->mLockState.username = username;
	mSharedMemory->mLockState.host = host;

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
	mSharedMemory->mLockState.username = "";
	mSharedMemory->mLockState.host = "";
	SV_LOG_GLOBAL(info) << "Release(): Shared memory lock released!";
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

void SharedMemoryLock::cleanSharedMemory()
{
	if (shared_memory_object::remove(mMemoryName.c_str()))
	{
		SV_LOG_GLOBAL(info) << mMemoryName << " shared memory removed!";
	}
	else
	{
		SV_LOG_GLOBAL(warning) << mMemoryName
			<< " shared memory can't be removed becasue it doesn't exist"
			<< ", is already open"
			<< " or memory segment is still mapped by other processes";
	}
}
