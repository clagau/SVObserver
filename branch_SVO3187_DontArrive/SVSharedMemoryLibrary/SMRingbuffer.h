//*****************************************************************************
/// \copyright (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file SMRingbuffer.h
/// All Rights Reserved 
//*****************************************************************************
/// Class implements Slot managment  
/// the class holds two ringbuffer of Bufferelemnt one for rejects  
/// To create and access the shared memory administrator rights are necessary
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <memory>
#pragma endregion Includes

namespace  SvSml
{
struct SMParameterStruct;

class SMRingBuffer
{
public:
	enum SlotType { Last, Reject };

private:
	struct BufferElement
	{
		volatile DWORD SyncDWord{0};
		volatile int  SlotNumberLast{0};
		volatile int SlotNumberReject{0};
		volatile int TriggerNumber{-1};
		volatile BOOL isReject{false};
	};

	struct RingbufferInfos
	{
		volatile int TotalSlotCount{0}; //<Number  of RingbufferEntries 
		volatile int  RejectSlotCount{0};
		volatile int LastEntry{0};
		volatile int LastReject{0};
		volatile int LastUsedRejectSlot{0};
	};

public:
	SMRingBuffer();
	~SMRingBuffer();
	/// Create the Ringbuffer in shared memory  
	void CreateConnection(LPCTSTR Name, int nTotal, int nRejects, const SMParameterStruct& rParam);
	///Opens The Ringbuffer in shared Memory
	void OpenConnection(LPCTSTR Name);
	///Close the connnection 
	void CloseConnection();

	///returns a slot number. The readercounter is increased for this slot
	/// returns -1 if the slot is not available 
	int GetReaderSlotLastWritten(SlotType  t);

	///returns a slot number the readercounter is increased for this slot. 
	//returns MAXDWORD if  the slot is not available 
	int GetReaderSlotByTrigger(int Triggercount);

	/// decrease the readercounter  
	void ReleaseReaderSlot(int readerslot);

	/// returns a negativ value if no write slot is available 
	int   GetNextWriteSlot();

	void SetToReject(int writerslot);
	///  Writerlock is released slot becomes the last written slot if it is valid 
	void ReleaseWriteSlot(int slot, int triggerNumber, bool isValid);

	///Triggernumber for Readerslot 
	int GetTriggerNumber(int Readerslot) const;

	///vector with trigger numbers return size of vector max number 
	int GetRejects(std::vector<int> &rRejects) const;

	///return the number of reject slots
	int GetRejectSlotCount() const;



private:
	static void  IncreaseSlotNumber(int &rSlotnumber, int SlotCount);
	static void DecreaseSlotNumber(int &rSlotnumber, int SlotCount);
	static DWORD MatchedSize(DWORD size, DWORD AllocationGranularity);    //<Match size to Allocation Granularity
	bool Push_ToReject(DWORD triggerNumber, int slotNumber);
	void MapViewOfFileWriteInfos();

	const static DWORD  WRITE_FLAG = 0x80000000;
	const static DWORD WRITE_FLAG_COMPLEMENT = ~WRITE_FLAG;
	const  static LPCTSTR s_BaseName;
	int				m_Retry;
	DWORD			m_AllocationGranularity;
	DWORD			m_InfoSize;
	DWORD			m_MatchedInfoSize;
	std::string		m_FileName;
	int				m_TotalSlotCount;
	int				m_RejectSlotCount;
	HANDLE				m_hMapFile;					// Handle to the mapped memory file
	RingbufferInfos		*m_pRingBufferInfos;		   // Buffer that points to the shared memory
	BufferElement		*m_pRingBufferLast;		// Buffer that points to the shared memory
	BufferElement		*m_pRingBufferReject;		// Buffer that points to the shared memory

};
typedef  std::shared_ptr<SMRingBuffer> RingBufferPointer;

}


