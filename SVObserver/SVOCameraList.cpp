//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCameraList
//* .File Name       : $Workfile:   SVOCameraList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 15:24:40  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVOCameraList.h"
#include "CameraLibrary/SVDeviceParams.h"

#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVOCameraList::SVOCameraList()
{

}

SVOCameraList::~SVOCameraList()
{

}

SVOCameraList &SVOCameraList::operator =(const SVOCameraList &rSource)
{
	if( this != &rSource )
	{
		m_CameraList.clear();

		if( rSource.m_CameraList.empty() )
		{
			const_iterator Iter = rSource.m_CameraList.begin();

			for(;rSource.m_CameraList.end() != Iter; ++Iter )
			{
				m_CameraList.insert(m_CameraList.end(), *Iter);
			}
		}
	}

	return (*this);
}


bool SVOCameraList::IsCameraInList(LPCTSTR CameraName) const
{
	bool bFound = false;

	const_iterator Iter( m_CameraList.begin() );

	while( Iter != m_CameraList.end() && !bFound )
	{
		const SVOCameraObjPtr pCameraObj = *Iter;

		if ( nullptr != pCameraObj && CameraName == pCameraObj->GetCameraDisplayName())
		{
			bFound = true;
		}
		else
		{
			++Iter;
		}
	}

	return bFound;
}

bool SVOCameraList::AddCameraToList(LPCTSTR CameraName, int Dig, int CameraID)
{
	bool bRet = false;

	if (!IsCameraInList(CameraName))
	{
		SVOCameraObjPtr pObj{ new SVOCameraObj() };
		pObj->SetCameraDisplayName( std::string(CameraName) );
		pObj->SetDigNumber(Dig);
		pObj->SetCameraID(CameraID);
		m_CameraList.insert(m_CameraList.end(), pObj);
		bRet = true;
	}

	return bRet;
}

bool SVOCameraList::AddCameraToList(LPCTSTR CameraName, int Dig, int Bands, int CameraID)
{
	bool bRet = false;

	if (!IsCameraInList(CameraName))
	{
		SVOCameraObjPtr pObj{ new SVOCameraObj() };
		pObj->SetCameraDisplayName( std::string(CameraName) );
		pObj->SetDigNumber(Dig);
		pObj->SetCameraID(CameraID);
		pObj->SetBandNumber(Bands);
		m_CameraList.insert(m_CameraList.end(), pObj);
		bRet = true;
	}

	return bRet;
}

SVOCameraList::iterator SVOCameraList::FindCameraPosition(LPCTSTR CameraName)
{
	iterator Iter( m_CameraList.begin() );
	bool Found = false;

	while( Iter != m_CameraList.end() && !Found )
	{
		SVOCameraObjPtr pCameraObj = *Iter;

		if( nullptr != pCameraObj && CameraName == pCameraObj->GetCameraDisplayName() )
		{
			Found = true;
		}
		else
		{
			++Iter;
		}
	}

	return Iter;
}

bool SVOCameraList::SetCameraFile(LPCTSTR CameraName, LPCTSTR FileName)
{
	iterator Iter = FindCameraPosition(CameraName);
	bool bRet = false;

	if (Iter != m_CameraList.end())
	{
		SVOCameraObjPtr pObj = *Iter;
		pObj->SetCameraFile( std::string(FileName) );
		*Iter = pObj;
		bRet = true;
	}

	return bRet;
}

bool SVOCameraList::SetCameraDeviceParams( LPCTSTR CameraName, const SVDeviceParamCollection& rCameraDeviceParams, const SVDeviceParamCollection& rCameraFileParams )
{
	iterator Iter = FindCameraPosition(CameraName);
	bool bRet = false;

	if (Iter != m_CameraList.end())
	{
		SVOCameraObjPtr pObj = *Iter;
		pObj->SetCameraFileParams( rCameraFileParams );
		pObj->SetCameraDeviceParams( rCameraDeviceParams );
		*Iter = pObj;
		bRet = true;
	}

	return bRet;
}


bool SVOCameraList::RemoveCameraFromList( LPCTSTR CameraName )
{
	iterator Iter = FindCameraPosition( CameraName );
	bool bRet = false;

	if (Iter != m_CameraList.end())
	{
		m_CameraList.erase(Iter);
		bRet = true;
	}
	//Reorder the camera digitizer
	Iter = m_CameraList.begin();
	int Digitizer(0);
	for( ; m_CameraList.end() != Iter; ++Iter)
	{
		(*Iter)->SetDigNumber( Digitizer );
		Digitizer++;
	}


	return bRet;
}

SVOCameraObjPtr SVOCameraList::GetCameraObjectByName(LPCTSTR CameraName)
{
	SVOCameraObjPtr pReturnObj( nullptr );

	iterator Iter = FindCameraPosition(CameraName);

	if (Iter != m_CameraList.end())
	{
		pReturnObj = *Iter;
	}

	return pReturnObj;
}

SVOCameraObjPtr SVOCameraList::GetCameraObjectByPosition(int iPos)
{
	SVOCameraObjPtr pResult(nullptr);

	if(0 <= iPos && iPos < static_cast<int>(m_CameraList.size()))
	{
		iterator Iter(m_CameraList.begin());
		std::advance(Iter, iPos);
		pResult = *Iter;
	}

	return pResult;
}

const SVOCameraObjPtr SVOCameraList::GetCameraObjectByPosition(int iPos) const
{
	SVOCameraObjPtr pReturnObj(nullptr);

	if (0 <= iPos && iPos < static_cast<int>(m_CameraList.size()))
	{
		const_iterator Iter(m_CameraList.begin());
		std::advance(Iter, iPos);
		pReturnObj = *Iter;
	}

	return pReturnObj;
}

int SVOCameraList::GetCameraListCount() const
{
	return static_cast<int> (m_CameraList.size());
}

void SVOCameraList::ResetContent()
{
	m_CameraList.clear();
}

