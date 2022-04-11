//******************************************************************************
/// COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Interface class for task object class
//******************************************************************************
#pragma once

#pragma region Includes
#include "IValueObject.h"
#include "SVProtoBuf/SVRC.h"
#include "Definitions/SVObjectTypeInfoStruct.h"
#include "Definitions/StringTypeDef.h"
#include "SVImageBufferHandleInterface.h"
#pragma endregion Includes

#pragma region Declarations
namespace SvOi
{
class IObjectClass;
}
//! Declaration is in SVStatusLibrary\MessageContainer.h
namespace SvStl
{
class MessageContainer;
typedef std::vector<MessageContainer> MessageContainerVector;
}
#pragma endregion Declarations

namespace SvOi
{

enum class ResetTarget{ None =0 ,FromObject =1, Inspection =2   };
struct ResetParameter
{
	ResetParameter(SvPb::ResetType t) { target = t; };
	SvPb::ResetType target = SvPb::ResetType::RT_None;
	SvPb::ResetType result = SvPb::ResetType::RT_None;
};
//this class is a interface. It should only have pure virtual public method and no member variables
class ITaskObject
{
public:
	virtual ~ITaskObject() {}

	/// Get the List of inputs (and connected object) to this Task Object.
	/// \param rList [in,out] The List to be populated.
	/// \param typeInfo [in] Type of the requested inputs. SVNotSetObjectType return all inputs.
	/// \param objectTypeToInclude [in] Object type until the name of the connected object will set. SVNotSetObjectType means only object name and e.g. SVToolSetObjectType means "Tool Set.Window Tool....". This parameter will not used for image objects.
	/// \param shouldExcludeFirstObjectType [in] Remove first object name. (If objectTypeToInclude == SVNotsetObjectType this parameter will not used) e.g. SVToolSetObjectType means "Window Tool....". This parameter will not used for image objects.
	/// \param maxNumbers [in] Define after how many inputs the search stops. 0 means all.
	virtual void GetInputs(SvPb::InputDataList& rList, const SvDef::SVObjectTypeInfoStruct& typeInfo = SvDef::SVObjectTypeInfoStruct(SvPb::SVNotSetObjectType), SvPb::SVObjectTypeEnum objectTypeToInclude = SvPb::SVNotSetObjectType, bool shouldExcludeFirstObjectName = false, int maxNumbers = 0) = 0;

	/// Connects an input to an object.
	/// \param rInputName [in] Name of the input.
	/// \param newID [in] id of the new object connected to the input
	/// \param objectType [in] Type of the new object (this type will be checked if it fit), if not set, it will not check and also tried to connected.
	/// \returns HRESULT
	virtual HRESULT ConnectToObject(const std::string& rInputName, uint32_t newID) = 0;
	virtual HRESULT connectToObject(const SvPb::ConnectToObjectRequest& rConnectData) = 0;
	
	/// Gets the list of error messages happen in offline modus and will be reset if object is reset.
	/// \return a const reference to the message list
	virtual const SvStl::MessageContainerVector& getResetErrorMessages() const = 0;
	/// Gets the list of error messages in run mode. Will be reset every run.
	/// \return a const reference to the message list
	virtual const SvStl::MessageContainerVector& getRunErrorMessages() const = 0;
	/// Gets the list of all error messages (Reset and run errors)
	/// \return a const reference to the message list
	virtual SvStl::MessageContainerVector getErrorMessages() const = 0;

	/// Validate values (and defaultValues) and set them if required.
	/// \param rValueVector [in] A vector of parameter-value pairs.
	/// \param shouldSet [in] If true, value will be set if validation was successfully.
	/// \returns SvStl::MessageContainerVector A list of error messages. This list is empty if all validations and set was successfully.
	virtual SvStl::MessageContainerVector validateAndSetEmbeddedValues(const SetValueStructVector& rValueVector, bool shouldSet, SvOi::ResetParameter* pPar = nullptr) = 0;

	// Resolve desired inputs, called on Construction from Class Factory
	/// \param rDesiredInputs [in] List of desired inputs.
	virtual void ResolveDesiredInputs(const SvDef::SVObjectTypeInfoVector& rDesiredInputs) = 0;

	// get the first task error message (usually from validation or run once)
	// This is temporary, in the future all the TaskObject error messages will be retrieved
	virtual SvStl::MessageContainer getFirstTaskMessage() const = 0;

	/// Get a list of special images from this task. If this task has no special images this list is empty.
	/// Special images are images which are not from type SVImageClass and is not in ObjectManager and so it has no ID. An example for special image is the model in the pattern analyzer.
	/// \returns a list of names of available special image of this task.
	virtual SvDef::StringVector getSpecialImageList() const = 0;

	/// Get a special image from this task. A list of available images can be get by getSpecialImageList.
	/// Special images are images which are not from type SVImageClass and is not in ObjectManager and so it has no ID. An example for special image is the model in the pattern analyzer.
	/// \param rName [in] The name of the special image of this task.
	/// \param rImagePtr [out] Pointer to the image.
	/// \returns bool return true if setting of this image was succeeded.
	virtual bool getSpecialImage(const std::string& rName, SVImageBufferHandlePtr& rImagePtr) const = 0;

	/// Get a image from this task.
	/// \param embeddedId [in] The embeddedId of image of this task.
	/// \param rImagePtr [out] Pointer to the image.
	/// \returns bool return true if setting of this image was succeeded.
	virtual bool getImage(SvPb::EmbeddedIdEnum embeddedId, SVImageBufferHandlePtr& rImagePtr) const = 0;

	// Get the embedded list of the task object
	/// \returns a list of embedded IDs
	virtual std::vector<uint32_t> getEmbeddedList() const = 0;

	virtual bool isErrorMessageEmpty() const = 0;

	/// Add the object to the friend list.
	/// \param rFriendID [in] id of the object
	/// \param rAddPreid [in] The new object will be added before this object. Default: SvDef::InvalidObjectId This means: it will be added at the end.
	/// \returns bool
	virtual bool AddFriend(uint32_t friendId, uint32_t addPreId = SvDef::InvalidObjectId) = 0;

	/// Move an object in the friend-list to a position in front of another object.
	/// \param objectToMoveId [in] ID of the object to move.
	/// \param preObjectId [in] ID of the object in front of the other object should moved. If this ID is SvDef::InvalidObjectId, the other object will be moved to the end of the list.
	virtual void moveFriendObject(uint32_t objectToMoveId, uint32_t preObjectId = SvDef::InvalidObjectId) = 0;

	/// Fill the list with tools with have a replaceable source image. Will check itself and his children.
	/// \param rRequest [in,out] The list
	virtual void getToolsWithReplaceableSourceImage(SvPb::GetToolsWithReplaceableSourceImageResponse& rRequest) const = 0;

	virtual SvPb::InspectionCmdResponse setAndSortEmbeddedValues(SvPb::SetAndSortEmbeddedValueRequest request) = 0;

	virtual void connectInput(SvOi::IObjectClass* pInput) = 0;

	virtual void getInputData(const SvPb::GetInputDataRequest& request, SvPb::InspectionCmdResponse& rCmdResponse) const = 0;
};
} //namespace SvOi
