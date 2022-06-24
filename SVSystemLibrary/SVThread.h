//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVThread
//* .File Name       : $Workfile:   SVThread.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Dec 2014 13:59:06  $
//******************************************************************************
#pragma once

#pragma region Includes
#pragma endregion Includes

namespace SvStl
{
struct SourceFileParams;
}

namespace SvSyl
{

class SVThread
{
#pragma region Constructor
public:
	SVThread() = default;
	~SVThread();
#pragma endregion Constructor

#pragma region Public Methods
public:
	HRESULT Create(LPCTSTR tag);
	void Destroy();

	bool IsDisabled() { return m_threadName.empty(); };

	int GetPriority() const;
	void SetPriority(int priority);

	bool IsActive() const;
	HANDLE GetThreadHandle() const;

#pragma endregion Public Methods

#pragma region Private Methods
private:
	static DWORD WINAPI ThreadProc( LPVOID lpParam );
#pragma endregion Private Methods

#pragma region Member Variables
private:
	HANDLE m_hShutdown{ nullptr };
	mutable std::thread m_thread;
	std::string m_threadName;
#pragma endregion Member Variables
};
} //namespace SvSyl