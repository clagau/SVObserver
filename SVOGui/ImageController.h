//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Controller class for accessing Images via the GUI.
//******************************************************************************
#pragma once

#pragma region Includes
#include <boost/noncopyable.hpp>
#include "SVUtilityLibrary\NameGuidList.h"
#include "SVUtilityLibrary\SVString.h"
#include "ObjectInterfaces\SVObjectTypeInfoStruct.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
		class ImageController : public boost::noncopyable
		{
			GUID m_InspectionID;
			GUID m_TaskObjectID;
			SVObjectSubTypeEnum m_subType;
			mutable SvUl::NameGuidList m_availableList;
			mutable SvUl::InputNameGuidPairList m_connectedList;
			mutable CComPtr<IPictureDisp> m_picture;

		public:
			ImageController(const GUID& rInspectionID, const GUID& rTaskObjectID, SVObjectSubTypeEnum subType = SVNotSetSubObjectType);
	
			HRESULT Init();

			const SvUl::NameGuidList& GetAvailableImageList() const;
			const SvUl::InputNameGuidPairList& GetConnectedImageList(const GUID& rChildObjectID = GUID_NULL, int maxImages=1) const;
			SvUl::NameGuidList GetResultImages() const;

			IPictureDisp* GetImage(const SVString& name) const;
			IPictureDisp* GetImage(const GUID& imageID) const;

			HRESULT ConnectToImage(const SVString& inputName, const SVString& name, const GUID& rInstanceID = GUID_NULL) const;
			HRESULT SaveImage(const SVString& rImageName, const SVString& rFilename);
			bool IsToolValid() const;
			HRESULT ToolRunOnce();
			
			SVObjectTypeInfoStruct GetImageTypeInfo(const GUID& imageID) const;

		private:
			HRESULT RetrieveAvailableImageList();
		};
	}
}

namespace SvOg = Seidenader::SVOGui;