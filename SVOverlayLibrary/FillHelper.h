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
#include "TriggerRecordController\ITriggerRecordControllerRW.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

namespace SvOv
{
/// Fill live data to protobuf ValueObject
/// \param pTr [in] triggerReocrd
/// \param trPos [in] Position of the data in triggerRecord
/// \param pValue [in,out] protoBuf Message
/// \returns bool True if value is set.
bool fillValue(const SvTrc::ITriggerRecordRPtr pTr, long trPos, SvPb::ValueObject* pValue);

/// Fill live data to protobuf message for rectangle.
/// \param pTr [in] triggerReocrd
/// \param rRect [in,out] protoBuf Message
void fillRect(const SvTrc::ITriggerRecordRPtr pTr, SvPb::OverlayShapeRect& rRect);

/// Fill live data to protobuf message for rectangle with x1/2 data (e.g. for blob-Overlays).
/// \param pTr [in] triggerReocrd
/// \param rRectArrayData [in] protoBuf SVObserver-input Data
/// \param rRectArray [in] protoBuf output Data
/// \returns bool True if value is set.
bool fillRectArray(const SvTrc::ITriggerRecordRPtr pTr, const SvPb::SVORectArray12Data& rRectArrayData, SvPb::OverlayShapeRectArray& rRectArray);

/// Fill live data to protobuf message for rectangle with pattern data.
/// \param pTr [in] triggerReocrd
/// \param rRectArrayData [in] protoBuf SVObserver-input Data
/// \param rRectArray [in] protoBuf output Data
/// \returns bool True if value is set.
bool fillRectArray(const SvTrc::ITriggerRecordRPtr pTr, const SvPb::SVORectArrayPatternData& rRectArrayData, SvPb::OverlayShapeRectArray& rRectArray);

/// Fill live data for all overlays to protobuf-message.
/// \param overlayDesc [in,out] Protobuf message
/// \param pTr [in] triggerReocrd
void fillOverlay(SvPb::OverlayDesc& overlayDesc, const SvTrc::ITriggerRecordRPtr pTr);
}