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
#include <boost/date_time/posix_time/posix_time.hpp>
#pragma endregion Includes

using namespace boost::interprocess;

SharedMemory::SharedMemory()
	: mLockState(LockState::Unlocked)
	, mMutex()
{
}

SharedMemoryLock::SharedMemoryLock(const std::string& name)
	: mMemoryName(name)
	, mSharedMemoryHandlerPtr(nullptr)
	, mMappedRegionPtr(nullptr)
	, mSharedMemory(nullptr)
	, mChanged(false)
{
	cleanSharedMemory(); // Clean potential garbage
	createOrOpenSharedMemorySegment();
}

SharedMemoryLock::~SharedMemoryLock()
{
	cleanSharedMemory();
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
		catch (const interprocess_exception& e)
		{
			SV_LOG_GLOBAL(error) << "Couldn't open shared memory named " << mMemoryName << ": " << e.what();
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

bool SharedMemoryLock::Acquire(const LockState lockState, const std::string& username)
{
	if (mSharedMemory == nullptr)
	{
		SV_LOG_GLOBAL(error) << "Acquire(): Pointer to shared memory is not initialized!";
		return false;
	}

	if (lockState == LockState::Unlocked)
	{
		SV_LOG_GLOBAL(error) << "Acquire(): Acquiring lock using 'Unlocked' state is not allowed!";
		return false;
	}

	scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex, try_to_lock);
	if (lock)
	{
		if (changeLockState(lockState))
		{
			mLockOwner = username;
			mChanged = true;
			return true;
		}

		return false;
	}
	else
	{
		SV_LOG_GLOBAL(debug) << "Interprocess mutex already locked by another process!";
		return false;
	}
}

bool SharedMemoryLock::changeLockState(const LockState lockState)
{
	const LockState currentLockState = getLockState();

	if (currentLockState == LockState::LockedBySVOGateway)
	{
		SV_LOG_GLOBAL(info)
			<< "Acquire(): Shared memory is already locked by RPC client connected to SVOGateway!";
		return false;
	}
	else if (currentLockState == LockState::LockedBySVObserver)
	{
		if (lockState == LockState::LockedBySVObserver)
		{
			// In SVObserver we don't use RPC mechanism, so if lockState is the same
			// as currentLockState then we treat it as true, because there is only one
			// direct user of SVObserver
			return true;
		}

		SV_LOG_GLOBAL(info) << "Acquire(): Shared memory is already locked by SVObserver!";
		return false;
	}

	setLockState(lockState);
	return true;
}

bool SharedMemoryLock::Takeover(const std::string& username)
{
	if (mSharedMemory == nullptr)
	{
		SV_LOG_GLOBAL(error) << "Takeover(): Pointer to shared memory is not initialized!";
		return false;
	}

	scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
	mLockOwner = username;
	mChanged = true;
	const LockState currentLockState = getLockState();

	// This is a little hack because in SVObserver we're using IPC only,
	// so it is difficult to notify SVObserver about changing lock state.
	// The result will be in next call of Acquire() from SVObserver which 
	// should return false
	if (currentLockState == LockState::LockedBySVObserver)
	{
		setLockState(LockState::LockedBySVOGateway);
	}

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
	mSharedMemory->mLockState = LockState::Unlocked;
	mLockOwner = "";
	mChanged = true;
	SV_LOG_GLOBAL(info) << "Release(): Shared memory lock released!";
}

bool SharedMemoryLock::ResetIfChanged()
{
	scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
	if (mChanged)
	{
		mChanged = false;
		return true;
	}

	return mChanged;
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

std::string SharedMemoryLock::GetLockOwner() const
{
	return mLockOwner;
}

LockState SharedMemoryLock::getLockState() const
{
	return mSharedMemory->mLockState;
}

void SharedMemoryLock::setLockState(const LockState lockState)
{
	mSharedMemory->mLockState = lockState;
	SV_LOG_GLOBAL(debug) << mMemoryName << " shared memory locked!";
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
