//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the AuxiliaryExtentsController class for use by the GUI.
//******************************************************************************
#pragma once

#pragma region Includes
#include "ImageController.h"
#include "DataController.h"
#pragma endregion Includes

namespace SvOg
{
	class AuxiliaryExtentsController
	{
		const uint32_t m_InspectionID ;
		const uint32_t m_TaskObjectID ;
		uint32_t m_ExternalToolTaskObjectId  {SvDef::InvalidObjectId};
		ImageController m_ImageController ;
		ValueController m_values;
		
		SvUl::NameObjectIdList m_auxSourceImages;
		std::unique_ptr<ValueController> m_pExternalToolTaskValues;

		public:
			AuxiliaryExtentsController(uint32_t inspectionID, uint32_t taskObjectID);
			virtual ~AuxiliaryExtentsController() {}
			AuxiliaryExtentsController(const AuxiliaryExtentsController&) = delete;
			AuxiliaryExtentsController& operator=(const AuxiliaryExtentsController&) = delete;
			HRESULT Init();
			HRESULT Commit();
			bool AreAuxiliaryExtentsAvailable() const;
			bool IsUpdateAuxExtentsEnabled() const;
			void EnableAuxExtents(bool bEnable);
			const SvUl::NameObjectIdList& GetAvailableImageList() const;
			std::string GetAuxSourceImageName() const;
			HRESULT SetAuxSourceImage(const std::string& rName);
			SvUl::NameObjectIdPair GetAuxSourceImage() const;
			
			bool isExternalTool() const;
			bool hasUnitMappeing() const;
			void setUseUnitMapping(bool);
			bool resetTaskObject() { return m_values.resetTaskObject(); };
			HRESULT FindAuxSourceImages();
	private:
		
		HRESULT RunOnce();
	};
} //namespace SvOg