//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the MaskController class for use by the GUI.
//******************************************************************************
#pragma once

#pragma region Includes
#include <boost/any.hpp>
#include <boost/noncopyable.hpp>
#include <string>
#include <comdef.h>
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
		class MaskController : public boost::noncopyable
		{
		public:
			MaskController(const GUID& rInspectionID, const GUID& rTaskObjectID);
			void Init();
			const GUID& GetInstanceID() const;
			IPictureDisp* GetReferenceImage() const;
			IPictureDisp* GetMaskImage() const;
			HRESULT SetMask(IPictureDisp* pImage);
			HRESULT ImportMask(const SVString& filename);
			HRESULT ExportMask(const SVString& filename);
			HGLOBAL GetMaskData() const;
			bool SetMaskData(HGLOBAL hGlobal);

		private:
			GUID m_InspectionID;
			GUID m_TaskObjectID;
			GUID m_maskOperatorID;
		};
	}
}

namespace SvOg = Seidenader::SVOGui;