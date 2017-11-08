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
#include "SVLibrary/SVFileNameClass.h"
#include "SVUtilityLibrary/SVUtilityGlobals.h"

#include "SVImageProcessingClass.h"
#include "SVTaskObject.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
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

		m_LastUpdate = SvTl::GetTimeStamp();
	}
}

SVImageObjectClass::SVImageObjectClassPtr SVImageObjectClass::GetParentImageObject() const
{
	return m_ParentImagePtr;
}

void SVImageObjectClass::SetParentImageObject( SVImageObjectClassPtr p_ParentPtr )
{
	if( m_ParentImagePtr != p_ParentPtr )
	{
		m_ParentImagePtr = p_ParentPtr;

		m_LastUpdate = SvTl::GetTimeStamp();
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

		m_LastUpdate = SvTl::GetTimeStamp();
	}
}

const SvTl::SVTimeStamp& SVImageObjectClass::GetLastResetTimeStamp() const
{
	return m_LastReset;
}

bool SVImageObjectClass::ResetObject( SvStl::MessageContainerVector *pErrorContainer )
{
	bool Result = CreateBufferArrays(pErrorContainer);
	
	if( Result )
	{
		UpdateTimeStamp();
	}

	return Result;
}

void SVImageObjectClass::UpdateTimeStamp()
{
	m_LastReset = SvTl::GetTimeStamp();
}

bool SVImageObjectClass::Clear( long lIndex, unsigned long ulValue )
{
	bool Result( true );
	
	SVSmartHandlePointer ImageHandle;

	Result = Result && GetImageHandle( lIndex, ImageHandle );
	Result = Result && ( S_OK == SVImageProcessingClass::InitBuffer( ImageHandle, ulValue ) );
	
	return Result;
}

bool SVImageObjectClass::Clear( unsigned long ulValue )
{
	bool Result( false );
	
	Result = Clear( m_CurrentDMIndexHandle.GetIndex(), ulValue );
	
	return Result;
}

bool SVImageObjectClass::ClearAll( unsigned long ulValue )
{
	bool Result( true );
	
	long lSize = static_cast< long >( m_ImageHandleArray.size() );
	
	for ( long l = 0; Result && l < lSize; l++ )
	{
		Result = Clear( l, ulValue );
	}
	
	return Result;
}

bool SVImageObjectClass::GetCurrentIndex( SVDataManagerHandle& rDMIndexHandle ) const
{
	bool Result( true );
	
	rDMIndexHandle.Assign( m_CurrentDMIndexHandle, m_CurrentDMIndexHandle.GetLockType() );

	return Result;
}

bool SVImageObjectClass::GetCurrentIndex( SVDataManagerHandle& rDMIndexHandle, SVDataManagerLockTypeEnum p_LockType ) const
{
	bool Result( true );
	
	rDMIndexHandle.Assign( m_CurrentDMIndexHandle, p_LockType );

	return Result;
}

bool SVImageObjectClass::SetCurrentIndex( const SVDataManagerHandle& rDMIndexHandle )
{
	return SetCurrentIndex( rDMIndexHandle, rDMIndexHandle.GetLockType() );
}

bool SVImageObjectClass::SetCurrentIndex( const SVDataManagerHandle& rDMIndexHandle, SVDataManagerLockTypeEnum p_LockType )
{
	bool Result( true );

	if( -1 != rDMIndexHandle.GetIndex() )
	{
		Result &= UnlockIndex( rDMIndexHandle.GetIndex() );
		Result &= LockIndex( rDMIndexHandle.GetIndex() );

		if( Result && m_ParentImagePtr.empty() )
		{
			HRESULT l_Code;

			SVSmartHandlePointer l_svOutBuffer;
			
			Result = GetImageHandle( rDMIndexHandle.GetIndex(), l_svOutBuffer ) && !( l_svOutBuffer.empty() );

			if( Result )
			{
				SVImageBufferHandleImage l_MilHandle;
				l_svOutBuffer->GetData( l_MilHandle );

				l_Code = SVMatroxBufferInterface::ClearBuffer( l_MilHandle.GetBuffer(), 0.0 );
				
				Result = l_Code == S_OK;
			}
		}
	}

	if ( Result )
	{
		m_CurrentDMIndexHandle.Assign( rDMIndexHandle, p_LockType );
	}
	else
	{
		m_CurrentDMIndexHandle.clear();
	}
	
	if( Result && 0 <= m_CurrentDMIndexHandle.GetIndex() && m_CurrentDMIndexHandle.GetIndex() < static_cast< long >( m_ImageHandleArray.size() ) )
	{
		Result = !( m_ImageHandleArray[ m_CurrentDMIndexHandle.GetIndex() ].empty() );

		assert( Result );
	}

	return Result;
}

bool SVImageObjectClass::CopyValue( const SVDataManagerHandle& p_FromDMIndexHandle, 
									const SVDataManagerHandle& p_ToDMIndexHandle )
{
	bool Result( true );
	
	if ( m_ParentImagePtr.empty() )
	{
		long l_FromIndex = p_FromDMIndexHandle.GetIndex();
		long l_ToIndex = p_ToDMIndexHandle.GetIndex();

		Result = l_FromIndex == l_ToIndex;
	
		if( !Result )
		{
			if( 0 <= l_ToIndex && l_ToIndex < static_cast< long >( m_ImageHandleArray.size() ) )
			{
				Result = UnlockIndex( l_ToIndex );

				if( Result )
				{
					if( 0 <= l_FromIndex && l_FromIndex < static_cast< long >( m_ImageHandleArray.size() ) )
					{
						m_ImageHandleArray[ l_ToIndex ] = m_ImageHandleArray[ l_FromIndex ];
					}
				}
			}
			else
			{
				Result = p_ToDMIndexHandle.GetIndex() == -1;
			}
		}

		if( Result )
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
				Result = SetCurrentIndex( m_CurrentDMIndexHandle );
			}

			assert( Result );
		}
	}			
	else
	{
		Result &= SetCurrentIndex( p_ToDMIndexHandle );
	}

	if( 0 <= m_CurrentDMIndexHandle.GetIndex() && m_CurrentDMIndexHandle.GetIndex() < static_cast< long >( m_ImageHandleArray.size() ) )
	{
		Result &= !( m_ImageHandleArray[ m_CurrentDMIndexHandle.GetIndex() ].empty() );

		assert( Result );
	}

	return Result;
}

bool SVImageObjectClass::CopyValue( const SVDataManagerHandle& p_ToDMIndexHandle )
{
	return CopyValue( m_CurrentDMIndexHandle, p_ToDMIndexHandle );
}

bool SVImageObjectClass::GetImageHandle( long lIndex, SVSmartHandlePointer& p_rValuePtr ) const
{
	bool Result( true );
	
	Result = Result && ( 0 <= lIndex );
	Result = Result && ( lIndex < static_cast< long >( m_ImageHandleArray.size() ) );
	Result = Result && !( m_ImageHandleArray[ lIndex ].empty() );
	
	p_rValuePtr.clear();

	if( Result )
	{
		p_rValuePtr = m_ImageHandleArray[ lIndex ]->m_ImageHandle;
	}

	Result = ! p_rValuePtr.empty();
	
	return Result;
}

bool SVImageObjectClass::GetImageHandle( SVSmartHandlePointer& p_rValuePtr ) const
{
	bool Result( true );

	long l_Index = m_CurrentDMIndexHandle.GetIndex();

	if( l_Index < 0 )
	{
		l_Index = 0;
	}

	Result = GetImageHandle( l_Index, p_rValuePtr );

	return Result;
}

HRESULT SVImageObjectClass::LoadImageFullSize( LPCTSTR p_szFileName, SVImageExtentClass& p_rNewExtent  )
{
	assert( 0 <= m_CurrentDMIndexHandle.GetIndex() );
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
		
		HRESULT l_Code;

		SVImageBufferHandleImage l_MilHandle;
		l_svHandle->GetData( l_MilHandle );
		
		l_Code = SVMatroxBufferInterface::ClearBuffer( l_MilHandle.GetBuffer(), 0 );
		
		if( l_Code == S_OK )
		{
			if( 0 < strlen( p_szFileName ) )
			{
				SVFileNameClass	svfncImageFile(p_szFileName);
				
				SVMatroxFileTypeEnum fileformatID = SVMatroxImageInterface::getFileType( svfncImageFile.GetExtension().c_str() );
				
				if( fileformatID != -1 && ::SVFileExists( p_szFileName ) )
				{
					std::string l_strFile(p_szFileName);
					
					SVImageBufferHandleImage l_MilHandle;
					l_svHandle->GetData( l_MilHandle );
		
					l_Code = SVMatroxBufferInterface::Import( l_MilHandle.GetBuffer(), l_strFile, fileformatID, p_bRestore );

					if( p_bRestore )
					{
						long l_lWidth = 0;
						long l_lHeight = 0;
						HRESULT l_Stat;
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

					if( l_Code == S_OK )
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
	HRESULT Code = E_FAIL; 
	long Width(0), Height(0);

	if( 0 < strlen( pFileName ) )
	{
		SVFileNameClass	svfncImageFile(pFileName);
		SVMatroxFileTypeEnum fileformatID = SVMatroxImageInterface::getFileType( svfncImageFile.GetExtension().c_str() );

		if( fileformatID != SVFileUnknown && ::SVFileExists( pFileName ) )
		{

			std::string strFile(pFileName);

			Code =  SVMatroxBufferInterface::GetImageSize(strFile,Width,Height);
		}

	}

	if(Code == S_OK)
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

		HRESULT l_Code;

		SVSmartHandlePointer l_svHandle;

		if ( GetImageHandle( l_svHandle ) && !( l_svHandle.empty() ) )
		{
			SVImageBufferHandleImage l_FromMilHandle;
			l_svHandle->GetData( l_FromMilHandle );

			l_Code = SVMatroxBufferInterface::CopyBuffer( l_ToMilHandle.GetBuffer(), l_FromMilHandle.GetBuffer() );

			if( l_Code != S_OK )
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
		l_Status = SVImageProcessingClass::CreateImageBuffer( m_ImageInfo, l_svCopyHandle );

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

		l_Status = SVImageProcessingClass::LoadImageBuffer( (void*)p_Image, l_Info, l_Handle, m_ImageInfo );
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

SVImageObjectClass::SVImageObjectElement::SVImageObjectElement( size_t p_MasterIndex, SVImageObjectClassPtr p_ParentPtr, SVSmartHandlePointer p_ImageHandle )
: m_MasterIndex( p_MasterIndex ), m_ParentPtr( p_ParentPtr ), m_ImageHandle( p_ImageHandle )
{
}

bool SVImageObjectClass::CreateBufferArrays(SvStl::MessageContainerVector *pErrorContainer)
{
	bool Result = true;
	bool l_ValidExtents = true;

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

	if( l_ValidExtents )
	{
		if( m_HandleCount < 1 )
		{
			m_HandleCount = 1;
		}

		Result = CreateImageHandleArray( m_HandleCount, pErrorContainer );

		// JMS - This block is used to initialize non-embedded image objects
		for( long i = m_HandleCount - 1; Result && 0 <= i; --i )
		{
			Result &= UnlockIndex( i );
			Result &= LockIndex( i );

			if( Result && m_ParentImagePtr.empty() )
			{
				SVSmartHandlePointer l_svOutBuffer;
				Result = GetImageHandle( i, l_svOutBuffer );

				if( Result )
				{
					SVImageBufferHandleImage l_MilHandle;
					l_svOutBuffer->GetData( l_MilHandle );

					HRESULT l_Code;

					l_Code = SVMatroxBufferInterface::ClearBuffer( l_MilHandle.GetBuffer(), 0.0 );

					Result = l_Code == S_OK;
				}
			}

			if (!Result && nullptr != pErrorContainer)
			{
				SvStl::MessageContainer message;
				message.setMessage( SVMSG_SVO_5065_COULDNOTCREATEIMAGEBUFFER, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
				pErrorContainer->push_back(message);
			}
		}
	}

	return Result;
}

bool SVImageObjectClass::DestroyBufferArrays()
{
	bool Result = DestroyImageHandleArray();

	return Result;
}

bool SVImageObjectClass::CreateImageBuffer(SVImageInfoClass &rInfo, long p_Index, SVImageObjectElementPtr& p_Handle, SvStl::MessageContainerVector *pErrorContainer )
{
	bool Result( true );

	DestroyImageBuffer( p_Handle );

	SVImageObjectElementPtr l_IndexHandle;
	SVSmartHandlePointer l_Handle;

	HRESULT hr = SVImageProcessingClass::CreateImageBuffer( rInfo, l_Handle, pErrorContainer );
	if (S_OK != hr)
	{
		Result = false;
	}

	if( !l_Handle.empty() && Result )
	{
		p_Handle = new SVImageObjectElement( p_Index, SVImageObjectClassPtr(), l_Handle );

		if(p_Handle.empty())
		{
			Result = false;
			if (nullptr != pErrorContainer)
			{
				SvStl::MessageContainer message;
				message.setMessage( SVMSG_SVO_5065_COULDNOTCREATEIMAGEBUFFER, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
				pErrorContainer->push_back(message);
			}
		}
	}
	else
	{
		p_Handle.clear();
	}
	
	return Result;
}

bool SVImageObjectClass::CreateImageChildBuffer(const SVImageInfoClass& rParentInfo, 
                                                SVSmartHandlePointer pParentHandle, 
                                                SVImageInfoClass& rChildInfo, 
                                                long p_Index, SVImageObjectElementPtr& p_Handle)
{
	bool Result( true );
	
	DestroyImageBuffer( p_Handle );

	Result = !( pParentHandle.empty() );

	SVSmartHandlePointer l_Handle;
	SVImageBufferHandleImage l_MilHandle;

	if( Result )
	{
		SVImageProcessingClass::CreateImageChildBuffer( rParentInfo, pParentHandle, rChildInfo, l_Handle );

		Result = !( l_Handle.empty() );
	}

	if( Result )
	{
		p_Handle = new SVImageObjectElement( p_Index, m_ParentImagePtr, l_Handle );

		Result = !( p_Handle.empty() );
	}
	else	
	{
		p_Handle.clear();
	}

	return Result;
}

bool SVImageObjectClass::DestroyImageBuffer(SVImageObjectElementPtr& pHandle)
{
	bool Result( true );
	
	if( !( pHandle.empty() ) )
	{
		long l_Index = static_cast<long>(pHandle->m_MasterIndex);

		m_ImageIndexPool.erase( l_Index );
	}
	
	return Result;
}

bool SVImageObjectClass::GetArrayImageHandle( long lIndex, SVSmartHandlePointer& rHandle ) const
{
	SVImageHandleMap::const_iterator l_Iter( m_MasterImageHandles.find( lIndex ) );

	bool Result = l_Iter != m_MasterImageHandles.end();

	Result = Result && !( l_Iter->second.empty() );
	
	if ( Result )
	{
		rHandle = l_Iter->second->m_ImageHandle;
	}
	else
	{
		rHandle.clear();
	}

	Result = ! rHandle.empty();
	
	return Result;
}

bool SVImageObjectClass::CreateImageHandleArray( long lSize, SvStl::MessageContainerVector *pErrorContainer ) 
{
	bool Result( true );

	DestroyImageHandleArray();

	m_ImageHandleArray.resize( lSize );

	for ( long l = 0; Result && l < lSize; l++ )
	{
		SVImageObjectElementPtr p_Handle;

		if( !( m_ParentImagePtr.empty() ) )
		{
			SVSmartHandlePointer l_ParentHandlePtr;
			
			Result = m_ParentImagePtr->GetArrayImageHandle( l, l_ParentHandlePtr );

			if( Result )
			{
				Result = CreateImageChildBuffer( m_ParentImagePtr->GetImageInfo(), l_ParentHandlePtr, m_ImageInfo, l, p_Handle );
			}

			if (!Result && nullptr != pErrorContainer)
			{
				SvStl::MessageContainer message;
				message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CreateChildBufferFailed, SvStl::SourceFileParams(StdMessageParams) );
				pErrorContainer->push_back(message);
			}
		}
		else
		{
			Result = CreateImageBuffer( m_ImageInfo, l, p_Handle, pErrorContainer );
		}

		if( Result )
		{
			m_MasterImageHandles[ l ] = p_Handle;

			m_ImageIndexPool.insert( l );

			assert( l != m_CurrentDMIndexHandle.GetIndex() );
		}
	}

	if ( Result )
	{
		SVDataManagerHandle l_DMIndexHandle = m_CurrentDMIndexHandle;

		if( !( m_ParentImagePtr.empty() ) )
		{
			m_ParentImagePtr->GetCurrentIndex( l_DMIndexHandle );
		}

		Result = SetCurrentIndex( l_DMIndexHandle );
		if( ! Result && nullptr != pErrorContainer )
		{
			SvStl::MessageContainer message;
			message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CreateImageHandleArrayFailed, SvStl::SourceFileParams(StdMessageParams) );
			pErrorContainer->push_back(message);
		}
	}

	if( ! Result )
	{
		DestroyImageHandleArray();
	}

	if( 0 <= m_CurrentDMIndexHandle.GetIndex() && m_CurrentDMIndexHandle.GetIndex() < static_cast< long >( m_ImageHandleArray.size() ) )
	{
		Result &= !( m_ImageHandleArray[ m_CurrentDMIndexHandle.GetIndex() ].empty() );

		assert( Result );
	}

	return Result;
}

bool SVImageObjectClass::DestroyImageHandleArray()
{
	bool Result( true );
	
	m_CurrentDMIndexHandle.clear();
	m_ImageHandleArray.clear();
	m_ImageIndexPool.clear();
	m_MasterImageHandles.clear();

	m_LastReset = 0;
	m_LastUpdate = 0;

	return Result;
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

bool SVImageObjectClass::LockIndex(long lIndex, long lHandleIndex )
{
	bool Result = ( -1 <= lIndex && lIndex < static_cast< long >( m_ImageHandleArray.size() ) );
	
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
		Result = lHandleIndex == -1;
	}
	
	return Result;
}

bool SVImageObjectClass::LockIndex(long lIndex)
{
	bool Result( true );
	
	if ( m_ParentImagePtr.empty() )
	{
		Result = ( -1 <= lIndex && lIndex < static_cast< long >( m_ImageHandleArray.size() ) );
		
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
		Result = !( m_ParentImagePtr.empty() );

		if( Result )
		{
			size_t l_MasterIndex = 0;

			Result = ( S_OK == m_ParentImagePtr->GetMasterIndex( lIndex, l_MasterIndex ) );

			if( Result )
			{
				Result = LockIndex( lIndex, static_cast<long>(l_MasterIndex ));
			}
		}
	}
	
	return Result;
}

bool SVImageObjectClass::UnlockIndex(long lIndex)
{
	bool Result = ( -1 <= lIndex && lIndex < static_cast< long >( m_ImageHandleArray.size() ) );

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
	
	return Result;
}

HRESULT SVImageObjectClass::AddImageHandleToPool( long p_Index )
{
	HRESULT l_Status = S_OK;

	m_ImageIndexPool.insert( p_Index );

	return l_Status;
}

bool SVImageObjectClass::DestroyLocal()
{
	bool Result = DestroyBufferArrays();

	return Result;
}

