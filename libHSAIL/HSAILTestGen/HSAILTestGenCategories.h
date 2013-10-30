//==- HSAILTestGenSample.h - HSAIL Test Generator - Categories of Instructions --==//
//
//===----------------------------------------------------------------------===//
//
// (C) 2013 AMD Inc. All rights reserved.
//
//===----------------------------------------------------------------------===//

//F: define in HDL and make this file autogenerated

CategoryDesc baseCategories[] =
{
    // Standard Instructions

    {C_ART, Brig::BRIG_OPCODE_ADD},
    {C_ART, Brig::BRIG_OPCODE_SUB},
    {C_ART, Brig::BRIG_OPCODE_DIV},
    {C_ART, Brig::BRIG_OPCODE_MUL},
    {C_ART, Brig::BRIG_OPCODE_MULHI},
    {C_ART, Brig::BRIG_OPCODE_MAX},
    {C_ART, Brig::BRIG_OPCODE_MIN},
    {C_ART, Brig::BRIG_OPCODE_REM},
    {C_ART, Brig::BRIG_OPCODE_FLOOR},
    {C_ART, Brig::BRIG_OPCODE_TRUNC},
    {C_ART, Brig::BRIG_OPCODE_CEIL},
    {C_ART, Brig::BRIG_OPCODE_RINT},
    {C_ART, Brig::BRIG_OPCODE_CARRY},
    {C_ART, Brig::BRIG_OPCODE_BORROW},
    {C_ART, Brig::BRIG_OPCODE_COPYSIGN},
    {C_ART, Brig::BRIG_OPCODE_ABS},
    {C_ART, Brig::BRIG_OPCODE_NEG},
    {C_ART, Brig::BRIG_OPCODE_FRACT},
    {C_ART, Brig::BRIG_OPCODE_SQRT},
    {C_ART, Brig::BRIG_OPCODE_MAD},
    {C_ART, Brig::BRIG_OPCODE_FMA},
    {C_ART, Brig::BRIG_OPCODE_CLASS},
    {C_ART, Brig::BRIG_OPCODE_NCOS},
    {C_ART, Brig::BRIG_OPCODE_NSIN},
    {C_ART, Brig::BRIG_OPCODE_NLOG2},
    {C_ART, Brig::BRIG_OPCODE_NEXP2},
    {C_ART, Brig::BRIG_OPCODE_NRSQRT},
    {C_ART, Brig::BRIG_OPCODE_NSQRT},
    {C_ART, Brig::BRIG_OPCODE_NRCP},
    {C_ART, Brig::BRIG_OPCODE_NFMA},
    {C_ART, Brig::BRIG_OPCODE_CMP},
    {C_ART, Brig::BRIG_OPCODE_MUL24},
    {C_ART, Brig::BRIG_OPCODE_MUL24HI},
    {C_ART, Brig::BRIG_OPCODE_MAD24},
    {C_ART, Brig::BRIG_OPCODE_MAD24HI},
    {C_ART, Brig::BRIG_OPCODE_SHL},
    {C_ART, Brig::BRIG_OPCODE_SHR},
    {C_ART, Brig::BRIG_OPCODE_AND},
    {C_ART, Brig::BRIG_OPCODE_OR},
    {C_ART, Brig::BRIG_OPCODE_XOR},
    {C_ART, Brig::BRIG_OPCODE_NOT},
    {C_ART, Brig::BRIG_OPCODE_POPCOUNT},
    {C_ART, Brig::BRIG_OPCODE_FIRSTBIT},
    {C_ART, Brig::BRIG_OPCODE_LASTBIT},
    {C_ART, Brig::BRIG_OPCODE_BITINSERT},
    {C_ART, Brig::BRIG_OPCODE_BITSELECT},
    {C_ART, Brig::BRIG_OPCODE_BITREV},
    {C_ART, Brig::BRIG_OPCODE_BITEXTRACT},
    {C_ART, Brig::BRIG_OPCODE_LDA},
    {C_ART, Brig::BRIG_OPCODE_LDC},
    {C_ART, Brig::BRIG_OPCODE_SHUFFLE},
    {C_ART, Brig::BRIG_OPCODE_UNPACKLO},
    {C_ART, Brig::BRIG_OPCODE_UNPACKHI},
    {C_ART, Brig::BRIG_OPCODE_PACK},
    {C_ART, Brig::BRIG_OPCODE_UNPACK},
    {C_ART, Brig::BRIG_OPCODE_CMOV},
    {C_ART, Brig::BRIG_OPCODE_SEGMENTP},
    {C_ART, Brig::BRIG_OPCODE_FTOS},
    {C_ART, Brig::BRIG_OPCODE_STOF},
    {C_ART, Brig::BRIG_OPCODE_CVT},
    {C_ART, Brig::BRIG_OPCODE_UNPACKCVT},
    {C_ART, Brig::BRIG_OPCODE_PACKCVT},
    {C_ART, Brig::BRIG_OPCODE_BITALIGN},
    {C_ART, Brig::BRIG_OPCODE_BYTEALIGN},
    {C_ART, Brig::BRIG_OPCODE_BITMASK},
    {C_ART, Brig::BRIG_OPCODE_LERP},
    {C_ART, Brig::BRIG_OPCODE_SAD},
    {C_ART, Brig::BRIG_OPCODE_SADHI},

    {C_MOV, Brig::BRIG_OPCODE_MOV},
    {C_MOV, Brig::BRIG_OPCODE_COMBINE},
    {C_MOV, Brig::BRIG_OPCODE_EXPAND},

    {C_MEM_NAT, Brig::BRIG_OPCODE_LD},
    {C_MEM_NAT, Brig::BRIG_OPCODE_ST},

    {C_IMG_NAT, Brig::BRIG_OPCODE_RDIMAGE},
    {C_IMG_NAT, Brig::BRIG_OPCODE_LDIMAGE},
    {C_IMG_NAT, Brig::BRIG_OPCODE_STIMAGE},
    {C_IMG_NAT, Brig::BRIG_OPCODE_QUERYIMAGEWIDTH},
    {C_IMG_NAT, Brig::BRIG_OPCODE_QUERYIMAGEHEIGHT},
    {C_IMG_NAT, Brig::BRIG_OPCODE_QUERYIMAGEDEPTH},
    {C_IMG_NAT, Brig::BRIG_OPCODE_QUERYIMAGEARRAY},
    {C_IMG_NAT, Brig::BRIG_OPCODE_QUERYIMAGEORDER},
    {C_IMG_NAT, Brig::BRIG_OPCODE_QUERYIMAGEFORMAT},
    {C_IMG_NAT, Brig::BRIG_OPCODE_QUERYSAMPLERCOORD},
    {C_IMG_NAT, Brig::BRIG_OPCODE_QUERYSAMPLERFILTER},
    {C_IMG_NAT, Brig::BRIG_OPCODE_QUERYSAMPLERBOUNDARY},

    {C_MEM_ATM, Brig::BRIG_OPCODE_ATOMIC},
    {C_MEM_ATM, Brig::BRIG_OPCODE_ATOMICNORET},

    {C_IMG_ATM, Brig::BRIG_OPCODE_ATOMICIMAGE},
    {C_IMG_ATM, Brig::BRIG_OPCODE_ATOMICIMAGENORET},

    {C_BR, Brig::BRIG_OPCODE_CBR},
    {C_BR, Brig::BRIG_OPCODE_BRN},
    {C_BR, Brig::BRIG_OPCODE_RET},
    {C_BR, Brig::BRIG_OPCODE_CALL},
    {C_BR, Brig::BRIG_OPCODE_SYSCALL},

    {C_SYNC, Brig::BRIG_OPCODE_BARRIER},
    {C_SYNC, Brig::BRIG_OPCODE_SYNC},
    {C_SYNC, Brig::BRIG_OPCODE_INITFBAR},
    {C_SYNC, Brig::BRIG_OPCODE_JOINFBAR},
    {C_SYNC, Brig::BRIG_OPCODE_WAITFBAR},
    {C_SYNC, Brig::BRIG_OPCODE_ARRIVEFBAR},
    {C_SYNC, Brig::BRIG_OPCODE_RELEASEFBAR},
    {C_SYNC, Brig::BRIG_OPCODE_LEAVEFBAR},
    {C_SYNC, Brig::BRIG_OPCODE_LDF},
    {C_SYNC, Brig::BRIG_OPCODE_MASKLANE},
    {C_SYNC, Brig::BRIG_OPCODE_COUNTLANE},
    {C_SYNC, Brig::BRIG_OPCODE_COUNTUPLANE},
    {C_SYNC, Brig::BRIG_OPCODE_SENDLANE},
    {C_SYNC, Brig::BRIG_OPCODE_RECEIVELANE},

    {C_MSC, Brig::BRIG_OPCODE_ALLOCA},
    {C_MSC, Brig::BRIG_OPCODE_WORKITEMID},
    {C_MSC, Brig::BRIG_OPCODE_WORKITEMABSID},
    {C_MSC, Brig::BRIG_OPCODE_WORKGROUPID},
    {C_MSC, Brig::BRIG_OPCODE_WORKGROUPSIZE},
    {C_MSC, Brig::BRIG_OPCODE_GRIDSIZE},
    {C_MSC, Brig::BRIG_OPCODE_GRIDGROUPS},
    {C_MSC, Brig::BRIG_OPCODE_WORKITEMFLATID},
    {C_MSC, Brig::BRIG_OPCODE_WORKITEMFLATABSID},
    {C_MSC, Brig::BRIG_OPCODE_LANEID},
    {C_MSC, Brig::BRIG_OPCODE_WAVEID},
    {C_MSC, Brig::BRIG_OPCODE_MAXWAVEID},
    {C_MSC, Brig::BRIG_OPCODE_DISPATCHID},
    {C_MSC, Brig::BRIG_OPCODE_CUID},
    {C_MSC, Brig::BRIG_OPCODE_DIM},
    {C_MSC, Brig::BRIG_OPCODE_CLOCK},
    {C_MSC, Brig::BRIG_OPCODE_CURRENTWORKGROUPSIZE},
    {C_MSC, Brig::BRIG_OPCODE_DEBUGTRAP},
    {C_MSC, Brig::BRIG_OPCODE_NOP},
    {C_MSC, Brig::BRIG_OPCODE_SETDETECTEXCEPT},
    {C_MSC, Brig::BRIG_OPCODE_CLEARDETECTEXCEPT},
    {C_MSC, Brig::BRIG_OPCODE_GETDETECTEXCEPT},
    {C_MSC, Brig::BRIG_OPCODE_DISPATCHPTR},
    {C_MSC, Brig::BRIG_OPCODE_NULLPTR},
    {C_MSC, Brig::BRIG_OPCODE_QPTR},
    {C_MSC, Brig::BRIG_OPCODE_MAXCUID},
    {C_MSC, Brig::BRIG_OPCODE_QID},
    {C_MSC, Brig::BRIG_OPCODE_CODEBLOCKEND},

    // GCN Extension

    {C_GCN_ART, Brig::BRIG_OPCODE_GCNMADU},
    {C_GCN_ART, Brig::BRIG_OPCODE_GCNMADS},
    {C_GCN_ART, Brig::BRIG_OPCODE_GCNMAX3},
    {C_GCN_ART, Brig::BRIG_OPCODE_GCNMIN3},
    {C_GCN_ART, Brig::BRIG_OPCODE_GCNMED3},
    {C_GCN_ART, Brig::BRIG_OPCODE_GCNBFM},
    {C_GCN_ART, Brig::BRIG_OPCODE_GCNFLDEXP},
    {C_GCN_ART, Brig::BRIG_OPCODE_GCNFREXP_EXP},
    {C_GCN_ART, Brig::BRIG_OPCODE_GCNFREXP_MANT},
    {C_GCN_ART, Brig::BRIG_OPCODE_GCNTRIG_PREOP},

    {C_GCN_MEM_NAT, Brig::BRIG_OPCODE_GCNREGIONALLOC},
    {C_GCN_MEM_NAT, Brig::BRIG_OPCODE_GCNLD},
    {C_GCN_MEM_NAT, Brig::BRIG_OPCODE_GCNST},

    {C_GCN_MEM_ATM, Brig::BRIG_OPCODE_GCNATOMIC},
    {C_GCN_MEM_ATM, Brig::BRIG_OPCODE_GCNATOMICNORET},
    {C_GCN_MEM_ATM, Brig::BRIG_OPCODE_GCNAPPEND},
    {C_GCN_MEM_ATM, Brig::BRIG_OPCODE_GCNCONSUME},

    {C_GCN_MSK, Brig::BRIG_OPCODE_GCNMSAD},
    {C_GCN_MSK, Brig::BRIG_OPCODE_GCNQSAD},
    {C_GCN_MSK, Brig::BRIG_OPCODE_GCNMQSAD},
    {C_GCN_MSK, Brig::BRIG_OPCODE_GCNMQSAD4},
    {C_GCN_MSK, Brig::BRIG_OPCODE_GCNSADW},
    {C_GCN_MSK, Brig::BRIG_OPCODE_GCNSADD},
    {C_GCN_MSK, Brig::BRIG_OPCODE_GCNSLEEP},
    {C_GCN_MSK, Brig::BRIG_OPCODE_GCNPRIORITY},
    {C_GCN_MSK, Brig::BRIG_OPCODE_GCNB4XCHG},
    {C_GCN_MSK, Brig::BRIG_OPCODE_GCNB32XCHG},
};