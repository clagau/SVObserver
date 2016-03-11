//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCameraObj
//* .File Name       : $Workfile:   SVOCameraObj.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:48:52  $
//******************************************************************************

#include "stdafx.h"
#include "SVOCameraObj.h"
#include "SVOMFCLibrary/SVFileAcquisitionLoadingModeEnum.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVOMFCLibrary/SVLongValueDeviceParam.h"
#include "SVOMFCLibrary/SVStringValueDeviceParam.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

SVOCameraObj::SVOCameraObj()
{
    m_sCameraDisplayName =_T(""); 
    m_iDigNumber = 0;
    m_iBandNumber = 0;
    m_IsColor = false;
    m_sCameraFile = _T("");
    m_bCameraFileSet = FALSE;
	m_bCameraFileChanged = FALSE;
    m_bDigSet = FALSE;
    m_bCameraObjOk = FALSE;

	m_bFileAcquisition = false;
	m_imageSizeEditModeFileBased = true;
	m_fileImageSize.cx = 640;
	m_fileImageSize.cy = 480;
	m_fileMode = ContinuousMode;
}

SVOCameraObj::~SVOCameraObj()
{

}

CString SVOCameraObj::GetCameraDisplayName() const
{
    return m_sCameraDisplayName;
}

int SVOCameraObj::GetDigNumber() const
{
    return m_iDigNumber;
}

void SVOCameraObj::SetCameraDisplayName(CString sCameraName)
{
    m_sCameraDisplayName = sCameraName;
}

void SVOCameraObj::SetDigNumber(int iDigNumber)
{
    m_iDigNumber = iDigNumber;
    m_bDigSet = TRUE;
}

void SVOCameraObj::SetCameraFile(CString sFileName)
{
    m_sCameraFile = sFileName;
    if (!m_sCameraFile.IsEmpty())
    {
        m_bCameraFileSet = TRUE;
    }
    else
    {
        m_bCameraFileSet = FALSE;
    }
    //check to see if Camera is ok...

    IsCameraObjOk();
}

void SVOCameraObj::SetCameraFileChanged()
{
    m_bCameraFileChanged = TRUE;
}

BOOL SVOCameraObj::GetCameraFileChanged()
{
    return m_bCameraFileChanged;
}

CString SVOCameraObj::GetCameraFile() const
{
    return m_sCameraFile;
}

BOOL SVOCameraObj::IsCameraObjOk()
{
	if (IsFileAcquisition())
	{
		m_bCameraObjOk = true;
	}
	else
	{
		if (m_bCameraFileSet)
		{
			m_bCameraObjOk = TRUE;
		}
		else
		{
			m_bCameraObjOk = FALSE;
		}
	}
    return m_bCameraObjOk;
}

void SVOCameraObj::SetBandNumber(int iBandNumber)
{
    m_iBandNumber = iBandNumber;
}

int SVOCameraObj::GetBandNumber() const
{
    return m_iBandNumber;
}

void SVOCameraObj::SetCameraFileParams(const SVDeviceParamCollection& rParams)
{
	m_CameraFileParams = rParams;

	/*
	// now update the parameter list if it is different than the camera file (stuff it with defaults)
	SVDeviceParamMap::const_iterator iter;
	for (iter = m_CameraFileParams.mapParameters.begin(); iter != m_CameraFileParams.mapParameters.end(); iter++)
	{
		const SVDeviceParam* pDeviceParam = m_CameraDeviceParams.GetParameter( iter->first );
		if ( pDeviceParam == NULL )
		{
			m_CameraDeviceParams.GetParameter( iter->first ) = iter->second;
		}
	}
	*/
	m_CameraDeviceParams = rParams;	// reset all settings when changing camera file!!

}

void SVOCameraObj::SetCameraDeviceParams(const SVDeviceParamCollection& rParams)
{
	m_CameraDeviceParams = rParams;
}

const SVDeviceParamCollection& SVOCameraObj::GetCameraFileParams() const
{
	return m_CameraFileParams;
}

SVDeviceParamCollection& SVOCameraObj::GetCameraFileParamsNonConst()
{
	return m_CameraFileParams;
}

const SVDeviceParamCollection& SVOCameraObj::GetCameraDeviceParams()
{
	return m_CameraDeviceParams;
}

SVDeviceParamCollection& SVOCameraObj::GetCameraDeviceParamsNonConst()
{
	return m_CameraDeviceParams;
}

SVOCameraObj& SVOCameraObj::operator =(const SVOCameraObj &source)
{
    m_sCameraDisplayName = source.m_sCameraDisplayName; 
    m_iDigNumber = source.m_iDigNumber;
    m_iBandNumber = source.m_iBandNumber;
    m_IsColor = source.m_IsColor;
    m_sCameraFile = source.m_sCameraFile;
    m_bCameraFileSet = source.m_bCameraFileSet;
    m_bCameraFileChanged = source.m_bCameraFileChanged;
    m_bDigSet = source.m_bDigSet;
    m_bCameraObjOk = source.m_bCameraObjOk;
	m_CameraFileParams = source.m_CameraFileParams;
	m_CameraDeviceParams = source.m_CameraDeviceParams;

	m_bFileAcquisition = source.m_bFileAcquisition;
	m_fileMode = source.m_fileMode;
	m_imageFilename = source.m_imageFilename;
	m_imageDirectory = source.m_imageDirectory;
	m_imageSizeEditModeFileBased = source.m_imageSizeEditModeFileBased;
	m_fileImageSize = source.m_fileImageSize;
	
    return (*this);
}

SVOCameraObj* SVOCameraObj::operator =(const SVOCameraObj *source)
{
    m_sCameraDisplayName = source->m_sCameraDisplayName; 
    m_iDigNumber = source->m_iDigNumber;
    m_iBandNumber = source->m_iBandNumber;
    m_IsColor = source->m_IsColor;
    m_sCameraFile = source->m_sCameraFile;
    m_bCameraFileSet = source->m_bCameraFileSet;
    m_bCameraFileChanged = source->m_bCameraFileChanged;
    m_bDigSet = source->m_bDigSet;
    m_bCameraObjOk = source->m_bCameraObjOk;
	m_CameraFileParams = source->m_CameraFileParams;
	m_CameraDeviceParams = source->m_CameraDeviceParams;

	m_bFileAcquisition = source->m_bFileAcquisition;
	m_fileMode = source->m_fileMode;
	m_imageFilename = source->m_imageFilename;
	m_imageDirectory = source->m_imageDirectory;
	m_imageSizeEditModeFileBased = source->m_imageSizeEditModeFileBased;
	m_fileImageSize = source->m_fileImageSize;

    return (this);
}

void SVOCameraObj::CheckObject()
{
    //check to see if camera file is empty

 //   if (!m_sCameraFile.IsEmpty())
   //     m_bCameraSet = TRUE;
}

bool SVOCameraObj::IsFileAcquisition() const
{
	return m_bFileAcquisition;
}

void SVOCameraObj::SetFileAcquisitionMode(bool bFileAcquisition)
{
	m_bFileAcquisition = bFileAcquisition;
}

bool SVOCameraObj::IsFileImageSizeEditModeFileBased() const
{
	return m_imageSizeEditModeFileBased;
}

void SVOCameraObj::SetFileImageSizeEditModeFileBased(bool bScanFile)
{
	m_imageSizeEditModeFileBased = bScanFile;
}

long SVOCameraObj::GetFileImageWidth() const
{
	return m_fileImageSize.cx;
}

void SVOCameraObj::SetFileImageWidth(long width)
{
	m_fileImageSize.cx = width;
}

long SVOCameraObj::GetFileImageHeight() const
{
	return m_fileImageSize.cy;
}

void SVOCameraObj::SetFileImageHeight(long height)
{
	m_fileImageSize.cy = height;
}

const SIZE& SVOCameraObj::GetFileImageSize() const
{
	return m_fileImageSize;
}

void SVOCameraObj::SetFileImageSize(const SIZE& size)
{
	m_fileImageSize = size;
}

LPCTSTR SVOCameraObj::GetImageFilename() const
{
	return m_imageFilename.c_str();
}

void SVOCameraObj::SetImageFilename(const SVString& filename)
{
	m_imageFilename = filename;
}

LPCTSTR SVOCameraObj::GetImageDirectoryName() const
{
	return m_imageDirectory.c_str();
}

void SVOCameraObj::SetImageDirectoryName(const SVString& directoryName)
{
	m_imageDirectory = directoryName;
}

long SVOCameraObj::GetFileLoadingMode() const
{
	return m_fileMode;
}

void SVOCameraObj::SetFileLoadingMode(long mode)
{
	m_fileMode = mode;
}

bool SVOCameraObj::IsSoftwareTriggerSupported() const
{
	bool bRetVal = true;

	if (!IsFileAcquisition())
	{
		// - check if camera file has been assigned
		CString camFile = GetCameraFile();
		if (!camFile.IsEmpty())
		{
			const SVDeviceParamCollection& rCameraFileParams = GetCameraFileParams();
			
			if (rCameraFileParams.ParameterExists(DeviceParamAcquisitionTriggerType))
			{
				const SVDeviceParamWrapper& rCamFileParam = rCameraFileParams.Parameter(DeviceParamAcquisitionTriggerType);
				const SVLongValueDeviceParam* pCamFileParam = rCamFileParam.DerivedValue(pCamFileParam);
				// - check if Trigger_Source_Option_N (Software Oneshot) is available
				// look for oneshot option
				if (!pCamFileParam->info.OptionExists(_T("Software OneShot")))
				{
					bRetVal = false;
				}
			}
			else if (rCameraFileParams.ParameterExists(DeviceParamGigeTriggerSource))
			{
				const SVDeviceParamWrapper& rCamFileParam = rCameraFileParams.Parameter(DeviceParamGigeTriggerSource);
				const SVStringValueDeviceParam* pCamFileParam = rCamFileParam.DerivedValue(pCamFileParam);
				// - check if Trigger_Source_Option_N (Software Oneshot) is available
				// look for oneshot option
				if (!pCamFileParam->info.OptionExists(_T("Software OneShot")))
				{
					bRetVal = false;
				}
			}
			else
			{
				bRetVal = false;
			}
		}
	}
	return bRetVal;
}

bool SVOCameraObj::IsCameraLineInputSupported() const
{
	bool bRetVal = false;

	if (!IsFileAcquisition())
	{
		// - check if camera file has been assigned
		CString camFile = GetCameraFile();
		if (!camFile.IsEmpty())
		{
			const SVDeviceParamCollection& rCameraFileParams = GetCameraFileParams();
			
			if (rCameraFileParams.ParameterExists(DeviceParamCameraInput))
			{
				bRetVal = true;
			}
		}
	}
	return bRetVal;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOCameraObj.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:48:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   21 Jan 2013 11:39:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added IsCameraLineInputSupported method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   16 Jan 2013 16:14:28   jspila
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
 *    Rev 1.10   30 Jul 2009 12:14:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   03 Jun 2009 09:54:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added IsSoftwareTriggerSupported method.
 * Revised Include file definitions due to changes in SVDeviceParams
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   05 Dec 2008 15:26:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Loading modes for File Acquisiiton
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   16 Sep 2008 14:37:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added methods and member variables in support of FileAcquistion.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   10 Sep 2008 16:41:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised GetCameraDisplayName, GetCameraFile, and GetCameraFileParams to be const
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   23 Jul 2004 08:39:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  435
 * SCR Title:  Add Bulb Mode Operation to the Digital SVIM for Point Gray Dragonfly Cameras
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a new method to get the camera parameters as non-const so that they can get changed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Oct 2003 13:17:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  386
 * SCR Title:  Change Light Reference handling to match earlier versions of SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Made changes to edit configuration to track camera file changes so that the default light reference will be read from the camera file only initially or when changed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Aug 2003 12:48:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed the way camera file params are handled
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Jul 2003 10:38:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for SVDeviceParamCollection
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 12:29:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:35:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
