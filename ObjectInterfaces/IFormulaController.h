//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// IFormulaController is the interface to get/set and validate equations 
/// from the GUI.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVUtilityLibrary\SVString.h"
#include "SVUtilityLibrary\SVSharedPtr.h"
#include "IObjectInfoStruct.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		class IFormulaController
		{
#pragma region Public Methods
		public:
			virtual ~IFormulaController() {}

			/**********
			Get the current used equation text.
			\return SVString equation text.
			**********/
			virtual SVString GetEquationText() const = 0;

			virtual SVString GetInspectionName() const = 0;
			virtual SVString GetPPQName() const = 0;
			virtual SVString GetOwnerName() const = 0;

			//************************************
			//! Sets the Selectable items for the ObjectSelector.
			//************************************
			virtual void BuildSelectableItems() = 0;

			/**********
			Return the state of the enable flags for owner and equation.
			\param ownerEnabled [out] flag if owner(toolset/tool) is enabled.
			\param equationEnabled [out] flag if equation is enabled.
			\return HRESULT return S_OK if values a set.
			**********/
			virtual HRESULT IsOwnerAndEquationEnabled(bool& ownerEnabled, bool& equationEnabled) const = 0;

			/**********
			Set the state of the enable flags for owner and equation.
			\param ownerEnabled [in] flag if owner (toolset/tool) is enabled.
			\param equationEnabled [in] flag if equation is enabled.
			\return HRESULT return S_OK if values a set.
			**********/
			virtual HRESULT SetOwnerAndEquationEnabled(bool ownerEnabled, bool equationEnabled) = 0;

			/**********
			Validate a string and set the value if bSetValue is true.
			\param equationString [in] the equation string.
			\param result [out] return the result of the equation, if the validation is successfully.
			\param bSetValue[in] boolean if true, set value and reset object, else the old value will restored.
			\return return the position of the failure. If the validation is successful, the value will be "validateSuccessful".
			**********/
			virtual int ValidateEquation(const SVString &equationString, double& result, bool bSetValue) const = 0;

			/**********
			/// Set the Default Inputs
			**********/
			virtual HRESULT SetDefaultInputs() = 0;
#pragma endregion Public Methods

#pragma region Constants
		public:
			// Constant for the validation is successful
			static const int validateSuccessful = -1;
			// Constant for the validation is successful, but the reset of object is not
			static const int resetFailed = -2;
#pragma endregion Constants
		};
		typedef SVSharedPtr<IFormulaController> IFormulaControllerPtr;
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\IFormulaController.h_v  $
 * 
 *    Rev 1.2   17 Jul 2014 17:39:36   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed namespaces and code review changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 May 2014 04:48:44   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  877
 * SCR Title:  Add undo-button to formula and conditional pages
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   removed const for return value of method getPPQVariableNames
 * used reference for parameter of method validateEquation and validateAndSetEquation 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Jan 2014 12:04:02   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  877
 * SCR Title:  Add undo-button to formula and conditional pages
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/