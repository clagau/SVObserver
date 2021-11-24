#pragma once

#include <memory>

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>

enum class LockState : std::uint8_t
{
	Unlocked = 0,
	LockedBySVOGateway = 1,
	LockedBySVObserver = 2,
};

struct SharedMemory
{
	SharedMemory();

	LockState mLockState;
	boost::interprocess::interprocess_mutex mMutex;
};

class SharedMemoryLock
{
public:
	SharedMemoryLock(const bool createFirst = false);
	~SharedMemoryLock();

	bool Acquire(const LockState lockState);
	bool Takeover();
	void Release();

private:
	bool changeLockState(const LockState lockState);
	void setLockState(const LockState lockState);
	LockState getLockState() const;

	bool createSharedMemorySegment();
	bool openSharedMemorySegment();
	void cleanSharedMemory();

	std::unique_ptr<boost::interprocess::shared_memory_object> mSharedMemoryHandlerPtr;
	std::unique_ptr<boost::interprocess::mapped_region> mMappedRegionPtr;
	SharedMemory* mSharedMemory;
};
