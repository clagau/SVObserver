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

#include "SVUtilityLibrary\SVSharedPtr.h"
#pragma endregion Includes

class SVOCameraObj  
{
public:
	SVOCameraObj();
	virtual ~SVOCameraObj();
    SVOCameraObj& operator =(const SVOCameraObj &source);
    SVOCameraObj* operator =(const SVOCameraObj *source);

    const std::string& GetCameraDisplayName() const;
    const std::string& GetCameraFile() const;
	int GetDigNumber() const;
	int GetCameraID() const;
    int GetBandNumber() const;
	const SVDeviceParamCollection& GetCameraFileParams() const;
	SVDeviceParamCollection& GetCameraFileParamsNonConst();
	const SVDeviceParamCollection& GetCameraDeviceParams();
	SVDeviceParamCollection& GetCameraDeviceParamsNonConst();

    void SetCameraDisplayName(const std::string& rCameraName);
	void SetDigNumber(int DigNumber);
	void SetCameraID(int CameraID);
    void SetCameraFile(const std::string& rFileName);
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
	const std::string& GetImageFilename() const;
	void SetImageFilename(const std::string& rFilename);
	const std::string& GetImageDirectoryName() const;
	void SetImageDirectoryName(const std::string& rPathName);
	long GetFileLoadingMode() const;
	void SetFileLoadingMode(long mode);
	bool IsSoftwareTriggerSupported() const;
	bool IsCameraLineInputSupported() const;

	inline void SetIsColor( bool IsColor ){ m_IsColor = IsColor; };
	inline bool IsColor() const { return m_IsColor; };

private:  //data members
    std::string m_CameraDisplayName;
	int m_DigNumber;
	int m_CameraID;
    int m_BandNumber;
    bool m_IsColor;
    std::string m_CameraFile;
    bool m_CameraFileSet;
	bool m_CameraFileChanged;
    bool m_CameraObjOk;
	SVDeviceParamCollection m_CameraDeviceParams;
	SVDeviceParamCollection m_CameraFileParams;

	bool m_FileAcquisition;
	std::string m_imageFilename;
	std::string m_imageDirectory;
	bool m_imageSizeEditModeFileBased;
	SIZE m_fileImageSize;
	long m_fileMode;
};

typedef SVSharedPtr< SVOCameraObj > SVOCameraObjPtr;


