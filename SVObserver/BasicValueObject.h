//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : BasicValueObject
//* .File Name       : $Workfile:   BasicValueObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Mar 2014 15:10:20  $
//* ----------------------------------------------------------------------------
//* This class is used as a value object to store different types in the 
//* object manager 
//******************************************************************************

#pragma once
#pragma region Includes
#include "SVObjectLibrary\SVObjectClass.h"
#include "SVOMFCLibrary\SVDeviceParams.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVObserver
	{
		class BasicValueObject : public SVObjectClass  
		{
		public:
		#pragma region Constructor
			/**********
			 The class constructor
			 \param ObjectName <in> the name of the object
			***********/
			BasicValueObject( LPCTSTR ObjectName );

			/**********
			 The class destructor
			***********/
			virtual ~BasicValueObject();
		#pragma endregion Constructor

		public:
		#pragma region Public Methods
			/**********
			 The method is the == operator
			 \param rRhs <in> a reference to the value to compare
			 \return True if the same
			***********/
			bool operator ==(const BasicValueObject&  rRhs) const;

			/**********
			 The method sets the value object
			 \param rValue <in> a reference of the variant to set
			 \return SOK on success
			***********/
			HRESULT setValue( const _variant_t& rValue );

			/**********
			 The method sets the value object
			 \param Value <in> a value of type string to set
			 \return SOK on success
			***********/
			HRESULT setValue( const LPCTSTR Value );

			/**********
			 The method sets the value object
			 \param Value <in> a value of type BOOL to set
			 \return SOK on success
			***********/
			HRESULT setValue( const BOOL Value );

			/**********
			 The method sets the value object
			 \param Value <in> a value of type long to set
			 \return SOK on success
			***********/
			HRESULT setValue( const long Value );

			/**********
			 The method sets the value object
			 \param Value <in> a value of type __int64 to set
			 \return SOK on success
			***********/
			HRESULT setValue( const __int64 Value );

			/**********
			 The method sets the value object
			 \param Value <in> a value of type double to set
			 \return SOK on success
			***********/
			HRESULT setValue( const double Value );

			/**********
			 The method gets the value object
			 \param rValue <out> a reference to the variant to get
			 \return SOK on success
			***********/
			HRESULT getValue( _variant_t& rValue ) const;

			/**********
			 The method gets the value object
			 \param Value <out> a value of type string to get
			 \return SOK on success
			***********/
			HRESULT getValue( CString& rValue ) const;

			/**********
			 The method gets the value object
			 \param rValue <out> a reference type bool to get
			 \return SOK on success
			***********/
			HRESULT getValue( BOOL& rValue ) const;

			/**********
			 The method gets the value object
			 \param rValue <out> a reference type long to get
			 \return SOK on success
			***********/
			HRESULT getValue( long& rValue ) const;

			/**********
			 The method gets the value object
			 \param rValue <out> a reference type __int64 to get
			 \return SOK on success
			***********/
			HRESULT getValue( __int64& rValue ) const;

			/**********
			 The method gets the value object
			 \param rValue <out> a reference type double to get
			 \return SOK on success
			***********/
			HRESULT getValue( double& rValue ) const;

			/**********
			 The method updates the corresponding device parameter
			 \param pDeviceParam <in> a reference to the parameter
			 \return SOK on success
			***********/
			HRESULT updateDeviceParameter(SVDeviceParam* pDeviceParam);
		#pragma endregion Public Methods

		private:
		#pragma region Private Methods
			/**********
			 The method creates the value object
			 \return True on success
			***********/
			virtual BOOL Create();

			/**********
			 The method destroys the value object
			 \return True on success
			***********/
			virtual BOOL Destroy();

			/**********
			 The method locks the value object for writing
			 \return True on success
			***********/
			BOOL Lock();

			/**********
			 The method unlocks the value object
			 \return True on success
			***********/
			BOOL Unlock();

			/**********
			 The method refreshes the parent object
			 \return True on success
			***********/
			BOOL RefreshOwner() const;
	#pragma endregion Private Methods

		private:
		#pragma region Member Variables
			_variant_t			m_Value;					//The value object container
			CRITICAL_SECTION	m_CriticalSection;			//The crtical section object
			BOOL				m_Created;					//Object is created
		#pragma endregion Member Variables
		};
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\BasicValueObject.h_v  $
 * 
 *    Rev 1.0   17 Mar 2014 15:10:20   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
