//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the MaskController class for use by the GUI.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/any.hpp>
//Moved to precompiled header: #include <boost/noncopyable.hpp>
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <comdef.h>

#pragma endregion Includes

namespace SvOg
{
	class MaskController : public boost::noncopyable
	{
	public:
		MaskController(const GUID& rInspectionID, const GUID& rTaskObjectID);
		void Init();
		const GUID& GetInstanceID() const;
		const GUID& GetShapeMaskHelperID() const;
		IPictureDisp* GetReferenceImage() const;
		IPictureDisp* GetMaskImage() const;
		HRESULT SetMask(IPictureDisp* pImage);
		HRESULT ImportMask(const std::string& filename);
		HRESULT ExportMask(const std::string& filename);
		HGLOBAL GetMaskData() const;
		bool SetMaskData(HGLOBAL hGlobal);

	private:
		GUID m_InspectionID{ GUID_NULL };
		GUID m_TaskObjectID{ GUID_NULL };
		GUID m_maskOperatorID{ GUID_NULL };
		GUID m_ShapeMaskHelperID{ GUID_NULL };
	};
} //namespace SvOg