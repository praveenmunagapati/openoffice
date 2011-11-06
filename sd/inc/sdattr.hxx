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



#ifndef _SDATTR_HXX
#define _SDATTR_HXX

#include <com/sun/star/presentation/FadeEffect.hpp>
#include <svl/intitem.hxx>
#include <svl/eitem.hxx>
#include <svl/stritem.hxx>
#include <sfx2/sfx.hrc>

#ifndef _SDATTR_HRC
#include "sdattr.hrc"
#endif
#include "glob.hxx"
#include "fadedef.h"
#include "diadef.h"

//------------------------------------------------------------------


//==================================================================
//	Layer-Attribute
//==================================================================

class SdAttrLayerName : public SfxStringItem
{
public:
	SdAttrLayerName() :
		SfxStringItem( ATTR_LAYER_NAME, String( RTL_CONSTASCII_USTRINGPARAM( "neue Ebene" ))) {}
	SdAttrLayerName( const String& aStr ) :
		SfxStringItem( ATTR_LAYER_NAME, aStr ) {}
};

//------------------------------------------------------------------

class SdAttrLayerTitle : public SfxStringItem
{
public:
	SdAttrLayerTitle() : SfxStringItem( ATTR_LAYER_TITLE, XubString()) {}
	SdAttrLayerTitle( const String& aStr ) : SfxStringItem( ATTR_LAYER_TITLE, aStr ) {}
};

//------------------------------------------------------------------

class SdAttrLayerDesc : public SfxStringItem
{
public:
	SdAttrLayerDesc() : SfxStringItem( ATTR_LAYER_DESC, XubString()) {}
	SdAttrLayerDesc( const String& aStr ) : SfxStringItem( ATTR_LAYER_DESC, aStr ) {}
};

//------------------------------------------------------------------

class SdAttrLayerVisible : public SfxBoolItem
{
public:
	SdAttrLayerVisible( sal_Bool bValue = sal_True ) :
		SfxBoolItem( ATTR_LAYER_VISIBLE, bValue ) {}
};

//------------------------------------------------------------------

class SdAttrLayerPrintable : public SfxBoolItem
{
public:
	SdAttrLayerPrintable( sal_Bool bValue = sal_True ) :
		SfxBoolItem( ATTR_LAYER_PRINTABLE, bValue ) {}
};

//------------------------------------------------------------------

class SdAttrLayerLocked : public SfxBoolItem
{
public:
	SdAttrLayerLocked( sal_Bool bValue = sal_False ) :
		SfxBoolItem( ATTR_LAYER_LOCKED, bValue ) {}
};

//------------------------------------------------------------------

class SdAttrLayerThisPage : public SfxBoolItem
{
public:
	SdAttrLayerThisPage( sal_Bool bValue = sal_False ) :
		SfxBoolItem( ATTR_LAYER_THISPAGE, bValue ) {}
};

//------------------------------------------------------------------

class DiaEffectItem : public SfxEnumItem
{
public:
			TYPEINFO();
			DiaEffectItem( ::com::sun::star::presentation::FadeEffect eFade = com::sun::star::presentation::FadeEffect_NONE );
			DiaEffectItem( SvStream& rIn );

	virtual SfxPoolItem*	Clone( SfxItemPool* pPool = 0 ) const;
	virtual SfxPoolItem*	Create( SvStream& rIn, sal_uInt16 nVer ) const;
			sal_uInt16			GetValueCount() const { return FADE_EFFECT_COUNT; }
			::com::sun::star::presentation::FadeEffect		GetValue() const
							{ return (::com::sun::star::presentation::FadeEffect) SfxEnumItem::GetValue(); }
};

//------------------------------------------------------------------

class DiaSpeedItem : public SfxEnumItem
{
public:
			TYPEINFO();
			DiaSpeedItem( FadeSpeed = FADE_SPEED_MEDIUM );
			DiaSpeedItem( SvStream& rIn );

	virtual SfxPoolItem*	Clone( SfxItemPool* pPool = 0 ) const;
	virtual SfxPoolItem*	Create( SvStream& rIn, sal_uInt16 nVer ) const;
			sal_uInt16			GetValueCount() const { return FADE_SPEED_COUNT; }
			FadeSpeed		GetValue() const
							{ return (FadeSpeed) SfxEnumItem::GetValue(); }
};

//------------------------------------------------------------------

class DiaAutoItem : public SfxEnumItem
{
public:
			TYPEINFO();
			DiaAutoItem( PresChange = PRESCHANGE_MANUAL );
			DiaAutoItem( SvStream& rIn );

	virtual SfxPoolItem*	Clone( SfxItemPool* pPool = 0 ) const;
	virtual SfxPoolItem*	Create( SvStream& rIn, sal_uInt16 nVer ) const;
			sal_uInt16			GetValueCount() const { return PRESCHANGE_COUNT; }
			PresChange		GetValue() const { return (PresChange) SfxEnumItem::GetValue(); }
};

//------------------------------------------------------------------

class DiaTimeItem : public SfxUInt32Item
{
public:
			TYPEINFO();
			DiaTimeItem( sal_uInt32 nValue = 0L );

	virtual SfxPoolItem* Clone( SfxItemPool* pPool = 0 ) const;
	virtual int 		 operator==( const SfxPoolItem& ) const;
};

#endif // _SDATTR_HXX


