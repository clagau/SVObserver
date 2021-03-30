//*****************************************************************************
// \copyright COPYRIGHT (c) 2020/09/04,2020/09/04 by Seidenader Maschinenbau GmbH. All Rights Reserved 
/// \file InspectionCommandMap.cpp
/// InspectionCommandMap is a map with all commands, which are used by InspectionCommands()
/// The single commands can be defined by an functor  or by lambda functions.
/// The map holds the information which function is called, in which context.
/// The map holds the message number and a tupel of three fuctionPointers
//******************************************************************************

#pragma region Includes 
#include "stdafx.h"
#include "InspectionCommandMap.h"
#include "SVProtoBuf/InspectionCommands.h"
#include "CommandExternalHelper.h"
#include "CommandFunction.h"


#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
namespace SvCmd
{

	class CGetObjectId
	{
	public:
		static DWORD GetMessageCase()
		{
			return SvPb::InspectionCmdRequest::kGetObjectIdRequest;
		}
		static  ThreadPref  GetThreadPref()
		{
			return ThreadPref::cur;
		};

		static  std::chrono::seconds  GetTimout()
		{
			return std::chrono::seconds{ 1 };
		}

		static SvPb::InspectionCmdResponse Execute(const SvPb::InspectionCmdRequest& rRequest)
		{
			return getObjectId(rRequest.getobjectidrequest());
		}

		static FktPtrs GetFunctionPtrTupe()
		{
			return std::make_tuple(GetThreadPref, GetTimout, Execute);
		}

	};

	class CInspectionRunOnce
	{
	public:
		static DWORD GetMessageCase()
		{
			return SvPb::InspectionCmdRequest::kInspectionRunOnceRequest;
		}
		static  ThreadPref  GetThreadPref()
		{
			return ThreadPref::inspection;
		};

		static  std::chrono::seconds  GetTimout()
		{
			return std::chrono::seconds{ 120 };
		}

		static SvPb::InspectionCmdResponse Execute(const SvPb::InspectionCmdRequest& rRequest)
		{
			return InspectionRunOnce(rRequest.inspectionrunoncerequest());
		}

		static FktPtrs GetFunctionPtrTupe()
		{
			return std::make_tuple(GetThreadPref, GetTimout, Execute);
		}

	};


	std::map<DWORD, FktPtrs > InspectionCommandMap =
	{

		{CGetObjectId::GetMessageCase(),CGetObjectId::GetFunctionPtrTupe() },
		{CInspectionRunOnce::GetMessageCase(),CInspectionRunOnce::GetFunctionPtrTupe() },

		{SvPb::InspectionCmdRequest::kDeleteObjectRequest,
		std::make_tuple(
			[] {return ThreadPref::inspection; },
			[] {return std::chrono::seconds{120}; } ,
			[](const SvPb::InspectionCmdRequest& rRequest) {return  DeleteObject(rRequest.deleteobjectrequest()); }
		)
		},

			{SvPb::InspectionCmdRequest::kGetMessageListRequest,
		std::make_tuple(
			[] {return ThreadPref::inspection; },
			[] {return std::chrono::seconds{120}; } ,
			[](const SvPb::InspectionCmdRequest& rRequest) {return  GetMessageList(rRequest.getmessagelistrequest()); }
		)
		},


			{SvPb::InspectionCmdRequest::kResetObjectRequest,
		std::make_tuple(
			[] {return ThreadPref::inspection; },
			[] {return std::chrono::seconds{120}; } ,
			[](const SvPb::InspectionCmdRequest& rRequest) {return  ResetObject(rRequest.resetobjectrequest()); }
		)
		},

		{SvPb::InspectionCmdRequest::kCreateModelRequest,
		std::make_tuple(
			[] {return ThreadPref::inspection; },
			[] {return std::chrono::seconds{120}; } ,
			[](const SvPb::InspectionCmdRequest& rRequest) {return  CreateModel(rRequest.createmodelrequest()); }
		)
		},


		{SvPb::InspectionCmdRequest::kGetObjectParametersRequest,
		std::make_tuple(
			[] {return ThreadPref::inspection; },
			[] {return std::chrono::seconds{120}; } ,
			[](const SvPb::InspectionCmdRequest& rRequest) {return  getObjectParameters(rRequest.getobjectparametersrequest()); }
		)
		},



		{SvPb::InspectionCmdRequest::kGetEquationRequest,
		std::make_tuple(
			[] {return ThreadPref::inspection; },
			[] {return std::chrono::seconds{120}; } ,
			[](const SvPb::InspectionCmdRequest& rRequest) {return  GetEquation(rRequest.getequationrequest()); }
		)
		},

		{SvPb::InspectionCmdRequest::kValidateAndSetEquationRequest,
		std::make_tuple(
			[] {return ThreadPref::inspection; },
			[] {return std::chrono::seconds{120}; } ,
			[](const SvPb::InspectionCmdRequest& rRequest) {return  ValidateAndSetEquation(rRequest.validateandsetequationrequest()); }
		)
		},


		{SvPb::InspectionCmdRequest::kGetObjectsForMonitorListRequest,
		std::make_tuple(
			[] {return ThreadPref::inspection; },
			[] {return std::chrono::seconds{120}; } ,
			[](const SvPb::InspectionCmdRequest& rRequest) {return  getObjectsForMonitorList(rRequest.getobjectsformonitorlistrequest()); }
		)
		},


		{SvPb::InspectionCmdRequest::kMoveObjectRequest,
		std::make_tuple(
			[] {return ThreadPref::inspection; },
			[] {return std::chrono::seconds{120}; } ,
			[](const SvPb::InspectionCmdRequest& rRequest) {return  MoveObject(rRequest.moveobjectrequest()); }
		)
		},


		{SvPb::InspectionCmdRequest::kTaskObjectListRequest,
		std::make_tuple(
			[] {return ThreadPref::inspection; },
			[] {return std::chrono::seconds{120}; } ,
			[](const SvPb::InspectionCmdRequest& rRequest) {return  GetTaskObjectsList(rRequest.taskobjectlistrequest()); }
		)
		},


		{SvPb::InspectionCmdRequest::kGetImageRequest,
		std::make_tuple(
			[] {return ThreadPref::inspection; },
			[] {return std::chrono::seconds{120}; } ,
			[](const SvPb::InspectionCmdRequest& rRequest) {return  getImage(rRequest.getimagerequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kAreAuxiliaryExtentsAvailableRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  areAuxiliaryExtentsAvailable(rRequest.areauxiliaryextentsavailablerequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kGetAvailableAuxImagesRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getAvailableAuxImages(rRequest.getavailableauximagesrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kGetAuxImageObjectRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getAuxImageObject(rRequest.getauximageobjectrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kSetAuxImageObjectRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  setAuxImageObject(rRequest.setauximageobjectrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kGetInputsRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getInputs(rRequest.getinputsrequest()); }
		)
		},


		{ SvPb::InspectionCmdRequest::kConnectToObjectRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  connectToObject(rRequest.connecttoobjectrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kSaveImageRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  saveImage(rRequest.saveimagerequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kSetObjectNameRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  setObjectName(rRequest.setobjectnamerequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kGetAvailableObjectsRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getAvailableObjects(rRequest.getavailableobjectsrequest()); }
		)
		},


		{ SvPb::InspectionCmdRequest::kGetSpecialImageListRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getSpecialImageList(rRequest.getspecialimagelistrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kExportMaskRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  exportMask(rRequest.exportmaskrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kImportMaskRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  importMask(rRequest.importmaskrequest()); }
		)
		},

			/*{ SvPb::InspectionCmdRequest::kGetObjectIdRequest,
			std::make_tuple(
			[] {return ThreadPref::cur;; },
			[] {return std::chrono::seconds{120}; } ,
			[](const SvPb::InspectionCmdRequest& rRequest) {return  getObjectId(rRequest.getobjectidrequest()); }
			)
			},*/

		{ SvPb::InspectionCmdRequest::kCreateObjectRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  createObject(rRequest.createobjectrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kGetCreatableObjectsRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getCreatableObjects(rRequest.getcreatableobjectsrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kShouldInspectionResetRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  shouldInspectionReset(rRequest.shouldinspectionresetrequest()); }
		)
		},


		{ SvPb::InspectionCmdRequest::kGetPPQNameRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getPPQName(rRequest.getppqnamerequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kGetValueObjectEnumsRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getValueObjectEnums(rRequest.getvalueobjectenumsrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kGetEmbeddedValuesRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getEmbeddedValues(rRequest.getembeddedvaluesrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kSetEmbeddedValuesRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  setEmbeddedValues(rRequest.setembeddedvaluesrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kGetOutputRectangleRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getOutputRectangle(rRequest.getoutputrectanglerequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kGetToolSizeAdjustParameterRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getToolSizeAdjustParameter(rRequest.gettoolsizeadjustparameterrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kGetExtentParameterRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getExtentParameter(rRequest.getextentparameterrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kSetExtentParameterRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  setExtentParameter(rRequest.setextentparameterrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kIsAllowedLocationRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  isAllowedLocation(rRequest.isallowedlocationrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kPropagateSizeAndPositionRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  propagateSizeAndPosition(rRequest.propagatesizeandpositionrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kUsePropagateSizeAndPositionRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  usePropagateSizeAndPosition(rRequest.usepropagatesizeandpositionrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kGetOverlayStructRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getOverlayStruct(rRequest.getoverlaystructrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kGetObjectSelectorItemsRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getObjectSelectorItems(rRequest.getobjectselectoritemsrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kSetDefaultInputsRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  setDefaultInputsRequest(rRequest.setdefaultinputsrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kGetBarCodeTypeInfosRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getBarCodeTypeInfos(rRequest.getbarcodetypeinfosrequest()); }
		)
		},



		{ SvPb::InspectionCmdRequest::kGetNormalizerValuesRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getNormalizerValues(rRequest.getnormalizervaluesrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kSetNormalizerRangesRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  setNormalizerRanges(rRequest.setnormalizerrangesrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kGetImageInfoRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getImageInfo(rRequest.getimageinforequest()); }
		)
		},


		{ SvPb::InspectionCmdRequest::kGetBlobAnalyzerInfoRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getBlobAnalyzerInfo(rRequest.getblobanalyzerinforequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kComputeOverscanRectRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  computeOverscanRect(rRequest.computeoverscanrectrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kGetFeaturesRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getFeatures(rRequest.getfeaturesrequest()); }
		)
		},


		{ SvPb::InspectionCmdRequest::kSetFeaturesRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  setFeatures(rRequest.setfeaturesrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kGetAvailableFeaturesRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getAvailableFeatures(rRequest.getavailablefeaturesrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kInitializeExternalToolTaskRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{ 120 }; },
		[](const SvPb::InspectionCmdRequest& rRequest) {return  initializeExternalToolTask(rRequest.initializeexternaltooltaskrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kResetAllObjectsRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{ 120 }; },
		[](const SvPb::InspectionCmdRequest& rRequest) {return  resetAllObjects(rRequest.resetallobjectsrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kClearDataExternalToolRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{ 120 }; },
		[](const SvPb::InspectionCmdRequest& rRequest) {return  clearDataExternalTool(rRequest.cleardataexternaltoolrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kSetAllAttributesExternalToolRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{ 120 }; },
		[](const SvPb::InspectionCmdRequest& rRequest) {return  setAllAttributesExternalTool(rRequest.setallattributesexternaltoolrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kGetInputValuesDefinitionExternalToolRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{ 120 }; },
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getInputValuesDefinitionExternalTool(rRequest.getinputvaluesdefinitionexternaltoolrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kGetPropTreeStateExternalToolRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{ 120 }; },
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getPropTreeStateExternalTool(rRequest.getproptreestateexternaltoolrequest()); }
		)
		},
		
		{ SvPb::InspectionCmdRequest::kSetPropTreeStateExternalToolRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{ 120 }; },
		[](const SvPb::InspectionCmdRequest& rRequest) {return  setPropTreeStateExternalTool(rRequest.setproptreestateexternaltoolrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kValidateValueParameterExternalToolRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{ 120 }; },
		[](const SvPb::InspectionCmdRequest& rRequest) {return  validateValueParameterExternalTool(rRequest.validatevalueparameterexternaltoolrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kGetDllMessageStringRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{ 120 }; },
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getDllMessageString(rRequest.getdllmessagestringrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kGetResultValuesDefinitionExternalToolRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{ 120 }; },
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getResultValuesDefinitionExternalTool(rRequest.getresultvaluesdefinitionexternaltoolrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kGetTableResultsExternalToolRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{ 120 }; },
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getTableResultsExternalTool(rRequest.gettableresultsexternaltoolrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kGetResultRangeObjectRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{ 120 }; },
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getResultRangeObject(rRequest.getresultrangeobjectrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kGetImageInfoExternalToolRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{ 120 }; },
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getImageInfoExternalTool(rRequest.getimageinfoexternaltoolrequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kGetToolsWithReplaceableSourceImageRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getToolsWithReplaceableSourceImage(rRequest.gettoolswithreplaceablesourceimagerequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kSetAndSortEmbeddedValueRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  setandSortEmbeddedValues(rRequest.setandsortembeddedvaluerequest()); }
		)
		},

		{ SvPb::InspectionCmdRequest::kGetInvalidDependenciesRequest,
		std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{ 120 }; },
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getInvalidDependencies(rRequest.getinvaliddependenciesrequest()); }
		)
		}

	};

}
