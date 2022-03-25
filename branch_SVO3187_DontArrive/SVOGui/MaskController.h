//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
// All Rights Reserved
//*****************************************************************************

// This is the MaskController class for use by the GUI.
//******************************************************************************
#pragma once

#pragma region Includes
#include "ImageController.h"

#pragma endregion Includes

namespace SvOg
{
	class MaskController
	{
	public:
		MaskController(uint32_t rInspectionID, uint32_t rTaskObjectID, uint32_t maskOperatorID);
		MaskController(const MaskController&) = delete;
		const MaskController& operator==(const MaskController&) = delete;

		void Init();
		uint32_t GetInstanceID() const;
		uint32_t GetShapeMaskHelperID() const;
		IPictureDisp* GetReferenceImage() const;
		IPictureDisp* GetMaskImage() const;
		IPictureDisp* GetResultImage() const;
		RECT GetResultRect() const;
		HRESULT ImportMask(const std::string& filename);
		HRESULT ExportMask(const std::string& filename);
		HGLOBAL GetMaskData() const;
		bool SetMaskData(HGLOBAL hGlobal);

	private:
		uint32_t m_InspectionID{ SvDef::InvalidObjectId };
		uint32_t m_TaskObjectID{ SvDef::InvalidObjectId };
		uint32_t m_maskOperatorID{ SvDef::InvalidObjectId };
		uint32_t m_ShapeMaskHelperID{ SvDef::InvalidObjectId };
		SvOg::ImageController m_MaskImageController;
		SvOg::ImageController m_TaskImageController;
	};
} //namespace SvOg