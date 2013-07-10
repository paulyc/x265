/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2010-2013, ITU/ISO/IEC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of the ITU/ISO/IEC nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/** \file     TEncCu.h
    \brief    Coding Unit (CU) encoder class (header)
*/

#ifndef __TENCCU__
#define __TENCCU__

// Include files
#include "TLibCommon/CommonDef.h"
#include "TLibCommon/TComYuv.h"
#include "TLibCommon/TComPrediction.h"
#include "TLibCommon/TComTrQuant.h"
#include "TLibCommon/TComBitCounter.h"
#include "TLibCommon/TComDataCU.h"

#include "TEncEntropy.h"
#include "TEncSearch.h"
#include "TEncRateCtrl.h"
#include "TShortYUV.h"

//! \ingroup TLibEncoder
//! \{

class TEncTop;
class TEncSbac;
class TEncCavlc;
class TEncSlice;

// ====================================================================================================================
// Class definition
// ====================================================================================================================

/// CU encoder class
class TEncCu
{
private:

    TComDataCU**            m_InterCU_2Nx2N;
    TComDataCU**            m_InterCU_2NxN;
    TComDataCU**            m_InterCU_Nx2N;
    TComDataCU**            m_IntraInInterCU;
    TComDataCU**            m_MergeCU;
    TComDataCU**            m_MergeBestCU;
    TComDataCU**            m_ppcBestCU;    ///< Best CUs in each depth
    TComDataCU**            m_ppcTempCU;    ///< Temporary CUs in each depth
    UChar                   m_uhTotalDepth;

    static const int MAX_PRED_TYPES = 6;

    TComYuv**               m_ppcPredYuvBest; ///< Best Prediction Yuv for each depth
    TShortYUV**             m_ppcResiYuvBest; ///< Best Residual Yuv for each depth
    TComYuv**               m_ppcRecoYuvBest; ///< Best Reconstruction Yuv for each depth
    TComYuv**               m_ppcPredYuvTemp; ///< Temporary Prediction Yuv for each depth
    TShortYUV**             m_ppcResiYuvTemp; ///< Temporary Residual Yuv for each depth
    TComYuv**               m_ppcRecoYuvTemp; ///< Temporary Reconstruction Yuv for each depth
    TComYuv**               m_ppcPredYuvMode[MAX_PRED_TYPES]; ///< Prediction buffers for inter, intra, rect(2) and merge
    TComYuv**               m_RecoYuvMergeBest;
    TComYuv**               m_ppcOrigYuv;     ///< Original Yuv for each depth

    //  Data : encoder control
    Bool                    m_bEncodeDQP;

    //  Access channel
    TEncCfg*                m_pcEncCfg;
    TEncSearch*             m_pcPredSearch;
    TComTrQuant*            m_pcTrQuant;
    TComBitCounter*         m_pcBitCounter;
    TComRdCost*             m_pcRdCost;

    TEncEntropy*            m_pcEntropyCoder;

    // SBAC RD
    TEncSbac***             m_pppcRDSbacCoder;
    TEncSbac*               m_pcRDGoOnSbacCoder;
    TEncRateCtrl*           m_pcRateCtrl;
    UInt                    m_LCUPredictionSAD;
    Int                     m_addSADDepth;
    Int                     m_temporalSAD;

    Bool                    m_abortFlag; // This flag is used to abort the recursive CU check when the child CU cost is greater than the parent CU

public:

    Void set_pppcRDSbacCoder(TEncSbac*** pppcRDSbacCoder) { m_pppcRDSbacCoder = pppcRDSbacCoder; }

    Void set_pcEntropyCoder(TEncEntropy* pcEntropyCoder) { m_pcEntropyCoder = pcEntropyCoder; }

    Void set_pcPredSearch(TEncSearch* pcPredSearch) { m_pcPredSearch = pcPredSearch; }

    Void set_pcRDGoOnSbacCoder(TEncSbac* pcRDGoOnSbacCoder) { m_pcRDGoOnSbacCoder = pcRDGoOnSbacCoder; }

    Void set_pcTrQuant(TComTrQuant* pcTrQuant) { m_pcTrQuant = pcTrQuant; }

    Void set_pcRdCost(TComRdCost* pcRdCost) { m_pcRdCost = pcRdCost; }

    /// copy parameters from encoder class
    Void  init(TEncTop* pcEncTop);

    /// create internal buffers
    Void  create(UChar uhTotalDepth, UInt iMaxWidth, UInt iMaxHeight);

    /// destroy internal buffers
    Void  destroy();

    /// CU analysis function
    Void  compressCU(TComDataCU* rpcCU);

    /// CU encoding function
    Void  encodeCU(TComDataCU* cu);

    Void setBitCounter(TComBitCounter* pcBitCounter) { m_pcBitCounter = pcBitCounter; }

    UInt getLCUPredictionSAD() { return m_LCUPredictionSAD; }

protected:

    Void  finishCU(TComDataCU* cu, UInt uiAbsPartIdx, UInt uiDepth);
    Void  xCompressCU(TComDataCU*& rpcBestCU, TComDataCU*& rpcTempCU, TComDataCU* rpcParentCU,  UInt uiDepth, UInt uiPartUnitIdx, PartSize eParentPartSize = SIZE_NONE);
    Void  xCompressIntraCU(TComDataCU*& rpcBestCU, TComDataCU*& rpcTempCU, TComDataCU* rpcParentCU,  UInt uiDepth, PartSize eParentPartSize = SIZE_NONE);
    Void  xCompressInterCU(TComDataCU*& rpcBestCU, TComDataCU*& rpcTempCU, TComDataCU*& cu, UInt uiDepth, UInt PartitionIndex);
    Void  xEncodeCU(TComDataCU* cu, UInt uiAbsPartIdx, UInt uiDepth);

    Int   xComputeQP(TComDataCU* cu, UInt uiDepth);
    Void  xCheckBestMode(TComDataCU*& rpcBestCU, TComDataCU*& rpcTempCU, UInt uiDepth);

    Void  xCheckRDCostMerge2Nx2N(TComDataCU*& rpcBestCU, TComDataCU*& rpcTempCU, Bool *earlyDetectionSkipMode, TComYuv*& rpcYuvPredBest, TComYuv*& rpcYuvReconBest);    
    Void  xComputeCostIntrainInter(TComDataCU*& rpcTempCU, PartSize eSize);
    Void  xCheckRDCostInter(TComDataCU*& rpcBestCU, TComDataCU*& rpcTempCU, PartSize ePartSize, Bool bUseMRG = false);
    Void  xComputeCostInter(TComDataCU*& rpcTempCU, PartSize ePartSize, UInt Index, Bool bUseMRG = false);
    Void  xEncodeIntrainInter(TComDataCU*& cu, TComYuv* fencYuv, TComYuv* predYuv, TShortYUV*& outResiYuv, TComYuv*& rpcYuvRec);
    Void  xCheckRDCostIntra(TComDataCU*& rpcBestCU, TComDataCU*& rpcTempCU, PartSize ePartSize);
    Void  xCheckRDCostIntrainInter(TComDataCU*& rpcBestCU, TComDataCU*& rpcTempCU, PartSize eSize);
    Void  xCheckDQP(TComDataCU* cu);

    Void  xCheckIntraPCM(TComDataCU*& rpcBestCU, TComDataCU*& rpcTempCU);
    Void  xCopyAMVPInfo(AMVPInfo* pSrc, AMVPInfo* pDst);
    Void  xCopyYuv2Pic(TComPic* rpcPic, UInt uiCUAddr, UInt uiAbsPartIdx, UInt uiDepth, UInt uiSrcDepth, TComDataCU* cu, UInt uiLPelX, UInt uiTPelY);
    Void  xCopyYuv2Tmp(UInt uhPartUnitIdx, UInt uiDepth);
    Void  xCopyYuv2Best(UInt uiPartUnitIdx, UInt uiNextDepth);

    Bool getdQPFlag()                      { return m_bEncodeDQP; }

    Void setdQPFlag(Bool b)                { m_bEncodeDQP = b; }

    // Adaptive reconstruction level (ARL) statistics collection functions
    Void xLcuCollectARLStats(TComDataCU* rpcCU);
    Int  xTuCollectARLStats(TCoeff* rpcCoeff, Int* rpcArlCoeff, Int NumCoeffInCU, Double* cSum, UInt* numSamples);

    Void deriveTestModeAMP(TComDataCU *&rpcBestCU, PartSize eParentPartSize, Bool &bTestAMP_Hor, Bool &bTestAMP_Ver, Bool &bTestMergeAMP_Hor, Bool &bTestMergeAMP_Ver);
    Void xFillPCMBuffer(TComDataCU*& pCU, TComYuv* pOrgYuv);
};

//! \}

#endif // __TENCMB__
