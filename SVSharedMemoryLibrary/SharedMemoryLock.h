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

struct lock_acquisition_stream_t;

constexpr size_t maxUsernameSize = 32;
constexpr size_t maxHostSize = 16;

enum class LockOwner : std::uint8_t
{
	SVOGateway = 0,
	SVObserver = 1,
};

struct LockState
{
	LockState();

	bool acquired;
	LockOwner owner;
	char username[maxUsernameSize];
	char host[maxHostSize];
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
		boost::asio::deadline_timer::duration_type expiryTime,
		const lock_state_changed_callback_t& onLockStateChangedCb,
		const std::string& name = "sv_default_shared_memory");
	~SharedMemoryLock();

	bool Acquire(LockOwner owner, const std::string& username, const std::string& host);
	bool Takeover(LockOwner owner, const std::string& username, const std::string& host);
	void Release();

	void PushBackStream(const std::shared_ptr<lock_acquisition_stream_t>& streamPtr);
	std::shared_ptr<lock_acquisition_stream_t> GetLockOwnerStream() const;
	std::shared_ptr<lock_acquisition_stream_t> GetStreamById(const std::uint32_t id) const;
	std::vector<std::shared_ptr<lock_acquisition_stream_t>>& GetStreams();
	LockState GetLockState() const;

private:
	void scheduleLockStateCheck(boost::asio::deadline_timer::duration_type expiryTime);
	void onLockStateCheckTimerExpired(
		boost::asio::deadline_timer::duration_type expiryTime,
		const boost::system::error_code& error);

	lock_state_changed_callback_t mOnLockStateChangedCb;
	boost::interprocess::windows_shared_memory mSharedMemoryHandler;
	boost::interprocess::mapped_region mMappedRegion;
	SharedMemory* mSharedMemory;

	std::vector<std::shared_ptr<lock_acquisition_stream_t>> mLockAcquisitionStreams;

	LockState mLastLockState;

	boost::asio::io_service mIoService;
	boost::asio::io_service::work mIoWork;
	std::thread mIoThread;
	boost::asio::deadline_timer mLockStateCheckTimer;
};
