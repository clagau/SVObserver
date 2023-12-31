//******************************************************************************
//* COPYRIGHT (c) 2008 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileCamera
//* .File Name       : $Workfile:   SVFileCamera.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Oct 2013 06:14:50  $
//******************************************************************************
#pragma once

#pragma region Includes
#include <mil.h>
#include "SVFileCameraStruct.h"
#include "SVSystemLibrary/SVThread.h"
#pragma endregion Includes

class SVAcquisitionBufferInterface;
namespace SvOi
{
class ITRCImage;
}

class SVFileCamera
{
public:
	using EventHandler = std::function<HRESULT (unsigned long)>;

private:
	long m_index{-1L};
	SVFileCameraStruct m_fileData;
	std::string m_name;
	std::vector<std::string> m_fileList;
	std::vector<std::string>::iterator m_currentFileIter {m_fileList.end()};
	MIL_ID m_image = M_NULL;
	SvSyl::SVThread m_processThread;
	EventHandler m_startFrameEvent;
	EventHandler m_endFrameEvent;

	static void __stdcall ProcessCallback(ULONG_PTR pCaller);

	std::string GetNextFilename();

	MIL_ID GetNextImageId();
	bool m_UsePreLoadImages{ true };
	int m_MaxPreloadFileNumber{ 50 };
	int m_PreloadTimeDelay{ -1 };
	mutable std::mutex m_fileCameraMutex;
	std::string m_acquisitionFile;
	DWORD m_preAcqPause {0U};

	std::vector<MIL_ID> m_imageList;
	std::vector<MIL_ID>::iterator m_currentImageIter {m_imageList.end()};
public:
	long m_lIsStarted{0L};
	SVFileCamera() = default;
	explicit SVFileCamera(LPCTSTR name) : m_name{name} {};
	SVFileCamera(const SVFileCamera& rRhs) = delete;
	SVFileCamera(SVFileCamera&& rRhs) = default;
	SVFileCamera &operator=(const SVFileCamera& rRhs) = delete;
	SVFileCamera &operator=(SVFileCamera&& rRhs) = default;

	const std::string& GetName() const;
	void SetName(const std::string& name);

	const SIZE& GetImageSize() const;
	void SetImageSize(const SIZE& imageSize);

	SvDef::SVImageFormatEnum GetImageFormat() const;
	void SetImageFormat(SvDef::SVImageFormatEnum imageFormat);
	
	// setup
	LPCTSTR GetDirectory() const;
	void SetDirectory(const std::string& dir);
	LPCTSTR GetFileName() const;
	void SetFileName(const std::string& fileName);

	void SetLoadingMode(SVFileAcquisitonLoadingModeEnum mode);
	SVFileAcquisitonLoadingModeEnum GetLoadingMode() const;
	
	void SetMaxPreLoadFilenumber(int number) { m_MaxPreloadFileNumber = number; };
	int GetMaxPreLoadFilenumber() const { return m_MaxPreloadFileNumber; };
	void SetPreloadTimeDelay(int del) { m_PreloadTimeDelay = del; };
	int GetPreloadTimeDelay() const { return m_PreloadTimeDelay; };

	bool IsSingleIterationLoadMode() const;
	
	HRESULT Start(const EventHandler& startFrameHandler, const EventHandler& endFrameHandler, unsigned long p_ulIndex);
	void Stop();

	bool IsRunning() const;

	void DoOneShot(LPCTSTR pAcquisitionFile, DWORD preAcqPause);

	HRESULT CopyImage(SvOi::ITRCImage* pImagePtr);

	HRESULT loadImage(const std::string& rFileName);
	std::string getAcquisitionFile() const;

	double m_StartTimeStamp{0.0};
	SVAcquisitionBufferInterface* m_pBufferInterface{nullptr};
};

