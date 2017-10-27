//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDataDefinitionTypeConverter 
//* .File Name       : $Workfile:   SVDataDefinitionListTypeConverter.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   07 Nov 2013 19:22:48  $
//******************************************************************************

using System;
using System.Collections;
using System.ComponentModel;
using System.Globalization;
using System.Reflection;
using System.Text;
using System.Runtime.InteropServices;

namespace SVRemoteTest
{
	public class SVDataDefinitionTypeConverter : ExpandableObjectConverter
	{
        #region Constructor
        public SVDataDefinitionTypeConverter() 
		: base() 
		{
        }
        #endregion

        #region CanConvertTo
        public override bool CanConvertTo(ITypeDescriptorContext context, System.Type destinationType)
		{
			if (destinationType == typeof(SVDataDefinitionTypeConverter))
				return true;

			return base.CanConvertTo(context, destinationType);
		}
		#endregion

		#region ConvertTo
		public override object ConvertTo(ITypeDescriptorContext context, CultureInfo culture, object value, System.Type destinationType)
		{
			Type type = value.GetType();
			if (destinationType == typeof(System.String) )
			{
				return "Expand to see options for this item";
			}
			return base.ConvertTo(context, culture, value, destinationType);
		}
		#endregion

		#region Get Properties
		public override PropertyDescriptorCollection GetProperties(ITypeDescriptorContext context, object value, Attribute[] attributes)
		{
			PropertyDescriptorCollection properties = base.GetProperties(context, value, attributes);
			object obj = context.Instance;
			object[] parameters = { properties };
			return properties;
		}
		#endregion
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\SVDataDefinitionListTypeConverter.cs_v  $
 * 
 *    Rev 1.0   07 Nov 2013 19:22:48   bwalter
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 9
 * SCR Title: Add GetDataDefinitionList Command to SVRemoteControl
 * Checked in by: bWalter; Ben Walter
 * Change Description: Initial check in.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/
