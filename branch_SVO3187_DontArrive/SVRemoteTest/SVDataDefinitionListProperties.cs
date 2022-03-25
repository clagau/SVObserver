//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDataDefinitionListProperties
//* .File Name       : $Workfile:   SVDataDefinitionListProperties.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   07 Nov 2013 19:22:46  $
//******************************************************************************

using System;
using System.ComponentModel;
using System.Collections;
using System.Collections.Generic;

namespace SVRemoteTest
{
	[TypeConverter(typeof(SVDataDefinitionTypeConverter))]
    public class SVDataDefinitionListProperties
	{
        private List<SVDataDefinition> m_DataDefinitions = new List<SVDataDefinition>();

        public List<SVDataDefinition> DataDefinitions
		{
            get { return m_DataDefinitions; }
            set { m_DataDefinitions = value; }
		}
    }
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\SVDataDefinitionListProperties.cs_v  $
 * 
 *    Rev 1.0   07 Nov 2013 19:22:46   bwalter
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 9
 * SCR Title: Add GetDataDefinitionList Command to SVRemoteControl
 * Checked in by: bWalter; Ben Walter
 * Change Description: Initial check in.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/
