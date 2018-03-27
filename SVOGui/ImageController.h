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
#include "Definitions/SVImageEnums.h"
#include "InspectionCommands\GetAllowedImageList.h"
#include "SVStatusLibrary\MessageContainer.h"
#pragma endregion Includes

namespace SvOg
{
	class ImageController : public boost::noncopyable
	{
		GUID m_InspectionID;
		GUID m_TaskObjectID;
		SvDef::SVObjectSubTypeEnum m_ImageSubType;
		bool m_OnlyAboveImages; //When true only returns images which are above the m_TaskObjectID

		mutable SvUl::NameGuidList m_availableList;
		mutable SvDef::StringVector m_specialImageList;;
		mutable SvUl::InputNameGuidPairList m_connectedList;
		mutable CComPtr<IPictureDisp> m_picture;

	public:
		ImageController(const GUID& rInspectionID, const GUID& rTaskObjectID, SvDef::SVObjectSubTypeEnum ImageSubType = SvDef::SVImageMonoType, bool OnlyAboveImages = true);
	
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
			
		SvDef::SVImageTypeEnum GetImageType(const GUID& rImageID) const;

	private:
		HRESULT RetrieveAvailableImageList();
	};

	/// Return the GUID of the first result image. If no result image, it return GUID_NULL.
	/// \param rController [in]
	/// \returns SVGUID
	SVGUID getFirstResultImageId(const ImageController& rController);
} //namespace SvOg