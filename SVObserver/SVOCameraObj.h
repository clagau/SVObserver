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
#include "CameraLibrary\SVDeviceParamCollection.h"
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

    const SVString& GetCameraDisplayName() const;
    const SVString& GetCameraFile() const;
	int GetDigNumber() const;
	int GetCameraID() const;
    int GetBandNumber() const;
	const SVDeviceParamCollection& GetCameraFileParams() const;
	SVDeviceParamCollection& GetCameraFileParamsNonConst();
	const SVDeviceParamCollection& GetCameraDeviceParams();
	SVDeviceParamCollection& GetCameraDeviceParamsNonConst();

    void SetCameraDisplayName(const SVString& rCameraName);
	void SetDigNumber(int DigNumber);
	void SetCameraID(int CameraID);
    void SetCameraFile(const SVString& rFileName);
	void SetCameraFileChanged();
	bool GetCameraFileChanged();
    void SetBandNumber(int BandNumber);
	void SetCameraFileParams(const SVDeviceParamCollection& rParams);
	void SetCameraDeviceParams(const SVDeviceParamCollection& rParams);
    bool IsCameraObjOk();

	bool IsFileAcquisition() const;
	void SetFileAcquisitionMode(bool FileAcquisition);
	bool IsFileImageSizeEditModeFileBased() const;
	void SetFileImageSizeEditModeFileBased(bool ScanFile);
	long GetFileImageWidth() const;
	void SetFileImageWidth(long width);
	long GetFileImageHeight() const;
	void SetFileImageHeight(long height);
	const SIZE& GetFileImageSize() const;
	void SetFileImageSize(const SIZE& size);
	const SVString& GetImageFilename() const;
	void SetImageFilename(const SVString& rFilename);
	const SVString& GetImageDirectoryName() const;
	void SetImageDirectoryName(const SVString& rPathName);
	long GetFileLoadingMode() const;
	void SetFileLoadingMode(long mode);
	bool IsSoftwareTriggerSupported() const;
	bool IsCameraLineInputSupported() const;

	inline void SetIsColor( bool IsColor ){ m_IsColor = IsColor; };
	inline bool IsColor() const { return m_IsColor; };

private:  //data members
    SVString m_CameraDisplayName;
	int m_DigNumber;
	int m_CameraID;
    int m_BandNumber;
    bool m_IsColor;
    SVString m_CameraFile;
    bool m_CameraFileSet;
	bool m_CameraFileChanged;
    bool m_CameraObjOk;
	SVDeviceParamCollection m_CameraDeviceParams;
	SVDeviceParamCollection m_CameraFileParams;

	bool m_FileAcquisition;
	SVString m_imageFilename;
	SVString m_imageDirectory;
	bool m_imageSizeEditModeFileBased;
	SIZE m_fileImageSize;
	long m_fileMode;
};

typedef SVSharedPtr< SVOCameraObj > SVOCameraObjPtr;


