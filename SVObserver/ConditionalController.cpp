//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Markt Schwaben
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ConditionalController
//* .File Name       : $Workfile:   ConditionalController.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   13 May 2014 04:45:08  $
//******************************************************************************
//Description:  ConditionalController is the class to get/set and
//              validate equation objects inside SVObserver.
//			    From outside SVObserver (in the GUI), use IFormulaController.

#pragma region Includes
#include "Stdafx.h"
#include "ConditionalController.h"
#include "SVInspectionProcess.h"
#include "..\SVObjectLibrary\SVObjectManagerClass.h"
#include "..\SVObjectLibrary\SVObjectClass.h"
#include "SVTool.h"
#include "SVToolSet.h"
#pragma endregion

namespace Seidenader
{
	namespace SVObserver
	{
#pragma region Constructor
		ConditionalController::ConditionalController( SVTaskObjectClass& pObject ) : FormulaController()
			, m_pTool(nullptr)
			, m_pIPDoc(nullptr)
		{
			setTaskObject( pObject );
		}

		ConditionalController::ConditionalController() : FormulaController()
			, m_pTool(nullptr)
			, m_pIPDoc(nullptr)
		{
		}
#pragma endregion

#pragma region Public Methods

#pragma region virtual Methods IFormulaController
		HRESULT ConditionalController::isToolAndEquationEnabled(bool& toolEnabled, bool& equationEnabled) const
		{
			if ( nullptr != m_pEquation )
			{
				equationEnabled = (TRUE == m_pEquation->IsEnabled());
				if ( nullptr != m_pTool )
				{
					toolEnabled = m_pTool->IsEnabled();		
					return S_OK;
				}
				else if ( nullptr != m_pToolSet )
				{
					toolEnabled = m_pToolSet->IsEnabled();
					return S_OK;
				}
			}

			return S_FALSE;
		}

		HRESULT ConditionalController::setToolAndEquationEnabled(bool toolEnabled, bool equationEnabled)
		{
			HRESULT retVal = S_FALSE;
			if ( nullptr != m_pInspection && nullptr != m_pEquation )
			{
				// Enable/Disable Tool or ToolSet
				if( nullptr != m_pTool )
				{
					retVal = m_objectValueInterface.AddInputRequest( &( m_pTool->enabled ), toolEnabled );
				}
				else if( nullptr != m_pToolSet )
				{
					retVal = m_objectValueInterface.AddInputRequest( &( m_pToolSet->enabled ), toolEnabled );
				}

				if( S_OK == retVal )
				{
					retVal = m_objectValueInterface.AddInputRequest( &( m_pEquation->enabled ), equationEnabled );
				}

				if( S_OK == retVal )
				{
					retVal = m_objectValueInterface.AddInputRequestMarker();
				}

				if( S_OK == retVal )
				{
					retVal = m_objectValueInterface.RunOnce( m_pTool );
				}
			}

			return retVal;
		}

		void ConditionalController::setTaskObject( SVTaskObjectClass& pObject )
		{
			m_objectValueInterface.SetTaskObject(&pObject);
			SVEquationClass* pCondition = nullptr;
			m_pIPDoc = nullptr;

			m_pInspection = pObject.GetInspection();
			m_pToolSet = m_pInspection->GetToolSet();
			m_pTool = dynamic_cast<SVToolClass*>(&pObject); 

			// Set the pointer to the Equation Class Object 
			SVObjectTypeInfoStruct info;
			info.ObjectType = SVEquationObjectType;
			info.SubType = SVConditionalObjectType;
			if ( nullptr != m_pTool )
			{
				pCondition = (SVEquationClass*) ::SVSendMessage( m_pTool, SVM_GETFIRST_OBJECT | SVM_NOTIFY_ONLY_FRIENDS, NULL, reinterpret_cast<LONG_PTR>( &info ) );
			}
			else
			{
				pCondition = (SVEquationClass*) ::SVSendMessage( m_pToolSet, SVM_GETFIRST_OBJECT | SVM_NOTIFY_ONLY_FRIENDS, NULL,reinterpret_cast<LONG_PTR>( &info ));
			}
			setEquation( pCondition );

			if( nullptr != m_pInspection )
			{
				m_pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( m_pInspection->GetUniqueObjectID() );
			}
		}
#pragma endregion
#pragma endregion

#pragma region Proteced Methods

#pragma endregion

#pragma region Private Methods
#pragma endregion
	} //namespace SVObserver
} //namespace Seidenader

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\ConditionalController.cpp_v  $
 * 
 *    Rev 1.2   13 May 2014 04:45:08   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  877
 * SCR Title:  Add undo-button to formula and conditional pages
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   fixed use of the constructor
 * changed order of if-comparisons 
 * removed using
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 10:16:26   tbair
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
