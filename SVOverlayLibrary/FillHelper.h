//*****************************************************************************
/// \copyright (c) 2019,2019 by Seidenader Maschinenbau GmbH
/// \file FillHelper.h
/// All Rights Reserved 
//*****************************************************************************
/// Helper Function for filling the Overlay-protoBuf Message with live data.
//******************************************************************************
#pragma once


#pragma region Includes
#include "SVProtoBuf/Overlay.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

namespace SvOv
{
/// Fill live data for all overlays to protobuf-message.
/// \param overlayDesc [in,out] Protobuf message
/// \param rTr [in] triggerReocrd
void fillOverlay(SvPb::OverlayDesc& overlayDesc, const SvOi::ITriggerRecordR& rTr);
}