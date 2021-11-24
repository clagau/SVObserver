#pragma region Includes
#include "stdafx.h"
#include "SharedMemoryLock.h"
#include "SVLogLibrary/Logging.h"

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

namespace
{
	const char* memoryName = "sv_shared_memory";
}

SharedMemory::SharedMemory()
	: mLockState(LockState::Unlocked)
	, mMutex()
{
}

SharedMemoryLock::SharedMemoryLock(const bool createFirst)
	: mSharedMemoryHandlerPtr(nullptr)
	, mMappedRegionPtr(nullptr)
	, mSharedMemory(nullptr)
{
	// SVObserver should be launched first to create space for shared memory.
	// SVOGateway only tries to open existing shared memory!
	if (createFirst)
	{
		cleanSharedMemory(); // Clean potential garbage
		createSharedMemorySegment();
	}
	else
	{
		openSharedMemorySegment();
	}
}

SharedMemoryLock::~SharedMemoryLock()
{
	cleanSharedMemory();
}

bool SharedMemoryLock::createSharedMemorySegment()
{
	try
	{
		mSharedMemoryHandlerPtr = std::make_unique<shared_memory_object>(
			create_only, memoryName, read_write);
		mSharedMemoryHandlerPtr->truncate(sizeof(SharedMemory));

		mMappedRegionPtr = std::make_unique<mapped_region>(*mSharedMemoryHandlerPtr, read_write);
		void* const address = mMappedRegionPtr->get_address();
		mSharedMemory = new (address) SharedMemory;
	}
	catch (const interprocess_exception& e)
	{
		SV_LOG_GLOBAL(error) << "Couldn't create " << memoryName << " shared memory. Reason: " << e.what();
		return false;
	}

	SV_LOG_GLOBAL(debug) << "Shared memory " << memoryName << " created!";
	return true;
}

bool SharedMemoryLock::openSharedMemorySegment()
{
	try
	{
		mSharedMemoryHandlerPtr = std::make_unique<shared_memory_object>(
			open_only, memoryName, read_write);

		mMappedRegionPtr = std::make_unique<mapped_region>(*mSharedMemoryHandlerPtr, read_write);
		void* const address = mMappedRegionPtr->get_address();
		mSharedMemory = static_cast<SharedMemory*>(address);
	}
	catch (const interprocess_exception& e)
	{
		SV_LOG_GLOBAL(error) << "Couldn't open " << memoryName << " shared memory. Reason: " << e.what();
		return false;
	}

	SV_LOG_GLOBAL(info) << "Shared memory " << memoryName << " opened!";
	return true;
}

bool SharedMemoryLock::Acquire(const LockState lockState)
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
		return changeLockState(lockState);
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

bool SharedMemoryLock::Takeover()
{
	if (mSharedMemory == nullptr)
	{
		SV_LOG_GLOBAL(error) << "Takeover(): Pointer to shared memory is not initialized!";
		return false;
	}

	scoped_lock<interprocess_mutex> lock(mSharedMemory->mMutex);
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
	SV_LOG_GLOBAL(info) << "Release(): Shared memory lock released!";
}

LockState SharedMemoryLock::getLockState() const
{
	return mSharedMemory->mLockState;
}

void SharedMemoryLock::setLockState(const LockState lockState)
{
	mSharedMemory->mLockState = lockState;
	SV_LOG_GLOBAL(debug) << memoryName << " shared memory locked!";
}

void SharedMemoryLock::cleanSharedMemory()
{
	if (shared_memory_object::remove(memoryName))
	{
		SV_LOG_GLOBAL(info) << memoryName << " shared memory removed!";
	}
}
