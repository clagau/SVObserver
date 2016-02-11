//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Markt Schwaben
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : FormulaController
//* .File Name       : $Workfile:   FormulaController.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   27 Aug 2014 01:22:48  $
//******************************************************************************
//Description:  FormulaController is the class to get/set and
//              validate equation objects inside SVObserver.
//			    From outside SVObserver (in the GUI), use IFormulaController.

#pragma region Includes
#include "Stdafx.h"
#include <string>
#include <boost/assign/list_of.hpp>
#include "FormulaController.h"
#include "GuiCommands/GetObjectName.h"
#include "GuiCommands/GetPPQObjectName.h"
#include "GuiCommands/GetPPQSelectorList.h"
#include "GuiCommands/GetTaskObjectInstanceID.h"
#include "GuiCommands/GetEquation.h"
#include "GuiCommands/SetEquation.h"
#include "GuiCommands/ValidateEquation.h"
#include "GuiCommands/TaskObjectGetEmbeddedValues.h"
#include "GuiCommands/TaskObjectSetEmbeddedValues.h"
#include "GuiCommands/ResetObject.h"
#include "GuiCommands/SetDefaultInputs.h"
#include "SVObjectLibrary\SVClsIds.h"
#include "ObjectSelectorLibrary\ObjectTreeGenerator.h"
#include "SVObjectLibrary\SVObjectSynchronousCommandTemplate.h"
#include "SVOGui/BoundValue.h"
#include "SVOGui/GlobalSelector.h"
#include "SVOGui/PPQSelector.h"
#include "SVOGui/ToolSetItemSelector.h"

#pragma endregion

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace Seidenader { namespace SVOGui
{
	static const std::string EnabledTag("Enabled");

	#pragma region Constructor
	FormulaController::FormulaController(const GUID& rInspectionID, const GUID& rTaskObjectID, const SVObjectTypeInfoStruct& rInfo, bool bEnabledReadOnly)
	: m_InspectionID(rInspectionID)
	, m_TaskObjectID(rTaskObjectID)
	, m_info(rInfo)
	, m_EquationID(GUID_NULL)
	, m_taskValues(SvOg::BoundValues(rInspectionID, rTaskObjectID, boost::assign::map_list_of(EnabledTag, SvOg::BoundValue(SVToolEnabledObjectGuid, bEnabledReadOnly))))
	, m_equationValues(SvOg::BoundValues(rInspectionID, rTaskObjectID, boost::assign::map_list_of(EnabledTag, SvOg::BoundValue(SVEquationEnabledObjectGuid, bEnabledReadOnly))))
	{
		init();
	}
	#pragma endregion Constructor

	#pragma region Destructor
	FormulaController::~FormulaController()
	{
	}
	#pragma endregion Destructor

	#pragma region Public Methods
	#pragma region virtual Methods IFormulaController
	SVString FormulaController::GetEquationText() const
	{
		SVString equationText;
	
		typedef GuiCmd::GetEquation Command;
		typedef SVSharedPtr<Command> CommandPtr;

		CommandPtr commandPtr = new Command(m_EquationID);
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			equationText = commandPtr->GetEquationString();
		}
		return equationText;
	}

	void FormulaController::BuildSelectableItems()
	{
		SvOsl::SelectorOptions BuildOptions( m_InspectionID, SV_SELECTABLE_FOR_EQUATION, m_InspectionID, true );
		SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<SvOg::GlobalSelector, SvOg::PPQSelector, SvOg::ToolSetItemSelector<>>( BuildOptions );
	}

	HRESULT FormulaController::IsOwnerAndEquationEnabled(bool& ownerEnabled, bool& equationEnabled) const
	{
		ownerEnabled = m_taskValues.Get<bool>(EnabledTag);
		equationEnabled = m_equationValues.Get<bool>(EnabledTag);
		return S_OK;
	}

	HRESULT FormulaController::SetOwnerAndEquationEnabled(bool ownerEnabled, bool equationEnabled)
	{
		HRESULT hr = S_OK;
		if (SVConditionalObjectType == m_info.SubType)
		{
			m_taskValues.Set<bool>(EnabledTag, ownerEnabled);
			m_equationValues.Set<bool>(EnabledTag, equationEnabled);
			m_taskValues.Commit();
			m_equationValues.Commit();
		}
		return hr;
	}

	int FormulaController::ValidateEquation( const SVString& equationString, double& result, bool bRestore ) const
	{
		int retValue = validateSuccessful;
		typedef GuiCmd::ValidateEquation Command;
		typedef SVSharedPtr<Command> CommandPtr;

		CommandPtr commandPtr(new Command(m_EquationID, equationString, bRestore));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			retValue = commandPtr->GetValidateStatus();
			result = commandPtr->GetResultValue();
		}
		return retValue;
	}

	int FormulaController::ValidateAndSetEquation( const SVString &equationString, double& result )
	{
		int retValue = ValidateEquation(equationString, result, false);

		if (validateSuccessful == retValue)
		{
			//set new equation text and reset all objects for using the new value
			typedef GuiCmd::SetEquation Command; // while SVEquationClass resides in the SVObserver project
			typedef SVSharedPtr<Command> CommandPtr;

			CommandPtr commandPtr(new Command(m_EquationID, equationString));
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
			HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
			if (S_OK != hr)
			{
				retValue = setFailed;
			}
			else
			{
				typedef GuiCmd::ResetObject ResetCommand;
				typedef SVSharedPtr<ResetCommand> ResetCommandPtr;

				ResetCommandPtr commandPtr(new ResetCommand(m_TaskObjectID, false));
				SVObjectSynchronousCommandTemplate<ResetCommandPtr> cmd(m_InspectionID, commandPtr);
				HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
				if (S_OK != hr)
				{
					retValue = setFailed; // maybe we need another value to indicate that the reset had an issue or do we ignore it?
				}
			}
		}
		return retValue;
	}

	HRESULT FormulaController::SetDefaultInputs()
	{
		typedef GuiCmd::SetDefaultInputs<SvOi::IInspectionProcess> Command;
		typedef SVSharedPtr<Command> CommandPtr;

		CommandPtr commandPtr(new Command(m_InspectionID));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		return hr;
	}

	#pragma endregion virtual Methods IFormulaController

	SVString FormulaController::GetOwnerName() const
	{
		SVString name;
		typedef GuiCmd::GetObjectName Command;
		typedef SVSharedPtr<Command> CommandPtr;

		CommandPtr commandPtr(new Command(m_TaskObjectID));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			name = commandPtr->GetName();
		}
		return name;
	}
	#pragma endregion Public Methods

	#pragma region Protected Methods
	SVString FormulaController::GetInspectionName() const
	{
		SVString inspectionName;
		typedef GuiCmd::GetObjectName Command;
		typedef SVSharedPtr<Command> CommandPtr;

		CommandPtr commandPtr(new Command(m_InspectionID));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			inspectionName = commandPtr->GetName();
		}
		return inspectionName;
	}

	SVString FormulaController::GetPPQName() const
	{
		SVString PPQName;
		typedef GuiCmd::GetPPQObjectName Command;
		typedef SVSharedPtr<Command> CommandPtr;

		CommandPtr commandPtr(new Command(m_InspectionID));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			PPQName = commandPtr->GetName();
		}
		return PPQName;
	}

	#pragma endregion Protected Methods

	void FormulaController::init()
	{
		m_taskValues.Init();
		m_equationValues.Init();

		typedef GuiCmd::GetTaskObjectInstanceID Command;
		typedef SVSharedPtr<Command> CommandPtr;
		// check for Math Container...
		if (SVMathContainerObjectType == m_info.ObjectType)
		{
			// Get the Math Container
			CommandPtr commandPtr(new Command(m_TaskObjectID, m_info));
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
			HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
			if (S_OK == hr)
			{
				// Get the Equation
				SVObjectTypeInfoStruct info(SVEquationObjectType, SVMathEquationObjectType);
				GUID containerID = commandPtr->GetInstanceID(); 
				commandPtr = CommandPtr(new Command(containerID, info));
				SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
				HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
				if (S_OK == hr)
				{
					m_EquationID = commandPtr->GetInstanceID();
				}		
			}
		}
		else
		{
			// Get the Equation
			CommandPtr commandPtr(new Command(m_TaskObjectID, m_info));
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
			HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
			if (S_OK == hr)
			{
				m_EquationID = commandPtr->GetInstanceID();
			}
		}
	}
} /* namespace SVOGui */ } /* namespace Seidenader */

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\FormulaController.cpp_v  $
 * 
 *    Rev 1.5   27 Aug 2014 01:22:48   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Fixed input variable sort problem
 * Changed methods: getPPQVariableNames
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Jul 2014 17:39:40   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed namespaces and code review changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 May 2014 10:06:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised to correct casting issues
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 May 2014 04:47:40   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  877
 * SCR Title:  Add undo-button to formula and conditional pages
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   fixed use of the constructor
 * changed order of if-comparisons 
 * removed using
 * removed const for return value of method getPPQVariableNames
 * used reference for parameter of method validateEquation and validateAndSetEquation
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 10:16:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Jan 2014 12:04:04   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  877
 * SCR Title:  Add undo-button to formula and conditional pages
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/