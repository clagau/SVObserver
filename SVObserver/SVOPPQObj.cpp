//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOPPQObj
//* .File Name       : $Workfile:   SVOPPQObj.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 06:48:20  $
//******************************************************************************

#include "stdafx.h"
#include "SVOPPQObj.h"
#include "SVPPQConstants.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSVOPPQObj::CSVOPPQObj()
: m_sPPQName(_T(""))
, m_sAttachedTrigger(_T(""))
, m_iPPQMode(0)
, m_lPPQLength(2)
, m_lPPQOutputResetDelay(0)
, m_lPPQOutputDelayTime(100)
, m_bMaintainSrcImg(false)
, m_lInspectionTimeout(0)
, m_conditionalOutputName(PPQ_CONDITIONAL_OUTPUT_ALWAYS)
{
}

CSVOPPQObj::~CSVOPPQObj()
{
	m_slAttachedCameraList.RemoveAll();
	m_slAttachedCameraList.RemoveAll();
}

void CSVOPPQObj::SetPPQName(CString sPPQName)
{
	m_sPPQName = sPPQName;
}

CString CSVOPPQObj::GetPPQName()
{
	return m_sPPQName;
}

void CSVOPPQObj::AttachTriggerToPPQ(CString sTriggerName)
{
	m_sAttachedTrigger = sTriggerName;
}

void CSVOPPQObj::DetachTriggerFromPPQ()
{
	m_sAttachedTrigger = "";
}

CString CSVOPPQObj::GetAttachedTriggerName() const
{
	return m_sAttachedTrigger;
}

BOOL CSVOPPQObj::AttachCameraToPPQ(CString sCameraName)
{
	BOOL bRet = FALSE;
	//check to see if it is already in list
	//if not add camera to list
	if (m_slAttachedCameraList.Find(sCameraName) == NULL)
	{ 
		m_slAttachedCameraList.AddTail(sCameraName);
		bRet = TRUE;
	}
	return bRet;
}

BOOL CSVOPPQObj::DetachCameraFromPPQ(CString sCameraName)
{
	BOOL bRet = FALSE;
	POSITION pos;
	
	if (!m_slAttachedCameraList.IsEmpty())
	{
		pos = m_slAttachedCameraList.Find(sCameraName);
		if (pos != NULL)
		{ //camera is in list.  now delete
			m_slAttachedCameraList.RemoveAt(pos);
			bRet = TRUE;
		}
	}
	return bRet;
}

int CSVOPPQObj::GetAttachedCameraCount() const
{
	return static_cast<int>(m_slAttachedCameraList.GetCount());
}

CString CSVOPPQObj::GetAttachedCamera(int iPos) const
{
	CString sReturnStr = "";
	POSITION pos = m_slAttachedCameraList.FindIndex(iPos);
	
	if (pos != NULL)
	{
		sReturnStr = m_slAttachedCameraList.GetAt(pos);
	}
	return sReturnStr;
}

BOOL CSVOPPQObj::AttachInspectionToPPQ(CString sInspectName)
{
	BOOL bRet = FALSE;
	
	if (m_slAttachedInspectList.Find(sInspectName) == NULL)
	{
		//was not found, add to list
		m_slAttachedInspectList.AddTail(sInspectName);
		bRet = TRUE;
	}
	return bRet;
}

BOOL CSVOPPQObj::DetachInspectionFromPPQ(CString sInspectName)
{
	BOOL bRet = FALSE;
	POSITION pos;
	
	if (!m_slAttachedInspectList.IsEmpty())
	{
		pos = m_slAttachedInspectList.Find(sInspectName);
		
		if (pos != NULL)
		{
			m_slAttachedInspectList.RemoveAt(pos);
			bRet = TRUE;
		}
	}
	return bRet;
}

int CSVOPPQObj::GetAttachedInspectionCount()
{
	return static_cast<int>(m_slAttachedInspectList.GetCount());
}

CString CSVOPPQObj::GetAttachedInspection(int iPos)
{
	POSITION pos = m_slAttachedInspectList.FindIndex(iPos);
	
	return m_slAttachedInspectList.GetAt(pos);
}

//property methods
void CSVOPPQObj::SetPPQMode(int iPPQMode)
{
	m_iPPQMode = iPPQMode;
}

void CSVOPPQObj::SetPPQLength(long lLength)
{
	m_lPPQLength = lLength;
}

void CSVOPPQObj::SetPPQOutputResetDelay(long lResetDelay)
{
	m_lPPQOutputResetDelay = lResetDelay;
}

void CSVOPPQObj::SetPPQOutputDelayTime(long lDelayTime)
{
	m_lPPQOutputDelayTime = lDelayTime;
}

int CSVOPPQObj::GetPPQMode() const
{
	return m_iPPQMode;
}

long CSVOPPQObj::GetPPQLength() const
{
	return m_lPPQLength;
}

long CSVOPPQObj::GetPPQOutputResetDelay() const
{
	return m_lPPQOutputResetDelay;
}

long CSVOPPQObj::GetPPQOutputDelayTime() const
{
	return m_lPPQOutputDelayTime;
}

CSVOPPQObj &CSVOPPQObj::operator =(const CSVOPPQObj &source)
{
	if (this != &source)
	{
		m_sPPQName = source.m_sPPQName;
		m_sAttachedTrigger = source.m_sAttachedTrigger;
		m_iPPQMode = source.m_iPPQMode;
		m_lPPQLength = source.m_lPPQLength;
		m_lPPQOutputResetDelay = source.m_lPPQOutputResetDelay;
		m_lPPQOutputDelayTime = source.m_lPPQOutputDelayTime;
		m_bMaintainSrcImg = source.m_bMaintainSrcImg;
		m_lInspectionTimeout = source.m_lInspectionTimeout;
		m_conditionalOutputName = source.m_conditionalOutputName;
		m_availableInputs = source.m_availableInputs;
		m_importedInputList = source.m_importedInputList;
	}
	return (*this);
}

void CSVOPPQObj::SetMaintainSourceImageProperty(bool bValue)
{
	m_bMaintainSrcImg = bValue;
}

bool CSVOPPQObj::GetMaintainSourceImageProperty() const
{
	return m_bMaintainSrcImg;
}

void CSVOPPQObj::SetInspectionTimeout( long lTimeoutMillisec )
{
	m_lInspectionTimeout = lTimeoutMillisec;
}

long CSVOPPQObj::GetInspectionTimeout() const
{
	return m_lInspectionTimeout;
}

void CSVOPPQObj::SetAvailableInputsForConditionalOutput(const SVNameGuidPairList& list)
{
	m_availableInputs = list;
}

const SVNameGuidPairList& CSVOPPQObj::GetAvailableInputsForConditionalOutput() const
{
	return m_availableInputs;
}

void CSVOPPQObj::SetConditionalOutputName(const SVString& name)
{
	m_conditionalOutputName = name;
}

const SVString& CSVOPPQObj::GetConditionalOutputName() const
{
	return m_conditionalOutputName;
}

bool CSVOPPQObj::IsConditionalOutputCameraInput() const
{
	return (m_conditionalOutputName.find("Camera") == 0);
}

void CSVOPPQObj::RemoveCameraInputConditionalOutput()
{
	if (IsConditionalOutputCameraInput())
	{
		m_conditionalOutputName = PPQ_CONDITIONAL_OUTPUT_ALWAYS;
	}
}

void CSVOPPQObj::SetImportedInputList(const SVImportedInputList& list)
{
	m_importedInputList = list;
}

const SVImportedInputList& CSVOPPQObj::GetImportedInputList() const
{
	return m_importedInputList;
}

void CSVOPPQObj::ClearImportedInputList()
{
	m_importedInputList.clear();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVOPPQObj.cpp_v  $
 * 
 *    Rev 1.1   02 Oct 2013 06:48:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:10:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   05 Mar 2013 16:41:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Inspection Import list
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   21 Jan 2013 11:34:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added support for Conditional outputs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   16 Jan 2013 16:35:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Migrating branch code into primary code base.  
 * 
 * Updated method with const correctness.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   05 Nov 2010 11:08:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Sep 2008 16:43:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised GetAttachedTriggerName, GetAttachedCameraCount, and GetAttachedCamera to be const
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Jan 2007 09:29:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  587
 * SCR Title:  Add Timeout Threshold to Extended Time Delay Mode
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added code for the new Inspection Timeout Threshold for the Extended Time Delay Mode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 13:17:24   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Feb 2003 13:27:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  316
 * SCR Title:  Add option to extent acquisition image source buffers to the length of the PPQ
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added code for the new PPQ property. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:39:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

