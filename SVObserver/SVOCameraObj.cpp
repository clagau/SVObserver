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

SVOCameraObj::SVOCameraObj() :
 m_DigNumber( 0 )
, m_CameraID( 0 )
, m_BandNumber( 0 )
, m_IsColor( false )
, m_CameraFileSet( false )
, m_CameraFileChanged( false )
, m_CameraObjOk( false )
, m_FileAcquisition( false )
, m_imageSizeEditModeFileBased( true )
, m_fileMode( ContinuousMode )
{
	m_fileImageSize.cx = 640;
	m_fileImageSize.cy = 480;
}

SVOCameraObj::~SVOCameraObj()
{

}

CString SVOCameraObj::GetCameraDisplayName() const
{
    return m_CameraDisplayName;
}

int SVOCameraObj::GetDigNumber() const
{
	return m_DigNumber;
}

int SVOCameraObj::GetCameraID() const
{
	return m_CameraID;
}

void SVOCameraObj::SetCameraDisplayName(CString CameraName)
{
    m_CameraDisplayName = CameraName;
}

void SVOCameraObj::SetDigNumber(int DigNumber)
{
	m_DigNumber = DigNumber;
}

void SVOCameraObj::SetCameraID(int CameraID)
{
	m_CameraID = CameraID;
}

void SVOCameraObj::SetCameraFile(CString sFileName)
{
    m_CameraFile = sFileName;
    if (!m_CameraFile.IsEmpty())
    {
        m_CameraFileSet = TRUE;
    }
    else
    {
        m_CameraFileSet = FALSE;
    }
    //check to see if Camera is ok...

    IsCameraObjOk();
}

void SVOCameraObj::SetCameraFileChanged()
{
    m_CameraFileChanged = TRUE;
}

BOOL SVOCameraObj::GetCameraFileChanged()
{
    return m_CameraFileChanged;
}

CString SVOCameraObj::GetCameraFile() const
{
    return m_CameraFile;
}

BOOL SVOCameraObj::IsCameraObjOk()
{
	if (IsFileAcquisition())
	{
		m_CameraObjOk = true;
	}
	else
	{
		if (m_CameraFileSet)
		{
			m_CameraObjOk = TRUE;
		}
		else
		{
			m_CameraObjOk = FALSE;
		}
	}
    return m_CameraObjOk;
}

void SVOCameraObj::SetBandNumber(int iBandNumber)
{
    m_BandNumber = iBandNumber;
}

int SVOCameraObj::GetBandNumber() const
{
    return m_BandNumber;
}

void SVOCameraObj::SetCameraFileParams(const SVDeviceParamCollection& rParams)
{
	m_CameraFileParams = rParams;

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

SVOCameraObj& SVOCameraObj::operator =(const SVOCameraObj& rRhs)
{
    m_CameraDisplayName = rRhs.m_CameraDisplayName; 
	m_DigNumber = rRhs.m_DigNumber;
	m_CameraID = rRhs.m_CameraID;
    m_BandNumber = rRhs.m_BandNumber;
    m_IsColor = rRhs.m_IsColor;
    m_CameraFile = rRhs.m_CameraFile;
    m_CameraFileSet = rRhs.m_CameraFileSet;
    m_CameraFileChanged = rRhs.m_CameraFileChanged;
    m_CameraObjOk = rRhs.m_CameraObjOk;
	m_CameraFileParams = rRhs.m_CameraFileParams;
	m_CameraDeviceParams = rRhs.m_CameraDeviceParams;

	m_FileAcquisition = rRhs.m_FileAcquisition;
	m_fileMode = rRhs.m_fileMode;
	m_imageFilename = rRhs.m_imageFilename;
	m_imageDirectory = rRhs.m_imageDirectory;
	m_imageSizeEditModeFileBased = rRhs.m_imageSizeEditModeFileBased;
	m_fileImageSize = rRhs.m_fileImageSize;
	
    return (*this);
}

SVOCameraObj* SVOCameraObj::operator =(const SVOCameraObj* pRhs)
{
    m_CameraDisplayName = pRhs->m_CameraDisplayName; 
    m_DigNumber = pRhs->m_DigNumber;
	m_CameraID = pRhs->m_CameraID;
    m_BandNumber = pRhs->m_BandNumber;
    m_IsColor = pRhs->m_IsColor;
    m_CameraFile = pRhs->m_CameraFile;
    m_CameraFileSet = pRhs->m_CameraFileSet;
    m_CameraFileChanged = pRhs->m_CameraFileChanged;
    m_CameraObjOk = pRhs->m_CameraObjOk;
	m_CameraFileParams = pRhs->m_CameraFileParams;
	m_CameraDeviceParams = pRhs->m_CameraDeviceParams;

	m_FileAcquisition = pRhs->m_FileAcquisition;
	m_fileMode = pRhs->m_fileMode;
	m_imageFilename = pRhs->m_imageFilename;
	m_imageDirectory = pRhs->m_imageDirectory;
	m_imageSizeEditModeFileBased = pRhs->m_imageSizeEditModeFileBased;
	m_fileImageSize = pRhs->m_fileImageSize;

    return (this);
}

bool SVOCameraObj::IsFileAcquisition() const
{
	return m_FileAcquisition;
}

void SVOCameraObj::SetFileAcquisitionMode(bool bFileAcquisition)
{
	m_FileAcquisition = bFileAcquisition;
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

