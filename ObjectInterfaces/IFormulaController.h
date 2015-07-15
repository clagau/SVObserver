//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// IFormulaController is the interface to get/set and validate equations
/// from outside SVObserver (in the GUI) to the object inside SVObserver.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVUtilityLibrary\SVString.h"
#include "IObjectClass.h"
#include "IInspectionProcess.h"
#include "IOutputInfoListClass.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		interface IFormulaController abstract
		{
#pragma region Public Methods
		public:
			/**********
			Get the current used equation text.
			\return SVString equation text.
			**********/
			virtual SVString getEquationText() const = 0;

			/**********
			Get names of all active ppqVarable for the used inspection.
			\return vector of names
			**********/
			virtual SVStringArray getPPQVariableNames() const = 0;

			/**********
			Get the tool set related to the formula
			\return point to the tool set.
			**********/
			virtual IInspectionProcess* getInspectionProcess() const = 0;

			//************************************
			//! Get the Output list of the current tool.
			//! \returns IOutputInfoListClass&
			//************************************
			virtual IOutputInfoListClass& GetToolSetOutputList( ) const = 0;

			/**********
			Return the state of the enable flags for tool and equation.
			\param toolEnabled [out] flag if tool is enabled.
			\param equationEnabled [out] flag if equation is enabled.
			\return HRESULT return S_OK if values a set.
			**********/
			virtual HRESULT isToolAndEquationEnabled(bool& toolEnabled, bool& equationEnabled) const = 0;

			/**********
			Set the state of the enable flags for tool and equation.
			\param toolEnabled [in] flag if tool is enabled.
			\param equationEnabled [in] flag if equation is enabled.
			\return HRESULT return S_OK if values a set.
			**********/
			virtual HRESULT setToolAndEquationEnabled(bool toolEnabled, bool equationEnabled) = 0;

			/**********
			Set the task object which use the equation.
			\param pObject task object.
			**********/
			virtual void setTaskObject( IObjectClass& pObject ) = 0;

			/**********
			Validate a string
			\param equationString [in] the equation string.
			\param result [out] return the result of the equation, if the validation is successfully.
			\param bRestore[in] boolean for determining whether to restore to the previous equation string.
			\return return the position of the failure. If the validation is successful, the value will be "validateSuccessful".
			**********/
			virtual int validateEquation(const SVString &equationString, double& result, bool bRestore) const = 0;
			/**********
			Validate a string and if successful it set the string to the task object..
			\param equationString [in] the equation string.
			\param result [out] return the result of the equation, if the validation is successfully.
			\return return the position of the failure. If the validation and set is successful, the value will be "validateSuccessful".
			If the validation is successful, but the set is not, the value will be "setFailed".
			**********/
			virtual int validateAndSetEquation(const SVString &equationString, double& result) = 0;
#pragma endregion Public Methods

#pragma region Constants
		public:
			// Constant for the validation is successful
			static const int validateSuccessful = -1;
			// Constant for the validation is successful, but the set is not
			static const int setFailed = -2;
#pragma endregion Constants
		};
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