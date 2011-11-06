/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



package org.openoffice.xmerge.converter.xml.sxw.pocketword;

import org.openoffice.xmerge.ConverterCapabilities;
import org.openoffice.xmerge.converter.xml.OfficeConstants;

/**
 *  <p>PocketWord implementation of <code>ConverterCapabilities</code> for
 *  the {@link
 *  org.openoffice.xmerge.converter.xml.sxw.pocketword.PluginFactoryImpl
 *  PluginFactoryImpl}.</p>
 *
 *  <p>Used with StarWriter XML to/from PocketWord conversions.  The
 *  <code>ConverterCapibilies</code> specify which &quot;Office&quot;
 *  <code>Document</code> tags and attributes are supported on the
 *  &quot;Device&quot; <code>Document</code> format.</p>
 */
public final class ConverterCapabilitiesImpl
    implements ConverterCapabilities {

    public boolean canConvertTag(String tag) {

        if (OfficeConstants.TAG_OFFICE_DOCUMENT.equals(tag))
            return true;
        else if (OfficeConstants.TAG_OFFICE_DOCUMENT_CONTENT.equals(tag))
            return true;
        else if (OfficeConstants.TAG_OFFICE_BODY.equals(tag))
            return true;
        else if (OfficeConstants.TAG_PARAGRAPH.equals(tag))
            return true;
        else if (OfficeConstants.TAG_HEADING.equals(tag))
            return true;
        else if (OfficeConstants.TAG_ORDERED_LIST.equals(tag))
            return true;
        else if (OfficeConstants.TAG_UNORDERED_LIST.equals(tag))
            return true;
        else if (OfficeConstants.TAG_LIST_ITEM.equals(tag))
            return true;
        else if (OfficeConstants.TAG_LIST_HEADER.equals(tag))
            return true;
        else if (OfficeConstants.TAG_SPAN.equals(tag))
            return true;
        else if (OfficeConstants.TAG_HYPERLINK.equals(tag))
            return true;
        else if (OfficeConstants.TAG_LINE_BREAK.equals(tag))
            return true;
        else if (OfficeConstants.TAG_SPACE.equals(tag))
            return true;
        else if (OfficeConstants.TAG_TAB_STOP.equals(tag))
            return true;

        return false;
    }

    public boolean canConvertAttribute(String tag,
                                       String attribute) {

        if (OfficeConstants.TAG_SPACE.equals(tag)) {

            if (OfficeConstants.ATTRIBUTE_SPACE_COUNT.equals(attribute))
                return true;
        }

        return false;
    }
}

