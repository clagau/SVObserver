//*****************************************************************************
/// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \file CifxLoadLibrary.cpp
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "CifxLoadLibrary.h"
#pragma endregion Includes

constexpr const char* cCifxDll = "cifX32dll.dll";

CifxLoadLibrary::~CifxLoadLibrary()
{
	Close();
}

HRESULT CifxLoadLibrary::Open()
{
	HRESULT result {S_OK};

	if ( nullptr == m_dllHandle )
	{
		m_dllHandle = ::LoadLibrary(cCifxDll);

		if ( nullptr != m_dllHandle)
		{
			m_pDriverOpen = reinterpret_cast<xDriverOpenPtr> (::GetProcAddress(m_dllHandle, "xDriverOpen"));
			m_pDriverClose = reinterpret_cast<xDriverClosePtr> (::GetProcAddress(m_dllHandle, "xDriverClose"));
			m_pDriverGetInformation = reinterpret_cast<xDriverGetInformationPtr> (::GetProcAddress(m_dllHandle, "xDriverGetInformation"));
			m_pChannelOpen = reinterpret_cast<xChannelOpenPtr> (::GetProcAddress(m_dllHandle, "xChannelOpen"));
			m_pChannelClose = reinterpret_cast<xChannelClosePtr> (::GetProcAddress(m_dllHandle, "xChannelClose"));
			m_pChannelInfo = reinterpret_cast<xChannelInfoPtr> (::GetProcAddress(m_dllHandle, "xChannelInfo"));
			m_pChannelReset = reinterpret_cast<xChannelResetPtr> (::GetProcAddress(m_dllHandle, "xChannelReset"));
			m_pChannelHostState = reinterpret_cast<xChannelHostStatePtr> (::GetProcAddress(m_dllHandle, "xChannelHostState"));
			m_pChannelSyncState = reinterpret_cast<xChannelSyncStatePtr> (::GetProcAddress(m_dllHandle, "xChannelSyncState"));
			m_pChannelPutPacket = reinterpret_cast<xChannelPutPacketPtr> (::GetProcAddress(m_dllHandle, "xChannelPutPacket"));
			m_pChannelGetPacket = reinterpret_cast<xChannelGetPacketPtr> (::GetProcAddress(m_dllHandle, "xChannelGetPacket"));
			m_pChannelBusState = reinterpret_cast<xChannelBusStatePtr> (::GetProcAddress(m_dllHandle, "xChannelBusState"));
			m_pChannelDMAState = reinterpret_cast<xChannelDMAStatePtr> (::GetProcAddress(m_dllHandle, "xChannelDMAState"));
			m_pChannelRegisterNotification = reinterpret_cast<xChannelRegisterNotificationPtr> (::GetProcAddress(m_dllHandle, "xChannelRegisterNotification"));
			m_pChannelUnregisterNotification = reinterpret_cast<xChannelUnregisterNotificationPtr> (::GetProcAddress(m_dllHandle, "xChannelUnregisterNotification"));
			m_pChannelIORead = reinterpret_cast<xChannelIOReadPtr> (::GetProcAddress(m_dllHandle, "xChannelIORead"));
			m_pChannelIOWrite = reinterpret_cast<xChannelIOWritePtr> (::GetProcAddress(m_dllHandle, "xChannelIOWrite"));

			if (nullptr == m_pDriverOpen ||
			    nullptr == m_pDriverClose ||
			    nullptr == m_pDriverGetInformation ||
			    nullptr == m_pChannelOpen ||
			    nullptr == m_pChannelClose ||
			    nullptr == m_pChannelInfo ||
			    nullptr == m_pChannelReset ||
				nullptr == m_pChannelHostState ||
				nullptr == m_pChannelSyncState ||
				nullptr == m_pChannelPutPacket ||
				nullptr == m_pChannelGetPacket ||
				nullptr == m_pChannelBusState ||
				nullptr == m_pChannelDMAState ||
				nullptr == m_pChannelRegisterNotification ||
				nullptr == m_pChannelUnregisterNotification ||
				nullptr == m_pChannelIORead ||
				nullptr == m_pChannelIOWrite)
			{
				result = E_FAIL;
			}
			else
			{
				m_isInitilized = true;
			}
		}
		else
		{
			result = E_FAIL;
		}
	}

	if ( S_OK != result )
	{
		Close();
	}

	return result;
}

HRESULT CifxLoadLibrary::Close()
{
	HRESULT result {S_OK};

	if ( nullptr != m_dllHandle )
	{
		if ( ::FreeLibrary( m_dllHandle ) )
		{
			Sleep(0);
			m_dllHandle = nullptr;
		}
		else
		{
			result = E_FAIL;
		}
	}

	return result;
}

