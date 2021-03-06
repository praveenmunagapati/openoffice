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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"

#include "hintids.hxx"
#include "cntfrm.hxx"       // _GetFly
#include "doc.hxx"
#include <IDocumentUndoRedo.hxx>
#include "pam.hxx"          // fuer SwTxtFlyCnt
#include "flyfrm.hxx"       // fuer SwTxtFlyCnt
#include "ndtxt.hxx"        // SwFlyFrmFmt
#include "frmfmt.hxx"       // SwFlyFrmFmt
#include <fmtflcnt.hxx>
#include <txtflcnt.hxx>
#include <fmtanchr.hxx>
#include "swfont.hxx"
#include "txtfrm.hxx"
#include "flyfrms.hxx"
#include <objectformatter.hxx>
#include <switerator.hxx>

SwFmtFlyCnt::SwFmtFlyCnt( SwFrmFmt *pFrmFmt )
	: SfxPoolItem( RES_TXTATR_FLYCNT ),
    pTxtAttr( 0 ),
    pFmt( pFrmFmt )
{
}

int __EXPORT SwFmtFlyCnt::operator==( const SfxPoolItem& rAttr ) const
{
	ASSERT( SfxPoolItem::operator==( rAttr ), "keine gleichen Attribute" );
	return( pTxtAttr && ((SwFmtFlyCnt&)rAttr).pTxtAttr &&
			*pTxtAttr->GetStart() == *((SwFmtFlyCnt&)rAttr).pTxtAttr->GetStart() &&
			pFmt == ((SwFmtFlyCnt&)rAttr).GetFrmFmt() );
}

SfxPoolItem* __EXPORT SwFmtFlyCnt::Clone( SfxItemPool* ) const
{
	return new SwFmtFlyCnt( pFmt );
}

SwTxtFlyCnt::SwTxtFlyCnt( SwFmtFlyCnt& rAttr, xub_StrLen nStartPos )
    : SwTxtAttr( rAttr, nStartPos )
{
    rAttr.pTxtAttr = this;
    SetHasDummyChar(true);
}



/*************************************************************************
 *                  SwTxtFlyCnt::MakeTxtHint()
 *
 * An dieser Stelle soll einmal der Gesamtzusammenhang bei der Erzeugung
 * eines neuen SwTxtFlyCnt erlaeutert werden.
 * Das MakeTxtHint() wird z.B. im SwTxtNode::Copy() gerufen.
 * Fuer die komplette Verdopplung sind folgende Schritte notwendig:
 * 1) Duplizieren des pFmt incl. Inhalt, Attributen etc.
 * 2) Setzen des Ankers
 * 3) Benachrichtigung
 * Da fuer die Bewaeltigung der Aufgaben nicht immer alle Informationen
 * bereitstehen und darueber hinaus bestimmte Methoden erst zu einem
 * spaeteren Zeitpunkt gerufen werden duerfen (weil nocht nicht alle
 * Nodeinformationen vorliegen), verteilt sich der Ablauf.
 * ad 1) MakeTxtHint() wird durch den Aufruf von SwDoc::CopyLayout()
 * der das neue FlyFrmFmt erzeugt und mit dem duplizierten Inhalt des
 * FlyFrm verbunden.
 * ad 2) SetAnchor() wird von SwTxtNode::Insert() gerufen und sorgt fuer das
 * setzen des Ankers (die SwPosition des Dummy-Zeichens wird dem FlyFrmFmt
 * per SetAttr bekannt gegeben). Dies kann nicht im MakeTxtHint erledigt
 * werden, da der Zielnode unbestimmt ist.
 * ad 3) _GetFlyFrm() wird im Formatierungsprozess vom LineIter gerufen
 * und sucht den FlyFrm zum Dummyzeichen des aktuellen CntntFrm. Wird keiner
 * gefunden, so wird ein neuer FlyFrm angelegt.
 * Kritisch an diesem Vorgehen ist, dass das pCntnt->AppendFly() eine
 * sofortige Neuformatierung von pCntnt anstoesst. Die Rekursion kommt
 * allerdings durch den Lockmechanismus in SwTxtFrm::Format() nicht
 * zu stande.
 * Attraktiv ist der Umstand, dass niemand ueber die vom Node abhaengigen
 * CntntFrms iterieren braucht, um die FlyInCntFrm anzulegen. Dies geschieht
 * bei der Arbeit.
 *************************************************************************/

void SwTxtFlyCnt::CopyFlyFmt( SwDoc* pDoc )
{
	SwFrmFmt* pFmt = GetFlyCnt().GetFrmFmt();
	ASSERT( pFmt, "von welchem Format soll ich eine Kopie erzeugen?" )
	// Das FlyFrmFmt muss dupliziert werden.
	// In CopyLayoutFmt (siehe doclay.cxx) wird das FlyFrmFmt erzeugt
	// und der Inhalt dupliziert.

    // disable undo while copying attribute
    ::sw::UndoGuard const undoGuard(pDoc->GetIDocumentUndoRedo());
	SwFmtAnchor aAnchor( pFmt->GetAnchor() );
    if ((FLY_AT_PAGE != aAnchor.GetAnchorId()) &&
        (pDoc != pFmt->GetDoc()))   // different documents?
    {
		// JP 03.06.96: dann sorge dafuer, das der koperierte Anker auf
		//				gueltigen Content zeigt! Die Umsetzung auf die
		//				richtige Position erfolgt spaeter.
		SwNodeIndex aIdx( pDoc->GetNodes().GetEndOfExtras(), +2 );
		SwCntntNode* pCNd = aIdx.GetNode().GetCntntNode();
		if( !pCNd )
			pCNd = pDoc->GetNodes().GoNext( &aIdx );

		SwPosition* pPos = (SwPosition*)aAnchor.GetCntntAnchor();
		pPos->nNode = aIdx;
        if (FLY_AS_CHAR == aAnchor.GetAnchorId())
        {
			pPos->nContent.Assign( pCNd, 0 );
        }
		else
		{
			pPos->nContent.Assign( 0, 0 );
			ASSERT( !this, "CopyFlyFmt: Was fuer ein Anker?" );
		}
	}

    SwFrmFmt* pNew = pDoc->CopyLayoutFmt( *pFmt, aAnchor, false, false );
	((SwFmtFlyCnt&)GetFlyCnt()).SetFlyFmt( pNew );
}

/*************************************************************************
 *                  SwTxtFlyCnt::SetAnchor()
 *
 * SetAnchor() wird von SwTxtNode::Insert() gerufen und sorgt fuer das
 * setzen des Ankers (die SwPosition des Dummy-Zeichens wird dem FlyFrmFmt
 * per SetAttr bekannt gegeben). Dies kann nicht im MakeTxtHint erledigt
 * werden, da der Zielnode unbestimmt ist.
 * (siehe Kommentar in SwTxtFlyCnt::MakeTxtHint)
 *************************************************************************/

void SwTxtFlyCnt::SetAnchor( const SwTxtNode *pNode )
{
	// fuers Undo muss der neue Anker schon bekannt sein !

	// Wir ermitteln den Index im Nodesarray zum Node

	SwDoc* pDoc = (SwDoc*)pNode->GetDoc();

	SwIndex aIdx( (SwTxtNode*)pNode, *GetStart() );
	SwPosition aPos( *pNode->StartOfSectionNode(), aIdx );
	SwFrmFmt* pFmt = GetFlyCnt().GetFrmFmt();
	SwFmtAnchor aAnchor( pFmt->GetAnchor() );

	if( !aAnchor.GetCntntAnchor() ||
		!aAnchor.GetCntntAnchor()->nNode.GetNode().GetNodes().IsDocNodes() ||
		&aAnchor.GetCntntAnchor()->nNode.GetNode() != (SwNode*)pNode )
		aPos.nNode = *pNode;
	else
		aPos.nNode = aAnchor.GetCntntAnchor()->nNode;

    aAnchor.SetType( FLY_AS_CHAR );        // default!
	aAnchor.SetAnchor( &aPos );

	// beim Ankerwechsel werden immer alle FlyFrms vom Attribut geloescht
	// JP 25.04.95: wird innerhalb des SplitNodes die Frames verschoben
	//				koennen die Frames erhalten bleiben.
	if( ( !pNode->GetpSwpHints() || !pNode->GetpSwpHints()->IsInSplitNode() )
		&& RES_DRAWFRMFMT != pFmt->Which() )
		pFmt->DelFrms();

	// stehen wir noch im falschen Dokument ?
	if( pDoc != pFmt->GetDoc() )
    {
        // disable undo while copying attribute
        ::sw::UndoGuard const undoGuard(pDoc->GetIDocumentUndoRedo());
        SwFrmFmt* pNew = pDoc->CopyLayoutFmt( *pFmt, aAnchor, false, false );

        ::sw::UndoGuard const undoGuardFmt(
            pFmt->GetDoc()->GetIDocumentUndoRedo());
		pFmt->GetDoc()->DelLayoutFmt( pFmt );
		((SwFmtFlyCnt&)GetFlyCnt()).SetFlyFmt( pNew );
	}
	else if( pNode->GetpSwpHints() &&
			pNode->GetpSwpHints()->IsInSplitNode() &&
			RES_DRAWFRMFMT != pFmt->Which() )
	{
		pFmt->LockModify();
        pFmt->SetFmtAttr( aAnchor );        // nur den Anker neu setzen
		pFmt->UnlockModify();
	}
	else
        pFmt->SetFmtAttr( aAnchor );        // nur den Anker neu setzen

	// Am Node haengen u.a. abhaengige CntFrms.
	// Fuer jeden CntFrm wird ein SwFlyInCntFrm angelegt.
}

/*************************************************************************
 *                      SwTxtFlyCnt::_GetFlyFrm()
 *
 * _GetFlyFrm() wird im Formatierungsprozess vom LineIter gerufen
 * und sucht den FlyFrm zum Dummyzeichen des aktuellen CntntFrm. Wird keiner
 * gefunden, so wird ein neuer FlyFrm angelegt.
 * (siehe Kommentar ind SwTxtFlyCnt::MakeTxtHint)
 *************************************************************************/

SwFlyInCntFrm *SwTxtFlyCnt::_GetFlyFrm( const SwFrm *pCurrFrm )
{
	SwFrmFmt* pFrmFmt = GetFlyCnt().GetFrmFmt();
	if( RES_DRAWFRMFMT == pFrmFmt->Which() )
	{
		ASSERT(  !this, "SwTxtFlyCnt::_GetFlyFrm: DrawInCnt-Baustelle!" );
		return NULL;
	}

	SwIterator<SwFlyFrm,SwFmt> aIter( *GetFlyCnt().pFmt );
	ASSERT( pCurrFrm->IsTxtFrm(), "SwTxtFlyCnt::_GetFlyFrm for TxtFrms only." );
    SwFrm* pFrm = aIter.First();
    if ( pFrm )
	{
		SwTxtFrm *pFirst = (SwTxtFrm*)pCurrFrm;
		while ( pFirst->IsFollow() )
			pFirst = pFirst->FindMaster();
		do
			{
				SwTxtFrm *pTmp = pFirst;
				do
                {   if( ( (SwFlyFrm*)pFrm )->GetAnchorFrm() == (SwFrm*) pTmp )
					{
						if ( pTmp != pCurrFrm )
						{
							pTmp->RemoveFly( (SwFlyFrm*)pFrm );
							((SwTxtFrm*)pCurrFrm)->AppendFly( (SwFlyFrm*)pFrm );
						}
						return (SwFlyInCntFrm*)pFrm;
					}
					pTmp = pTmp->GetFollow();
				} while ( pTmp );

                pFrm = aIter.Next();

		} while( pFrm );
	}

	// Wir haben keinen passenden FlyFrm gefunden, deswegen wird ein
	// neuer angelegt.
	// Dabei wird eine sofortige Neuformatierung von pCurrFrm angestossen.
	// Die Rekursion wird durch den Lockmechanismus in SwTxtFrm::Format()
	// abgewuergt.
    SwFrm* pCurrFrame = const_cast< SwFrm* >(pCurrFrm); 
	SwFlyInCntFrm *pFly = new SwFlyInCntFrm( (SwFlyFrmFmt*)pFrmFmt, pCurrFrame, pCurrFrame );
	pCurrFrame->AppendFly( pFly );
	pFly->RegistFlys();

    // 7922: Wir muessen dafuer sorgen, dass der Inhalt des FlyInCnt
    // nach seiner Konstruktion stramm durchformatiert wird.
    // --> OD 2004-11-09 #i26945# - Use new object formatter to format Writer
    // fly frame and its content.
    SwObjectFormatter::FormatObj( *pFly, const_cast<SwFrm*>(pCurrFrm),
                                  pCurrFrm->FindPageFrm() );
    // <--

	return pFly;
}


