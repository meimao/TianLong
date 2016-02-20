/*
 * Copyright 1999-2001,2004 The Apache Software Foundation.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * $Id: DTDElementDecl.cpp 176169 2004-10-28 20:21:06Z peiyongz $
 */


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <../../../SceneEdit/Xerces/src/xercesc/util/XMLString.hpp>
#include <../../../SceneEdit/Xerces/src/xercesc/util/XMLUniDefs.hpp>
#include <../../../SceneEdit/Xerces/src/xercesc/util/XMLUni.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <xercesc/validators/common/DFAContentModel.hpp>
#include <xercesc/validators/common/ContentSpecNode.hpp>
#include <xercesc/validators/common/MixedContentModel.hpp>
#include <xercesc/validators/common/SimpleContentModel.hpp>
#include <xercesc/validators/DTD/DTDAttDefList.hpp>
#include <xercesc/validators/DTD/DTDElementDecl.hpp>

#include <xercesc/internal/XTemplateSerializer.hpp>

XERCES_CPP_NAMESPACE_BEGIN

// ---------------------------------------------------------------------------
//  DTDElementDecl: Constructors and Destructor
// ---------------------------------------------------------------------------
DTDElementDecl::DTDElementDecl(MemoryManager* const manager) :

    XMLElementDecl(manager)
    , fModelType(Any)
    , fAttDefs(0)
    , fAttList(0)
    , fContentSpec(0)
    , fContentModel(0)
    , fFormattedModel(0)
{
}

DTDElementDecl::DTDElementDecl( const XMLCh* const               elemRawName
                              , const unsigned int               uriId
                              , const DTDElementDecl::ModelTypes type
                              , MemoryManager* const             manager) :
    XMLElementDecl(manager)
    , fModelType(type)
    , fAttDefs(0)
    , fAttList(0)
    , fContentSpec(0)
    , fContentModel(0)
    , fFormattedModel(0)
{
    setElementName(elemRawName, uriId);
}

DTDElementDecl::DTDElementDecl( QName* const                     elementName
                              , const DTDElementDecl::ModelTypes type
                              , MemoryManager* const             manager) :
    XMLElementDecl(manager)
    , fModelType(type)
    , fAttDefs(0)
    , fAttList(0)
    , fContentSpec(0)
    , fContentModel(0)
    , fFormattedModel(0)
{
    setElementName(elementName);
}

DTDElementDecl::~DTDElementDecl()
{
    delete fAttDefs;
    delete fAttList;
    delete fContentSpec;
    delete fContentModel;
    getMemoryManager()->deallocate(fFormattedModel);//delete [] fFormattedModel;
}



// ---------------------------------------------------------------------------
//  The virtual element decl interface
// ---------------------------------------------------------------------------
XMLAttDef* DTDElementDecl::findAttr(const   XMLCh* const    qName
                                    , const unsigned int
                                    , const XMLCh* const
                                    , const XMLCh* const
                                    , const LookupOpts      options
                                    ,       bool&           wasAdded) const
{
    DTDAttDef* retVal = 0;

    // If no att list faulted in yet, then it cannot exist
    if (fAttDefs)
        retVal = fAttDefs->get(qName);

    // Fault it in if not found and ask to add it
    if (!retVal && (options == XMLElementDecl::AddIfNotFound))
    {
        // Fault in the list itself if not already
        if (!fAttDefs)
            faultInAttDefList();

        // And add a default attribute for this name
        retVal = new (getMemoryManager()) DTDAttDef
        (
            qName
            , XMLAttDef::CData
            , XMLAttDef::Implied
            , getMemoryManager()
        );
        retVal->setElemId(getId());
        fAttDefs->put((void*)retVal->getFullName(), retVal);
        // update and/or create fAttList
        if(!fAttList)
            ((DTDElementDecl*)this)->fAttList = new (getMemoryManager()) DTDAttDefList(fAttDefs,getMemoryManager());
        fAttList->addAttDef(retVal);

        wasAdded = true;
    }
     else
    {
        wasAdded = false;
    }
    return retVal;
}


XMLAttDefList& DTDElementDecl::getAttDefList() const
{
    if (!fAttList)
    {
        // If the att def list is not made yet, then fault it in too
        if (!fAttDefs)
            faultInAttDefList();

        ((DTDElementDecl*)this)->fAttList = new (getMemoryManager()) DTDAttDefList(fAttDefs,getMemoryManager());
    }

    // Reset it before we return it
    fAttList->Reset();
    return *fAttList;
}


XMLElementDecl::CharDataOpts DTDElementDecl::getCharDataOpts() const
{
    XMLElementDecl::CharDataOpts retVal;
    switch(fModelType)
    {
        case Children :
            retVal = XMLElementDecl::SpacesOk;
            break;

        case Empty :
            retVal = XMLElementDecl::NoCharData;
            break;

        default :
            retVal = XMLElementDecl::AllCharData;
            break;
    }
    return retVal;
}


bool DTDElementDecl::hasAttDefs() const
{
    // If the collection hasn't been faulted in, then no att defs
    if (!fAttDefs)
        return false;

    return !fAttDefs->isEmpty();
}


bool DTDElementDecl::resetDefs()
{
    // If the collection hasn't been faulted in, then no att defs
    if (!fAttDefs)
        return false;

    //
    //  Ok, run through them and clear the 'provided' flag on each of them.
    //  This lets the scanner use them to track which has been provided and
    //  which have not.
    //
    RefHashTableOfEnumerator<DTDAttDef> enumDefs(fAttDefs, false, getMemoryManager());
    while (enumDefs.hasMoreElements())
        enumDefs.nextElement().setProvided(false);
    return true;
}

void
DTDElementDecl::setContentSpec(ContentSpecNode* toAdopt)
{
    delete fContentSpec;
    fContentSpec = toAdopt;

    //reset Content Model
    setContentModel(0);

}

const XMLCh*
DTDElementDecl::getFormattedContentModel() const
{
    //
    //  If its not already built, then call the protected virtual method
    //  to allow the derived class to build it (since only it knows.)
    //  Otherwise, just return the previously formatted methods.
    //
    //  Since we are faulting this in, within a const getter, we have to
    //  cast off the const-ness.
    //
    if (!fFormattedModel)
        ((DTDElementDecl*)this)->fFormattedModel = formatContentModel();

    return fFormattedModel;
}


// ---------------------------------------------------------------------------
//  DTDElementDecl: Getter methods
// ---------------------------------------------------------------------------
const DTDAttDef* DTDElementDecl::getAttDef(const XMLCh* const attName) const
{
    // If no list, then return a null
    if (!fAttDefs)
        return 0;

    return fAttDefs->get(attName);
}


DTDAttDef* DTDElementDecl::getAttDef(const XMLCh* const attName)
{
    // If no list, then return a null
    if (!fAttDefs)
        return 0;

    return fAttDefs->get(attName);
}


// ---------------------------------------------------------------------------
//  DTDElementDecl: Implementation of the protected virtual interface
// ---------------------------------------------------------------------------
void DTDElementDecl::addAttDef(DTDAttDef* const toAdd)
{
    // Fault in the att list if required
    if (!fAttDefs)
            faultInAttDefList();

    // Tell this guy the element id of its parent (us)
    toAdd->setElemId(getId());

    fAttDefs->put((void*)(toAdd->getFullName()), toAdd);
    // update and/or create fAttList
    if(!fAttList)
        ((DTDElementDecl*)this)->fAttList = new (getMemoryManager()) DTDAttDefList(fAttDefs,getMemoryManager());
    fAttList->addAttDef(toAdd);
}


// ---------------------------------------------------------------------------
//  DTDElementDecl: Private helper methods
// ---------------------------------------------------------------------------
XMLCh* DTDElementDecl::formatContentModel() const
{
    XMLCh* newValue = 0;
    if (fModelType == Any)
    {
        newValue = XMLString::replicate(XMLUni::fgAnyString, getMemoryManager());
    }
     else if (fModelType == Empty)
    {
        newValue = XMLString::replicate(XMLUni::fgEmptyString, getMemoryManager());
    }
     else
    {
        //
        //  Use a temp XML buffer to format into. Content models could be
        //  pretty long, but very few will be longer than one K. The buffer
        //  will expand to handle the more pathological ones.
        //
        XMLBuffer bufFmt(1023, getMemoryManager());
        getContentSpec()->formatSpec(bufFmt);
        newValue = XMLString::replicate(bufFmt.getRawBuffer(), getMemoryManager());
    }
    return newValue;
}

XMLContentModel* DTDElementDecl::makeContentModel()
{
    XMLContentModel* cmRet = 0;
    if (fModelType == Mixed_Simple)
    {
        //
        //  Just create a mixel content model object. This type of
        //  content model is optimized for mixed content validation.
        //
        cmRet = new (getMemoryManager()) MixedContentModel(true, this->getContentSpec(), false, getMemoryManager());
    }
     else if (fModelType == Children)
    {
        //
        //  This method will create an optimal model for the complexity
        //  of the element's defined model. If its simple, it will create
        //  a SimpleContentModel object. If its a simple list, it will
        //  create a SimpleListContentModel object. If its complex, it
        //  will create a DFAContentModel object.
        //
        cmRet = createChildModel();
    }
     else
    {
        ThrowXMLwithMemMgr(RuntimeException, XMLExcepts::CM_MustBeMixedOrChildren, getMemoryManager());
    }
    return cmRet;
}


XMLContentModel* DTDElementDecl::createChildModel()
{
    // Get the content spec node of the element
    ContentSpecNode* specNode = getContentSpec();

    if(!specNode)
        ThrowXMLwithMemMgr(RuntimeException, XMLExcepts::CM_UnknownCMSpecType, getMemoryManager());

    //
    //  Do a sanity check that the node does not have a PCDATA id. Since,
    //  if it was, it should have already gotten taken by the Mixed model.
    //
    if (specNode->getElement()) {
        if (specNode->getElement()->getURI() == XMLElementDecl::fgPCDataElemId)
            ThrowXMLwithMemMgr(RuntimeException, XMLExcepts::CM_NoPCDATAHere, getMemoryManager());
    }

    //
    //  According to the type of node, we will create the correct type of
    //  content model.
    //
    if (specNode->getType() == ContentSpecNode::Leaf)
    {
        // Create a simple content model
        return new (getMemoryManager()) SimpleContentModel
        (
            true
            , specNode->getElement()
            , 0
            , ContentSpecNode::Leaf
            , getMemoryManager()
        );
    }
     else if ((specNode->getType() == ContentSpecNode::Choice)
          ||  (specNode->getType() == ContentSpecNode::Sequence))
    {
        //
        //  Lets see if both of the children are leafs. If so, then it has to
        //  be a simple content model
        //
        if ((specNode->getFirst()->getType() == ContentSpecNode::Leaf)
        &&  (specNode->getSecond()->getType() == ContentSpecNode::Leaf))
        {
            return new (getMemoryManager()) SimpleContentModel
            (
                true
                , specNode->getFirst()->getElement()
                , specNode->getSecond()->getElement()
                , specNode->getType()
                , getMemoryManager()
            );
        }
    }
     else if ((specNode->getType() == ContentSpecNode::OneOrMore)
          ||  (specNode->getType() == ContentSpecNode::ZeroOrMore)
          ||  (specNode->getType() == ContentSpecNode::ZeroOrOne))
    {
        //
        //  Its a repetition, so see if its one child is a leaf. If so its a
        //  repetition of a single element, so we can do a simple content
        //  model for that.
        //
        if (specNode->getFirst()->getType() == ContentSpecNode::Leaf)
        {
            return new (getMemoryManager()) SimpleContentModel
            (
                true
                , specNode->getFirst()->getElement()
                , 0
                , specNode->getType()
                , getMemoryManager()
            );
        }
    }
     else
    {
        ThrowXMLwithMemMgr(RuntimeException, XMLExcepts::CM_UnknownCMSpecType, getMemoryManager());
    }

    // Its not any simple type of content, so create a DFA based content model
    return new (getMemoryManager()) DFAContentModel
    (
        true
        , this->getContentSpec()
        , getMemoryManager()
    );
}


void DTDElementDecl::faultInAttDefList() const
{
    // Use a hash modulus of 29 and tell it owns its elements
    ((DTDElementDecl*)this)->fAttDefs = new (getMemoryManager()) RefHashTableOf<DTDAttDef>(29, true, getMemoryManager());
}

/***
 * Support for Serialization/De-serialization
 ***/

IMPL_XSERIALIZABLE_TOCREATE(DTDElementDecl)

void DTDElementDecl::serialize(XSerializeEngine& serEng)
{

    XMLElementDecl::serialize(serEng);

    if (serEng.isStoring())
    {
        serEng<<(int) fModelType;

        /***
         *
         * Serialize RefHashTableOf<DTDAttDef>
         *
         ***/
        XTemplateSerializer::storeObject(fAttDefs, serEng);

        serEng<<fAttList;
        serEng<<fContentSpec;

        /***
         *   don't serialize
         *
         *   XMLContentModel*            fContentModel;
         *   XMLCh*                      fFormattedModel;
         *
         ***/

    }
    else
    {
        int i;
        serEng>>i;
        fModelType=(ModelTypes)i;

        /***
         *
         * Deserialize RefHashTableOf<DTDAttDef>           
         *
         ***/
        XTemplateSerializer::loadObject(&fAttDefs, 29, true, serEng);

        serEng>>fAttList;
        serEng>>fContentSpec;

        /***
         *   don't deserialize
         *
         *   XMLContentModel*            fContentModel;
         *   XMLCh*                      fFormattedModel;
         *
         ***/
        fContentModel   = 0;
        fFormattedModel = 0;
    }

}

XMLElementDecl::objectType  DTDElementDecl::getObjectType() const
{
    return DTD;
}

XERCES_CPP_NAMESPACE_END