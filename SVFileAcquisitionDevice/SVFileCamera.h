//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
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
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <set>
//Moved to precompiled header: #include <iterator>
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/function.hpp>
#include <mil.h>
#include "SVFileCameraStruct.h"
#include "SVEventHandler.h"
#include "SVFileSystemLibrary/SVFileInfo.h"
#include "SVFileSystemLibrary/SVFileInfoComparator.h"
#include "SVSystemLibrary/SVSequencer.h"
#include "SVSystemLibrary/SVAsyncProcedure.h"
#pragma endregion Includes

class SVAcquisitionBufferInterface;
namespace SvOi
{
class ITRCImage;
}

class SVFileCamera
{
private:
	typedef std::set<SVFileInfo, SVFileInfoComparator> SVFileList;
	typedef SVFileList::const_iterator FileListIterator;
	typedef std::insert_iterator<SVFileList> Insertor;

public:
	typedef boost::function<HRESULT (unsigned long)> EventHandler;
	typedef SVEventHandler<EventHandler> SVFrameEventHandler;

private:
	long m_index{-1L};
	SVFileCameraStruct m_fileData;
	std::string m_name;
	SVFileList m_fileList;
	SVSequencer<FileListIterator> m_loadSequence;
	MIL_ID m_image = M_NULL;
	SVAsyncProcedure m_thread;
	SVFrameEventHandler m_startFrameEvent;
	SVFrameEventHandler m_endFrameEvent;

	static void CALLBACK OnAPCEvent( ULONG_PTR data );
	void OnThreadEvent( bool& p_WaitForEvents );

	std::string GetNextFilename();

public:
	long m_lIsStarted{0L};

	SVFileCamera() = default;
	explicit SVFileCamera(LPCTSTR name) : m_name{name} {};
	SVFileCamera::SVFileCamera(const SVFileCamera& rRhs) = delete;
	SVFileCamera::SVFileCamera(SVFileCamera&& rRhs) = default;
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
	bool IsSingleIterationLoadMode() const;
	
	HRESULT Start(const EventHandler& startFrameHandler, const EventHandler& endFrameHandler, unsigned long p_ulIndex);
	void Stop();

	bool IsRunning() const;

	HRESULT DoOneShot();

	HRESULT CopyImage(SvOi::ITRCImage* pImagePtr);

	HRESULT loadImage(std::string fileName);


	double m_StartTimeStamp{0.0};
	SVAcquisitionBufferInterface* m_pBufferInterface{nullptr};
};

