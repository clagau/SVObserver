#pragma once

#include <memory>
#include <string>
#include <functional>

#include <boost/asio.hpp>
#include <boost/asio/basic_deadline_timer.hpp>
#include <boost/asio/time_traits.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>

struct lock_acquisition_stream_t;

enum class LockOwner : std::uint8_t
{
	SVOGateway,
	SVObserver,
};

struct LockState
{
	bool acquired;
	LockOwner owner;
	std::string username;
	std::string host;
};

struct SharedMemory
{
	SharedMemory();

	LockState mLockState;
	boost::interprocess::interprocess_mutex mMutex;
};

using lock_state_changed_callback_t = std::function<void(LockState)>;

class SharedMemoryLock
{
public:
	SharedMemoryLock(
		boost::asio::io_service& ioService,
		boost::asio::deadline_timer::duration_type expiryTime,
		const lock_state_changed_callback_t& onLockStateChangedCb,
		const std::string& name = "sv_default_shared_memory");

	bool Acquire(LockOwner owner, const std::string& username, const std::string& host);
	bool Takeover(LockOwner owner, const std::string& username, const std::string& host);
	void Release();

	void PushBackStream(const std::shared_ptr<lock_acquisition_stream_t>& streamPtr);
	std::shared_ptr<lock_acquisition_stream_t> GetLockOwnerStream() const;
	std::shared_ptr<lock_acquisition_stream_t> GetStreamById(const std::uint32_t id) const;
	std::vector<std::shared_ptr<lock_acquisition_stream_t>>& GetStreams();
	LockState GetLockState() const;

private:
	void createOrOpenSharedMemorySegment();
	void cleanSharedMemory();

	void scheduleLockStateCheck(boost::asio::deadline_timer::duration_type expiryTime);
	void onLockStateCheckTimerExpired(
		boost::asio::deadline_timer::duration_type expiryTime,
		const boost::system::error_code& error);

	boost::asio::io_service& mIoService;
	const std::string& mMemoryName;
	lock_state_changed_callback_t mOnLockStateChangedCb;
	std::unique_ptr<boost::interprocess::shared_memory_object> mSharedMemoryHandlerPtr;
	std::unique_ptr<boost::interprocess::mapped_region> mMappedRegionPtr;
	SharedMemory* mSharedMemory;

	std::vector<std::shared_ptr<lock_acquisition_stream_t>> mLockAcquisitionStreams;

	boost::asio::deadline_timer mLockStateCheckTimer;
	LockState mLastLockState;
};
