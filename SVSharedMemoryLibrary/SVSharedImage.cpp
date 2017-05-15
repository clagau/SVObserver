//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedImage
//* .File Name       : $Workfile:   SVSharedImage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:08:34  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVSharedImage.h"
#include "SVSharedConfiguration.h"
#pragma endregion Includes

namespace SvSml
{
	SVSharedImage::SVSharedImage(const void_allocator& rAlloc)
	: m_Status(E_FAIL)
	, m_Allocator(rAlloc)
	, m_ElementName("", rAlloc)
	,m_ImageIndex(-1)
	,m_ImageStoreIndex(-1)
	,m_SlotIndex(-1)
	,m_IsReject(false)
	{
		memset(m_Filename, 0, sizeof(m_Filename));
	}

	SVSharedImage::SVSharedImage( const SVSharedImage& rImage )
	: m_Status(rImage.m_Status)
	, m_Allocator(rImage.m_Allocator)
	, m_ElementName(rImage.m_ElementName)
	,m_ImageIndex(rImage.m_ImageIndex)
	,m_ImageStoreIndex(rImage.m_ImageStoreIndex)
	,m_IsReject(rImage.m_IsReject)
	,m_SlotIndex(rImage.m_SlotIndex)
	{
		strcpy_s(m_Filename, sizeof(m_Filename), rImage.m_Filename);
	}

	const SVSharedImage& SVSharedImage::operator=(const SVSharedImage& rImage)
	{
		if (this != &rImage)
		{
			strcpy_s(m_Filename, sizeof(m_Filename), rImage.m_Filename);
			m_Status = rImage.m_Status;
			m_ElementName = rImage.m_ElementName;
			m_ImageIndex = rImage.m_ImageIndex;
			m_ImageStoreIndex = rImage.m_ImageStoreIndex;
			m_IsReject = rImage.m_IsReject;
			m_SlotIndex = rImage.m_SlotIndex;
		}
		return *this;
	}

	void SVSharedImage::SetName(const SVString& Name)
	{
		m_ElementName = bip_string(Name.c_str(), m_Allocator);
	}

	void SVSharedImage::SetFileName(LPCTSTR Filename)
	{
		strcpy_s(m_Filename, sizeof(m_Filename), Filename);
	}

	void SVSharedImage::SetData( LPCTSTR  Filename, int Status )
	{
		SetFileName(Filename);
		m_Status = Status;
	}
	
	
	void SVSharedImage::SetImageStoreProperties(LPCTSTR Name ,DWORD imageStoreIndex ,DWORD imageIndex, DWORD slotIndex, bool isRejct)
	{
		m_SlotIndex = slotIndex;
		m_IsReject = isRejct;
		m_ImageIndex = imageIndex;
		m_ImageStoreIndex = imageStoreIndex;
		m_ElementName = bip_string(Name, m_Allocator);
	}

	void SVSharedImage::SetImageStoreStatus( int Status )
	{
		m_Status = Status;
	}

	LPCTSTR  SVSharedImage::BuildImageFileName( DWORD ImageIndex, DWORD ImageStoreIndex, DWORD SlotIndex, bool bReject)
	{
		static SVString RejectPath =  SVSharedConfiguration::GetRejectImageDirectoryName()  + "\\R";
		static SVString LastPath =  SVSharedConfiguration::GetImageDirectoryName() + "\\L";
		static LPCTSTR pRejectPath = RejectPath.c_str();
		static LPCTSTR pLastPath = LastPath.c_str();
		static TCHAR Buffer[statics::max_result_size];
		_stprintf_s(Buffer,statics::max_result_size,_T("%s%i_%i_%i.bmp"),bReject? pRejectPath : pLastPath,ImageStoreIndex,ImageIndex,SlotIndex);   
		return Buffer;
	}

	
	void  CopySharedImages( SVSharedImageVector& rTo  , const SVSharedImageVector& rfrom)
	{
		if(rTo.size() != rfrom.size())
		{
			assert(false);
			throw std::exception("internal error 1");
		}
		for(int i =0 ; i <rTo.size(); i++ )
		{
			rTo[i].m_Status = rfrom[i].m_Status;
			/* SVString elementnameTo =  rTo[i].m_ElementName.c_str();
			SVString elementnameFrom  = rfrom[i].m_ElementName.c_str();
			assert(elementnameTo == elementnameFrom);*/
		
		}
	
	}
} //namespace SvSml
