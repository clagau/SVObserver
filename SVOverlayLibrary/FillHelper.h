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
/// Fill live data to protobuf ValueObject
/// \param rTr [in] triggerReocrd
/// \param trPos [in] Position of the data in triggerRecord
/// \param pValue [in,out] protoBuf Message
/// \returns bool True if value is set.
bool fillValue(const SvTrc::ITriggerRecordR& rTr, long trPos, SvPb::ValueObject* pValue);

/// Fill live data to protobuf message for rectangle.
/// \param rTr [in] triggerReocrd
/// \param rRect [in,out] protoBuf Message
void fillRect(const SvTrc::ITriggerRecordR& rTr, SvPb::OverlayShapeRect& rRect);

/// Fill live data to protobuf message for rectangle with x1/2 data (e.g. for blob-Overlays).
/// \param rTr [in] triggerReocrd
/// \param rRectArrayData [in] protoBuf SVObserver-input Data
/// \param rRectArray [in] protoBuf output Data
/// \returns bool True if value is set.
bool fillRectArray(const SvTrc::ITriggerRecordR& rTr, const SvPb::SVORectArray12Data& rRectArrayData, SvPb::OverlayShapeRectArray& rRectArray);

/// Fill live data to protobuf message for rectangle with pattern data.
/// \param rTr [in] triggerReocrd
/// \param rRectArrayData [in] protoBuf SVObserver-input Data
/// \param rRectArray [in] protoBuf output Data
/// \returns bool True if value is set.
bool fillRectArray(const SvTrc::ITriggerRecordR& rTr, const SvPb::SVORectArrayPatternData& rRectArrayData, SvPb::OverlayShapeRectArray& rRectArray);

/// Fill live data for all overlays to protobuf-message.
/// \param overlayDesc [in,out] Protobuf message
/// \param rTr [in] triggerReocrd
void fillOverlay(SvPb::OverlayDesc& overlayDesc, const SvTrc::ITriggerRecordR& rTr);
}