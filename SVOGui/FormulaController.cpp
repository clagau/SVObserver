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
#include "GuiCommands/ValidateAndSetEquation.h"
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

	int FormulaController::ValidateEquation( const SVString& equationString, double& result, bool bSetValue ) const
	{
		int retValue = validateSuccessful;
		typedef GuiCmd::ValidateAndSetEquation Command;
		typedef SVSharedPtr<Command> CommandPtr;

		CommandPtr commandPtr(new Command(m_EquationID, equationString, bSetValue));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			retValue = commandPtr->GetValidateStatus();
			result = commandPtr->GetResultValue();

			if (validateSuccessful == retValue && bSetValue)
			{
				typedef GuiCmd::ResetObject ResetCommand;
				typedef SVSharedPtr<ResetCommand> ResetCommandPtr;

				ResetCommandPtr commandPtr(new ResetCommand(m_TaskObjectID, false));
				SVObjectSynchronousCommandTemplate<ResetCommandPtr> cmd(m_InspectionID, commandPtr);
				HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
				if (S_OK != hr)
				{
					retValue = resetFailed; 
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

