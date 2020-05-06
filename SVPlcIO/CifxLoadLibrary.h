//*****************************************************************************
/// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \file CifxLoadLibrary.h
/// \brief Loads the Cifx library dynamically to avoid problems when Cifx
/// card and dll is not available
//******************************************************************************
#pragma once

#pragma region Includes
#include <stdint.h>
#include "cifXUser.h"
#pragma endregion Includes

typedef int32_t (APIENTRY *xDriverOpenPtr)(CIFXHANDLE*);
typedef int32_t (APIENTRY *xDriverClosePtr)(CIFXHANDLE);
typedef int32_t (APIENTRY *xDriverGetInformationPtr)(CIFXHANDLE, uint32_t, void*);
typedef int32_t (APIENTRY *xChannelOpenPtr)(CIFXHANDLE, char*, uint32_t, CIFXHANDLE*);
typedef int32_t (APIENTRY *xChannelClosePtr)(CIFXHANDLE);
typedef int32_t (APIENTRY *xChannelInfoPtr)(CIFXHANDLE, uint32_t, void*);
typedef int32_t (APIENTRY *xChannelResetPtr)(CIFXHANDLE, uint32_t, uint32_t);
typedef int32_t (APIENTRY *xChannelHostStatePtr)(CIFXHANDLE, uint32_t, uint32_t*, uint32_t);
typedef int32_t (APIENTRY *xChannelPutPacketPtr)(CIFXHANDLE, CIFX_PACKET*, uint32_t);
typedef int32_t (APIENTRY *xChannelGetPacketPtr)(CIFXHANDLE, uint32_t, CIFX_PACKET*, uint32_t);
typedef int32_t (APIENTRY *xChannelBusStatePtr)(CIFXHANDLE, uint32_t, uint32_t*, uint32_t);
typedef int32_t (APIENTRY *xChannelDMAStatePtr)(CIFXHANDLE, uint32_t, uint32_t*);
typedef int32_t (APIENTRY *xChannelRegisterNotificationPtr)(CIFXHANDLE, uint32_t, PFN_NOTIFY_CALLBACK, void*);
typedef int32_t (APIENTRY *xChannelUnregisterNotificationPtr)(CIFXHANDLE, uint32_t);
typedef int32_t (APIENTRY *xChannelIOReadPtr)(CIFXHANDLE, uint32_t, uint32_t, uint32_t, void*, uint32_t);
typedef int32_t (APIENTRY *xChannelIOWritePtr)(CIFXHANDLE, uint32_t, uint32_t, uint32_t, void*, uint32_t);

class CifxLoadLibrary  
{
public:
	CifxLoadLibrary() = default;
	~CifxLoadLibrary();

	HRESULT Open();
	HRESULT Close();
	bool isInitilized() { return m_isInitilized; }
	
	xDriverOpenPtr m_pDriverOpen {nullptr};
	xDriverClosePtr m_pDriverClose {nullptr};
	xDriverGetInformationPtr m_pDriverGetInformation {nullptr};
	xChannelOpenPtr m_pChannelOpen {nullptr};
	xChannelClosePtr m_pChannelClose {nullptr};
	xChannelInfoPtr m_pChannelInfo {nullptr};
	xChannelResetPtr m_pChannelReset {nullptr};
	xChannelHostStatePtr m_pChannelHostState {nullptr};
	xChannelPutPacketPtr m_pChannelPutPacket {nullptr};
	xChannelGetPacketPtr m_pChannelGetPacket {nullptr};
	xChannelBusStatePtr m_pChannelBusState {nullptr};
	xChannelDMAStatePtr m_pChannelDMAState {nullptr};
	xChannelRegisterNotificationPtr m_pChannelRegisterNotification {nullptr};
	xChannelUnregisterNotificationPtr m_pChannelUnregisterNotification {nullptr};
	xChannelIOReadPtr m_pChannelIORead {nullptr};
	xChannelIOWritePtr m_pChannelIOWrite {nullptr};

private:
	HMODULE m_dllHandle {nullptr};
	bool m_isInitilized{false};
};

