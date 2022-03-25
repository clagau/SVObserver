//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVComEnumConverter
//* .File Name       : $Workfile:   SVComEnumConverter.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 May 2013 17:29:00  $
//******************************************************************************

using System;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Design;
using System.Reflection;
using System.Collections;

namespace SeidenaderVision
{
    /// <summary>
    /// SVComEnumConverter supporting System.ComponentModel.DescriptionAttribute
    /// </summary>
    public class SVComEnumConverter : System.ComponentModel.EnumConverter
    {
        protected System.Type m_Val;

        /// <summary>
        /// Constructs the Type convertor for the Enum Type
        /// </summary>
        public SVComEnumConverter(System.Type type)
            : base(type)
        {
            m_Val = type;
        }

        /// <summary>
        /// Gets Enum Value's Description Attribute
        /// </summary>
        /// <param name="value">The value you want the description attribute for</param>
        /// <returns>The description, if any, else it's .ToString()</returns>
        public static string GetEnumDescription(Enum value)
        {
            string l_Retval = value.ToString();

            FieldInfo fi = value.GetType().GetField(l_Retval);
            if (fi != null)
            {
                DescriptionAttribute[] attributes = (DescriptionAttribute[])fi.GetCustomAttributes(typeof(DescriptionAttribute), false);
                if (attributes.Length > 0)
                {
                    l_Retval = attributes[0].Description;
                }
            }

            return l_Retval;
        }

        /// <summary>
        /// Gets the description for certain named value in an Enumeration
        /// </summary>
        /// <param name="value">The type of the Enumeration</param>
        /// <param name="name">The name of the Enumeration value</param>
        /// <returns>The description, if any, else the passed name</returns>
        public static string GetEnumDescription(System.Type value, string name)
        {
            string l_Retval = name;

            FieldInfo fi = value.GetField(name);
            if (fi != null)
            {
                DescriptionAttribute[] attributes = (DescriptionAttribute[])fi.GetCustomAttributes(typeof(DescriptionAttribute), false);
                if (attributes.Length > 0)
                {
                    l_Retval = attributes[0].Description;
                }
            }

            return l_Retval;
        }

        /// <summary>
        /// Gets the value of an Enum, based on it's Description Attribute or named value
        /// </summary>
        /// <param name="value">The Enum type</param>
        /// <param name="description">The description or name of the element</param>
        /// <returns>The value, or the passed in description, if it was not found</returns>
        public static object GetEnumValue(System.Type value, string description)
        {
            FieldInfo[] fis = value.GetFields();
            foreach (FieldInfo fi in fis)
            {
                DescriptionAttribute[] attributes = (DescriptionAttribute[])fi.GetCustomAttributes(typeof(DescriptionAttribute), false);
                if (attributes.Length > 0)
                {
                    if ( attributes[0].Description == description)
                    {
                        return fi.GetValue(fi.Name);
                    }
                }
                if (fi.Name == description)
                {
                    return fi.GetValue(fi.Name);
                }
            }
            return description;
        }

        /// <summary>
        /// Converts to
        /// </summary>
        /// <param name="context">The TypeDecriptorContext</param>
        /// <param name="culture">The Culture</param>
        /// <param name="value">The value to be converted</param>
        /// <param name="destinationType">The Description Type</param>
        /// <returns>The value converted to the Description Type</returns>
        public override object ConvertTo(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value, Type destinationType)
        {
            if (value is Enum && destinationType == typeof(string))
            {
                return SVComEnumConverter.GetEnumDescription((Enum)value);
            }
            if (value is string && destinationType == typeof(string))
            {
                return SVComEnumConverter.GetEnumDescription(m_Val, (string)value);
            }
            return base.ConvertTo(context, culture, value, destinationType);
        }

        /// <summary>
        /// Converts from
        /// </summary>
        /// <param name="context">The TypeDecriptorContext</param>
        /// <param name="culture">The Culture</param>
        /// <param name="value">The value to be converted</param>
        /// <returns>The value converted to the Enum Type</returns>
        public override object ConvertFrom(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value)
        {
            if (value is string)
            {
                return SVComEnumConverter.GetEnumValue(m_Val, (string)value);
            }
            if (value is Enum)
            {
                return SVComEnumConverter.GetEnumDescription((Enum)value);
            }
            return base.ConvertFrom(context, culture, value);
        }

        /// <summary>
        /// Gets whether Properties are supported or not
        /// </summary>
        /// <param name="context">The TypeDescriptorContext</param>
        /// <returns>True - Properties are supported</returns>
        public override bool GetPropertiesSupported(ITypeDescriptorContext context)
        {
            return false;
        }

        /// <summary>
        /// Gets the collection of Standard values
        /// </summary>
        /// <param name="context">The TypeDescriptorContext</param>
        /// <returns>The Collection of values</returns>
        public override TypeConverter.StandardValuesCollection GetStandardValues(ITypeDescriptorContext context)
        {
            return new TypeConverter.StandardValuesCollection(Enum.GetValues(m_Val));
        }
    }
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\SVComEnumConverter.cs_v  $
 * 
 *    Rev 1.0   14 May 2013 17:29:00   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 4
 * SCR Title: Changes for 2.01 version
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial Check-In
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/
