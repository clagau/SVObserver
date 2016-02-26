//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Markt Schwaben
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : FormulaController
//* .File Name       : $Workfile:   FormulaController.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   17 Jul 2014 17:39:42  $
//******************************************************************************
//Description:  FormulaController is the class to get/set and validate equation objects from the GUI,
//               IFormulaController is the interface.

#pragma once

#pragma region Includes
#include "ObjectInterfaces/IFormulaController.h"
#include "GuiController.h"
#include "ValuesAccessor.h"
#include "BoundValue.h"
#pragma endregion

namespace Seidenader
{
	namespace SVOGui
	{
		class FormulaController : public SvOi::IFormulaController
		{
		#pragma region Constructor
		public:
			FormulaController(const GUID& rInspectionID, const GUID& rTaskObjectId, const SVObjectTypeInfoStruct& rInfo, bool bEnabledReadOnly=true);
		#pragma endregion Constructor

		#pragma region Destructor
			virtual ~FormulaController();
		#pragma endregion Destructor

		#pragma region Public Methods
		public:
		#pragma region Virtual Methods (IFormulaController)
			virtual SVString GetInspectionName() const override;
			virtual SVString GetPPQName() const override;
			virtual SVString GetOwnerName() const override;
			virtual SVString GetEquationText() const override;

			virtual void BuildSelectableItems() override;

			virtual HRESULT IsOwnerAndEquationEnabled(bool& ownerEnabled, bool& equationEnabled) const override;
			virtual HRESULT SetOwnerAndEquationEnabled(bool ownerEnabled, bool equationEnabled) override;

			//**********
			/// Validate an equationstring
			/// \param equationString [in] the equation string.
			/// \param result [out] return the result of the equation, if the validation is successfully.
			/// \param bSetValue[in] boolean for determining whether to set (true) the new value or restore (false) to the previous equation string. if the string is invalid the previous equation string is restored in every case 
			/// \return return the position of the failure. If the validation is successful, the value will be "validateSuccessful". If the reset of the object failed the value will be "resetFailed", but the string will be set (if bSetValue == true).
			//**********
			virtual int ValidateEquation(const SVString &equationString, double& result, bool bSetValue) const override;
			virtual HRESULT SetDefaultInputs();
		#pragma endregion Virtual Methods (IFormulaController)
		#pragma endregion Public Methods

		#pragma region Member Variables
		protected:
			GUID m_InspectionID; // Instance ID of the Inspection
			GUID m_TaskObjectID; // Instance ID of the Owner (Toolset or Tool or other TaskObject)
			SVObjectTypeInfoStruct m_info;
			GUID m_EquationID; // Instance ID of the Equation

			typedef SvOg::ValuesAccessor<SvOg::BoundValues> FormulaCommand;
			typedef SvOg::GuiController<FormulaCommand, FormulaCommand::value_type> Controller;
			Controller m_taskValues;
			Controller m_equationValues;
		#pragma endregion Member Variables

		private:
			void init();
		};
	}
}

namespace SvOg = Seidenader::SVOGui;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\FormulaController.h_v  $
 * 
 *    Rev 1.2   17 Jul 2014 17:39:42   gramseier
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
 *    Rev 1.0   14 Jan 2014 12:04:06   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  877
 * SCR Title:  Add undo-button to formula and conditional pages
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/