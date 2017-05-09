//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for task object class
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost\function.hpp>
#include "IObjectClass.h"
#include "IValueObject.h"
#include "ISelectorItemVector.h"
#include "ISVImage.h"
#include "SVInterfaceVector.h"
#include "SVObjectTypeInfoStruct.h"
#include "SVStatusLibrary\MessageContainer.h"
#include "SVUtilityLibrary\NameGuidList.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVObjectLibrary/SVOutputInfoListClass.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		typedef boost::function<bool (const IObjectClass* pObject, unsigned int Attribute, int ArrayIndex)> IsObjectInfoAllowed;

		//this class is a interface. It should only have pure virtual public method and no member variables
		class ITaskObject
		{
		public:
			virtual ~ITaskObject() {}

			//************************************
			/// Return the output list of this task object, filtered by functor.
			/// /param func <in> functor to check the attribute
			/// /param Attribute <in> attribute
			/// /param WholeArray <in> flag determines if whole arrays are used
			/// \returns Seidenader::ObjectInterfaces::ISelectorItemVectorPtr
			//************************************
			virtual ISelectorItemVectorPtr GetSelectorList( IsObjectInfoAllowed func, UINT Attribute, bool WholeArray ) const = 0;

			/// Get the List of Images connected to this Task Object.
			/// /param rList <in> The List to be populated.
			/// /param maxEntries <in> maximum number of entries requested.
			virtual void GetConnectedImages(SvUl::InputNameGuidPairList& rList, int maxEntries) = 0;

			/// Get the List of inputs (and connected object) to this Task Object.
			/// \param rList [in,out] The List to be populated.
			/// \param typeInfo [in] Type of the requested inputs. SVNotSetObjectType return all inputs.
			/// \param objectTypeToInclude [in] Object type until the name of the connected object will set. SVNotSetObjectType means only object name and e.g. SVToolSetObjectType means "Tool Set.Window Tool....". This parameter will not used for image objects.
			virtual void GetInputs(SvUl::InputNameGuidPairList& rList, const SVObjectTypeInfoStruct& typeInfo = SVObjectTypeInfoStruct(SVNotSetObjectType), SVObjectTypeEnum objectTypeToInclude = SVNotSetObjectType ) = 0;

			/// Connects an input to an object.
			/// \param rInputName [in] Name of the input.
			/// \param rNewID [in] Guid of the new object connected to the input
			/// \param objectType [in] Type of the new object (this type will be checked if it fit), if not set, it will not check and also tried to connected.
			/// \returns HRESULT
			virtual HRESULT ConnectToObject(const SVString& rInputName, const SVGUID& rNewID, SVObjectTypeEnum objectType = SVNotSetObjectType) = 0;

			//************************************
			/// Check if the object if valid.
			/// \returns the valid state as a boolean
			//************************************
			virtual bool IsObjectValid() const = 0;

			/// Gets the list of error messages happen in offline modus and will be reset if object is reset.
			/// \return a const reference to the message list
			virtual const SvStl::MessageContainerVector& getResetErrorMessages() const = 0;
			/// Gets the list of error messages in run mode. Will be reset every run.
			/// \return a const reference to the message list
			virtual const SvStl::MessageContainerVector& getRunErrorMessages() const = 0;
			/// Gets the list of all error messages (Reset and run errors)
			/// \return a const reference to the message list
			virtual SvStl::MessageContainerVector getErrorMessages() const = 0;

			/// Validate values and set them if required.
			/// \param rValueVector [in] A vector of parameter-value pairs.
			/// \param shouldSet [in] If true, value will be set if validation was successfully.
			/// \returns SvStl::MessageContainerVector A list of error messages. This list is empty if all validations and set was successfully.
			virtual SvStl::MessageContainerVector validateAndSetEmmeddedValues(const SetValueObjectPairVector& rValueVector, bool shouldSet) = 0;

			// Resolve desired inputs, called on Construction from Class Factory
			/// \param rDesiredInputs [in] List of desired inputs.
			virtual void ResolveDesiredInputs(const SvOi::SVInterfaceVector& rDesiredInputs) = 0;

			// get the first task error message (usually from validation or run once)
			// This is temporary, in the future all the TaskObject error messages will be retrieved
			virtual SvStl::MessageContainer getFirstTaskMessage() const = 0;

			/// Get a list of special images from this task. If this task has no special images this list is empty.
			/// Special images are images which are not from type SVImageClass and is not in ObjectManager and so it has no GUID. An example for special image is the model in the pattern analyzer.
			/// \param rList [in,out] A list of names of available special image of this task.
			virtual void getSpecialImageList(std::vector<SVString>& rList) const = 0;

			/// Get a special image from this task. A list of available images can be get by getSpecialImageList.
			/// Special images are images which are not from type SVImageClass and is not in ObjectManager and so it has no GUID. An example for special image is the model in the pattern analyzer.
			/// \param rName [in] The name of the special image of this task.
			/// \param rImagePtr [out] Pointer to the image.
			/// \returns bool return true if setting of this image was succeeded.
			virtual bool getSpecialImage(const SVString& rName, MatroxImageSmartHandlePtr& rImagePtr) const = 0;

			virtual HRESULT GetOutputList(SVOutputInfoListClass& p_rOutputInfoList) const = 0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;