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
#include "Definitions/StringTypeDef.h"
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
		mutable SvDef::StringVector m_specialImageList;;
		mutable SvUl::InputNameGuidPairList m_connectedList;
		mutable CComPtr<IPictureDisp> m_picture;

	public:
		ImageController(const GUID& rInspectionID, const GUID& rTaskObjectID, SVObjectSubTypeEnum subType = SVNotSetSubObjectType, bool bAllowColor = false);
	
		HRESULT Init();

		const SvUl::NameGuidList& GetAvailableImageList() const;
		const SvDef::StringVector& GetSpecialImageList() const;
		const SvUl::InputNameGuidPairList& GetConnectedImageList(const GUID& rChildObjectID = GUID_NULL, int maxImages=1) const;
		SvUl::NameGuidList GetResultImages() const;

		IPictureDisp* GetImage(const std::string& name) const;
		IPictureDisp* GetImage(const std::string& name, long& rWidth, long& rHeight) const;
		IPictureDisp* GetImage(const GUID& rImageID) const;
		IPictureDisp* GetImage(const GUID& rImageID, long& rWidth, long& rHeight) const;

		HRESULT ConnectToImage(const std::string& inputName, const std::string& name, const GUID& rInstanceID = GUID_NULL) const;
		HRESULT SaveImage(const std::string& rImageName, const std::string& rFilename);
		bool IsToolValid() const;
		HRESULT ResetTask(SvStl::MessageContainerVector& messages) const;
		HRESULT ToolRunOnce();
			
		SVObjectTypeInfoStruct GetImageTypeInfo(const GUID& imageID) const;

	private:
		HRESULT RetrieveAvailableImageList();
	};
} //namespace SvOg