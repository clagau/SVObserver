//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Markt Schwaben
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : FormulaController
//* .File Name       : $Workfile:   FormulaController.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   15 May 2014 10:06:52  $
//******************************************************************************
//Description:  FormulaController is the class to get/set and
//              validate equation objects inside SVObserver.
//			    From outside SVObserver (in the GUI), use IFormulaController.

#pragma region Includes
#include "Stdafx.h"
#include "FormulaController.h"
#include "SVInspectionProcess.h"
#include "..\SVObjectLibrary\SVObjectClass.h"
#include "SVTool.h"
#include "SVToolSet.h"
#pragma endregion

namespace Seidenader
{
	namespace SVObserver
	{
#pragma region Constructor
		FormulaController::FormulaController( SVTaskObjectClass& pObject ) 
				: m_pInspection(nullptr) 
				, m_pToolSet(nullptr)
				, m_pEquation (nullptr)
				, m_pEquationStruct (nullptr)
		{
			setTaskObject( pObject );
		}

		FormulaController::FormulaController( )
			: m_pInspection(nullptr) 
			, m_pToolSet(nullptr)
			, m_pEquation (nullptr)
			, m_pEquationStruct (nullptr)
		{
		}
#pragma endregion

#pragma region Public Methods
#pragma region virtual Methods IFormulaController
		SVString FormulaController::getEquationText() const
		{
			SVString equationText("");
			ASSERT( m_pEquationStruct );
			if( nullptr != m_pEquationStruct )
			{
				m_pEquationStruct->GetEquationText( equationText );
			}
			return equationText;
		}

		std::vector<SVString> FormulaController::getPPQVariableNames() const
		{
			std::vector<SVString> retVals;
			if ( nullptr != m_pInspection )
			{
				for( size_t i = 0; i < m_pInspection->m_PPQInputs.size(); i++ )
				{	
					SVIOEntryHostStructPtr ioEntryPtr = m_pInspection->m_PPQInputs[i].m_IOEntryPtr;
					
					//check if input is enable for this inspection
					if( ioEntryPtr->m_Enabled )
					{
						SVString name = ioEntryPtr->m_pValueObject->GetName();
						retVals.push_back( name );
					}					
				}// end for
			}
			return retVals;
		}

		SVToolSetClass* FormulaController::getToolSet() const
		{
			return m_pToolSet;
		}

		HRESULT FormulaController::isToolAndEquationEnabled(bool& toolEnabled, bool& equationEnabled) const
		{
			if ( nullptr != m_pToolSet && nullptr != m_pEquation )
			{
				toolEnabled = m_pToolSet->IsEnabled();
				equationEnabled = (TRUE == m_pEquation->IsEnabled());
				return S_OK;
			}
			
			return S_FALSE;
		}

		HRESULT FormulaController::setToolAndEquationEnabled(bool toolEnabled, bool equationEnabled)
		{
			// to nothing, because for this class is the function invalid.
			return S_FALSE;
		}

		void FormulaController::setTaskObject( SVTaskObjectClass& pObject )
		{
			m_pInspection = pObject.GetInspection();
			m_pToolSet = m_pInspection->GetToolSet();
				
			SVEquationClass* pEquation = dynamic_cast <SVEquationClass*> (&pObject);
			if ( pEquation == nullptr )
			{
				SVObjectTypeInfoStruct info;
				info.ObjectType = SVEquationObjectType;
				info.SubType    = SVMathEquationObjectType;	// we are not looking for conditional equation !!!
				pEquation = dynamic_cast < SVEquationClass* >
					( reinterpret_cast < SVObjectClass* > (::SVSendMessage( &pObject, SVM_GETFIRST_OBJECT | SVM_NOTIFY_ONLY_FRIENDS, 0,reinterpret_cast<DWORD_PTR>( &info ) )));
			}
			// Set the pointer to the Equation Class Object 
			setEquation( pEquation );
		}

		int FormulaController::validateEquation( const SVString &equationString, double& result ) const
		{
			int retValue = validateSuccessful;
			SVString oldString("");
			//save old string
			m_pEquationStruct->GetEquationText(oldString);
			m_pEquationStruct->SetEquationText(equationString);
			
			SVEquationTestResult testResult = m_pEquation->Test();
			if (testResult.bPassed)
			{// set result and set return value to successful
				result = m_pEquation->GetYACCResult();
				retValue = validateSuccessful;
			}
			else
			{  // set return value to position of failed
				retValue = testResult.iPositionFailed;
			}
			//reset old string
			m_pEquationStruct->SetEquationText(oldString);
			return retValue;
		}

		int FormulaController::validateAndSetEquation( const SVString &equationString, double& result )
		{
			int retValue = validateEquation( equationString, result);

			if ( validateSuccessful == retValue )
			{
				//set new equation text and reset all objects for using the new value
				m_pEquationStruct->SetEquationText(equationString);
				SVObjectClass* object = dynamic_cast<SVObjectClass*>(m_pEquation->GetTool());
				if( nullptr != object )
				{
					DWORD_PTR l_dwRet = ::SVSendMessage( object, SVM_RESET_ALL_OBJECTS & ~SVM_NOTIFY_FRIENDS , 0, 0); // Do not reset friends because they may be invalid.
					if( SVMR_SUCCESS != l_dwRet )
					{
						return setFailed;
					}
				}
			}
			return retValue;
		}
#pragma endregion
#pragma endregion

#pragma region Proteced Methods
		void FormulaController::setEquation( SVEquationClass* pEquation )
		{
			m_pEquation = pEquation;
			if( nullptr != m_pEquation )
			{
				m_pEquationStruct = m_pEquation->GetEquationStruct();
			}
			else
			{
				m_pEquationStruct = nullptr;
			}
		}
#pragma endregion

#pragma region Private Methods

#pragma endregion
	} //namespace SVObserver
} //namespace Seidenader

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\FormulaController.cpp_v  $
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
