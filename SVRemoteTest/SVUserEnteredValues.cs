//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUserEnteredValues
//* .File Name       : $Workfile:   SVUserEnteredValues.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   28 Jan 2015 13:18:24  $
//******************************************************************************

using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Text;
using System.Xml;
using System.ComponentModel;
using System.Reflection;
using SeidenaderVision;

namespace SVRemoteTest
{
    public class SVUserEnteredValues
    {
        #region Member Variables
        private Dictionary<string, SVDynamicPropertyCollection> remoteInputParameters = new Dictionary<string, SVDynamicPropertyCollection>();
        private Dictionary<string, SVDynamicPropertyCollection> failStatusStreamInputParameters = new Dictionary<string, SVDynamicPropertyCollection>();
        private Dictionary<string, SVDynamicPropertyCollection> inputBrokerInputParameters = new Dictionary<string, SVDynamicPropertyCollection>();
        private Dictionary<string, SVDynamicPropertyCollection> outputBrokerInputParameters = new Dictionary<string, SVDynamicPropertyCollection>();
        #endregion

        #region GetRemoteInputProperties
        public SVDynamicPropertyCollection GetRemoteInputProperties(string methodName)
        {
            if (remoteInputParameters.ContainsKey(methodName))
            {
                return remoteInputParameters[methodName];
            }
            return null;
        }
        #endregion

        #region GetFailStatusStreamInputProperties
        public SVDynamicPropertyCollection GetFailStatusStreamInputProperties(string methodName)
        {
            if (failStatusStreamInputParameters.ContainsKey(methodName))
            {
                return failStatusStreamInputParameters[methodName];
            }
            return null;
        }
        #endregion

        #region GetInputBrokerInputProperties
        public SVDynamicPropertyCollection GetInputBrokerInputProperties(string methodName)
        {
            if (inputBrokerInputParameters.ContainsKey(methodName))
            {
                return inputBrokerInputParameters[methodName];
            }
            return null;
        }
        #endregion

        #region GetOutputBrokerInputProperties
        public SVDynamicPropertyCollection GetOutputBrokerInputProperties(string methodName)
        {
            if (outputBrokerInputParameters.ContainsKey(methodName))
            {
                return outputBrokerInputParameters[methodName];
            }
            return null;
        }
        #endregion

        #region GetInputProperties
        public SVDynamicPropertyCollection GetInputProperties(Type objectType, String methodName)
        {
            if (objectType.FullName.Contains("FailStatusStream"))
            {
                return GetFailStatusStreamInputProperties(methodName);
            }
            else if (objectType.FullName.Contains("InputBroker"))
            {
                return GetInputBrokerInputProperties(methodName);
            }
            else if (objectType.FullName.Contains("OutputBroker"))
            {
                return GetOutputBrokerInputProperties(methodName);
            }
            return GetRemoteInputProperties(methodName);
        }
        #endregion

        #region SetInputProperties
        public void SetInputProperties(Type objectType, String methodName, SVDynamicPropertyCollection properties)
        {
            if (objectType.FullName.Contains("FailStatusStream"))
            {
                failStatusStreamInputParameters[methodName] = properties;
            }
            else if (objectType.FullName.Contains("InputBroker"))
            {
                inputBrokerInputParameters[methodName] = properties;
            }
            else if (objectType.FullName.Contains("OutputBroker"))
            {
                outputBrokerInputParameters[methodName] = properties;
            }
            else
            {
                remoteInputParameters[methodName] = properties;
            }
        }
        #endregion

        #region SetRemoteInputProperties
        public void SetRemoteInputProperties(string methodName, SVDynamicPropertyCollection properties)
        {
            remoteInputParameters[methodName] = properties;
        }
        #endregion

        #region SaveInputProperties
        public void SaveInputProperties(XmlWriter writer)
        {
            writer.WriteStartElement(SVUserEnteredValuesTags.RemoteValues);
            SaveProperties(writer, remoteInputParameters);
            writer.WriteEndElement();

            writer.WriteStartElement(SVUserEnteredValuesTags.FailStatusStreamValues);
            SaveProperties(writer, failStatusStreamInputParameters);
            writer.WriteEndElement();

            writer.WriteStartElement(SVUserEnteredValuesTags.InputBrokerValues);
            SaveProperties(writer, inputBrokerInputParameters);
            writer.WriteEndElement();

            writer.WriteStartElement(SVUserEnteredValuesTags.OutputBrokerValues);
            SaveProperties(writer, outputBrokerInputParameters);
            writer.WriteEndElement();
        }
        #endregion

        #region SaveProperties
        private void SaveProperties(XmlWriter writer, Dictionary<string, SVDynamicPropertyCollection> values)
        {
            try
            {
                foreach (string methodName in values.Keys)
                {
                    writer.WriteStartElement(SVUserEnteredValuesTags.Method);
                    writer.WriteAttributeString(SVUserEnteredValuesTags.Name, methodName);
                    try
                    {
                        foreach (SVDynamicProperty prop in values[methodName].Properties)
                        {
                            if (prop.Value != null)
                            {
                                writer.WriteStartElement(SVUserEnteredValuesTags.Property);
                                writer.WriteAttributeString(SVUserEnteredValuesTags.Category, prop.Category);
                                writer.WriteAttributeString(SVUserEnteredValuesTags.Name, prop.Name);
                                writer.WriteAttributeString(SVUserEnteredValuesTags.TypeName, prop.TypeName);

                                if (prop.TypeName.Contains("System.Collections.Generic.List"))
                                {
                                    SaveArrayProperties(writer, prop.Value);
                                }
                                else if (prop.TypeName.Contains("SVValue") || 
                                        prop.TypeName.Contains("SVImage") ||
                                        prop.TypeName.Contains("SVInputImage") || 
                                        prop.TypeName.Contains("SVInputMatchItemWrapper"))
                                {
                                    SaveClassProperties(writer, prop.Value);
                                }
                                else
                                {
                                    SaveValueProperties(writer, prop.Value);
                                }
                                writer.WriteEndElement(); // Property
                            }
                        }
                    }
                    catch (System.Exception e)
                    {
                        System.Diagnostics.Trace.WriteLine(e.Message);
                    }
                    writer.WriteEndElement(); // Method
                }
            }
            catch (System.Exception e)
            {
                System.Diagnostics.Trace.WriteLine(e.Message);
            }
        }
        #endregion

        #region SaveArrayProperties
        private void SaveArrayProperties(XmlWriter writer, object value)
        {
            // Write out an Array of items
            writer.WriteStartElement(SVUserEnteredValuesTags.Array);
            IList list = value as IList;
            if (list.Count > 0)
            {
                foreach (object obj in list)
                {
                    if (obj != null)
                    {
                        if (obj.GetType().FullName.Contains("SVValue") ||
                            obj.GetType().FullName.Contains("SVImage") ||
                            obj.GetType().FullName.Contains("SVInputImage") ||
                            obj.GetType().FullName.Contains("SVInputMatchItemWrapper"))
                        {
                            SaveClassProperties(writer, obj);
                        }
                        else
                        {
                            writer.WriteStartElement(SVUserEnteredValuesTags.Value);
                            writer.WriteAttributeString(SVUserEnteredValuesTags.TypeName, obj.GetType().FullName);
                            writer.WriteValue(obj.ToString());
                            writer.WriteEndElement(); // Value
                        }
                    }
                }
            }
            writer.WriteEndElement(); // Array
        }
        #endregion

        #region SaveValueProperties
        private void SaveValueProperties(XmlWriter writer, object value)
        {
            writer.WriteStartElement(SVUserEnteredValuesTags.Value);
            Type propType = value.GetType();
            if (propType.IsEnum && propType.FullName.Contains("SVRemoteControlLib"))
            {
                writer.WriteAttributeString(SVUserEnteredValuesTags.TypeName, propType.AssemblyQualifiedName);
            }
            else
            {
                writer.WriteAttributeString(SVUserEnteredValuesTags.TypeName, propType.FullName);
            }
            writer.WriteValue(value.ToString());
            writer.WriteEndElement(); // Value
        }
        #endregion

        #region SaveClassProperties
        private void SaveClassProperties(XmlWriter writer, object value)
        {
            writer.WriteStartElement(SVUserEnteredValuesTags.ClassTag);
            writer.WriteAttributeString(SVUserEnteredValuesTags.TypeName, value.GetType().FullName);
                                        
            // Write members as properties...
            PropertyInfo[] properties = value.GetType().GetProperties();
            foreach (PropertyInfo propInfo in properties)
            {
                // Get category
                CategoryAttribute[] attributes = (CategoryAttribute[])propInfo.GetCustomAttributes(typeof(CategoryAttribute), false);
                if (attributes != null && attributes.Length > 0)
                {
                    if (SVRemoteTestHelper.scInputCategory == attributes[0].Category)
                    {
                        object propValue = propInfo.GetValue(value, null);
                        if (propValue != null)
                        {
                            writer.WriteStartElement(SVUserEnteredValuesTags.Property);
                            writer.WriteAttributeString(SVUserEnteredValuesTags.Category, SVRemoteTestHelper.scInputCategory);
                            writer.WriteAttributeString(SVUserEnteredValuesTags.Name, propInfo.Name);
                            writer.WriteAttributeString(SVUserEnteredValuesTags.TypeName, propInfo.PropertyType.FullName);
                            if (propInfo.PropertyType.FullName.Contains("System.Collections.Generic.List"))
                            {
                                SaveArrayProperties(writer, propValue);
                            }
                            else
                            {
                                SaveValueProperties(writer, propValue);
                            }
                            writer.WriteEndElement(); // Property
                        }
                    }
                }
            }
            writer.WriteEndElement(); // Class
        }
        #endregion

        #region SaveRemoteViewProperties
        public void SaveRemoteViewProperties(String methodName, ISVComObjectView view)
        {
            SVDynamicPropertyCollection dynamicProperties = SaveViewProperties(methodName, view);
            if (dynamicProperties.Properties.Count > 0)
            {
                remoteInputParameters[methodName] = dynamicProperties;
            }
        }
        #endregion

        #region SaveInputBrokerViewProperties
        public void SaveInputBrokerViewProperties(String methodName, ISVComObjectView view)
        {
            SVDynamicPropertyCollection dynamicProperties = SaveViewProperties(methodName, view);
            if (dynamicProperties.Properties.Count > 0)
            {
                inputBrokerInputParameters[methodName] = dynamicProperties;
            }
        }
        #endregion

        #region SaveOutputBrokerViewProperties
        public void SaveOutputBrokerViewProperties(String methodName, ISVComObjectView view)
        {
            SVDynamicPropertyCollection dynamicProperties = SaveViewProperties(methodName, view);
            if (dynamicProperties.Properties.Count > 0)
            {
                outputBrokerInputParameters[methodName] = dynamicProperties;
            }
        }
        #endregion

        #region SaveFailStausStreamViewProperties
        public void SaveFailStatusStreamViewProperties(String methodName, ISVComObjectView view)
        {
            SVDynamicPropertyCollection dynamicProperties = SaveViewProperties(methodName, view);
            if (dynamicProperties.Properties.Count > 0)
            {
                failStatusStreamInputParameters[methodName] = dynamicProperties;
            }
        }
        #endregion

        #region SaveViewProperties
        private SVDynamicPropertyCollection SaveViewProperties(String methodName, ISVComObjectView view)
        {
            SVDynamicPropertyCollection dynamicProperties = new SVDynamicPropertyCollection();
            
            try
            {
                Type type = view.GetType();
                PropertyInfo[] properties = type.GetProperties();
                if (properties.Length > 0)
                {
                    try
                    {
                        foreach (PropertyInfo propInfo in properties)
                        {
                            CategoryAttribute[] attributes = (CategoryAttribute[])propInfo.GetCustomAttributes(typeof(CategoryAttribute), false);
                            if (attributes != null && attributes.Length > 0)
                            {
                                if (attributes[0].Category == SVRemoteTestHelper.scInputCategory)
                                {
                                    object value = propInfo.GetValue(view, null);
                                    if (value != null && propInfo.CanWrite)
                                    {
                                        SVDynamicProperty prop = new SVDynamicProperty();
                                        prop.Category = attributes[0].Category;
                                        prop.Name = propInfo.Name;
                                        Type propType = propInfo.PropertyType;
                                        if (propType.IsEnum && propType.FullName.Contains("SVRemoteControlLib"))
                                        {
                                            prop.TypeName = propType.AssemblyQualifiedName;
                                        }
                                        else
                                        {
                                            prop.TypeName = propType.FullName;
                                        }
                                        prop.Value = value;
                                        dynamicProperties.Properties.Add(prop);
                                    }
                                }
                            }
                        }
                    }
                    catch (System.Exception e)
                    {
                        System.Diagnostics.Trace.WriteLine(e.Message);
                    }
                }
            }
            catch (System.Exception e)
            {
                System.Diagnostics.Trace.WriteLine(e.Message);
            }
            return dynamicProperties;
        }
        #endregion

        #region LoadInputProperties
        public void LoadInputProperties(XmlReader reader, string endTag)
        {
            bool done = false;
            // Parse the file. 
            while (!done && reader.Read())
            {
                switch (reader.NodeType)
                {
                    case XmlNodeType.Element:
                        if (reader.Name == SVUserEnteredValuesTags.RemoteValues)
                        {
                            if (!reader.IsEmptyElement)
                            {
                                LoadProperties(reader, reader.Name, remoteInputParameters);
                            }
                        }
                        else if (reader.Name == SVUserEnteredValuesTags.FailStatusStreamValues)
                        {
                            if (!reader.IsEmptyElement)
                            {
                                LoadProperties(reader, reader.Name, failStatusStreamInputParameters);
                            }
                        }
                        else if (reader.Name == SVUserEnteredValuesTags.InputBrokerValues)
                        {
                            if (!reader.IsEmptyElement)
                            {
                                LoadProperties(reader, reader.Name, inputBrokerInputParameters);
                            }
                        }
                        else if (reader.Name == SVUserEnteredValuesTags.OutputBrokerValues)
                        {
                            if (!reader.IsEmptyElement)
                            {
                                LoadProperties(reader, reader.Name, outputBrokerInputParameters);
                            }
                        }
                        else if (reader.Name == endTag && reader.IsEmptyElement)
                        {
                            done = true;
                        }
                        break;

                    case XmlNodeType.EndElement:
                        if (reader.Name == endTag) // check if end
                        {
                            done = true;
                        }
                        break;
                }
            }
        }
        #endregion
        
        #region LoadProperties
        public void LoadProperties(XmlReader reader, string endTag, Dictionary<string, SVDynamicPropertyCollection> values)
        {
            try
            {
                String categoryName = String.Empty;
                String methodName = String.Empty;
                SVDynamicPropertyCollection collection = new SVDynamicPropertyCollection();

                bool done = false;
                // Parse the file. 
                while (!done && reader.Read())
                {
                    switch (reader.NodeType)
                    {
                        case XmlNodeType.Element:
                            if (SVUserEnteredValuesTags.Method == reader.Name)
                            {
                                if (!reader.IsEmptyElement)
                                {
                                    if (methodName != null && methodName != String.Empty)
                                    {
                                        // save the method and user properties
                                        values[methodName] = collection;
                                        collection = new SVDynamicPropertyCollection();
                                    }
                                    methodName = ReadAttribute(reader, SVUserEnteredValuesTags.Name);
                                }
                                else
                                {
                                    done = true;
                                }
                            }
                            else if (SVUserEnteredValuesTags.Property == reader.Name)
                            {
                                if (!reader.IsEmptyElement)
                                {
                                    SVDynamicProperty prop = ReadProperty(reader);
                                    collection.Properties.Add(prop);
                                }
                            }
                            break;

                        case XmlNodeType.EndElement:
                            // check if end
                            if (reader.Name == endTag)
                            {
                                done = true;
                            }
                            break;
                    }
                }
                if (collection.Properties.Count > 0)
                {
                    values[methodName] = collection;
                }
            }
            catch (System.Xml.XmlException e)
            {
                System.Diagnostics.Trace.WriteLine(e.Message);
            }
		}
        #endregion

        #region ReadAttribute
		private String ReadAttribute(XmlReader reader, string attrName)
		{
            return reader.GetAttribute(attrName);
		}
        #endregion

        #region ReadProperty
        private SVDynamicProperty ReadProperty(XmlReader reader)
        {
            SVDynamicProperty prop = new SVDynamicProperty();
            prop.Name = ReadAttribute(reader, SVUserEnteredValuesTags.Name);
            prop.Category = ReadAttribute(reader, SVUserEnteredValuesTags.Category);
            prop.TypeName = ReadAttribute(reader, SVUserEnteredValuesTags.TypeName);
            // read the Array or Value Element
            prop.Value = ReadValues(reader, prop.TypeName);
            return prop;
        }
        #endregion

        #region ReadValues
        private object ReadValues(XmlReader reader, String valueTypeName)
        {
            object obj = null;
            bool done = false;
            bool isArray = false;
            IList list = null;
            String typeName = String.Empty;
            while (!done)
            {
                if (reader.Read())
                {
                    switch (reader.NodeType)
                    {
                        case XmlNodeType.Element:
                            {
                                if (SVUserEnteredValuesTags.Array == reader.Name)
                                {
                                    if (!reader.IsEmptyElement)
                                    {
                                        isArray = true;
                                    }
                                    else
                                    {
                                        done = true;
                                        obj = list;
                                    }
                                }
                                else if (SVUserEnteredValuesTags.Value == reader.Name)
                                {
                                    typeName = ReadAttribute(reader, SVUserEnteredValuesTags.TypeName);
                                    if (isArray && null == list)
                                    {
                                        var type = Type.GetType(valueTypeName);
                                        if (type != null)
                                        {
                                            list = (IList)Activator.CreateInstance(type);
                                        }
                                    }
                                }
                                else if (SVUserEnteredValuesTags.ClassTag == reader.Name)
                                {
                                    if (!reader.IsEmptyElement)
                                    {
                                        obj = ReadClass(reader);
                                        if (isArray && obj != null)
                                        {
                                            if (null == list)
                                            {
                                                var type = Type.GetType(valueTypeName);
                                                if (type != null)
                                                {
                                                    list = (IList)Activator.CreateInstance(type);
                                                }
                                            }

                                            if (list != null)
                                            {
                                                list.Add(obj);
                                            }
                                        }
                                    }
                                }
                            }
                            break;

                        case XmlNodeType.Text:
                            {
                                String value = reader.Value;
                                // convert to type required
                                obj = ConvertToType(value, typeName);
                                if (isArray && obj != null)
                                {
                                    if (list != null)
                                    {
                                        list.Add(obj);
                                    }
                                }
                            }
                            break;

                        case XmlNodeType.EndElement:
                            {
                                if (SVUserEnteredValuesTags.Array == reader.Name && isArray)
                                {
                                    done = true;
                                    obj = list;
                                }
                                else if (SVUserEnteredValuesTags.Value == reader.Name && !isArray)
                                {
                                    done = true;
                                }
                            }
                            break;
                    }
                }
                else
                {
                    done = true;
                }
            }
            return obj;
        }
        #endregion

        #region ReadClass
        object ReadClass(XmlReader reader)
        {
            object obj = null;
            string typeName = ReadAttribute(reader, SVUserEnteredValuesTags.TypeName);
            var type = Type.GetType(typeName);
            if (type != null)
            {
                obj = Activator.CreateInstance(type);
            }
            bool done = false;
            // Parse the file. 
            while (!done && reader.Read())
            {
                switch (reader.NodeType)
                {
                    case XmlNodeType.Element:
                    if (SVUserEnteredValuesTags.Property == reader.Name)
                    {
                        if (!reader.IsEmptyElement)
                        {
                            SVDynamicProperty prop = ReadProperty(reader);
                            // Set the member property
                            PropertyInfo propInfo = obj.GetType().GetProperty(prop.Name);
                            if (propInfo != null)
                            {
                                propInfo.SetValue(obj, prop.Value, null);
                            }
                        }
                    }
                    break;

                    case XmlNodeType.EndElement:
                        if (SVUserEnteredValuesTags.ClassTag == reader.Name)
                        {
                            done = true;
                        }
                        break;
                }
            }
            return obj;
        }
        #endregion

        #region ConvertToType
        object ConvertToType(String value, String typeName)
        {
            object obj = null;
            if (typeName == typeof(System.String).FullName)
            {
                obj = value;
            }
            else if (typeName == typeof(System.Int32).FullName)
            {
                Int32 i;
                bool success = Int32.TryParse(value, out i);
                if (success)
                {
                    obj = i;
                }
            }
            else if (typeName == typeof(System.Double).FullName)
            {
                Double d;
                bool success = Double.TryParse(value, out d);
                if (success)
                {
                    obj = d;
                }
            }
            else if (typeName == typeof(System.Boolean).FullName)
            {
                Boolean b;
                bool success = Boolean.TryParse(value, out b);
                if (success)
                {
                    obj = b;
                }
            }
            else // special case enum
            {
                Type valueType = Type.GetType(typeName);
                if (valueType != null)
                {
                    if (valueType.IsEnum) // check for enum
                    {
                        obj = Enum.Parse(valueType, value);
                    }
                }
            }
            return obj;
        }
        #endregion
    }
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\SVUserEnteredValues.cs_v  $
 * 
 *    Rev 1.2   28 Jan 2015 13:18:24   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 21
 * SCR Title: Add Fail Status Streaming
 * Checked in by: sJones; Steve Jones
 * Change Description: Added failStatusStreamInputParameters 
 * Added GetFailStatusStreamInputProperties method
 * Revised GetInputProperties method
 * Revised SetInputProperties method
 * Revised SaveInputProperties method
 * Added SaveFailStatusStreamViewProperties method
 * Revised LoadInputProperties method
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.1   04 Nov 2014 16:10:22   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 19
 * SCR Title: Enchance SVRC Test Application to remember method data
 * Checked in by: sJones; Steve Jones
 * Change Description: Revised to Save and Load class members using reflection, for SVValue and SVInputMatchItemWrapper.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.0   28 Oct 2014 11:39:54   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 19
 * SCR Title: Enchance SVRC Test Application to remember method data
 * Checked in by: sJones; Steve Jones
 * Change Description: Initial checkin
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/
