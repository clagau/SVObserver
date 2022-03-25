//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteCtrlGetDataDefinitionView
//* .File Name       : $Workfile:   SVRemoteCtrlGetDataDefinitionView.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   07 Nov 2013 19:22:46  $
//******************************************************************************

using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Design;
using System.Drawing;
using System.Drawing.Design;
using System.Reflection;
using SeidenaderVision;

namespace SVRemoteTest
{
	[TypeConverter(typeof(ExpandableObjectConverter))]
	public class SVRemoteCtrlGetDataDefinitionView : ISVComObjectView
    {
        #region Member Variables
        private String m_inspectionName;
        private SVRemoteControlLib.SVDataDefinitionListTypeEnum m_listType;
        private SVDataDefinitionListProperties m_returnValue = new SVDataDefinitionListProperties();
        #endregion

        #region Properties
        [Category("Input Parameters")]
        [Description("inspectionName")]
        public String inspectionName
        {
            get { return m_inspectionName; }
            set { m_inspectionName = value; }
        }
        [Category("Input Parameters")]
        [Description("listType")]
        public SVRemoteControlLib.SVDataDefinitionListTypeEnum ListType
        {
            get { return m_listType; }
            set { m_listType = value; }
        }
        [Category("Results")]
		[DisplayName("Return Value")]
		[Description("Result of operation")]
		[TypeConverter(typeof(SVDataDefinitionTypeConverter))]
		[ReadOnly(true)]
        public SVDataDefinitionListProperties ReturnValue
		{
			get { return m_returnValue; }
        }
        #endregion

        #region Interface Implementation
        public void GetPropertyValues(List<object> items)
		{
            items.Add(m_inspectionName);
            items.Add(m_listType);
        }

		public void SetOutput(String name, object value)
		{
		}

		public void SetResult(object value)
		{
			// Copy from Com to .Net Collection
			SVRemoteControlLib.ISVDataDefObjectList entries = value as SVRemoteControlLib.ISVDataDefObjectList;
			m_returnValue = new SVDataDefinitionListProperties();

			List<SVDataDefinition> DataDefList = new List<SVDataDefinition>();

            //foreach (SVRemoteControlLib.ISVDataDefObject entry in entries)
            foreach (SVRemoteControlLib.ISVDataDefObject entry in entries)
            {
                SVDataDefinition DataDefObj = new SVDataDefinition();
                DataDefObj.Name = entry.Name;
                DataDefObj.Writable = entry.Writable;
                DataDefObj.Published = entry.Published;
                DataDefObj.Type = entry.DataType;
				foreach (object AddInfo in entry)
				{
                    DataDefObj.AdditionalInfo.Add(AddInfo.ToString());
				}
                
                DataDefList.Add(DataDefObj);
			}
            m_returnValue.DataDefinitions = DataDefList;
        }
        #endregion
    }
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\SVRemoteCtrlGetDataDefinitionView.cs_v  $
 * 
 *    Rev 1.0   07 Nov 2013 19:22:46   bwalter
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 9
 * SCR Title: Add GetDataDefinitionList Command to SVRemoteControl
 * Checked in by: bWalter; Ben Walter
 * Change Description: Initial check in.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/
