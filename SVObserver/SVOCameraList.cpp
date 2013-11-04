//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCameraList
//* .File Name       : $Workfile:   SVOCameraList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:48:30  $
//******************************************************************************

#include "stdafx.h"
#include "SVOCameraList.h"
#include "SVOCameraObj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CSVOCameraList::CSVOCameraList()
{

}

CSVOCameraList::~CSVOCameraList()
{

}

CSVOCameraList &CSVOCameraList::operator =(const CSVOCameraList &source)
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


BOOL CSVOCameraList::IsCameraInList(CString sCameraName)
{
	BOOL bFound = FALSE;

	iterator pos( m_CameraList.begin() );

	while( pos != m_CameraList.end() && !bFound )
	{
		CSVOCameraObj* pObj = m_CameraList.GetAt(pos);

		if (sCameraName == pObj->GetCameraDisplayName())
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

BOOL CSVOCameraList::AddCameraToList(CString sCameraName)
{
	BOOL bRet = FALSE;
	CSVOCameraObj *pObj;
	if (!IsCameraInList(sCameraName))
	{
		pObj = new CSVOCameraObj();
		pObj->SetCameraDisplayName(sCameraName);
		m_CameraList.AddTail(pObj);
		bRet = TRUE;
	}
	//check to see if a camera exist of that name.  if not add it

	return bRet;
}

BOOL CSVOCameraList::AddCameraToList(CString sCameraName, int iDig)
{
	BOOL bRet = FALSE;
	CSVOCameraObj *pObj;

	if (!IsCameraInList(sCameraName))
	{
		pObj = new CSVOCameraObj();
		pObj->SetCameraDisplayName(sCameraName);
		pObj->SetDigNumber(iDig);
		m_CameraList.AddTail(pObj);
		bRet = TRUE;
	}

	return bRet;
}

BOOL CSVOCameraList::AddCameraToList(CString sCameraName, int iDig, int iBands)
{
	BOOL bRet = FALSE;
	CSVOCameraObj *pObj;

	if (!IsCameraInList(sCameraName))
	{
		pObj = new CSVOCameraObj();
		pObj->SetCameraDisplayName(sCameraName);
		pObj->SetDigNumber(iDig);
		pObj->SetBandNumber(iBands);
		m_CameraList.AddTail(pObj);
		bRet = TRUE;
	}

	return bRet;
}

CSVOCameraList::iterator CSVOCameraList::FindCameraPosition(CString sCameraName)
{
	iterator pos( m_CameraList.begin() );
	BOOL bFound = FALSE;

	while( pos != m_CameraList.end() && !bFound )
	{
		CSVOCameraObj* pObj = m_CameraList.GetAt(pos);

		if (sCameraName == pObj->GetCameraDisplayName())
		{
			bFound = TRUE;
		}
		else
		{
			++pos;
		}
	}

	return pos;
}


BOOL CSVOCameraList::SetCameraDig(CString sCameraName, int iDig)
{
	iterator pos = FindCameraPosition(sCameraName);
	BOOL bRet = FALSE;

	if (pos != m_CameraList.end())
	{
		CSVOCameraObj *pObj = m_CameraList.GetAt(pos);
		pObj->SetDigNumber(iDig);
		m_CameraList.SetAt(pos,pObj);
		bRet = TRUE;
	}
	return bRet;
}

BOOL CSVOCameraList::SetCameraFile(CString sCameraName, CString sFileName)
{
	iterator pos = FindCameraPosition(sCameraName);
	BOOL bRet = FALSE;

	if (pos != m_CameraList.end())
	{
		CSVOCameraObj *pObj = m_CameraList.GetAt(pos);
		pObj->SetCameraFile(sFileName);
		m_CameraList.SetAt(pos,pObj);
		bRet = TRUE;
	}

	return bRet;
}

BOOL CSVOCameraList::SetCameraDeviceParams( CString sCameraName, const SVDeviceParamCollection& rCameraDeviceParams, const SVDeviceParamCollection& rCameraFileParams )
{
	iterator pos = FindCameraPosition(sCameraName);
	BOOL bRet = FALSE;

	if (pos != m_CameraList.end())
	{
		CSVOCameraObj* pObj = m_CameraList.GetAt(pos);
		pObj->SetCameraFileParams( rCameraFileParams );
		pObj->SetCameraDeviceParams( rCameraDeviceParams );
		m_CameraList.SetAt(pos,pObj);
		bRet = TRUE;
	}

	return bRet;
}


BOOL CSVOCameraList::RemoveCameraFromList(CString sCameraName)
{
	iterator pos = FindCameraPosition(sCameraName);
	BOOL bRet = FALSE;

	if (pos != m_CameraList.end())
	{
		CSVOCameraObj *pObj = m_CameraList.GetAt(pos);
		m_CameraList.RemoveAt(pos);
		bRet = TRUE;
		delete pObj;
	}

	return bRet;
}

CSVOCameraObj *CSVOCameraList::GetCameraObjectByName(CString sCameraName)
{
	iterator pos = FindCameraPosition(sCameraName);
	CSVOCameraObj *pReturnObj = NULL;

	if (pos != m_CameraList.end())
	{
		pReturnObj = m_CameraList.GetAt(pos);
	}

	return pReturnObj;
}

CSVOCameraObj *CSVOCameraList::GetCameraObjectByPosition(int iPos)
{
	iterator pos = m_CameraList.FindIndex(iPos);

	return m_CameraList.GetAt(pos);
}

const CSVOCameraObj *CSVOCameraList::GetCameraObjectByPosition(int iPos) const
{
	const_iterator pos = m_CameraList.FindIndex(iPos);
	if (pos != m_CameraList.end())
		return (*pos);

	return NULL;
}

int CSVOCameraList::GetCameraListCount() const
{
	return m_CameraList.GetCount();
}

void CSVOCameraList::ResetContent()
{
	iterator pos( m_CameraList.begin() );

	while( pos != m_CameraList.end() )
	{
		CSVOCameraObj* pObj = m_CameraList.GetAt(pos);
		pos = m_CameraList.erase(pos);
		delete pObj;
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOCameraList.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:48:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   16 Jan 2013 16:14:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Migrating branch code into primary code base.  
 * 
 * Updated camera look-up functionality and const correctness of methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   09 Nov 2010 16:25:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Sep 2008 16:39:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised GetCameraListCount to be const
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Aug 2003 12:48:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed the way camera file params are handled
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Jul 2003 10:38:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for SVDeviceParamCollection
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 12:29:36   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:35:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
