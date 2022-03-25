//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVProductItemsTypeConverter 
//* .File Name       : $Workfile:   SVProductItemsTypeConverter.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:42:38  $
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
	public class SVProductItemsTypeConverter : ExpandableObjectConverter
	{
        #region Constructor
		public SVProductItemsTypeConverter() 
		: base() 
		{
        }
        #endregion

        #region CanConvertTo
        public override bool CanConvertTo(ITypeDescriptorContext context, System.Type destinationType)
		{
			if (destinationType == typeof(SVProductItemsTypeConverter))
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
			//return base.GetProperties(context, value, attributes).Concat(TypeDescriptor.GetConverter(typeof(SVRemoteControlLib.ProductItemsClass)).GetProperties(context, value, attributes));
		}
		#endregion
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSARCH65\PROJECTFILES\ARCHIVES\SVREMOTECONTROL\SVRemoteTest\SVProductItemsTypeConverter.cs_v  $
 * 
 *    Rev 1.0   24 Apr 2012 15:42:38   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 1
 * SCR Title: Initial check-in
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial check-in.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

