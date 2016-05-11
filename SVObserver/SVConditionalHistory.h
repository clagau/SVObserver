//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConditionalHistory
//* .File Name       : $Workfile:   SVConditionalHistory.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:02:06  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVObjectAppClass.h"
#include "SVScalarValue.h"
#include "SVImageBufferStruct.h"
#include "SVOLibrary/SVContainableCriticalSection.h"
#include "SVValueObject.h"
#include "SVLibrary/SVSimpleIndexQueue.h"
#include "SVImageBuffer.h"	// SVImageOverlayClass
#pragma endregion Includes

extern LPCTSTR const PPQCH_Size;
extern LPCTSTR const PPQCH_Overwrite;

//!!!!!!
typedef SVReferenceCountingTemplate < SVImageOverlayClass > SVRefCountImageOverlayClass;
typedef std::vector <SVRefCountImageOverlayClass> SVRefCountImageOverlayStructVectorType;

struct SVConditionalHistoryEntry	// all the data for one product
{
	std::vector <SVScalarValue> vecValues;
	SVRefCountImageBufferStructVectorType vecImages;
	std::vector <SVScalarValue> vecConditionals;
	long lProcessCount;

	SVConditionalHistoryEntry() : lProcessCount(-1) {}
};

class SVInspectionProcess;

struct SVConditionalHistoryData
{
	// the actual history
	std::vector<SVConditionalHistoryEntry> vecHistory;

	// the history input specification
	std::vector <SVScalarValue>        vecConditionals;// list of conditional objects
	std::vector <SVScalarValue>        vecValues;      // list of values to store
	std::vector <SVImageBufferStruct>  vecImages;      // list of images (no buffers) and their infos 

	SVBoolValueObjectClass bvoOverwrite;
	SVLongValueObjectClass lvoSize;

	void swap( SVConditionalHistoryData& rhs );
};

class SVConditionalHistory : public SVObjectClass
{
public:
	SVConditionalHistory();
	virtual ~SVConditionalHistory();

	HRESULT Validate( std::vector <SVScalarValue>& rvecValues, std::vector <SVScalarValue>& rvecImages, std::vector <SVScalarValue>& rvecConditionals );
	virtual HRESULT ResetObject();

	HRESULT SetProperties( SVScalarValueVector& rvecProperties, bool bResetObject = true );
	HRESULT GetProperties( SVScalarValueVector& rvecProperties );
	HRESULT SetList( std::vector <SVScalarValue>* pvecValues, std::vector <SVScalarValue>* pvecImages, std::vector <SVScalarValue>* pvecConditionals, bool bResetObject = true );	// vectors not const because we return status info
	HRESULT GetList( std::vector <SVScalarValue>& rvecValues, std::vector <SVScalarValue>& rvecImages, std::vector <SVScalarValue>& rvecConditionals );
	HRESULT GetHistoryAndClear( std::vector < std::vector <SVScalarValue> >& rvecValues, std::vector < std::vector <SVImageBufferStruct> >& rvecImages, std::vector < std::vector <SVScalarValue> >& rvecConditionals, std::vector<long>& rvecProcessCount );
	HRESULT GetMostRecentHistory( std::vector <SVScalarValue>& rvecValues, std::vector <SVImageBufferStruct>& rvecImages, std::vector <SVScalarValue>& rvecConditionals, long& rlProcessCount );

	HRESULT CollectDataAndStore();
	HRESULT DeleteTool( LPCTSTR p_strToolName );

	void SetInspection( SVInspectionProcess* pInspection );
private:
	std::pair<SVObjectReference, SVInspectionProcess*> ParseName( const CString& strName );
	HRESULT Validate( std::vector <SVScalarValue>& rvec );
	HRESULT GetHistoryEntry( long lIndex, std::vector <SVScalarValue>& rvecValues, std::vector <SVImageBufferStruct>& rvecImages, std::vector <SVScalarValue>& rvecConditionals, long& rlProcessCount );

	SVInspectionProcess*            m_pInspection;
	SVEmptySyncObject               m_CritSec;// does no locking; a placeholder so we can maintain the locking logic

	// the following serve as holders for the inputs between a Set and a ResetObject:
	std::vector <SVScalarValue>        m_vecConditionals;// list of conditional objects
	std::vector <SVScalarValue>        m_vecValues;      // list of values to store
	std::vector <SVImageBufferStruct>  m_vecImages;      // list of images and their infos 
	SVBoolValueObjectClass             m_bvoOverwrite;
	SVLongValueObjectClass             m_lvoSize;

	bool m_bClearQueue;	// a flag instructing the ResetObject to clear the queue (changing value or image inputs)

	SVConditionalHistoryData   m_Data;	// the actual history data used at runtime

	SVSimpleIndexQueue m_Queue;

	//SVTQueueObject<SVConditionalHistoryEntry*> m_Queue;
	// no need for locking (like SVTQueueObject) because all access
	// will be within m_CritSec
	//std::deque<SVConditionalHistoryEntry*> m_Queue;

						// but will deque do allocation of the chunks? can we preallocate?
	                    // we can pass in our own allocator, but how will that interact with freeing the memory?
	                    // we will make copies on the way out, but we need to allocate the vector to put them in
	                    // (do that outside the critsec)

						// preallocate everything on the Reset and use the Data Manager to get the next available
						// slot.   -  overkill

						
						// queue swap on rebuild
};

// SIAC function needs to have a crit sec?
