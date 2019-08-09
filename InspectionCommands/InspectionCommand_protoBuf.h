//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file InspectionCommand_protoBuf.h
/// All Rights Reserved
//*****************************************************************************
/// This is the Command for Executing a InspectionCommand.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVProtoBuf/InspectionCommands.h"
#include "CommandFunction.h"
#pragma endregion Includes


#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{

class InspectionCommands_protoBuf
{
public:
	InspectionCommands_protoBuf(const InspectionCommands_protoBuf&) = delete;
	InspectionCommands_protoBuf& operator=(const InspectionCommands_protoBuf&) = delete;
	InspectionCommands_protoBuf(const SvPb::InspectionCmdMsgs& rRequest)
		: m_rRequest(rRequest)
	{
	};

	virtual ~InspectionCommands_protoBuf() {};

	const InspectionCmdResult& getResult() {return m_result;}

	HRESULT Execute()
	{
		m_result.m_hResult = E_FAIL;

		switch (m_rRequest.message_case())
		{
			case SvPb::InspectionCmdMsgs::kInspectionRunOnceRequest:
				m_result = InspectionRunOnce(m_rRequest.inspectionrunoncerequest());
				break;
			case SvPb::InspectionCmdMsgs::kDestroyChildRequest:
				m_result = DestroyChildObject(m_rRequest.destroychildrequest());
				break;
			case SvPb::InspectionCmdMsgs::kGetMessageListRequest:
				m_result = GetMessageList(m_rRequest.getmessagelistrequest());
				break;
			case SvPb::InspectionCmdMsgs::kResetObjectRequest:
				m_result = ResetObject(m_rRequest.resetobjectrequest());
				break;
			case SvPb::InspectionCmdMsgs::kCreateModelRequest:
				m_result = CreateModel(m_rRequest.createmodelrequest());
				break;
			case SvPb::InspectionCmdMsgs::kGetObjectParametersRequest:
				m_result = getObjectParameters(m_rRequest.getobjectparametersrequest());
				break;
			case SvPb::InspectionCmdMsgs::kGetEquationRequest:
				m_result = GetEquation(m_rRequest.getequationrequest());
				break;
			case SvPb::InspectionCmdMsgs::kValidateAndSetEquationRequest:
				m_result = ValidateAndSetEquation(m_rRequest.validateandsetequationrequest());
				break;
			case SvPb::InspectionCmdMsgs::kGetObjectsForMonitorListRequest:
				m_result = getObjectsForMonitorList(m_rRequest.getobjectsformonitorlistrequest());
				break;
			case SvPb::InspectionCmdMsgs::kMoveObjectRequest:
				m_result = MoveObject(m_rRequest.moveobjectrequest());
				break;
			case SvPb::InspectionCmdMsgs::kTaskObjectListRequest:
				m_result = GetTaskObjectsList(m_rRequest.taskobjectlistrequest());
				break;
			case SvPb::InspectionCmdMsgs::kGetImageRequest:
				m_result = getImage(m_rRequest.getimagerequest());
				break;
			case SvPb::InspectionCmdMsgs::kAreAuxiliaryExtentsAvailableRequest:
				m_result = areAuxiliaryExtentsAvailable(m_rRequest.areauxiliaryextentsavailablerequest());
				break;
			case SvPb::InspectionCmdMsgs::kGetAvailableAuxImagesRequest:
				m_result = getAvailableAuxImages(m_rRequest.getavailableauximagesrequest());
				break;
			case SvPb::InspectionCmdMsgs::kGetAuxImageObjectRequest:
				m_result = getAuxImageObject(m_rRequest.getauximageobjectrequest());
				break;
			case SvPb::InspectionCmdMsgs::kSetAuxImageObjectRequest:
				m_result = setAuxImageObject(m_rRequest.setauximageobjectrequest());
				break;
			case SvPb::InspectionCmdMsgs::kGetInputsRequest:
				m_result = getInputs(m_rRequest.getinputsrequest());
				break;
			case SvPb::InspectionCmdMsgs::kConnectToObjectRequest:
				m_result = connectToObject(m_rRequest.connecttoobjectrequest());
				break;
			case SvPb::InspectionCmdMsgs::kSaveImageRequest:
				m_result = saveImage(m_rRequest.saveimagerequest());
				break;
			case SvPb::InspectionCmdMsgs::kSetObjectNameRequest:
				m_result = setObjectName(m_rRequest.setobjectnamerequest());
				break;
			case SvPb::InspectionCmdMsgs::kGetAvailableObjectsRequest:
				m_result = getAvailableObjects(m_rRequest.getavailableobjectsrequest());
				break;
			case SvPb::InspectionCmdMsgs::kGetSpecialImageListRequest:
				m_result = getSpecialImageList(m_rRequest.getspecialimagelistrequest());
				break;
			case SvPb::InspectionCmdMsgs::kExportMaskRequest:
				m_result = exportMask(m_rRequest.exportmaskrequest());
				break;
			case SvPb::InspectionCmdMsgs::kImportMaskRequest:
				m_result = importMask(m_rRequest.importmaskrequest());
				break;
			case SvPb::InspectionCmdMsgs::kGetObjectIdRequest:
				m_result = getObjectId(m_rRequest.getobjectidrequest());
				break;
			case SvPb::InspectionCmdMsgs::kConstructAndInsertRequest:
				m_result = constructAndInsert(m_rRequest.constructandinsertrequest());
				break;
			case SvPb::InspectionCmdMsgs::kGetCreatableObjectsRequest:
				m_result = getCreatableObjects(m_rRequest.getcreatableobjectsrequest());
				break;
			case SvPb::InspectionCmdMsgs::kShouldInspectionResetRequest:
				m_result = shouldInspectionReset(m_rRequest.shouldinspectionresetrequest());
				break;
			case SvPb::InspectionCmdMsgs::kGetPPQNameRequest:
				m_result = getPPQName(m_rRequest.getppqnamerequest());
				break;
			case SvPb::InspectionCmdMsgs::kGetValueObjectEnumsRequest:
				m_result = getValueObjectEnums(m_rRequest.getvalueobjectenumsrequest());
				break;
			case SvPb::InspectionCmdMsgs::kGetEmbeddedValuesRequest:
				m_result = getEmbeddedValues(m_rRequest.getembeddedvaluesrequest());
				break;
			case SvPb::InspectionCmdMsgs::kSetEmbeddedValuesRequest:
				m_result = setEmbeddedValues(m_rRequest.setembeddedvaluesrequest());
				break;
			case SvPb::InspectionCmdMsgs::kGetOutputRectangleRequest:
				m_result = getOutputRectangle(m_rRequest.getoutputrectanglerequest());
				break;
			case SvPb::InspectionCmdMsgs::kGetToolSizeAdjustParameterRequest:
				m_result = getToolSizeAdjustParameter(m_rRequest.gettoolsizeadjustparameterrequest());
				break;
			case SvPb::InspectionCmdMsgs::kGetExtentParameterRequest:
				m_result = getExtentParameter(m_rRequest.getextentparameterrequest());
				break;
			case SvPb::InspectionCmdMsgs::kSetExtentParameterRequest:
				m_result = setExtentParameter(m_rRequest.setextentparameterrequest());
				break;
			case SvPb::InspectionCmdMsgs::kIsAllowedLocationRequest:
				m_result = isAllowedLocation(m_rRequest.isallowedlocationrequest());
				break;
			case SvPb::InspectionCmdMsgs::kPropagateSizeAndPositionRequest:
				m_result = propagateSizeAndPosition(m_rRequest.propagatesizeandpositionrequest());
				break;
			case SvPb::InspectionCmdMsgs::kUsePropagateSizeAndPositionRequest:
				m_result = usePropagateSizeAndPosition(m_rRequest.usepropagatesizeandpositionrequest());
				break;
			case SvPb::InspectionCmdMsgs::kGetOverlayStructRequest:
				m_result = getOverlayStruct(m_rRequest.getoverlaystructrequest());
				break;
			case SvPb::InspectionCmdMsgs::kGetObjectSelectorItemsRequest:
				m_result = getObjectSelectorItems(m_rRequest.getobjectselectoritemsrequest());
				break;
			default:;
		}

		return m_result.m_hResult;
	}

	bool empty() const
	{
		return true;
	}

private:
	const SvPb::InspectionCmdMsgs& m_rRequest;
	InspectionCmdResult m_result;
};
typedef std::shared_ptr<InspectionCommands_protoBuf> InspectionCommands_protoBufPtr;
} //namespace SvCmd

