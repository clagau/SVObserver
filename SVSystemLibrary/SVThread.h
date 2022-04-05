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
using ProcessThread = std::function<void(bool&)>;

class SVThread
{
#pragma region Constructor
public:
	SVThread() = default;
	~SVThread();
#pragma endregion Constructor

#pragma region Public Methods
public:
	HRESULT Create(const ProcessThread& rProcessThread, LPCTSTR tag);
	void Destroy();

	unsigned long GetThreadID() const;

	bool IsDisabled() { return (m_pProcessThread == nullptr || m_tag.empty()); };
	HRESULT Restart();

	int GetPriority() const;
	void SetPriority(int priority);

	bool IsActive() const;
	HANDLE GetThreadHandle() const;

	static void SetDiagnostic(bool diagnostic) { m_diagnostic = diagnostic; }
#pragma endregion Public Methods

#pragma region Private Methods
private:
	static DWORD WINAPI ThreadProc( LPVOID lpParam );
	static void SetThreadError(DWORD MessageCode, LPCTSTR Message, SvStl::SourceFileParams SourceFile);
#pragma endregion Private Methods

#pragma region Member Variables
private:
	static bool m_diagnostic;
	HANDLE m_hShutdown{ nullptr };
	HANDLE m_hThreadComplete{ nullptr };
	HANDLE m_hThread{ nullptr };
	unsigned long m_ulThreadID{0UL};
	std::string m_tag;
	ProcessThread m_pProcessThread{ nullptr };
#pragma endregion Member Variables
};
} //namespace SvSyl