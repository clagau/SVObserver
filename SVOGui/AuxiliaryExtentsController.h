//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the AuxiliaryExtentsController class for use by the GUI.
//******************************************************************************
#pragma once

#pragma region Includes
#include "ImageController.h"
#include "BoundValue.h"
#include "ValuesAccessor.h"
#include "DataController.h"
#pragma endregion Includes

namespace SvOg
{
	class AuxiliaryExtentsController
	{
		typedef SvOg::DataController<SvOg::ValuesAccessor, SvOg::ValuesAccessor::value_type> ValuesController;
		const uint32_t m_InspectionID;
		const uint32_t m_TaskObjectID;
		ImageController m_ImageController;
		ValuesController m_Values;
		SvUl::NameObjectIdList m_auxSourceImages;

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

	private:
		HRESULT FindAuxSourceImages();
		HRESULT RunOnce();
	};
} //namespace SvOg