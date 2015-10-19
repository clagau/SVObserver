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


BOOL SVOCameraList::IsCameraInList(CString sCameraName) const
{
	BOOL bFound = FALSE;

	const_iterator pos( m_CameraList.begin() );

	while( pos != m_CameraList.end() && !bFound )
	{
		const SVOCameraObjPtr pCameraObj = m_CameraList.GetAt(pos);

		if ( nullptr != pCameraObj && sCameraName == pCameraObj->GetCameraDisplayName())
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

BOOL SVOCameraList::AddCameraToList(CString sCameraName)
{
	BOOL bRet = FALSE;

	if (!IsCameraInList(sCameraName))
	{
		SVOCameraObjPtr pObj = new SVOCameraObj();
		pObj->SetCameraDisplayName(sCameraName);
		m_CameraList.AddTail(pObj);
		bRet = TRUE;
	}
	//check to see if a camera exist of that name.  if not add it

	return bRet;
}

BOOL SVOCameraList::AddCameraToList(CString sCameraName, int iDig)
{
	BOOL bRet = FALSE;

	if (!IsCameraInList(sCameraName))
	{
		SVOCameraObjPtr pObj = new SVOCameraObj();
		pObj->SetCameraDisplayName(sCameraName);
		pObj->SetDigNumber(iDig);
		m_CameraList.AddTail(pObj);
		bRet = TRUE;
	}

	return bRet;
}

BOOL SVOCameraList::AddCameraToList(CString sCameraName, int iDig, int iBands)
{
	BOOL bRet = FALSE;

	if (!IsCameraInList(sCameraName))
	{
		SVOCameraObjPtr pObj = new SVOCameraObj();
		pObj->SetCameraDisplayName(sCameraName);
		pObj->SetDigNumber(iDig);
		pObj->SetBandNumber(iBands);
		m_CameraList.AddTail(pObj);
		bRet = TRUE;
	}

	return bRet;
}

SVOCameraList::iterator SVOCameraList::FindCameraPosition(CString sCameraName)
{
	iterator pos( m_CameraList.begin() );
	bool Found = false;

	while( pos != m_CameraList.end() && !Found )
	{
		SVOCameraObjPtr pCameraObj = m_CameraList.GetAt(pos);

		if( nullptr != pCameraObj && sCameraName == pCameraObj->GetCameraDisplayName() )
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


BOOL SVOCameraList::SetCameraDig(CString sCameraName, int iDig)
{
	iterator pos = FindCameraPosition(sCameraName);
	BOOL bRet = FALSE;

	if (pos != m_CameraList.end())
	{
		SVOCameraObjPtr pObj = m_CameraList.GetAt(pos);
		pObj->SetDigNumber(iDig);
		m_CameraList.SetAt(pos,pObj);
		bRet = TRUE;
	}
	return bRet;
}

BOOL SVOCameraList::SetCameraFile(CString sCameraName, CString sFileName)
{
	iterator pos = FindCameraPosition(sCameraName);
	BOOL bRet = FALSE;

	if (pos != m_CameraList.end())
	{
		SVOCameraObjPtr pObj = m_CameraList.GetAt(pos);
		pObj->SetCameraFile(sFileName);
		m_CameraList.SetAt(pos,pObj);
		bRet = TRUE;
	}

	return bRet;
}

BOOL SVOCameraList::SetCameraDeviceParams( CString sCameraName, const SVDeviceParamCollection& rCameraDeviceParams, const SVDeviceParamCollection& rCameraFileParams )
{
	iterator pos = FindCameraPosition(sCameraName);
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


BOOL SVOCameraList::RemoveCameraFromList(CString sCameraName)
{
	iterator pos = FindCameraPosition(sCameraName);
	BOOL bRet = FALSE;

	if (pos != m_CameraList.end())
	{
		m_CameraList.RemoveAt(pos);
		bRet = TRUE;
	}

	return bRet;
}

SVOCameraObjPtr SVOCameraList::GetCameraObjectByName(CString sCameraName)
{
	SVOCameraObjPtr pReturnObj( nullptr );

	iterator pos = FindCameraPosition(sCameraName);

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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVOCameraList.cpp_v  $
 * 
 *    Rev 1.1   01 Oct 2013 15:24:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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
