//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for task object class
//******************************************************************************

#pragma once

#pragma region Includes
#include <boost\function.hpp>
#include "IObjectAppClass.h"
#include "ISelectorItemVector.h"
#include "SVObjectTypeInfoStruct.h"
#include "DependencyList.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		typedef boost::function<bool (const IObjectClass* pObject, unsigned int Attribute, int ArrayIndex)> IsObjectInfoAllowed;

		//this class is a interface. It should only have pure virtual public method and new member parameter
		class ITaskObject : virtual public IObjectAppClass
		{
		public:
			virtual ~ITaskObject() {}

			/**********
				The method add a input request marker to the IP.
			***********/
			virtual HRESULT AddInputRequestMarker() = 0;

			/**********
				The method start the run of the inspection (or a specified tool) once.
				/param pTool <in> Tool object which should run. If empty it use the whole inspection.
			***********/
			virtual HRESULT RunOnce(IObjectClass* pTool = nullptr) = 0;

			//************************************
			/// Return the output list of this task object, filtered by functor.
			/// /param func <in> functor to check the attribute
			/// /param Attribute <in> attribute
			/// /param WholeArray <in> flag determines if whole arrays are used
			/// \returns Seidenader::ObjectInterfaces::ISelectorItemVectorPtr
			//************************************
			virtual ISelectorItemVectorPtr GetSelectorList( IsObjectInfoAllowed func, UINT Attribute, bool WholeArray ) const = 0;

			//************************************
			/// Return the dependency list for this task object
			/// /param bImagesOnly <in> Only Images.
			/// /param nameToObjectType <in> get Name to ObjectType.
			/// \returns DependencyList
			//************************************
			virtual DependencyList GetDependents(bool bImagesOnly, SVObjectTypeEnum nameToObjectType) const = 0;

			//************************************
			/// Get the List of Images connected to this Task Object.
			/// /param rList <in> The List to be populated.
			/// /param maxEntries <in> maximum number of entries requested.
			//************************************
			virtual void GetConnectedImages(SvUl::InputNameGuidPairList& rList, int maxEntries) = 0;

			//************************************
			/// Connects the Image to the Object.
			/// \returns the status as a HRESULT
			//************************************
			virtual HRESULT ConnectToImage(const SVString& rInputName, const SVGUID& rNewID) = 0;

			//************************************
			/// Check if the object if valid.
			/// \returns the valid state as a boolean
			//************************************
			virtual bool IsObjectValid() const = 0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;