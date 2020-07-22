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
#pragma endregion Declarations

namespace SvCmd
{

class InspectionCommands_protoBuf
{
public:
	InspectionCommands_protoBuf(const InspectionCommands_protoBuf&) = delete;
	InspectionCommands_protoBuf& operator=(const InspectionCommands_protoBuf&) = delete;
	explicit InspectionCommands_protoBuf(const SvPb::InspectionCmdRequest& rRequest)
		: m_rRequest(rRequest)
	{
	};

	virtual ~InspectionCommands_protoBuf() {};

	const SvPb::InspectionCmdResponse& getResponse() { return m_response; }

	HRESULT Execute()
	{
		m_response.set_hresult(E_FAIL);

		switch (m_rRequest.message_case())
		{
			case SvPb::InspectionCmdRequest::kInspectionRunOnceRequest:
				m_response = InspectionRunOnce(m_rRequest.inspectionrunoncerequest());
				break;
			case SvPb::InspectionCmdRequest::kDeleteObjectRequest:
				m_response = DeleteObject(m_rRequest.deleteobjectrequest());
				break;
			case SvPb::InspectionCmdRequest::kGetMessageListRequest:
				m_response = GetMessageList(m_rRequest.getmessagelistrequest());
				break;
			case SvPb::InspectionCmdRequest::kResetObjectRequest:
				m_response = ResetObject(m_rRequest.resetobjectrequest());
				break;
			case SvPb::InspectionCmdRequest::kCreateModelRequest:
				m_response = CreateModel(m_rRequest.createmodelrequest());
				break;
			case SvPb::InspectionCmdRequest::kGetObjectParametersRequest:
				m_response = getObjectParameters(m_rRequest.getobjectparametersrequest());
				break;
			case SvPb::InspectionCmdRequest::kGetEquationRequest:
				m_response = GetEquation(m_rRequest.getequationrequest());
				break;
			case SvPb::InspectionCmdRequest::kValidateAndSetEquationRequest:
				m_response = ValidateAndSetEquation(m_rRequest.validateandsetequationrequest());
				break;
			case SvPb::InspectionCmdRequest::kGetObjectsForMonitorListRequest:
				m_response = getObjectsForMonitorList(m_rRequest.getobjectsformonitorlistrequest());
				break;
			case SvPb::InspectionCmdRequest::kMoveObjectRequest:
				m_response = MoveObject(m_rRequest.moveobjectrequest());
				break;
			case SvPb::InspectionCmdRequest::kTaskObjectListRequest:
				m_response = GetTaskObjectsList(m_rRequest.taskobjectlistrequest());
				break;
			case SvPb::InspectionCmdRequest::kGetImageRequest:
				m_response = getImage(m_rRequest.getimagerequest());
				break;
			case SvPb::InspectionCmdRequest::kAreAuxiliaryExtentsAvailableRequest:
				m_response = areAuxiliaryExtentsAvailable(m_rRequest.areauxiliaryextentsavailablerequest());
				break;
			case SvPb::InspectionCmdRequest::kGetAvailableAuxImagesRequest:
				m_response = getAvailableAuxImages(m_rRequest.getavailableauximagesrequest());
				break;
			case SvPb::InspectionCmdRequest::kGetAuxImageObjectRequest:
				m_response = getAuxImageObject(m_rRequest.getauximageobjectrequest());
				break;
			case SvPb::InspectionCmdRequest::kSetAuxImageObjectRequest:
				m_response = setAuxImageObject(m_rRequest.setauximageobjectrequest());
				break;
			case SvPb::InspectionCmdRequest::kGetInputsRequest:
				m_response = getInputs(m_rRequest.getinputsrequest());
				break;
			case SvPb::InspectionCmdRequest::kConnectToObjectRequest:
				m_response = connectToObject(m_rRequest.connecttoobjectrequest());
				break;
			case SvPb::InspectionCmdRequest::kSaveImageRequest:
				m_response = saveImage(m_rRequest.saveimagerequest());
				break;
			case SvPb::InspectionCmdRequest::kSetObjectNameRequest:
				m_response = setObjectName(m_rRequest.setobjectnamerequest());
				break;
			case SvPb::InspectionCmdRequest::kGetAvailableObjectsRequest:
				m_response = getAvailableObjects(m_rRequest.getavailableobjectsrequest());
				break;
			case SvPb::InspectionCmdRequest::kGetSpecialImageListRequest:
				m_response = getSpecialImageList(m_rRequest.getspecialimagelistrequest());
				break;
			case SvPb::InspectionCmdRequest::kExportMaskRequest:
				m_response = exportMask(m_rRequest.exportmaskrequest());
				break;
			case SvPb::InspectionCmdRequest::kImportMaskRequest:
				m_response = importMask(m_rRequest.importmaskrequest());
				break;
			case SvPb::InspectionCmdRequest::kGetObjectIdRequest:
				m_response = getObjectId(m_rRequest.getobjectidrequest());
				break;
			case SvPb::InspectionCmdRequest::kCreateObjectRequest:
				m_response = createObject(m_rRequest.createobjectrequest());
				break;
			case SvPb::InspectionCmdRequest::kGetCreatableObjectsRequest:
				m_response = getCreatableObjects(m_rRequest.getcreatableobjectsrequest());
				break;
			case SvPb::InspectionCmdRequest::kShouldInspectionResetRequest:
				m_response = shouldInspectionReset(m_rRequest.shouldinspectionresetrequest());
				break;
			case SvPb::InspectionCmdRequest::kGetPPQNameRequest:
				m_response = getPPQName(m_rRequest.getppqnamerequest());
				break;
			case SvPb::InspectionCmdRequest::kGetValueObjectEnumsRequest:
				m_response = getValueObjectEnums(m_rRequest.getvalueobjectenumsrequest());
				break;
			case SvPb::InspectionCmdRequest::kGetEmbeddedValuesRequest:
				m_response = getEmbeddedValues(m_rRequest.getembeddedvaluesrequest());
				break;
			case SvPb::InspectionCmdRequest::kSetEmbeddedValuesRequest:
				m_response = setEmbeddedValues(m_rRequest.setembeddedvaluesrequest());
				break;
			case SvPb::InspectionCmdRequest::kGetOutputRectangleRequest:
				m_response = getOutputRectangle(m_rRequest.getoutputrectanglerequest());
				break;
			case SvPb::InspectionCmdRequest::kGetToolSizeAdjustParameterRequest:
				m_response = getToolSizeAdjustParameter(m_rRequest.gettoolsizeadjustparameterrequest());
				break;
			case SvPb::InspectionCmdRequest::kGetExtentParameterRequest:
				m_response = getExtentParameter(m_rRequest.getextentparameterrequest());
				break;
			case SvPb::InspectionCmdRequest::kSetExtentParameterRequest:
				m_response = setExtentParameter(m_rRequest.setextentparameterrequest());
				break;
			case SvPb::InspectionCmdRequest::kIsAllowedLocationRequest:
				m_response = isAllowedLocation(m_rRequest.isallowedlocationrequest());
				break;
			case SvPb::InspectionCmdRequest::kPropagateSizeAndPositionRequest:
				m_response = propagateSizeAndPosition(m_rRequest.propagatesizeandpositionrequest());
				break;
			case SvPb::InspectionCmdRequest::kUsePropagateSizeAndPositionRequest:
				m_response = usePropagateSizeAndPosition(m_rRequest.usepropagatesizeandpositionrequest());
				break;
			case SvPb::InspectionCmdRequest::kGetOverlayStructRequest:
				m_response = getOverlayStruct(m_rRequest.getoverlaystructrequest());
				break;
			case SvPb::InspectionCmdRequest::kGetObjectSelectorItemsRequest:
				m_response = getObjectSelectorItems(m_rRequest.getobjectselectoritemsrequest());
				break;
			case SvPb::InspectionCmdRequest::kSetDefaultInputsRequest:
				m_response = setDefaultInputsRequest(m_rRequest.setdefaultinputsrequest());
				break;
			case SvPb::InspectionCmdRequest::kGetBarCodeTypeInfosRequest:
				m_response = getBarCodeTypeInfos(m_rRequest.getbarcodetypeinfosrequest());
				break;
			case SvPb::InspectionCmdRequest::kGetNormalizerValuesRequest:
				m_response = getNormalizerValues(m_rRequest.getnormalizervaluesrequest());
				break;
			case SvPb::InspectionCmdRequest::kSetNormalizerRangesRequest:
				m_response = setNormalizerRanges(m_rRequest.setnormalizerrangesrequest());
				break;
			case SvPb::InspectionCmdRequest::kGetImageInfoRequest:
				m_response = getImageInfo(m_rRequest.getimageinforequest());
				break;
			case SvPb::InspectionCmdRequest::kGetBlobAnalyzerInfoRequest:
				m_response = getBlobAnalyzerInfo(m_rRequest.getblobanalyzerinforequest());
				break;
			case SvPb::InspectionCmdRequest::kComputeOverscanRectRequest:
				m_response = computeOverscanRect(m_rRequest.computeoverscanrectrequest());
				break;
			default:;
		}

		return m_response.hresult();
	}

	bool empty() const
	{
		return true;
	}

private:
	const SvPb::InspectionCmdRequest& m_rRequest;
	SvPb::InspectionCmdResponse m_response;
};
typedef std::shared_ptr<InspectionCommands_protoBuf> InspectionCommands_protoBufPtr;
} //namespace SvCmd

