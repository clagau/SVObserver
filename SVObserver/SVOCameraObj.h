//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCameraObj
//* .File Name       : $Workfile:   SVOCameraObj.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:49:02  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOMFCLibrary\SVDeviceParamCollection.h"
#include "SVUtilityLibrary\SVString.h"
#include "SVUtilityLibrary\SVSharedPtr.h"
#pragma endregion Includes

class SVOCameraObj  
{
public:
	SVOCameraObj();
	virtual ~SVOCameraObj();
    SVOCameraObj& operator =(const SVOCameraObj &source);
    SVOCameraObj* operator =(const SVOCameraObj *source);

    CString GetCameraDisplayName() const;
    CString GetCameraFile() const;
    int GetDigNumber() const;
    int GetBandNumber() const;
	const SVDeviceParamCollection& GetCameraFileParams() const;
	SVDeviceParamCollection& GetCameraFileParamsNonConst();
	const SVDeviceParamCollection& GetCameraDeviceParams();
	SVDeviceParamCollection& GetCameraDeviceParamsNonConst();

    void SetCameraDisplayName(CString sCameraName);
    void SetDigNumber(int iDigNumber);
    void SetCameraFile(CString sFileName);
	void SetCameraFileChanged();
	BOOL GetCameraFileChanged();
    void SetBandNumber(int iBandNumber);
	void SetCameraFileParams(const SVDeviceParamCollection& rParams);
	void SetCameraDeviceParams(const SVDeviceParamCollection& rParams);
    BOOL IsCameraObjOk();

	bool IsFileAcquisition() const;
	void SetFileAcquisitionMode(bool bFileAcquisition);
	bool IsFileImageSizeEditModeFileBased() const;
	void SetFileImageSizeEditModeFileBased(bool bScanFile);
	long GetFileImageWidth() const;
	void SetFileImageWidth(long width);
	long GetFileImageHeight() const;
	void SetFileImageHeight(long height);
	const SIZE& GetFileImageSize() const;
	void SetFileImageSize(const SIZE& size);
	LPCTSTR GetImageFilename() const;
	void SetImageFilename(const SVString& filename);
	LPCTSTR GetImageDirectoryName() const;
	void SetImageDirectoryName(const SVString& directoryName);
	long GetFileLoadingMode() const;
	void SetFileLoadingMode(long mode);
	bool IsSoftwareTriggerSupported() const;
	bool IsCameraLineInputSupported() const;

	inline void SetIsColor( bool IsColor ){ m_IsColor = IsColor; };
	inline bool IsColor() const { return m_IsColor; };

private:  //data members
    CString m_sCameraDisplayName; 
    int m_iDigNumber;
    int m_iBandNumber;
    bool m_IsColor;
    CString m_sCameraFile;
    BOOL m_bCameraFileSet;
	BOOL m_bCameraFileChanged;
    BOOL m_bDigSet;
    BOOL m_bCameraObjOk;
	SVDeviceParamCollection m_CameraDeviceParams;
	SVDeviceParamCollection m_CameraFileParams;

	bool m_bFileAcquisition;
	SVString m_imageFilename;
	SVString m_imageDirectory;
	bool m_imageSizeEditModeFileBased;
	SIZE m_fileImageSize;
	long m_fileMode;
};

typedef SVSharedPtr< SVOCameraObj > SVOCameraObjPtr;


