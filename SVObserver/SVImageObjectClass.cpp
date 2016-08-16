//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageObjectClass
//* .File Name       : $Workfile:   SVImageObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   01 Oct 2013 14:31:00  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "SVImageObjectClass.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVOMFCLibrary/SVFileNameClass.h"
#include "SVGlobal.h"
#include "SVImageProcessingClass.h"
#include "SVTool.h"
#include "SVStatusLibrary\MessageManagerResource.h"  // Use SvStl::MessageMgrDisplayAndNotify
#include "ObjectInterfaces\ErrorNumbers.h"
#pragma endregion Includes

SVImageObjectClass::SVImageObjectClass()
: m_LastUpdate()
, m_LastReset()
, m_HandleCount( 0 )
, m_CurrentDMIndexHandle()
, m_ImageInfo()
, m_ParentImagePtr()
, m_MasterImageHandles()
, m_ImageIndexPool()
, m_ImageHandleArray()
{
}

SVImageObjectClass::~SVImageObjectClass()
{
	DestroyLocal();
}

bool SVImageObjectClass::empty() const
{
	return ( m_ImageHandleArray.empty() || m_MasterImageHandles.empty() );
}

void SVImageObjectClass::clear()
{
	DestroyLocal();
}

unsigned long SVImageObjectClass::size() const
{
	return m_HandleCount;
}

void SVImageObjectClass::resize( unsigned long p_NewSize )
{
	if( m_HandleCount != p_NewSize )
	{
		m_HandleCount = p_NewSize;

		m_LastUpdate = SVClock::GetTimeStamp();
	}
}

SVImageObjectClass::SVImageObjectParentPtr SVImageObjectClass::GetParentImageObject() const
{
	return m_ParentImagePtr;
}

void SVImageObjectClass::SetParentImageObject( SVImageObjectParentPtr p_ParentPtr )
{
	if( m_ParentImagePtr != p_ParentPtr )
	{
		m_ParentImagePtr = p_ParentPtr;

		m_LastUpdate = SVClock::GetTimeStamp();
	}
}

const SVImageInfoClass& SVImageObjectClass::GetImageInfo() const
{
	return m_ImageInfo;
}

void SVImageObjectClass::SetImageInfo( const SVImageInfoClass& p_rImageInfo )
{
	if( m_ImageInfo != p_rImageInfo )
	{
		m_ImageInfo = p_rImageInfo;

		m_LastUpdate = SVClock::GetTimeStamp();
	}
}

const SVClock::SVTimeStamp& SVImageObjectClass::GetLastResetTimeStamp() const
{
	return m_LastReset;
}

HRESULT SVImageObjectClass::ResetObject()
{
	HRESULT l_hrOk = S_OK;

	if ( ! CreateBufferArrays() )
	{
		//@WARNING  S_FALSE should probably not be used here.
		l_hrOk = S_FALSE;  
	}

	if( S_OK == l_hrOk )
	{
		l_hrOk = UpdateTimeStamp();
	}

	return l_hrOk;
}

HRESULT SVImageObjectClass::UpdateTimeStamp()
{
	HRESULT l_hrOk = S_OK;

	m_LastReset = SVClock::GetTimeStamp();

	return l_hrOk;
}

BOOL SVImageObjectClass::Clear( long lIndex, unsigned long ulValue )
{
	BOOL bOk = true;
	
	SVSmartHandlePointer ImageHandle;

	bOk = bOk && GetImageHandle( lIndex, ImageHandle );
	bOk = bOk && ( S_OK == SVImageProcessingClass::Instance().InitBuffer( ImageHandle, ulValue ) );
	
	return bOk;
}

BOOL SVImageObjectClass::Clear( unsigned long ulValue )
{
	BOOL bOk = FALSE;
	
	bOk = Clear( m_CurrentDMIndexHandle.GetIndex(), ulValue );
	
	return bOk;
}

BOOL SVImageObjectClass::ClearAll( unsigned long ulValue )
{
	BOOL bOk = true;
	
	long lSize = static_cast< long >( m_ImageHandleArray.size() );
	
	for ( long l = 0; bOk && l < lSize; l++ )
	{
		bOk = Clear( l, ulValue );
	}
	
	return bOk;
}

BOOL SVImageObjectClass::GetCurrentIndex( SVDataManagerHandle& rDMIndexHandle ) const
{
	BOOL bOk = TRUE;
	
	rDMIndexHandle.Assign( m_CurrentDMIndexHandle, m_CurrentDMIndexHandle.GetLockType() );

	return bOk;
}

BOOL SVImageObjectClass::GetCurrentIndex( SVDataManagerHandle& rDMIndexHandle, SVDataManagerLockTypeEnum p_LockType ) const
{
	BOOL bOk = TRUE;
	
	rDMIndexHandle.Assign( m_CurrentDMIndexHandle, p_LockType );

	return bOk;
}

BOOL SVImageObjectClass::SetCurrentIndex( const SVDataManagerHandle& rDMIndexHandle )
{
	return SetCurrentIndex( rDMIndexHandle, rDMIndexHandle.GetLockType() );
}

BOOL SVImageObjectClass::SetCurrentIndex( const SVDataManagerHandle& rDMIndexHandle, SVDataManagerLockTypeEnum p_LockType )
{
	BOOL bOk = true;

	if( -1 != rDMIndexHandle.GetIndex() )
	{
		bOk &= UnlockIndex( rDMIndexHandle.GetIndex() );
		bOk &= LockIndex( rDMIndexHandle.GetIndex() );

		if( bOk && m_ParentImagePtr.empty() )
		{
			SVMatroxImageInterface::SVStatusCode l_Code;

			SVSmartHandlePointer l_svOutBuffer;
			
			bOk = GetImageHandle( rDMIndexHandle.GetIndex(), l_svOutBuffer ) && !( l_svOutBuffer.empty() );

			if( bOk )
			{
				SVImageBufferHandleImage l_MilHandle;
				l_svOutBuffer->GetData( l_MilHandle );

				l_Code = SVMatroxBufferInterface::ClearBuffer( l_MilHandle.GetBuffer(), 0.0 );
				
				bOk = l_Code == SVMEE_STATUS_OK;
			}
		}
	}

	if ( bOk )
	{
		m_CurrentDMIndexHandle.Assign( rDMIndexHandle, p_LockType );
	}
	else
	{
		m_CurrentDMIndexHandle.clear();
	}
	
	if( bOk && 0 <= m_CurrentDMIndexHandle.GetIndex() && m_CurrentDMIndexHandle.GetIndex() < static_cast< long >( m_ImageHandleArray.size() ) )
	{
		bOk = !( m_ImageHandleArray[ m_CurrentDMIndexHandle.GetIndex() ].empty() );

		assert( bOk );
	}

	return bOk;
}

BOOL SVImageObjectClass::CopyValue( const SVDataManagerHandle& p_FromDMIndexHandle, 
									const SVDataManagerHandle& p_ToDMIndexHandle )
{
	BOOL bOk = TRUE;
	
	if ( m_ParentImagePtr.empty() )
	{
		long l_FromIndex = p_FromDMIndexHandle.GetIndex();
		long l_ToIndex = p_ToDMIndexHandle.GetIndex();

		bOk = l_FromIndex == l_ToIndex;
	
		if( !bOk )
		{
			if( 0 <= l_ToIndex && l_ToIndex < static_cast< long >( m_ImageHandleArray.size() ) )
			{
				bOk = UnlockIndex( l_ToIndex );

				if( bOk )
				{
					if( 0 <= l_FromIndex && l_FromIndex < static_cast< long >( m_ImageHandleArray.size() ) )
					{
						m_ImageHandleArray[ l_ToIndex ] = m_ImageHandleArray[ l_FromIndex ];
					}
				}
			}
			else
			{
				bOk = p_ToDMIndexHandle.GetIndex() == -1;
			}
		}

		if( bOk )
		{
			m_CurrentDMIndexHandle.Assign( p_ToDMIndexHandle, p_ToDMIndexHandle.GetLockType() );
		}
		else
		{
			m_CurrentDMIndexHandle.clear();
		}

		if( 0 <= m_CurrentDMIndexHandle.GetIndex() && m_CurrentDMIndexHandle.GetIndex() < static_cast< long >( m_ImageHandleArray.size() ) )
		{
			if( m_ImageHandleArray[ m_CurrentDMIndexHandle.GetIndex() ].empty() )
			{
				bOk = SetCurrentIndex( m_CurrentDMIndexHandle );
			}

			assert( bOk );
		}
	}			
	else
	{
		bOk &= SetCurrentIndex( p_ToDMIndexHandle );
	}

	if( 0 <= m_CurrentDMIndexHandle.GetIndex() && m_CurrentDMIndexHandle.GetIndex() < static_cast< long >( m_ImageHandleArray.size() ) )
	{
		bOk &= !( m_ImageHandleArray[ m_CurrentDMIndexHandle.GetIndex() ].empty() );

		assert( bOk );
	}

	return bOk;
}

BOOL SVImageObjectClass::CopyValue( const SVDataManagerHandle& p_ToDMIndexHandle )
{
	return CopyValue( m_CurrentDMIndexHandle, p_ToDMIndexHandle );
}

BOOL SVImageObjectClass::GetImageHandle( long lIndex, SVSmartHandlePointer& p_rValuePtr ) const
{
	BOOL bOk = true;
	
	bOk = bOk && ( 0 <= lIndex );
	bOk = bOk && ( lIndex < static_cast< long >( m_ImageHandleArray.size() ) );
	bOk = bOk && !( m_ImageHandleArray[ lIndex ].empty() );
	
	p_rValuePtr.clear();

	if( bOk )
	{
		p_rValuePtr = m_ImageHandleArray[ lIndex ]->m_ImageHandle;
	}

	bOk = ! p_rValuePtr.empty();
	
	return bOk;
}

BOOL SVImageObjectClass::GetImageHandle( SVSmartHandlePointer& p_rValuePtr ) const
{
	BOOL bOk = true;

	long l_Index = m_CurrentDMIndexHandle.GetIndex();

	if( l_Index < 0 )
	{
		l_Index = 0;
	}

	bOk = GetImageHandle( l_Index, p_rValuePtr );

	return bOk;
}

HRESULT SVImageObjectClass::LoadImageFullSize( LPCTSTR p_szFileName, SVImageExtentClass& p_rNewExtent  )
{
	ASSERT(m_CurrentDMIndexHandle.GetIndex() >= 0);
	HRESULT l_hrOk = LoadImage( p_szFileName, m_CurrentDMIndexHandle, true );

	p_rNewExtent = GetImageInfo().GetExtents();

	return l_hrOk;
}

HRESULT SVImageObjectClass::LoadImage( LPCTSTR p_szFileName, SVDataManagerHandle& p_rToDMIndexHandle, bool p_bRestore )
{
	HRESULT l_hrOk = S_FALSE;

	SVSmartHandlePointer l_svHandle;

	if( SetCurrentIndex( p_rToDMIndexHandle ) &&
			GetImageHandle( p_rToDMIndexHandle.GetIndex(), l_svHandle ) && !( l_svHandle.empty() ) )
	{
		
		SVMatroxImageInterface::SVStatusCode l_Code;

		SVImageBufferHandleImage l_MilHandle;
		l_svHandle->GetData( l_MilHandle );
		
		l_Code = SVMatroxBufferInterface::ClearBuffer( l_MilHandle.GetBuffer(), 0 );
		
		if( l_Code == SVMEE_STATUS_OK )
		{
			if( 0 < strlen( p_szFileName ) )
			{
				SVFileNameClass	svfncImageFile(p_szFileName);
				
				SVMatroxFileTypeEnum fileformatID = SVFileUnknown;
				
				CString strExtension = svfncImageFile.GetExtension();
				
				if( strExtension.CompareNoCase(_T( ".mim" )) == 0 )
					fileformatID = SVFileMIL;
				
				if( strExtension.CompareNoCase(_T( ".tif" )) == 0 )
					fileformatID = SVFileTiff;
				
				if( strExtension.CompareNoCase(_T( ".bmp" )) == 0 )
					fileformatID = SVFileBitmap;
				
				if( fileformatID != -1 && ::SVFileExists( p_szFileName ) )
				{
					SVString l_strFile(p_szFileName);
					
					SVImageBufferHandleImage l_MilHandle;
					l_svHandle->GetData( l_MilHandle );
		
					l_Code = SVMatroxBufferInterface::Import( l_MilHandle.GetBuffer(), l_strFile, fileformatID, p_bRestore );

					if( p_bRestore )
					{
						long l_lWidth = 0;
						long l_lHeight = 0;
						SVMatroxBufferInterface::SVStatusCode l_Stat;
						l_Stat = SVMatroxBufferInterface::Get( l_MilHandle.GetBuffer(), SVSizeX, l_lWidth );
						l_Stat = SVMatroxBufferInterface::Get( l_MilHandle.GetBuffer(), SVSizeY, l_lHeight );
						m_ImageInfo.SetExtentProperty( SVExtentPropertyEnum::SVExtentPropertyWidth, l_lWidth);
						m_ImageInfo.SetExtentProperty( SVExtentPropertyEnum::SVExtentPropertyHeight, l_lHeight);
						m_ImageInfo.SetExtentProperty( SVExtentPropertyEnum::SVExtentPropertyPositionPointX, 0 );
						m_ImageInfo.SetExtentProperty( SVExtentPropertyEnum::SVExtentPropertyPositionPointY, 0 );
						m_ImageInfo.SetExtentProperty( SVExtentPropertyEnum::SVExtentPropertyHeightScaleFactor, 1.0 );
						m_ImageInfo.SetExtentProperty( SVExtentPropertyEnum::SVExtentPropertyWidthScaleFactor, 1.0 );
						ResetObject();
					}

					if( l_Code == SVMEE_STATUS_OK )
					{
						l_hrOk = S_OK;
					}
				}
			}
			else
			{
				l_hrOk = S_OK;
			}
		}
	}

	return l_hrOk;
}

HRESULT SVImageObjectClass::GetImageExtentFromFile( LPCTSTR pFileName, SVImageExtentClass& rExtent  )
{
	HRESULT hrOk = E_FAIL;
	SVMatroxImageInterface::SVStatusCode Code = E_FAIL; 
	long Width(0), Height(0);

	if( 0 < strlen( pFileName ) )
	{
		SVFileNameClass	svfncImageFile(pFileName);
		SVMatroxFileTypeEnum fileformatID = SVFileUnknown;
		CString strExtension = svfncImageFile.GetExtension();

		if( strExtension.CompareNoCase(_T( ".mim" )) == 0 )
			fileformatID = SVFileMIL;

		if( strExtension.CompareNoCase(_T( ".tif" )) == 0 )
			fileformatID = SVFileTiff;

		if( strExtension.CompareNoCase(_T( ".bmp" )) == 0 )
			fileformatID = SVFileBitmap;

		if( fileformatID != -1 && ::SVFileExists( pFileName ) )
		{

			SVString strFile(pFileName);

			Code =  SVMatroxBufferInterface::GetImageSize(strFile,Width,Height);
		}

	}

	if(Code == SVMEE_STATUS_OK)
	{
		rExtent.SetExtentProperty( SVExtentPropertyEnum::SVExtentPropertyWidth, Width);
		rExtent.SetExtentProperty( SVExtentPropertyEnum::SVExtentPropertyHeight, Height);
		rExtent.SetExtentProperty( SVExtentPropertyEnum::SVExtentPropertyPositionPointX, 0 );
		rExtent.SetExtentProperty( SVExtentPropertyEnum::SVExtentPropertyPositionPointY, 0 );
		rExtent.SetTranslation(SVExtentTranslationNone);
		rExtent.UpdateData();
		hrOk = S_OK;
	}
	return hrOk;	
}

HRESULT SVImageObjectClass::CopyToHandle( SVSmartHandlePointer &p_rHandle )
{
	HRESULT l_Status = S_OK;

	if( ! p_rHandle.empty() )
	{
		SVImageBufferHandleImage l_ToMilHandle;
		p_rHandle->GetData( l_ToMilHandle );

		SVMatroxBufferInterface::SVStatusCode l_Code;

		SVSmartHandlePointer l_svHandle;

		if ( GetImageHandle( l_svHandle ) && !( l_svHandle.empty() ) )
		{
			SVImageBufferHandleImage l_FromMilHandle;
			l_svHandle->GetData( l_FromMilHandle );

			l_Code = SVMatroxBufferInterface::CopyBuffer( l_ToMilHandle.GetBuffer(), l_FromMilHandle.GetBuffer() );

			if( l_Code != SVMEE_STATUS_OK )
			{
				l_Status = E_FAIL;
			}
		}
		else
		{
			SVMatroxBufferInterface::ClearBuffer( l_ToMilHandle.GetBuffer(), 0.0 );

			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}
	
	return l_Status;
}

HRESULT SVImageObjectClass::CopyToBSTR( BSTR &p_rbstrData )
{
	HRESULT l_Status = S_OK;

	BITMAPINFO* pbmInfo = nullptr;

	SVSmartHandlePointer l_svOrigHandle;
	SVSmartHandlePointer l_svCopyHandle;

	SVSmartHandlePointer l_psvHandle;

	if( ! GetImageHandle( l_svOrigHandle ) || l_svOrigHandle.empty() || l_svOrigHandle->GetBitmapInfo().empty() || nullptr == l_svOrigHandle->GetBufferAddress() )
	{
		l_Status = S_FALSE;
	}

	if( S_OK == l_Status )
	{
		l_psvHandle = l_svOrigHandle;
	}
	else
	{
		l_Status = SVImageProcessingClass::Instance().CreateImageBuffer( m_ImageInfo, l_svCopyHandle );

		if( S_OK == l_Status )
		{
			if( ! CopyToHandle( l_svCopyHandle ) || l_svCopyHandle.empty() || l_svCopyHandle->GetBitmapInfo().empty() || nullptr == l_svCopyHandle->GetBufferAddress() )
			{
				l_Status = E_FAIL;
			}
		}

		if( S_OK == l_Status )
		{
			l_psvHandle = l_svCopyHandle;
		}
	}

	if( S_OK == l_Status )
	{
		SVBitmapInfo l_BitmapInfo = l_psvHandle->GetBitmapInfo();
		const BITMAPINFO* pbmInfo = l_BitmapInfo.GetBitmapInfo();
		const BITMAPINFOHEADER* pbmhInfo = (LPBITMAPINFOHEADER) &pbmInfo->bmiHeader;

		// Source images seem to be flipped even though MIL is not supposed to flip them
		if( pbmhInfo->biHeight > 0 )
		{
			l_BitmapInfo.FlipHeight();
		}
		
		long l_lBitmapHeaderSize = sizeof( BITMAPINFOHEADER );
		long l_lColorTableSize = 0;
		long l_lDIBSize = pbmhInfo->biSizeImage;

		if( 0 < pbmhInfo->biClrUsed )
		{
			if( pbmhInfo->biBitCount < 16 )
			{
				l_lColorTableSize = pbmhInfo->biClrUsed * sizeof( RGBQUAD );
			}
			else
			{
				l_lColorTableSize = pbmhInfo->biClrUsed * sizeof( RGBQUAD );

				if( pbmhInfo->biBitCount == 16 || pbmhInfo->biBitCount == 32 )
				{
					l_lColorTableSize += 3 * sizeof( DWORD );
				}
			}
		}

		// Calculate total size buffer needed for image
		long lBufSize = 0;
		
		lBufSize += l_lBitmapHeaderSize;
		lBufSize += l_lColorTableSize;
		lBufSize += l_lDIBSize;
		
		if( nullptr != p_rbstrData )
		{
			::SysFreeString( p_rbstrData );

			p_rbstrData = nullptr;
		}

		p_rbstrData = ::SysAllocStringByteLen(nullptr, lBufSize);

		unsigned char *l_pucStart = (unsigned char *)(p_rbstrData);
		BITMAPINFO *l_pbmiInfo = (BITMAPINFO *)(p_rbstrData);

		memcpy( l_pucStart, pbmhInfo, l_lBitmapHeaderSize );

		l_pucStart += l_lBitmapHeaderSize;

		if( 0 < l_lColorTableSize )
		{
			memcpy( l_pucStart, pbmInfo->bmiColors, l_lColorTableSize );

			l_pucStart += l_lColorTableSize;
		}

		memcpy( l_pucStart, l_psvHandle->GetBufferAddress(), l_lDIBSize );
	}

	l_svCopyHandle.clear();
	
	return l_Status;
}

HRESULT SVImageObjectClass::CopyFromBSTR( BSTR p_Image )
{
	HRESULT l_Status = S_OK;
	
	SVSmartHandlePointer l_Handle;

	if( GetImageHandle( l_Handle ) )
	{
		SVImageInfoClass l_Info = m_ImageInfo;

		l_Status = SVImageProcessingClass::Instance().LoadImageBuffer( (void*)p_Image, l_Info, l_Handle, m_ImageInfo );
	}
	else
	{
		l_Status = S_FALSE;
	}

	return l_Status;
}

void SVImageObjectClass::SetTranslationOffset(double offsetX, double offsetY)
{
	m_ImageInfo.SetExtentProperty(SVExtentPropertyTranslationOffsetX, offsetX);
	m_ImageInfo.SetExtentProperty(SVExtentPropertyTranslationOffsetY, offsetY);
}

SVImageObjectClass::SVImageObjectElement::SVImageObjectElement()
: m_MasterIndex( -1 ), m_ParentPtr(), m_ImageHandle()
{
}

SVImageObjectClass::SVImageObjectElement::SVImageObjectElement( size_t p_MasterIndex, SVImageObjectParentPtr p_ParentPtr, SVSmartHandlePointer p_ImageHandle )
: m_MasterIndex( p_MasterIndex ), m_ParentPtr( p_ParentPtr ), m_ImageHandle( p_ImageHandle )
{
}

BOOL SVImageObjectClass::CreateBufferArrays()
{
	BOOL bOk = true;
	BOOL l_ValidExtents = true;

	DestroyBufferArrays();

	if( !( m_ParentImagePtr.empty() ) )
	{
		SVImageInfoClass l_ParentInfo = m_ParentImagePtr->GetImageInfo();

		m_HandleCount = m_ParentImagePtr->size();

		l_ValidExtents = ( S_OK == l_ParentInfo.ValidateAgainstOutputSpace( m_ImageInfo.GetExtents() ) );

		if( !l_ValidExtents )
		{
			clear();
		}
	}

	if( l_ValidExtents && bOk )
	{
		if( m_HandleCount < 1 )
		{
			m_HandleCount = 1;
		}

		bOk = CreateImageHandleArray( m_HandleCount );

		if( bOk )
		{
			// JMS - This block is used to initialize non-embedded image objects

			for( long i = m_HandleCount - 1; bOk && 0 <= i; --i )
			{
				bOk &= UnlockIndex( i );
				bOk &= LockIndex( i );

				if( bOk && m_ParentImagePtr.empty() )
				{
					SVSmartHandlePointer l_svOutBuffer;
					
					bOk = GetImageHandle( i, l_svOutBuffer );

					if( bOk )
					{
						SVImageBufferHandleImage l_MilHandle;
						l_svOutBuffer->GetData( l_MilHandle );

						SVMatroxBufferInterface::SVStatusCode l_Code;

						l_Code = SVMatroxBufferInterface::ClearBuffer( l_MilHandle.GetBuffer(), 0.0 );

						bOk = l_Code == SVMEE_STATUS_OK;
					}
				}
			}
		}
	}

	if( 0 <= m_CurrentDMIndexHandle.GetIndex() && m_CurrentDMIndexHandle.GetIndex() < static_cast< long >( m_ImageHandleArray.size() ) )
	{
		bOk &= !( m_ImageHandleArray[ m_CurrentDMIndexHandle.GetIndex() ].empty() );
	}

	return bOk;
}

BOOL SVImageObjectClass::DestroyBufferArrays()
{
	BOOL bOk = DestroyImageHandleArray();

	return bOk;
}

BOOL SVImageObjectClass::CreateImageBuffer(SVImageInfoClass &rInfo, long p_Index, SVImageObjectElementPtr& p_Handle )
{
	BOOL bOk = true;
	HRESULT hr = S_OK;
	
	DestroyImageBuffer( p_Handle );

	SVImageObjectElementPtr l_IndexHandle;
	SVSmartHandlePointer l_Handle;

	hr = SVImageProcessingClass::Instance().CreateImageBuffer( rInfo, l_Handle );
	if (S_FALSE == hr)
	{
		hr = SVMSG_SVO_5065_COULDNOTCREATEIMAGEBUFFER;
	}
	else
	if (SVMSG_SVO_5067_IMAGEALLOCATIONFAILED == hr)
	{
		SVToolClass*	parentTool = static_cast <SVToolClass*> (rInfo.GetOwner());

		if (nullptr == parentTool)
		{
			// Image does not have a Tool for a parent. Not sure if this can 
			// happen.
			SvStl::MessageMgrDisplayAndNotify Exception(  SvStl::LogAndDisplay );
			Exception.setMessage( hr, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), SvOi::ProgCode_5066_CreateImageBuffer);
		}
		else
		{
			SvStl::MessageContainer message;
			message.setMessage( hr, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
			parentTool->addTaskMessage( message );
		}
	}

	bOk = !( l_Handle.empty() ) && SUCCEEDED (hr);

	if( bOk )
	{
		p_Handle = new SVImageObjectElement( p_Index, SVImageObjectParentPtr(), l_Handle );

		bOk = !( p_Handle.empty() );
	}
	else
	{
		p_Handle.clear();
	}
	
	return bOk;
}

BOOL SVImageObjectClass::CreateImageChildBuffer(const SVImageInfoClass& rParentInfo, 
                                                SVSmartHandlePointer pParentHandle, 
                                                SVImageInfoClass& rChildInfo, 
                                                long p_Index, SVImageObjectElementPtr& p_Handle)
{
	BOOL bOk = true;
	
	DestroyImageBuffer( p_Handle );

	bOk = !( pParentHandle.empty() );

	SVSmartHandlePointer l_Handle;
	SVImageBufferHandleImage l_MilHandle;

	if( bOk )
	{
		SVImageProcessingClass::Instance().CreateImageChildBuffer( rParentInfo, pParentHandle, rChildInfo, l_Handle );

		bOk = !( l_Handle.empty() );
	}

	if( bOk )
	{
		p_Handle = new SVImageObjectElement( p_Index, m_ParentImagePtr, l_Handle );

		bOk = !( p_Handle.empty() );
	}
	else	
	{
		p_Handle.clear();
	}

	return bOk;
}

BOOL SVImageObjectClass::DestroyImageBuffer(SVImageObjectElementPtr& pHandle)
{
	BOOL bOk = true;
	
	if( !( pHandle.empty() ) )
	{
		long l_Index = static_cast<long>(pHandle->m_MasterIndex);

		m_ImageIndexPool.erase( l_Index );
	}
	
	return bOk;
}

BOOL SVImageObjectClass::GetArrayImageHandle( long lIndex, SVSmartHandlePointer& rHandle ) const
{
	SVImageHandleMap::const_iterator l_Iter( m_MasterImageHandles.find( lIndex ) );

	BOOL bOk = l_Iter != m_MasterImageHandles.end();

	bOk = bOk && !( l_Iter->second.empty() );
	
	if ( bOk )
	{
		rHandle = l_Iter->second->m_ImageHandle;
	}
	else
	{
		rHandle.clear();
	}

	bOk = ! rHandle.empty();
	
	return bOk;
}

BOOL SVImageObjectClass::CreateImageHandleArray( long lSize ) 
{
	BOOL bOk = true;

	DestroyImageHandleArray();

	m_ImageHandleArray.resize( lSize );

	for ( long l = 0; bOk && l < lSize; l++ )
	{
		SVImageObjectElementPtr p_Handle;

		if( !( m_ParentImagePtr.empty() ) )
		{
			SVSmartHandlePointer l_ParentHandlePtr;
			
			bOk = m_ParentImagePtr->GetArrayImageHandle( l, l_ParentHandlePtr );

			if( bOk )
			{
				bOk = CreateImageChildBuffer( m_ParentImagePtr->GetImageInfo(), l_ParentHandlePtr, m_ImageInfo, l, p_Handle );
			}
		}
		else
		{
			bOk = CreateImageBuffer( m_ImageInfo, l, p_Handle );
		}

		if( bOk )
		{
			m_MasterImageHandles[ l ] = p_Handle;

			m_ImageIndexPool.insert( l );

			assert( l != m_CurrentDMIndexHandle.GetIndex() );
		}
	}

	if ( bOk )
	{
		SVDataManagerHandle l_DMIndexHandle = m_CurrentDMIndexHandle;

		if( !( m_ParentImagePtr.empty() ) )
		{
			m_ParentImagePtr->GetCurrentIndex( l_DMIndexHandle );
		}

		bOk = SetCurrentIndex( l_DMIndexHandle );
	}

	if( ! bOk )
	{
		DestroyImageHandleArray();
	}

	if( 0 <= m_CurrentDMIndexHandle.GetIndex() && m_CurrentDMIndexHandle.GetIndex() < static_cast< long >( m_ImageHandleArray.size() ) )
	{
		bOk &= !( m_ImageHandleArray[ m_CurrentDMIndexHandle.GetIndex() ].empty() );

		assert( bOk );
	}

	return bOk;
}

BOOL SVImageObjectClass::DestroyImageHandleArray()
{
	BOOL bOk = TRUE;
	
	m_CurrentDMIndexHandle.clear();
	m_ImageHandleArray.clear();
	m_ImageIndexPool.clear();
	m_MasterImageHandles.clear();

	m_LastReset = 0;
	m_LastUpdate = 0;

	return bOk;
}

HRESULT SVImageObjectClass::GetMasterIndex( size_t p_Index, size_t& p_rMasterIndex ) const
{
	HRESULT l_Status = S_OK;

	p_rMasterIndex = -1;

	if( p_Index < m_ImageHandleArray.size() )
	{
		SVImageObjectElementPtr l_Handle = m_ImageHandleArray[ p_Index ];

		if( !( l_Handle.empty() ) )
		{
			p_rMasterIndex = l_Handle->m_MasterIndex;
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

BOOL SVImageObjectClass::LockIndex(long lIndex, long lHandleIndex )
{
	BOOL bOk = ( -1 <= lIndex && lIndex < static_cast< long >( m_ImageHandleArray.size() ) );
	
	if( 0 <= lIndex && lIndex < static_cast< long >( m_ImageHandleArray.size() ) )
	{
		SVImageObjectElementPtr l_Handle;

		if ( -1 <= lHandleIndex && lHandleIndex < static_cast< long >( m_ImageHandleArray.size() ) )
		{
			if ( lHandleIndex != -1 )
			{
				SVImageHandleMap::iterator l_Iter( m_MasterImageHandles.find( lHandleIndex ) );

				if( ( l_Iter != m_MasterImageHandles.end() ) )
				{
					l_Handle = l_Iter->second;

					m_ImageIndexPool.erase( lHandleIndex );
				}
			}
		}

		m_ImageHandleArray[ lIndex ] = l_Handle;
	}
	else
	{
		bOk = lHandleIndex == -1;
	}
	
	return bOk;
}

BOOL SVImageObjectClass::LockIndex(long lIndex)
{
	BOOL bOk = TRUE;
	
	if ( m_ParentImagePtr.empty() )
	{
		bOk = ( -1 <= lIndex && lIndex < static_cast< long >( m_ImageHandleArray.size() ) );
		
		if( 0 <= lIndex && lIndex < static_cast< long >( m_ImageHandleArray.size() ) )
		{
			SVImageIndexSet::iterator l_Iter( m_ImageIndexPool.begin() );

			SVImageObjectElementPtr l_Handle;

			if( l_Iter != m_ImageIndexPool.end() )
			{
				long l_Index = *( l_Iter );

				SVImageHandleMap::iterator l_Iter( m_MasterImageHandles.find( l_Index ) );

				if( l_Iter != m_MasterImageHandles.end() )
				{
					l_Handle = l_Iter->second;
				}

				m_ImageIndexPool.erase( l_Index );
			}

			m_ImageHandleArray[ lIndex ] = l_Handle;
		}
	}
	else
	{
		bOk = !( m_ParentImagePtr.empty() );

		if( bOk )
		{
			size_t l_MasterIndex = 0;

			bOk = ( S_OK == m_ParentImagePtr->GetMasterIndex( lIndex, l_MasterIndex ) );

			if( bOk )
			{
				bOk = LockIndex( lIndex, static_cast<long>(l_MasterIndex ));
			}
		}
	}
	
	return bOk;
}

BOOL SVImageObjectClass::UnlockIndex(long lIndex)
{
	BOOL bOk = ( -1 <= lIndex && lIndex < static_cast< long >( m_ImageHandleArray.size() ) );

	if( 0 <= lIndex && lIndex < static_cast< long >( m_ImageHandleArray.size() ) )
	{
		SVImageObjectElementPtr l_Handle = m_ImageHandleArray[ lIndex ];

		m_ImageHandleArray[ lIndex ].clear();

		if( !( l_Handle.empty() ) )
		{
			if( l_Handle.use_count() < 3 )
			{
				AddImageHandleToPool( static_cast<long>(l_Handle->m_MasterIndex) );
			}
		}
	}
	
	return bOk;
}

HRESULT SVImageObjectClass::AddImageHandleToPool( long p_Index )
{
	HRESULT l_Status = S_OK;

	m_ImageIndexPool.insert( p_Index );

	return l_Status;
}

BOOL SVImageObjectClass::DestroyLocal()
{
	BOOL bOk = DestroyBufferArrays();

	return bOk;
}

