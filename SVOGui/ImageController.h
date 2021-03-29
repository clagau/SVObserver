//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Controller class for accessing Images via the GUI.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVUtilityLibrary\NameObjectIdList.h"
#include "Definitions/StringTypeDef.h"
#include "SVProtoBuf/SVO-Enum.h"
#include "SVStatusLibrary\MessageContainer.h"
#pragma endregion Includes

namespace SvOg
{
	class ImageController
	{
		uint32_t m_InspectionID;
		uint32_t m_TaskObjectID;
		SvPb::SVObjectSubTypeEnum m_ImageSubType;
		bool m_OnlyAboveImages; //When true only returns images which are above the m_TaskObjectID

		mutable SvUl::NameObjectIdList m_availableList;
		mutable SvDef::StringVector m_specialImageList;;
		mutable SvUl::InputNameObjectIdPairList m_connectedList;
		mutable CComPtr<IPictureDisp> m_picture;

	public:
		ImageController(uint32_t inspectionID, uint32_t taskObjectID, SvPb::SVObjectSubTypeEnum ImageSubType = SvPb::SVImageMonoType, bool OnlyAboveImages = true);
		~ImageController() {}

		ImageController(const ImageController&) = delete;
		ImageController& operator=(const ImageController&) = delete;

		HRESULT Init();

		const SvUl::NameObjectIdList& GetAvailableImageList() const;
		const SvDef::StringVector& GetSpecialImageList() const;
		const SvUl::InputNameObjectIdPairList& GetInputImageList(uint32_t childObjectID = SvDef::InvalidObjectId, size_t maxImages = 0) const;
		SvUl::NameObjectIdList GetResultImages() const;

		IPictureDisp* GetImage(const std::string& name) const;
		IPictureDisp* GetImage(const std::string& name, long& rWidth, long& rHeight) const;
		IPictureDisp* GetImage(uint32_t imageID) const;
		IPictureDisp* GetImage(uint32_t imageID, long& rWidth, long& rHeight) const;

		HRESULT ConnectToImage(const std::string& inputName, const std::string& name, uint32_t instanceID = SvDef::InvalidObjectId) const;
		HRESULT SaveImage(const std::string& rImageName, const std::string& rFilename);
		bool IsToolValid() const;
		HRESULT ResetTask(SvStl::MessageContainerVector& messages) const;
		HRESULT ToolRunOnce();
			
		SvPb::SVImageTypeEnum GetImageType(uint32_t imageID) const;

	private:
		HRESULT RetrieveAvailableImageList();
	};

	/// Return the ID of the first result image. If no result image, it return SvDef::InvalidObjectId.
	/// \param rController [in]
	/// \returns uint32_t
	uint32_t getFirstResultImageId(const ImageController& rController);
} //namespace SvOg