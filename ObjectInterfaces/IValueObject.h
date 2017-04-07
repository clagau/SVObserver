//******************************************************************************
//! COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
//! All Rights Reserved
//******************************************************************************
//! Interface class for the SVValueObject class
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <utility>
//Moved to precompiled header: #include <comdef.h>
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{
		enum SVResetItemEnum //! used with Embedded Object registration.
		{
			//!  Used in cases where changing the Tool value can have an impact on 
			//! 	dependent Tools. In most cases this would be overkill.	
			SVResetItemIP = 0,

			//!  Generally the default.  
			SVResetItemTool = 1,

			//!  An optimization IF you are editing a sub-task object (friend or child)
			//!  and you can get away with only parent task for some duration.  Usually  
			//!  requires "manual" Tool level reset when accepting sub-task values. 
			SVResetItemOwner = 2,

			//	An SVResetItemNone example are Tool Results, which do not require reseting the 
			//!  Tool, Owner, or IP when their values change.
			SVResetItemNone = 3,
			SVResetItemSize,
		};

		class IValueObject
		{
		public:
			virtual ~IValueObject() {}

			//! Gets the default value for the Value object
			//! \returns the default value as a variant
			virtual _variant_t getDefaultValue() const = 0;

			//! Sets the value for Value object
			//! \param rValue [in] The value to set the Value object to
			//! \param Bucket [in] The corresponding bucket index to write to, if required
			//! \param Index [in] The corresponding array index to write to, if required
			//! \returns S_OK if succeeded
			virtual HRESULT setValue( const _variant_t& rValue, int Bucket = -1, int Index = -1 ) = 0;

			//! Gets the value for Value object
			//! \param rValue [out] The reference to write the value to
			//! \param Bucket [in] The corresponding bucket index to get, if required
			//! \param Index [in] The corresponding array index to write to, if required
			//! \returns S_OK if succeeded
			virtual HRESULT getValue( _variant_t& rValue, int Bucket = -1, int Index = -1 ) const = 0;

			//! Gets the values for Value object
			//! \param rValue [out] The reference to _variant_t vector to store the values
			//! \param Bucket [in] The corresponding bucket index to get, if required
			//! \returns S_OK if succeeded
			virtual HRESULT getValues( std::vector<_variant_t>& rValues, int Bucket = -1 ) const = 0;

			//! Sets the value for Value object
			//! \param rValue [in] The value to set the Value object to
			//! \param Bucket [in] The corresponding bucket index to write to, if required
			//! \param Index [in] The corresponding array index to write to, if required
			//! \returns S_OK if succeeded
			virtual HRESULT setValue( const SVString& rValue, int Bucket = -1, int Index = -1 ) = 0;

			//! Gets the value for Value object
			//! \param rValue [out] The reference to write the value to
			//! \param Bucket [in] The corresponding bucket index to get, if required
			//! \param Index [in] The corresponding array index to write to, if required
			//! \returns S_OK if succeeded
			virtual HRESULT getValue( SVString& rValue, int Bucket = -1, int Index = -1 ) const = 0;

			//! Set the value object bucketized flag
			//! \param isBucketized [in] true if bucketized
			virtual void setBucketized( bool isBucketized ) = 0;

			//! Set the reset options for the value object
			//! \param bResetAlways [in] 
			//! \param eResetItem [in] Which item to reset
			virtual void setResetOptions( bool bResetAlways, SVResetItemEnum eResetItem ) = 0;

			//! Validate the value. If value invalid an exception message will be thrown.
			//! \param rValue [in] The value to validate
			virtual void validateValue( const _variant_t& rValue ) const = 0;

			//! Gets the type name for the Value object
			//! \returns the type name
			virtual SVString getTypeName() const = 0;

			//! Checks if the value object is an array
			//! \returns true if an array
			virtual bool isArray() const = 0;

			//! Gets the size of the value object array
			//! \returns size 0 if not an array
			virtual int getArraySize() const = 0;

			//! Gets the result size of the value object
			//! \returns size
			virtual int getResultSize( int Bucket = -1 ) const = 0;

			//! Gets the reset item type
			//! \returns the reset item enum type
			virtual SVResetItemEnum getResetItem() const = 0;

			//! Gets the reset always value object flag
			//! \returns true if reset always
			virtual bool ResetAlways() const = 0;

			//! Copies the last set value to the destination bucket
			//! \returns the result of copying
			virtual HRESULT CopyLastSetValue( int DestBucket ) = 0;
		};

		typedef std::set<IValueObject*> IValueObjectPtrSet;
		typedef std::pair<IValueObject*, _variant_t> SetValueObjectPair;
		typedef std::vector<SetValueObjectPair> SetValueObjectPairVector;
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;