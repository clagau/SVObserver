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

