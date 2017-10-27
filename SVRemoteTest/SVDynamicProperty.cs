//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDynamicProperty
//* .File Name       : $Workfile:   SVDynamicProperty.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:42:12  $
//******************************************************************************
using System;
using System.ComponentModel;

namespace SeidenaderVision
{
	#region SVDynamicProperty class definition
	/// <summary>
	/// a class to represent the dynamic properties.
	/// </summary>
	public class SVDynamicProperty
	{
		private Attribute[] attributes;
		private string category;
		private object defaultValue;
		private string description;
		private string editor;
		private string name;
		private string type;
		private string typeConverter;
		private object dataValue;

		/// <summary>
		/// /// Initializes a new instance of the DynamicProperty class.
		/// </summary>
		public SVDynamicProperty() { }

		public SVDynamicProperty(SVDynamicProperty p)
		{
			this.attributes = p.Attributes;
			this.category = p.category;
			this.defaultValue = p.defaultValue;
			this.description = p.description;
			this.editor = p.editor;
			this.name = p.name;
			this.type = p.type;
			this.typeConverter = p.typeConverter;
		}
		/// <summary>
		/// Initializes a new instance of the DynamicProperty class.
		/// </summary>
		/// <param name="name">
		/// The name of the property.
		/// </param>
		/// <param name="type">
		/// The fully qualified name of the type of the property.
		/// </param>
		/// <param name="category">
		/// The category of the property (for display purposes).
		/// </param>
		/// <param name="description">
		/// The descriptive text (displayed in the help area of the property grid).
		/// </param>
		/// <param name="defaultValue">
		/// The default value of the property, or null if there is no default value.
		/// </param>
		/// <param name="editor">
		/// The fully qualified name of the type of the editor for this
		/// property.  This type must derive from UITypeEditor.</param>
		/// <param name="typeConverter">
		/// The fully qualified name of the type of the type
		/// converter for this property.  This type must derive from TypeConverter.
		/// </param>
		public SVDynamicProperty(string name, string type, string category, string description, object defaultValue,
			string editor, string typeConverter)
		{
			this.name = name;
			this.type = type;
			this.category = category;
			this.description = description;
			this.defaultValue = defaultValue;
			this.editor = editor;
			this.typeConverter = typeConverter;
		}

		/// <summary>
		/// Gets or sets a collection of additional Attributes for this property.  This can
		/// be used to specify attributes beyond those supported intrinsically by the
		/// DynamicProperty class, such as ReadOnly and Browsable.
		/// </summary>
		public Attribute[] Attributes
		{
			get { return attributes; }
			set { attributes = value; }
		}

		/// <summary>
		/// Gets or sets the category name of this property.
		/// </summary>
		public string Category
		{
			get { return category; }
			set { category = value; }
		}

		/// <summary>
		/// Gets or sets the fully qualified name of the type converter type for this property.
		/// </summary>
		public string ConverterTypeName
		{
			get { return typeConverter; }
			set { typeConverter = value; }
		}

		/// <summary>
		/// Gets or sets the default value of this property.
		/// </summary>
		public object DefaultValue
		{
			get { return defaultValue; }
			set { defaultValue = value; }
		}

		/// <summary>
		/// Gets or sets the help text description of this property.
		/// </summary>
		public string Description
		{
			get { return description; }
			set { description = value; }
		}

		/// <summary>
		/// Gets or sets the fully qualified name of the editor type for
		/// this property.
		/// </summary>
		public string EditorTypeName
		{
			get { return editor; }
			set { editor = value; }
		}

		/// <summary>
		/// Gets or sets the name of this property.
		/// </summary>
		public string Name
		{
			get { return name; }
			set { name = value; }
		}

		/// <summary>
		/// Gets or sets the fully qualfied name of the type of this property.
		/// </summary>
		public string TypeName
		{
			get { return type; }
			set { type = value; }
		}

		/// <summary>
		/// Gets or sets the data value property.
		/// </summary>
		public object Value
		{
			get { return dataValue; }
			set { dataValue = value; }
		}
	}
	#endregion
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSARCH65\PROJECTFILES\ARCHIVES\SVREMOTECONTROL\SVRemoteTest\SVDynamicProperty.cs_v  $
 * 
 *    Rev 1.0   24 Apr 2012 15:42:12   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 1
 * SCR Title: Initial check-in
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial check-in.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

