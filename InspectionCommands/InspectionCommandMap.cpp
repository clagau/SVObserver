//*****************************************************************************
// \copyright COPYRIGHT (c) 2020/09/04,2020/09/04 by Körber Pharma Inspection GmbH. All Rights Reserved 
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
#include "SVStatusLibrary/SVSVIMStateClass.h"


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
		return std::chrono::seconds {1};
	}

	static SvPb::InspectionCmdResponse Execute(const SvPb::InspectionCmdRequest& rRequest)
	{
		return getObjectId(rRequest.getobjectidrequest());
	}

	static DWORD ForbiddenState()
	{
		return 0;
	}

	static FktPtrs GetFunctionPtrTupel()
	{
		return std::make_tuple(GetThreadPref, GetTimout, Execute, ForbiddenState);
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
		return std::chrono::seconds {120};
	}

	static SvPb::InspectionCmdResponse Execute(const SvPb::InspectionCmdRequest& rRequest)
	{
		return InspectionRunOnce(rRequest.inspectionrunoncerequest());
	}
	static DWORD ForbiddenState()
	{
		return SV_DEFAULT_NOT_ALLOWED_STATES;
	}
	static FktPtrs GetFunctionPtrTupel()
	{
		return std::make_tuple(GetThreadPref, GetTimout, Execute, ForbiddenState);

	}

};


std::map<DWORD, FktPtrs > InspectionCommandMap =
{

	{CGetObjectId::GetMessageCase(),CGetObjectId::GetFunctionPtrTupel() },
	{CInspectionRunOnce::GetMessageCase(),CInspectionRunOnce::GetFunctionPtrTupel() },

	{SvPb::InspectionCmdRequest::kDeleteObjectRequest,
	std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  DeleteObject(rRequest.deleteobjectrequest()); },
		[](){return  (SV_DEFAULT_NOT_ALLOWED_STATES | SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION); }
	)
	},


		{SvPb::InspectionCmdRequest::kGetMessageListRequest,
	std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  GetMessageList(rRequest.getmessagelistrequest()); },
		[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
	)
	},


		{SvPb::InspectionCmdRequest::kResetObjectRequest,
	std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  ResetObject(rRequest.resetobjectrequest()); },
		[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }

	)
	},

	{SvPb::InspectionCmdRequest::kCreateModelRequest,
	std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  CreateModel(rRequest.createmodelrequest()); },
		[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
	)
	},


	{SvPb::InspectionCmdRequest::kGetObjectParametersRequest,
	std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getObjectParameters(rRequest.getobjectparametersrequest()); },
		[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
	)
	},



	{SvPb::InspectionCmdRequest::kGetEquationRequest,
	std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  GetEquation(rRequest.getequationrequest()); },
		[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
	)
	},

	{SvPb::InspectionCmdRequest::kValidateAndSetEquationRequest,
	std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  ValidateAndSetEquation(rRequest.validateandsetequationrequest()); },
		[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
	)
	},


	{SvPb::InspectionCmdRequest::kGetObjectsForMonitorListRequest,
	std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getObjectsForMonitorList(rRequest.getobjectsformonitorlistrequest()); },
		[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
	)
	},


	{SvPb::InspectionCmdRequest::kMoveObjectRequest,
	std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  MoveObject(rRequest.moveobjectrequest()); },
		[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
	)
	},


	{SvPb::InspectionCmdRequest::kTaskObjectListRequest,
	std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  GetTaskObjectsList(rRequest.taskobjectlistrequest()); },
		[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
	)
	},


	{SvPb::InspectionCmdRequest::kGetImageRequest,
	std::make_tuple(
		[] {return ThreadPref::inspection; },
		[] {return std::chrono::seconds{120}; } ,
		[](const SvPb::InspectionCmdRequest& rRequest) {return  getImage(rRequest.getimagerequest()); },
		[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
	)
	},

	{ SvPb::InspectionCmdRequest::kAreAuxiliaryExtentsAvailableRequest,
	std::make_tuple(
	[] {return ThreadPref::inspection; },
	[] {return std::chrono::seconds{120}; } ,
	[](const SvPb::InspectionCmdRequest& rRequest) {return  areAuxiliaryExtentsAvailable(rRequest.areauxiliaryextentsavailablerequest()); },
	[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
	)
	},

	{ SvPb::InspectionCmdRequest::kGetAvailableAuxImagesRequest,
	std::make_tuple(
	[] {return ThreadPref::inspection; },
	[] {return std::chrono::seconds{120}; } ,
	[](const SvPb::InspectionCmdRequest& rRequest) {return  getAvailableAuxImages(rRequest.getavailableauximagesrequest()); },
	[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
	)
	},

	{ SvPb::InspectionCmdRequest::kGetAuxImageObjectRequest,
	std::make_tuple(
	[] {return ThreadPref::inspection; },
	[] {return std::chrono::seconds{120}; } ,
	[](const SvPb::InspectionCmdRequest& rRequest) {return  getAuxImageObject(rRequest.getauximageobjectrequest()); },
	[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
	)
	},

	{ SvPb::InspectionCmdRequest::kSetAuxImageObjectRequest,
	std::make_tuple(
	[] {return ThreadPref::inspection; },
	[] {return std::chrono::seconds{120}; } ,
	[](const SvPb::InspectionCmdRequest& rRequest) {return  setAuxImageObject(rRequest.setauximageobjectrequest()); },
	[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
	)
	},

	{ SvPb::InspectionCmdRequest::kGetInputsRequest,
	std::make_tuple(
	[] {return ThreadPref::inspection; },
	[] {return std::chrono::seconds{120}; } ,
	[](const SvPb::InspectionCmdRequest& rRequest) {return  getInputs(rRequest.getinputsrequest()); },
	[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
	)
	},

	{SvPb::InspectionCmdRequest::kGetInputDataRequest,
	std::make_tuple(
	[] {return ThreadPref::inspection; },
	[] {return std::chrono::seconds{120}; } ,
	[](const SvPb::InspectionCmdRequest& rRequest) {return  getInputData(rRequest.getinputdatarequest()); },
	[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
	)
	},

	{ SvPb::InspectionCmdRequest::kConnectToObjectRequest,
	std::make_tuple(
	[] {return ThreadPref::inspection; },
	[] {return std::chrono::seconds{120}; } ,
	[](const SvPb::InspectionCmdRequest& rRequest) {return  connectToObject(rRequest.connecttoobjectrequest()); },
	[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
	)
	},

	{ SvPb::InspectionCmdRequest::kSaveImageRequest,
	std::make_tuple(
	[] {return ThreadPref::inspection; },
	[] {return std::chrono::seconds{120}; } ,
	[](const SvPb::InspectionCmdRequest& rRequest) {return  saveImage(rRequest.saveimagerequest()); },
	[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
	)
	},

	{ SvPb::InspectionCmdRequest::kSetObjectNameRequest,
	std::make_tuple(
	[] {return ThreadPref::inspection; },
	[] {return std::chrono::seconds{120}; } ,
	[](const SvPb::InspectionCmdRequest& rRequest) {return  setObjectName(rRequest.setobjectnamerequest()); },
	[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
	)
	},

	{ SvPb::InspectionCmdRequest::kGetAvailableObjectsRequest,
	std::make_tuple(
	[] {return ThreadPref::inspection; },
	[] {return std::chrono::seconds{120}; } ,
	[](const SvPb::InspectionCmdRequest& rRequest) {return  getAvailableObjects(rRequest.getavailableobjectsrequest()); },
	[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
	)
	},


	{ SvPb::InspectionCmdRequest::kGetSpecialImageListRequest,
	std::make_tuple(
	[] {return ThreadPref::inspection; },
	[] {return std::chrono::seconds{120}; } ,
	[](const SvPb::InspectionCmdRequest& rRequest) {return  getSpecialImageList(rRequest.getspecialimagelistrequest()); },
	[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
	)
	},

	{ SvPb::InspectionCmdRequest::kExportMaskRequest,
	std::make_tuple(
	[] {return ThreadPref::inspection; },
	[] {return std::chrono::seconds{120}; } ,
	[](const SvPb::InspectionCmdRequest& rRequest) {return  exportMask(rRequest.exportmaskrequest()); },
	[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
	)
	},

	{ SvPb::InspectionCmdRequest::kImportMaskRequest,
	std::make_tuple(
	[] {return ThreadPref::inspection; },
	[] {return std::chrono::seconds{120}; } ,
	[](const SvPb::InspectionCmdRequest& rRequest) {return  importMask(rRequest.importmaskrequest()); },
	[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
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
[](const SvPb::InspectionCmdRequest& rRequest) {return  createObject(rRequest.createobjectrequest()); },
	[](){ return SV_DEFAULT_NOT_ALLOWED_STATES | SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION ; }
)
},

{ SvPb::InspectionCmdRequest::kGetCreatableObjectsRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  getCreatableObjects(rRequest.getcreatableobjectsrequest()); },
	[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kShouldInspectionResetRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  shouldInspectionReset(rRequest.shouldinspectionresetrequest()); },
	[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},


{ SvPb::InspectionCmdRequest::kGetPPQNameRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  getPPQName(rRequest.getppqnamerequest()); },
	[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kGetValueObjectEnumsRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  getValueObjectEnums(rRequest.getvalueobjectenumsrequest()); },
	[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kGetEmbeddedValuesRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  getEmbeddedValues(rRequest.getembeddedvaluesrequest()); },
	[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kSetEmbeddedValuesRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  setEmbeddedValues(rRequest.setembeddedvaluesrequest()); },
	[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kValidateLinkedValueRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  validateLinkedValue(rRequest.validatelinkedvaluerequest()); },
	[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kGetOutputRectangleRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  getOutputRectangle(rRequest.getoutputrectanglerequest()); },
	[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kGetToolSizeAdjustParameterRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  getToolSizeAdjustParameter(rRequest.gettoolsizeadjustparameterrequest()); },
	[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kGetExtentParameterRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  getExtentParameter(rRequest.getextentparameterrequest()); },
	[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kSetExtentParameterRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  setExtentParameter(rRequest.setextentparameterrequest()); },
	[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kIsAllowedLocationRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  isAllowedLocation(rRequest.isallowedlocationrequest()); },
	[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kPropagateSizeAndPositionRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  propagateSizeAndPosition(rRequest.propagatesizeandpositionrequest()); },
	[](){ return DWORD(SV_DEFAULT_NOT_ALLOWED_STATES | SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION); }
)
},

{ SvPb::InspectionCmdRequest::kUsePropagateSizeAndPositionRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  usePropagateSizeAndPosition(rRequest.usepropagatesizeandpositionrequest()); },
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kGetOverlayStructRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  getOverlayStruct(rRequest.getoverlaystructrequest()); },
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kGetObjectSelectorItemsRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  getObjectSelectorItems(rRequest.getobjectselectoritemsrequest()); },
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{SvPb::InspectionCmdRequest::kGetObjectSelectorItems2Request,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  getObjectSelectorItems(rRequest.getobjectselectoritems2request()); },
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{SvPb::InspectionCmdRequest::kAddParameterAndUseItRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  addParameterAndUseIt(rRequest.addparameteranduseitrequest()); },
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{SvPb::InspectionCmdRequest::kCheckParameterNamesRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  checkParameterNames(rRequest.checkparameternamesrequest()); },
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kSetDefaultInputsRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  setDefaultInputsRequest(rRequest.setdefaultinputsrequest()); } ,
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kGetBarCodeTypeInfosRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  getBarCodeTypeInfos(rRequest.getbarcodetypeinfosrequest()); }
 ,
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},



{ SvPb::InspectionCmdRequest::kGetNormalizerValuesRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  getNormalizerValues(rRequest.getnormalizervaluesrequest()); }
 ,
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kSetNormalizerRangesRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  setNormalizerRanges(rRequest.setnormalizerrangesrequest()); } ,
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kGetImageInfoRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  getImageInfo(rRequest.getimageinforequest()); } ,
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},


{ SvPb::InspectionCmdRequest::kGetBlobAnalyzerInfoRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  getBlobAnalyzerInfo(rRequest.getblobanalyzerinforequest()); } ,
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kComputeOverscanRectRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  computeOverscanRect(rRequest.computeoverscanrectrequest()); } ,
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kGetFeaturesRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  getFeatures(rRequest.getfeaturesrequest()); } ,
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},


{ SvPb::InspectionCmdRequest::kSetFeaturesRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  setFeatures(rRequest.setfeaturesrequest()); } ,
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kGetAvailableFeaturesRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  getAvailableFeatures(rRequest.getavailablefeaturesrequest()); } ,
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kInitializeExternalToolTaskRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{ 120 }; },
[](const SvPb::InspectionCmdRequest& rRequest) {return  initializeExternalToolTask(rRequest.initializeexternaltooltaskrequest()); } ,
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kResetAllObjectsRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{ 120 }; },
[](const SvPb::InspectionCmdRequest& rRequest) {return  resetAllObjects(rRequest.resetallobjectsrequest()); } ,
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kClearDataExternalToolRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{ 120 }; },
[](const SvPb::InspectionCmdRequest& rRequest) {return  clearDataExternalTool(rRequest.cleardataexternaltoolrequest()); },
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kSetAllAttributesExternalToolRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{ 120 }; },
[](const SvPb::InspectionCmdRequest& rRequest) {return  setAllAttributesExternalTool(rRequest.setallattributesexternaltoolrequest()); },
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kGetInputValuesDefinitionExternalToolRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{ 120 }; },
[](const SvPb::InspectionCmdRequest& rRequest) {return  getInputValuesDefinitionExternalTool(rRequest.getinputvaluesdefinitionexternaltoolrequest()); },
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kGetPropTreeStateExternalToolRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{ 120 }; },
[](const SvPb::InspectionCmdRequest& rRequest) {return  getPropTreeStateExternalTool(rRequest.getproptreestateexternaltoolrequest()); },
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kSetPropTreeStateExternalToolRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{ 120 }; },
[](const SvPb::InspectionCmdRequest& rRequest) {return  setPropTreeStateExternalTool(rRequest.setproptreestateexternaltoolrequest()); },
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kValidateValueParameterExternalToolRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{ 120 }; },
[](const SvPb::InspectionCmdRequest& rRequest) {return  validateValueParameterExternalTool(rRequest.validatevalueparameterexternaltoolrequest()); },
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kGetDllMessageStringRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{ 120 }; },
[](const SvPb::InspectionCmdRequest& rRequest) {return  getDllMessageString(rRequest.getdllmessagestringrequest()); },
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kGetResultValuesDefinitionExternalToolRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{ 120 }; },
[](const SvPb::InspectionCmdRequest& rRequest) {return  getResultValuesDefinitionExternalTool(rRequest.getresultvaluesdefinitionexternaltoolrequest()); },
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kGetTableResultsExternalToolRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{ 120 }; },
[](const SvPb::InspectionCmdRequest& rRequest) {return  getTableResultsExternalTool(rRequest.gettableresultsexternaltoolrequest()); },
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kGetResultRangeObjectRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{ 120 }; },
[](const SvPb::InspectionCmdRequest& rRequest) {return  getResultRangeObject(rRequest.getresultrangeobjectrequest()); },
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kGetImageInfoExternalToolRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{ 120 }; },
[](const SvPb::InspectionCmdRequest& rRequest) {return  getImageInfoExternalTool(rRequest.getimageinfoexternaltoolrequest()); },
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kGetToolsWithReplaceableSourceImageRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  getToolsWithReplaceableSourceImage(rRequest.gettoolswithreplaceablesourceimagerequest()); },
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kSetAndSortEmbeddedValueRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  setandSortEmbeddedValues(rRequest.setandsortembeddedvaluerequest()); },
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kGetInvalidDependenciesRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{ 120 }; },
[](const SvPb::InspectionCmdRequest& rRequest) {return  getInvalidDependencies(rRequest.getinvaliddependenciesrequest()); },
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{ SvPb::InspectionCmdRequest::kGetObjectNameRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{ 120 }; },
[](const SvPb::InspectionCmdRequest& rRequest) {return  getObjectName(rRequest.getobjectnamerequest()); },
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{SvPb::InspectionCmdRequest::kGetDependencyRequest,
std::make_tuple(
	[] {return ThreadPref::async; },
	[] {return std::chrono::seconds {120}; },
	[](const SvPb::InspectionCmdRequest& rRequest) {return  getDependencyRequest(rRequest.getdependencyrequest()); },
	[](){return  (SV_DEFAULT_NOT_ALLOWED_STATES | SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION); }
)
},

{SvPb::InspectionCmdRequest::kSetObjectCommentRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  setObjectComment(rRequest.setobjectcommentrequest()); },
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

{SvPb::InspectionCmdRequest::kGetToolDepthRequest,
std::make_tuple(
[] {return ThreadPref::inspection; },
[] {return std::chrono::seconds{120}; } ,
[](const SvPb::InspectionCmdRequest& rRequest) {return  getToolDepth(rRequest.gettooldepthrequest()); },
[](){return  SV_DEFAULT_NOT_ALLOWED_STATES; }
)
},

};

}
