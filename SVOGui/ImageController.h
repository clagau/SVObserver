//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Controller class for accessing Images via the GUI.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/noncopyable.hpp>
#include "SVUtilityLibrary\NameGuidList.h"
#include "SVUtilityLibrary\SVString.h"
#include "Definitions/SVObjectTypeInfoStruct.h"
#include "SVStatusLibrary\MessageContainer.h"
#pragma endregion Includes

namespace SvOg
{
	class ImageController : public boost::noncopyable
	{
		GUID m_InspectionID;
		GUID m_TaskObjectID;
		SVObjectSubTypeEnum m_subType;
		bool m_bAllowColor;

		mutable SvUl::NameGuidList m_availableList;
		mutable std::vector<SVString> m_specialImageList;;
		mutable SvUl::InputNameGuidPairList m_connectedList;
		mutable CComPtr<IPictureDisp> m_picture;

	public:
		ImageController(const GUID& rInspectionID, const GUID& rTaskObjectID, SVObjectSubTypeEnum subType = SVNotSetSubObjectType, bool bAllowColor = false);
	
		HRESULT Init();

		const SvUl::NameGuidList& GetAvailableImageList() const;
		const std::vector<SVString>& GetSpecialImageList() const;
		const SvUl::InputNameGuidPairList& GetConnectedImageList(const GUID& rChildObjectID = GUID_NULL, int maxImages=1) const;
		SvUl::NameGuidList GetResultImages() const;

		IPictureDisp* GetImage(const SVString& name) const;
		IPictureDisp* GetImage(const SVString& name, long& rWidth, long& rHeight) const;
		IPictureDisp* GetImage(const GUID& rImageID) const;
		IPictureDisp* GetImage(const GUID& rImageID, long& rWidth, long& rHeight) const;

		HRESULT ConnectToImage(const SVString& inputName, const SVString& name, const GUID& rInstanceID = GUID_NULL) const;
		HRESULT SaveImage(const SVString& rImageName, const SVString& rFilename);
		bool IsToolValid() const;
		HRESULT ResetTask(SvStl::MessageContainerVector& messages) const;
		HRESULT ToolRunOnce();
			
		SVObjectTypeInfoStruct GetImageTypeInfo(const GUID& imageID) const;

	private:
		HRESULT RetrieveAvailableImageList();
	};
} //namespace SvOg