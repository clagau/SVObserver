//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the AuxiliaryExtentsController class for use by the GUI.
//******************************************************************************
#pragma once

#pragma region Includes
#include <boost/noncopyable.hpp>
#include "ObjectInterfaces\NameValueList.h"
#include "SVUtilityLibrary\SVGUID.h"
#include "ImageController.h"
#include "SVOGui\BoundValue.h"
#include "SVOGui\ValuesAccessor.h"
#include "SVOGui\GuiController.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
		class AuxiliaryExtentsController : public boost::noncopyable
		{
			typedef ValuesAccessor<BoundValues> Values;
			typedef GuiController<Values, Values::value_type> ValuesController;
			SVGUID m_InspectionID;
			SVGUID m_TaskObjectID;
			ImageController m_ImageController;
			ValuesController m_Values;
			SvUl::NameGuidList m_auxSourceImages;

			public:
				AuxiliaryExtentsController(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID);
				virtual ~AuxiliaryExtentsController() {}
				HRESULT Init();
				HRESULT Commit();
				bool AreAuxiliaryExtentsAvailable() const;
				bool IsUpdateAuxExtentsEnabled() const;
				void EnableAuxExtents(bool bEnable);
				const SvUl::NameGuidList& GetAvailableImageList() const;
				SVString GetAuxSourceImageName() const;
				HRESULT SetAuxSourceImage(const SVString& rName);
				SvUl::NameGuidPair GetAuxSourceImage() const;

		private:
			HRESULT FindAuxSourceImages();
			HRESULT RunOnce();
		};
	}
}

namespace SvOg = Seidenader::SVOGui;