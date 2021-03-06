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



#ifndef ARY_IDL_IT_EXPLICIT_HXX
#define ARY_IDL_IT_EXPLICIT_HXX

// BASE CLASSES
#include "it_named.hxx"




namespace ary
{
namespace idl
{


/** A named @->Type, not yet related to its corresponding
    @->CodeEntity.
*/
class ExplicitType : public Named_Type
{
  public:
    enum E_ClassId { class_id = 2203 };

    // LIFECYCLE
                        ExplicitType(
                            const String &      i_sName,
                            Type_id             i_nXNameRoom,
                            Ce_id               i_nModuleOfOccurrence,
                            const std::vector<Type_id> *
                                                i_templateParameters );
    virtual             ~ExplicitType();

    // INQUIRY
    Ce_id               ModuleOfOccurrence() const
                                                { return nModuleOfOccurrence; }
    Type_id             NameRoom() const        { return nXNameRoom; }

  private:
    // Interface csv::ConstProcessorClient:
    virtual void        do_Accept(
                            csv::ProcessorIfc & io_processor ) const;
    // Interface CppEntity:
    virtual ClassId     get_AryClass() const;

    // Interface Type:
    virtual void        inq_Get_Text(
                            StringVector &      o_module,
                            String &            o_name,
                            Ce_id &             o_nRelatedCe,
                            int &               o_nSequemceCount,
                            const Gate &        i_rGate ) const;
    virtual const std::vector<Type_id> *
                        inq_TemplateParameters() const;
    // DATA
    Type_id             nXNameRoom;             // As written in code.
    Ce_id               nModuleOfOccurrence;
    Dyn< const std::vector<Type_id> >
                        pTemplateParameters;
};




}   // namespace idl
}   // namespace ary
#endif
