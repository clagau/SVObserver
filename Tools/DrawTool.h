//*****************************************************************************
/// \copyright (c) 2021,2021 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// This class manged the draw tool.
//******************************************************************************

#pragma once
#pragma region Includes
#include "SVTool.h"
#include "SVObjectLibrary/SVObjectLibrary.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "SVValueObjectLibrary/SVByteValueObjectClass.h"
#pragma endregion Includes

struct SVObjectLevelCreateStruct;

namespace SvTo
{

class DrawTool : public SVToolClass
{
	SV_DECLARE_CLASS

#pragma region Constructor
public:
	DrawTool(SVObjectClass*	pOwner = nullptr, int stringResourceID = IDS_CLASSNAME_DRAW_TOOL );

	virtual ~DrawTool(void);
#pragma endregion Constructor

#pragma region Public Methods
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	virtual bool DoesObjectHaveExtents() const override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	virtual bool areAuxExtentsAvailable() const override { return false; };
	virtual std::vector<std::string> getToolAdjustNameList() const override;
	virtual bool Run(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages = nullptr) override;
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	virtual bool useOverlayColorTool() const override { return false; };
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	SvOi::ITRCImagePtr onRunLocal(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages);

	/// Build the input object list for this tool.
	void BuildInputObjectList();

	/// Build the embedded object list for this tool.
	void BuildEmbeddedObjectList();

	/// Build the available task list for this tool.
	void BuildAvailableTaskList();

#pragma endregion Private Methods

#pragma region Member Variables
private:
	SvVol::SVBoolValueObjectClass m_useBGImageObject;
	bool m_useBGImage = false;
	SvVol::SVBoolValueObjectClass m_isColorImageObject;
	bool m_isColorImage = false;
	SvVol::SVBoolValueObjectClass m_autoFitSizeObject;
	bool m_useAutofit = false;
	SvVol::LinkedValue m_BackgroundImageX;
	SvVol::LinkedValue m_BackgroundImageY;
	SvVol::SVByteValueObjectClass  m_backgroundColor1Object;
	byte m_backgroundColor1 = 0;
	SvVol::SVByteValueObjectClass  m_backgroundColor2Object;
	byte m_backgroundColor2 = 0;
	SvVol::SVByteValueObjectClass  m_backgroundColor3Object;
	byte m_backgroundColor3 = 0;
	
	SvOl::InputObject m_InputBGImage;
	// Output Image - embedded
	SvIe::SVImageClass m_OutputImage;

#pragma endregion Member Variables
};

} //namespace SvTo
