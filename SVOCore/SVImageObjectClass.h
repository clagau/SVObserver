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

#pragma region Includes
//Moved to precompiled header: #include <set>
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <vector>
#include "SVDataManagerLibrary/SVDataManagerHandle.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVStatusLibrary/MessageContainer.h"
#pragma endregion Includes

class SVImageObjectClass
{
public:
	typedef SVSharedPtr<SVImageObjectClass> SVImageObjectClassPtr;

	SVImageObjectClass();
	virtual ~SVImageObjectClass();

	bool empty() const;
	void clear();

	unsigned long size() const;
	void resize( unsigned long p_NewSize );

	SVImageObjectClassPtr GetParentImageObject() const;
	void SetParentImageObject( SVImageObjectClassPtr p_ParentPtr );

	const SVImageInfoClass& GetImageInfo() const;
	void SetImageInfo( const SVImageInfoClass& p_rImageInfo );

	const SvTl::SVTimeStamp& GetLastResetTimeStamp() const;
	
	bool ResetObject(SvStl::MessageContainerVector *pErrorContainer=nullptr);
	void UpdateTimeStamp();

	bool Clear( long lIndex, unsigned long ulValue = 0 );
	bool Clear( unsigned long ulValue = 0 );
	bool ClearAll( unsigned long ulValue = 0 );

	bool GetCurrentIndex( SVDataManagerHandle& rDMIndexHandle ) const;
	bool GetCurrentIndex( SVDataManagerHandle& rDMIndexHandle, SVDataManagerLockTypeEnum p_LockType ) const;

	bool SetCurrentIndex( const SVDataManagerHandle& rDMIndexHandle );
	bool SetCurrentIndex( const SVDataManagerHandle& rDMIndexHandle, SVDataManagerLockTypeEnum p_LockType );
	
	bool CopyValue( const SVDataManagerHandle& p_FromDMIndexHandle, const SVDataManagerHandle& p_ToDMIndexHandle );
	bool CopyValue( const SVDataManagerHandle& p_ToDMIndexHandle );

	bool GetImageHandle( long lIndex, SVSmartHandlePointer& p_rValuePtr ) const;
	bool GetImageHandle( SVSmartHandlePointer& p_rValuePtr ) const;

	HRESULT LoadImageFullSize( LPCTSTR p_szFileName, SVImageExtentClass& p_rNewExtent );
	HRESULT LoadImage( LPCTSTR p_szFileName, SVDataManagerHandle& p_rToDMIndexHandle, bool p_bRestore=false );

	HRESULT CopyToHandle( SVSmartHandlePointer& p_rHandle );
	HRESULT CopyToBSTR( BSTR &p_rbstrData );
	HRESULT CopyFromBSTR( BSTR p_Image );

	void SetTranslationOffset(double offsetX, double offsetY);

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
		SVImageObjectElement( size_t p_MasterIndex, SVImageObjectClassPtr p_ParentPtr, SVSmartHandlePointer p_ImageHandle );

		size_t m_MasterIndex;
		SVImageObjectClassPtr m_ParentPtr;
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

	bool CreateBufferArrays(SvStl::MessageContainerVector *pErrorContainer=nullptr);
	bool DestroyBufferArrays();

	bool CreateImageBuffer( SVImageInfoClass &rInfo, long p_Index, SVImageObjectElementPtr& p_Handle, SvStl::MessageContainerVector *pErrorContainer=nullptr );
	bool CreateImageChildBuffer( const SVImageInfoClass &rParentInfo, SVSmartHandlePointer pParentHandle, 
	                             SVImageInfoClass &rChildInfo, long p_Index, SVImageObjectElementPtr& p_Handle );
	bool DestroyImageBuffer( SVImageObjectElementPtr& p_Handle );

	bool GetArrayImageHandle( long lIndex, SVSmartHandlePointer& rHandle ) const;

	bool CreateImageHandleArray( long lSize, SvStl::MessageContainerVector *pErrorContainer=nullptr );
	bool DestroyImageHandleArray();

	HRESULT GetMasterIndex( size_t p_Index, size_t& p_rMasterIndex ) const;
	bool LockIndex( long lIndex, long lHandleIndex );
	bool LockIndex( long lIndex );
	bool UnlockIndex( long lIndex );
	
	HRESULT AddImageHandleToPool( long p_Index );

	bool DestroyLocal();


	
private:
	// Do not implement
	SVImageObjectClass( const SVImageObjectClass& p_rObject );

	// Do not implement
	const SVImageObjectClass& operator=( const SVImageObjectClass& p_rObject );

	SvTl::SVTimeStamp m_LastUpdate;
	SvTl::SVTimeStamp m_LastReset;

	long m_HandleCount;

	SVDataManagerHandle	m_CurrentDMIndexHandle;

	SVImageInfoClass m_ImageInfo;

	//- m_ParentImagePtr - If this is not Empty, then the image object must be a 
	//- Matrox child buffer.
	SVImageObjectClassPtr m_ParentImagePtr;

	SVImageHandleMap m_MasterImageHandles;

	SVImageIndexSet m_ImageIndexPool;
	SVImageHandlePtrVector m_ImageHandleArray;
};

typedef SVSharedPtr<SVImageObjectClass> SVImageObjectClassPtr;

