//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageObjectClass
//* .File Name       : $Workfile:   SVImageObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   01 Oct 2013 14:31:00  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <set>
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <vector>
#include "SVContainerLibrary/SVVector.h"
#include "SVDataManagerLibrary/SVDataManagerHandle.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVOLibrary/SVQueueObject.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVUtilityLibrary/SVSharedPtr.h"

class SVImageObjectClass
{
public:
	typedef SVSharedPtr< SVImageObjectClass > SVImageObjectParentPtr;

	SVImageObjectClass();
	virtual ~SVImageObjectClass();

	bool empty() const;
	void clear();

	unsigned long size() const;
	void resize( unsigned long p_NewSize );

	SVImageObjectParentPtr GetParentImageObject() const;
	void SetParentImageObject( SVImageObjectParentPtr p_ParentPtr );

	const SVImageInfoClass& GetImageInfo() const;
	void SetImageInfo( const SVImageInfoClass& p_rImageInfo );

	const SVClock::SVTimeStamp& GetLastResetTimeStamp() const;
	
	HRESULT ResetObject();
	HRESULT UpdateTimeStamp();

	BOOL Clear( long lIndex, unsigned long ulValue = 0 );
	BOOL Clear( unsigned long ulValue = 0 );
	BOOL ClearAll( unsigned long ulValue = 0 );

	BOOL GetCurrentIndex( SVDataManagerHandle& rDMIndexHandle ) const;
	BOOL GetCurrentIndex( SVDataManagerHandle& rDMIndexHandle, SVDataManagerLockTypeEnum p_LockType ) const;

	BOOL SetCurrentIndex( const SVDataManagerHandle& rDMIndexHandle );
	BOOL SetCurrentIndex( const SVDataManagerHandle& rDMIndexHandle, SVDataManagerLockTypeEnum p_LockType );
	
	BOOL CopyValue( const SVDataManagerHandle& p_FromDMIndexHandle, const SVDataManagerHandle& p_ToDMIndexHandle );
	BOOL CopyValue( const SVDataManagerHandle& p_ToDMIndexHandle );

	BOOL GetImageHandle( long lIndex, SVSmartHandlePointer& p_rValuePtr ) const;
	BOOL GetImageHandle( SVSmartHandlePointer& p_rValuePtr ) const;

	HRESULT LoadImageFullSize( LPCTSTR p_szFileName, SVImageExtentClass& p_rNewExtent );
	HRESULT LoadImage( LPCTSTR p_szFileName, SVDataManagerHandle& p_rToDMIndexHandle, bool p_bRestore=false );

	HRESULT CopyToHandle( SVSmartHandlePointer& p_rHandle );
	HRESULT CopyToBSTR( BSTR &p_rbstrData );
	HRESULT CopyFromBSTR( BSTR p_Image );

	//************************************
	//! Get the Extension for the Image
	//! \param pFileName [in]
	//! \param rExtent [out]
	//! \returns HRESULT S_OK if no error occurs
	//************************************
	static HRESULT GetImageExtentFromFile( LPCTSTR pFileName, SVImageExtentClass& rExtent );
protected:
	struct SVImageObjectElement
	{
		SVImageObjectElement();
		SVImageObjectElement( size_t p_MasterIndex, SVImageObjectParentPtr p_ParentPtr, SVSmartHandlePointer p_ImageHandle );

		size_t m_MasterIndex;
		SVImageObjectParentPtr m_ParentPtr;
		SVSmartHandlePointer m_ImageHandle;

	private:
		// Do not implement
		SVImageObjectElement( const SVImageObjectElement& p_rObject );

		// Do not implement
		const SVImageObjectElement& operator=( const SVImageObjectElement& p_rObject );

	};

	typedef SVSharedPtr< SVImageObjectElement > SVImageObjectElementPtr;
	typedef std::vector< SVImageObjectElementPtr > SVImageHandlePtrVector;

	typedef std::set< long > SVImageIndexSet;
	typedef std::map< long, SVImageObjectElementPtr > SVImageHandleMap;

	BOOL CreateBufferArrays();
	BOOL DestroyBufferArrays();

	BOOL CreateImageBuffer( SVImageInfoClass &rInfo, long p_Index, SVImageObjectElementPtr& p_Handle );
	BOOL CreateImageChildBuffer( const SVImageInfoClass &rParentInfo, SVSmartHandlePointer pParentHandle, 
	                             SVImageInfoClass &rChildInfo, long p_Index, SVImageObjectElementPtr& p_Handle );
	BOOL DestroyImageBuffer( SVImageObjectElementPtr& p_Handle );

	BOOL GetArrayImageHandle( long lIndex, SVSmartHandlePointer& rHandle ) const;

	BOOL CreateImageHandleArray( long lSize );
	BOOL DestroyImageHandleArray();

	HRESULT GetMasterIndex( size_t p_Index, size_t& p_rMasterIndex ) const;
	BOOL LockIndex( long lIndex, long lHandleIndex );
	BOOL LockIndex( long lIndex );
	BOOL UnlockIndex( long lIndex );
	
	HRESULT AddImageHandleToPool( long p_Index );

	BOOL DestroyLocal();
	
	SVClock::SVTimeStamp m_LastUpdate;
	SVClock::SVTimeStamp m_LastReset;

	long m_HandleCount;

	SVDataManagerHandle	m_CurrentDMIndexHandle;

	SVImageInfoClass m_ImageInfo;

//- m_ParentImagePtr - If this is not NULL, then the image object must be a 
//- Matrox child buffer.
	SVImageObjectParentPtr m_ParentImagePtr;

	SVImageHandleMap m_MasterImageHandles;

	SVImageIndexSet m_ImageIndexPool;
	SVImageHandlePtrVector m_ImageHandleArray;
	
private:
	// Do not implement
	SVImageObjectClass( const SVImageObjectClass& p_rObject );

	// Do not implement
	const SVImageObjectClass& operator=( const SVImageObjectClass& p_rObject );
	
};

typedef SVSharedPtr< SVImageObjectClass > SVImageObjectClassPtr;
typedef SVVector< SVImageObjectClassPtr > SVImageObjectClassPtrVector;

