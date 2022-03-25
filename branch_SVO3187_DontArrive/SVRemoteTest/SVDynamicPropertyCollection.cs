//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDynamicPropertyCollection
//* .File Name       : $Workfile:   SVDynamicPropertyCollection.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:42:14  $
//******************************************************************************
using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using System.Drawing.Design;

namespace SeidenaderVision
{
    #region SVDynamicPropertyCollection class definition
    /// <summary>
    /// Encapsulates a collection of DynamicProperty objects.
    /// </summary>
	[TypeConverterAttribute(typeof(SVDynamicPropertyCollectionConverter)),
	DescriptionAttribute("Expand to see the options for this item.")]

    public class SVDynamicPropertyCollection : List<SVDynamicProperty>, ICustomTypeDescriptor
    {
        private List<SVDynamicProperty> propertyList;
        private string defaultProperty;

		public SVDynamicPropertyCollection() { propertyList = new List<SVDynamicProperty>(); }

        #region List attributes
        /// <summary>
        /// Gets a value indicating whether the PropertySpecCollection has a fixed size.
        /// </summary>
        /// <value>
        /// true if the PropertySpecCollection has a fixed size; otherwise, false.
        /// </value>
        public bool IsFixedSize
        {
            get { return false; }
        }

        /// <summary>
        /// Gets a value indicating whether the PropertySpecCollection is read-only.
        /// </summary>
        public bool IsReadOnly
        {
            get { return false; }
        }

        /// <summary>
        /// Gets a value indicating whether access to the collection is synchronized (thread-safe).
        /// </summary>
        /// <value>
        /// true if access to the PropertySpecCollection is synchronized (thread-safe); otherwise, false.
        /// </value>
        public bool IsSynchronized
        {
            get { return false; }
        }

        /// <summary>
        /// Searches for the DynamicProperty with the specified name and returns the zero-based index of
        /// the first occurrence within the entire PropertySpecCollection.
        /// </summary>
        /// <param name="name">The name of the DynamicProperty to locate in the PropertySpecCollection.</param>
        /// <returns>The zero-based index of the first occurrence of value within the entire PropertySpecCollection,
        /// if found; otherwise, -1.</returns>
        public int IndexOf(string name)
        {
            int i = 0;

            foreach (SVDynamicProperty spec in propertyList)
            {
                if (spec.Name == name)
                    return i;

                i++;
            }
            return -1;
        }
        #endregion

        /// <summary>
        /// Gets or sets the name of the default property in the collection.
        /// </summary>
        public string DefaultProperty
        {
            get { return defaultProperty; }
            set { defaultProperty = value; }
        }

        /// <summary>
        /// Gets the collection of properties contained within this PropertyBag.
        /// </summary>
        public List<SVDynamicProperty> Properties
        {
            get { return propertyList; }
		}

		#region SVDynamicPropertySpecEventArgs handlers
		/// <summary>
        /// Raises the GetValue event.
        /// </summary>
		/// <param name="e">A SVDynamicPropertySpecEventArgs that contains the event data.</param>
		protected virtual void OnGetValue(SVDynamicPropertySpecEventArgs e)
        {
            if (GetValue != null)
                GetValue(this, e);
        }

        /// <summary>
        /// Raises the SetValue event.
        /// </summary>
        /// <param name="e">A PropertySpecEventArgs that contains the event data.</param>
		protected virtual void OnSetValue(SVDynamicPropertySpecEventArgs e)
        {
            if (SetValue != null)
                SetValue(this, e);
        }
        #endregion

        #region PropertySpecEventHandler Delegate and Events
        /// <summary>
        /// Represents the method that will handle the GetValue and SetValue events of the
        /// PropertyBag class.
        /// </summary>
		public delegate void PropertySpecEventHandler(object sender, SVDynamicPropertySpecEventArgs e);

        /// <summary>
        /// Occurs when a PropertyGrid requests the value of a property.
        /// </summary>
        public event PropertySpecEventHandler GetValue;

        /// <summary>
        /// Occurs when the user changes the value of a property in a PropertyGrid.
        /// </summary>
        public event PropertySpecEventHandler SetValue;

        #endregion

        #region SVDynamicPropertySpecDescriptor class definition
        private class SVDynamicPropertySpecDescriptor : PropertyDescriptor
        {
			private SVDynamicPropertyCollection list;
            private SVDynamicProperty item;

			public SVDynamicPropertySpecDescriptor(SVDynamicProperty item, SVDynamicPropertyCollection collection, string name, Attribute[] attrs)
                : base(name, attrs)
            {
                this.list = collection;
                this.item = item;

				// Set some default handlers
				list.GetValue = new PropertySpecEventHandler(this.GetValueAsObject);
				list.SetValue = new PropertySpecEventHandler(this.SetValueAsString);
            }

            public override Type ComponentType
            {
                get { return item.GetType(); }
            }

            public override bool IsReadOnly
            {
                get { return (Attributes.Matches(ReadOnlyAttribute.Yes)); }
            }

            public override Type PropertyType
            {
                get { return Type.GetType(item.TypeName); }
            }

            public override bool CanResetValue(object component)
            {
                if (item.DefaultValue == null)
                    return false;
                
                return !this.GetValue(component).Equals(item.DefaultValue);
            }

            public override object GetValue(object component)
            {
                // Have the property bag raise an event to get the current value
                // of the property.

				SVDynamicPropertySpecEventArgs e = new SVDynamicPropertySpecEventArgs(item, null);
                list.OnGetValue(e);
                return e.Value;
            }

            public override void ResetValue(object component)
            {
                SetValue(component, item.DefaultValue);
            }

            public override void SetValue(object component, object value)
            {
                // Have the property bag raise an event to set the current value
                // of the property.

				SVDynamicPropertySpecEventArgs e = new SVDynamicPropertySpecEventArgs(item, value);
                list.OnSetValue(e);
            }

			// default Get/Set delegates Handlers
			private void GetValueAsObject(object sender, SVDynamicPropertySpecEventArgs e)
			{
				int index = list.Properties.FindIndex(delegate(SVDynamicProperty p) { return p.Name == e.Property.Name; });
				if (index >= 0)
				{
					if (list.Properties[index].Value != null)
						e.Value = list.Properties[index].Value;
				}
			}
			private void SetValueAsObject(object sender, SVDynamicPropertySpecEventArgs e)
			{
				int index = list.Properties.FindIndex(delegate(SVDynamicProperty p) { return p.Name == e.Property.Name; });
				if (index >= 0)
				{
					if (e.Value != null)
						list.Properties[index].Value = e.Value;
					else
						list.Properties[index].Value = null;
				}
			}
			private void GetValueAsString(object sender, SVDynamicPropertySpecEventArgs e)
			{
				int index = list.Properties.FindIndex(delegate(SVDynamicProperty p) { return p.Name == e.Property.Name; });
				if (index >= 0)
				{
					if (list.Properties[index].Value != null)
						e.Value = list.Properties[index].Value.ToString();
				}
			}
			private void SetValueAsString(object sender, SVDynamicPropertySpecEventArgs e)
			{
				int index = list.Properties.FindIndex(delegate(SVDynamicProperty p) { return p.Name == e.Property.Name; });
				if (index >= 0)
				{
					if (e.Value != null)
						list.Properties[index].Value = e.Value.ToString();
					else
						list.Properties[index].Value = String.Empty;
				}
			}

            public override bool ShouldSerializeValue(object component)
            {
                return false;
            }
        }
        #endregion

        #region ICustomTypeDescriptor explicit interface definitions
        // Most of the functions required by the ICustomTypeDescriptor are
        // merely pssed on to the default TypeDescriptor for this type,
        // which will do something appropriate.  The exceptions are noted
        // below.
        AttributeCollection ICustomTypeDescriptor.GetAttributes()
        {
            return TypeDescriptor.GetAttributes(this, true);
        }

        string ICustomTypeDescriptor.GetClassName()
        {
            return TypeDescriptor.GetClassName(this, true);
        }

        string ICustomTypeDescriptor.GetComponentName()
        {
            return TypeDescriptor.GetComponentName(this, true);
        }

        TypeConverter ICustomTypeDescriptor.GetConverter()
        {
            return TypeDescriptor.GetConverter(this, true);
        }

        EventDescriptor ICustomTypeDescriptor.GetDefaultEvent()
        {
            return TypeDescriptor.GetDefaultEvent(this, true);
        }

        PropertyDescriptor ICustomTypeDescriptor.GetDefaultProperty()
        {
            // This function searches the property list for the property
            // with the same name as the DefaultProperty specified, and
            // returns a property descriptor for it.  If no property is
            // found that matches DefaultProperty, a null reference is
            // returned instead.

            SVDynamicProperty propertySpec = null;
            if (defaultProperty != null)
            {
                int index = propertyList.FindIndex( delegate(SVDynamicProperty p){ return p.Name == defaultProperty;} );
                if (index >= 0 )
                    propertySpec = propertyList[index];
            }

            if (propertySpec != null)
				return new SVDynamicPropertySpecDescriptor(propertySpec, this, propertySpec.Name, null);
            else
                return null;
        }

        object ICustomTypeDescriptor.GetEditor(Type editorBaseType)
        {
            return TypeDescriptor.GetEditor(this, editorBaseType, true);
        }

        EventDescriptorCollection ICustomTypeDescriptor.GetEvents()
        {
            return TypeDescriptor.GetEvents(this, true);
        }

        EventDescriptorCollection ICustomTypeDescriptor.GetEvents(Attribute[] attributes)
        {
            return TypeDescriptor.GetEvents(this, attributes, true);
        }

        PropertyDescriptorCollection ICustomTypeDescriptor.GetProperties()
        {
            return ((ICustomTypeDescriptor)this).GetProperties(new Attribute[0]);
        }

        PropertyDescriptorCollection ICustomTypeDescriptor.GetProperties(Attribute[] attributes)
        {
            // Rather than passing this function on to the default TypeDescriptor,
            // which would return the actual properties of PropertyBag, construct
            // a list here that contains property descriptors for the elements of the
            // Properties list.

            ArrayList props = new ArrayList();

            foreach (SVDynamicProperty property in propertyList)
            {
                ArrayList attrs = new ArrayList();

                // If a category, description, editor, or type converter are specified
                // in the PropertySpec, create attributes to define that relationship.
                if (property.Category != null)
                    attrs.Add(new CategoryAttribute(property.Category));

                if (property.Description != null)
                    attrs.Add(new DescriptionAttribute(property.Description));

                if (property.EditorTypeName != null)
                    attrs.Add(new EditorAttribute(property.EditorTypeName, typeof(UITypeEditor)));

                if (property.ConverterTypeName != null)
                    attrs.Add(new TypeConverterAttribute(property.ConverterTypeName));

                // Additionally, append the custom attributes associated with the
                // PropertySpec, if any.
                if (property.Attributes != null)
                    attrs.AddRange(property.Attributes);

                Attribute[] attrArray = (Attribute[])attrs.ToArray(typeof(Attribute));

                // Create a new property descriptor for the property item, and add
                // it to the list.
				SVDynamicPropertySpecDescriptor pd = new SVDynamicPropertySpecDescriptor(property, this, property.Name, attrArray);
                props.Add(pd);
            }

            // Convert the list of PropertyDescriptors to a collection that the
            // ICustomTypeDescriptor can use, and return it.
            PropertyDescriptor[] propArray = (PropertyDescriptor[])props.ToArray(typeof(PropertyDescriptor));
            return new PropertyDescriptorCollection(propArray);
        }

        object ICustomTypeDescriptor.GetPropertyOwner(PropertyDescriptor pd)
        {
            return this;
        }
        #endregion
	}
	#endregion
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSARCH65\PROJECTFILES\ARCHIVES\SVREMOTECONTROL\SVRemoteTest\SVDynamicPropertyCollection.cs_v  $
 * 
 *    Rev 1.0   24 Apr 2012 15:42:14   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 1
 * SCR Title: Initial check-in
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial check-in.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/
