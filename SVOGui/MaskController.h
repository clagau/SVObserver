//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the MaskController class for use by the GUI.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/any.hpp>
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <comdef.h>
#include "ImageController.h"

#pragma endregion Includes

namespace SvOg
{
	class MaskController
	{
	public:
		MaskController(const GUID& rInspectionID, const GUID& rTaskObjectID, const GUID& maskOperatorID);
		MaskController(const MaskController&) = delete;
		const MaskController& operator==(const MaskController&) = delete;

		void Init();
		const GUID& GetInstanceID() const;
		const GUID& GetShapeMaskHelperID() const;
		IPictureDisp* GetReferenceImage() const;
		IPictureDisp* GetMaskImage() const;
		IPictureDisp* GetResultImage() const;
		HRESULT ImportMask(const std::string& filename);
		HRESULT ExportMask(const std::string& filename);
		HGLOBAL GetMaskData() const;
		bool SetMaskData(HGLOBAL hGlobal);

	private:
		GUID m_InspectionID{ GUID_NULL };
		GUID m_TaskObjectID{ GUID_NULL };
		GUID m_maskOperatorID{ GUID_NULL };
		GUID m_ShapeMaskHelperID{ GUID_NULL };
		SvOg::ImageController m_MaskImageController;
		SvOg::ImageController m_TaskImageController;
	};
} //namespace SvOg