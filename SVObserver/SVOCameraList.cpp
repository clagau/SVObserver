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
#include "SVOMFCLibrary/SVDeviceParams.h"
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

SVOCameraList &SVOCameraList::operator =(const SVOCameraList &source)
{
	if( this != &source )
	{
		iterator pos;

		m_CameraList.RemoveAll();

		if( source.m_CameraList.IsEmpty() )
		{
			pos = source.m_CameraList.GetHeadPosition();

			do
			{
				m_CameraList.AddTail(source.m_CameraList.GetNext(pos));
			}

			while( pos != source.m_CameraList.end() );
		}
	}

	return (*this);
}


BOOL SVOCameraList::IsCameraInList(CString CameraName) const
{
	BOOL bFound = FALSE;

	const_iterator pos( m_CameraList.begin() );

	while( pos != m_CameraList.end() && !bFound )
	{
		const SVOCameraObjPtr pCameraObj = m_CameraList.GetAt(pos);

		if ( nullptr != pCameraObj && CameraName == pCameraObj->GetCameraDisplayName())
		{
			bFound = TRUE;
		}
		else
		{
			++pos;
		}
	}

	return bFound;
}

BOOL SVOCameraList::AddCameraToList(CString CameraName, int Dig, int CameraID)
{
	BOOL bRet = FALSE;

	if (!IsCameraInList(CameraName))
	{
		SVOCameraObjPtr pObj = new SVOCameraObj();
		pObj->SetCameraDisplayName(CameraName);
		pObj->SetDigNumber(Dig);
		pObj->SetCameraID(CameraID);
		m_CameraList.AddTail(pObj);
		bRet = TRUE;
	}

	return bRet;
}

BOOL SVOCameraList::AddCameraToList(CString CameraName, int Dig, int Bands, int CameraID)
{
	BOOL bRet = FALSE;

	if (!IsCameraInList(CameraName))
	{
		SVOCameraObjPtr pObj = new SVOCameraObj();
		pObj->SetCameraDisplayName(CameraName);
		pObj->SetDigNumber(Dig);
		pObj->SetCameraID(CameraID);
		pObj->SetBandNumber(Bands);
		m_CameraList.AddTail(pObj);
		bRet = TRUE;
	}

	return bRet;
}

SVOCameraList::iterator SVOCameraList::FindCameraPosition(CString CameraName)
{
	iterator pos( m_CameraList.begin() );
	bool Found = false;

	while( pos != m_CameraList.end() && !Found )
	{
		SVOCameraObjPtr pCameraObj = m_CameraList.GetAt(pos);

		if( nullptr != pCameraObj && CameraName == pCameraObj->GetCameraDisplayName() )
		{
			Found = true;
		}
		else
		{
			++pos;
		}
	}

	return pos;
}

BOOL SVOCameraList::SetCameraFile(CString CameraName, CString FileName)
{
	iterator pos = FindCameraPosition(CameraName);
	BOOL bRet = FALSE;

	if (pos != m_CameraList.end())
	{
		SVOCameraObjPtr pObj = m_CameraList.GetAt(pos);
		pObj->SetCameraFile(FileName);
		m_CameraList.SetAt(pos,pObj);
		bRet = TRUE;
	}

	return bRet;
}

BOOL SVOCameraList::SetCameraDeviceParams( CString CameraName, const SVDeviceParamCollection& rCameraDeviceParams, const SVDeviceParamCollection& rCameraFileParams )
{
	iterator pos = FindCameraPosition(CameraName);
	BOOL bRet = FALSE;

	if (pos != m_CameraList.end())
	{
		SVOCameraObjPtr pObj = m_CameraList.GetAt(pos);
		pObj->SetCameraFileParams( rCameraFileParams );
		pObj->SetCameraDeviceParams( rCameraDeviceParams );
		m_CameraList.SetAt(pos,pObj);
		bRet = TRUE;
	}

	return bRet;
}


BOOL SVOCameraList::RemoveCameraFromList(CString CameraName)
{
	iterator pos = FindCameraPosition(CameraName);
	BOOL bRet = FALSE;

	if (pos != m_CameraList.end())
	{
		m_CameraList.RemoveAt(pos);
		bRet = TRUE;
	}
	//Reorder the camera digitizer
	pos = m_CameraList.begin();
	int Digitizer(0);
	for( ; m_CameraList.end() != pos; ++pos)
	{
		(*pos)->SetDigNumber( Digitizer );
		Digitizer++;
	}


	return bRet;
}

SVOCameraObjPtr SVOCameraList::GetCameraObjectByName(CString CameraName)
{
	SVOCameraObjPtr pReturnObj( nullptr );

	iterator pos = FindCameraPosition(CameraName);

	if (pos != m_CameraList.end())
	{
		pReturnObj = m_CameraList.GetAt(pos);
	}

	return pReturnObj;
}

SVOCameraObjPtr SVOCameraList::GetCameraObjectByPosition(int iPos)
{
	SVOCameraObjPtr pReturnObj( nullptr );

	iterator pos = m_CameraList.FindIndex(iPos);
	if (pos != m_CameraList.end())
	{
		pReturnObj =  *pos;
	}

	return pReturnObj;
}

const SVOCameraObjPtr SVOCameraList::GetCameraObjectByPosition(int iPos) const
{
	SVOCameraObjPtr pReturnObj( nullptr );

	const_iterator pos = m_CameraList.FindIndex(iPos);
	if (pos != m_CameraList.end())
	{
		pReturnObj =  *pos;
	}

	return pReturnObj;
}

int SVOCameraList::GetCameraListCount() const
{
	return static_cast<int>(m_CameraList.GetCount());
}

void SVOCameraList::ResetContent()
{
	m_CameraList.RemoveAll();
}

