//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
// All Rights Reserved
//*****************************************************************************

// This is the AuxiliaryExtentsController class for use by the GUI.
//******************************************************************************
#pragma once

#pragma region Includes
#include "ImageController.h"
#include "DataController.h"
#pragma endregion Includes

namespace SvOgu
{
class AuxiliaryExtentsController
{
public:
	AuxiliaryExtentsController(uint32_t inspectionID, uint32_t taskObjectID);
	virtual ~AuxiliaryExtentsController() {}
	AuxiliaryExtentsController(const AuxiliaryExtentsController&) = delete;
	AuxiliaryExtentsController& operator=(const AuxiliaryExtentsController&) = delete;
	HRESULT Init();
	HRESULT Commit();
	bool areAuxiliaryExtentsAvailable() const;
	bool isUpdateAuxExtentsEnabled() const;
	bool isShowOverlayInAncestorEnabled() const;
	void enableAuxExtents(bool bEnable);
	void enableShowOverlayInAncestor(bool bEnable);
	const SvUl::NameObjectIdList& getAvailableImageList() const;
	const SvUl::NameObjectIdList& getAvailableAncestorForOverlayImageList() const;
	std::string getAuxSourceImageName() const;
	HRESULT setAuxSourceImage(const std::string& rName);
	bool setAncestorforOverlay(const std::string& rName);
	SvUl::NameObjectIdPair getAuxSourceImage() const;
	SvUl::NameObjectIdPair getAncestorForOverlay() const;

	bool isExternalTool() const;
	bool hasUnitMappeing() const;
	void setUseUnitMapping(bool);
	bool resetTaskObject() { return m_values.resetTaskObject(); };
	HRESULT findAuxSourceImages();
private:

	HRESULT RunOnce();

private:
	const uint32_t m_InspectionID;
	const uint32_t m_TaskObjectID;
	uint32_t m_ExternalToolTaskObjectId {SvDef::InvalidObjectId};
	ImageController m_ImageController;
	ValueController m_values;

	SvUl::NameObjectIdList m_auxSourceImages;
	SvUl::NameObjectIdList m_ancestorImageList;
	std::unique_ptr<ValueController> m_pExternalToolTaskValues;
};
} //namespace SvOgu