#pragma once

#include <memory>
#include <string>

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>

struct lock_acquisition_stream_t;

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
	SharedMemoryLock(const std::string& name = "sv_default_shared_memory");

	bool Acquire(const LockState lockState, const std::string& username);
	bool Takeover(const std::string& username);
	void Release();
	bool ResetIfChanged();

	void PushBackStream(const std::shared_ptr<lock_acquisition_stream_t>& streamPtr);
	std::shared_ptr<lock_acquisition_stream_t> GetLockOwnerStream() const;
	std::shared_ptr<lock_acquisition_stream_t> GetStreamById(const std::uint32_t id) const;
	std::vector<std::shared_ptr<lock_acquisition_stream_t>>& GetStreams();
	std::string GetLockOwner() const;

private:
	bool changeLockState(const LockState lockState);
	void setLockState(const LockState lockState);
	LockState getLockState() const;

	void createOrOpenSharedMemorySegment();
	void cleanSharedMemory();

	const std::string& mMemoryName;
	std::unique_ptr<boost::interprocess::shared_memory_object> mSharedMemoryHandlerPtr;
	std::unique_ptr<boost::interprocess::mapped_region> mMappedRegionPtr;
	SharedMemory* mSharedMemory;

	std::vector<std::shared_ptr<lock_acquisition_stream_t>> mLockAcquisitionStreams;
	std::string mLockOwner;
	bool mChanged;
};
