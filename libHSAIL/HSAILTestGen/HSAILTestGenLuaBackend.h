//===-- HSAILTestGenLuaBackend.h - LUA Backend ----------------------------===//
//
//===----------------------------------------------------------------------===//
//
// (C) 2013 AMD Inc. All rights reserved.
//
//===----------------------------------------------------------------------===//

#ifndef INCLUDED_HSAIL_TESTGEN_LUA_BACKEND_H
#define INCLUDED_HSAIL_TESTGEN_LUA_BACKEND_H

#include "HSAILTestGenDataProvider.h"
#include "HSAILTestGenEmulator.h"
#include "HSAILTestGenBackend.h"
#include "HSAILTestGenOptions.h"
#include "HSAILDisassembler.h"

#include <sstream>
#include <fstream>
#include <iomanip>

using std::string;
using std::ostringstream;
using std::ofstream;
using std::setprecision;

using Brig::BRIG_TYPE_NONE;
using Brig::BRIG_TYPE_B1;
using Brig::BRIG_TYPE_B8;
using Brig::BRIG_TYPE_B32;
using Brig::BRIG_TYPE_B64;
using Brig::BRIG_TYPE_B128;
using Brig::BRIG_TYPE_S8;
using Brig::BRIG_TYPE_S16;
using Brig::BRIG_TYPE_S32;
using Brig::BRIG_TYPE_S64;
using Brig::BRIG_TYPE_U8;
using Brig::BRIG_TYPE_U16;
using Brig::BRIG_TYPE_U32;
using Brig::BRIG_TYPE_U64;
using Brig::BRIG_TYPE_F16;
using Brig::BRIG_TYPE_F32;
using Brig::BRIG_TYPE_F64;

using HSAIL_ASM::DirectiveKernel;
using HSAIL_ASM::DirectiveFunction;
using HSAIL_ASM::DirectiveExecutable;
using HSAIL_ASM::DirectiveVariable;
using HSAIL_ASM::DirectiveLabel;

using HSAIL_ASM::InstBasic;
using HSAIL_ASM::InstSourceType;
using HSAIL_ASM::InstAtomic;
using HSAIL_ASM::InstAtomicImage;
using HSAIL_ASM::InstBar;
using HSAIL_ASM::InstCmp;
using HSAIL_ASM::InstCvt;
using HSAIL_ASM::InstImage;
using HSAIL_ASM::InstMem;
using HSAIL_ASM::InstMod;
using HSAIL_ASM::InstBr;

using HSAIL_ASM::OperandReg;
using HSAIL_ASM::OperandRegVector;
using HSAIL_ASM::OperandImmed;
using HSAIL_ASM::OperandWavesize;
using HSAIL_ASM::OperandAddress;
using HSAIL_ASM::OperandArgumentList;
using HSAIL_ASM::OperandFunctionRef;
using HSAIL_ASM::OperandLabelRef;
using HSAIL_ASM::OperandFbarrierRef;

using HSAIL_ASM::isFloatType;
using HSAIL_ASM::isSignedType;
using HSAIL_ASM::getOperandType;

namespace TESTGEN {

//==============================================================================
//==============================================================================
//==============================================================================

#define LUA_SEPARATOR ("--------------------------------------------------\n")
#define LUA_COMMENT   ("--- ")
#define LUA_FILE_EXT  (".lua")

//==============================================================================
//==============================================================================
//==============================================================================
// Container for storing test values

class TestDataBundle
{
    //==========================================================================
private:
    static const unsigned MAX_BNDLS   = 64;                     // Max number of elements in the bundle (LUA limitation)
    static const unsigned ELEM_SIZE   = 6;                      // 5 dst/src values + 1 mem value
    static const unsigned MAX_SIZE    = MAX_BNDLS * ELEM_SIZE;  // Max number of elements in all bundles (LUA limitation)
    static const unsigned DST_IDX     = 0;                      // Dst is always 0-th operand
    static const unsigned MEM_IDX     = 5;                      // Mem index
    static const unsigned MAX_OPR_IDX = 4;                      // Max operand index

    //==========================================================================
private:
    Val data[MAX_SIZE];                                         // Array for storing test values
    unsigned size;                                              // Number of used elements (not values!) in the array (multiple of ELEM_SIZE)
    unsigned pos;                                               // Current position in the bundle

    //==========================================================================
public:
    TestDataBundle()      { clear(); }

    //==========================================================================
    // PUBLIC INTERFACE
    // 1. ADDING DATA:
    //      clear() -> expand() -> (addXXX(...), addXXX(...)...) -> expand() -> (addXXX(...), addXXX(...)...) -> ...
    // 2. READING DATA:
    //      resetPos() -> (getXXX(), getXXX(), ...) -> next()  -> (getXXX(), getXXX(), ...) -> ...
    //
public: 
    void resetPos()       { pos = 0; }
    void clear()          { size = 0; resetPos(); }
    bool expand()         { assert(empty() || valid(0)); if (!full()) { pos = size; size += ELEM_SIZE; return true; } return false; }
    bool next()           { assert(empty() || valid(0)); if (pos + ELEM_SIZE < size) { pos += ELEM_SIZE; return true; } return false; }
    bool full()     const { return size == MAX_SIZE; } 
    bool empty()    const { return size == 0; }

    unsigned getSize()                    const { assert(empty() || valid(0)); return size / ELEM_SIZE; }
                                                
    void setSrcVal(unsigned idx, Val val)       { assert(valid(idx)); data[pos + idx]     = val; }
    void setDstVal(Val val)                     { assert(valid(0));   data[pos + DST_IDX] = val; }
    void setMemVal(Val val)                     { assert(valid(0));   data[pos + MEM_IDX] = val; }

    Val getSrcVal(unsigned idx)           const { assert(valid(idx)); return data[pos + idx];     }
    Val getDstVal()                       const { assert(valid(0));   return data[pos + DST_IDX]; }
    Val getMemVal()                       const { assert(valid(0));   return data[pos + MEM_IDX]; }

    unsigned getSrcValType(unsigned idx)  const { assert(valid(idx)); return getType(idx);     } // Type of first element in the bundle
    unsigned getDstValType()              const { assert(valid(0));   return getType(DST_IDX); }
    unsigned getMemValType()              const { assert(valid(0));   return getType(MEM_IDX); }

    unsigned getSrcValDim(unsigned idx)   const { assert(valid(idx)); return getDim(idx);     } // Type of first element in the bundle
    unsigned getDstValDim()               const { assert(valid(0));   return getDim(DST_IDX); }
    unsigned getMemValDim()               const { assert(valid(0));   return getDim(MEM_IDX); }

    //==========================================================================
private:
    bool     valid(unsigned idx)   const { return pos < size && size <= MAX_SIZE && 0 <= idx && idx <= MAX_OPR_IDX; }
    unsigned getType(unsigned idx) const { return data[idx].isVector()? data[idx].getVecType() : data[idx].getType(); }
    unsigned getDim(unsigned idx)  const { return data[idx].getDim(); }
    //==========================================================================
};

//==============================================================================
//==============================================================================
//==============================================================================

class LuaTestGen : public TestGenBackend
{
    //==========================================================================
    // Register map for generated code is as follows:
    //
    //   --------------------------------------------------------------
    //   Registers              Usage
    //   --------------------------------------------------------------
    //   $c0  $s0  $d0  $q0     0-th argument of test instruction
    //   $c1  $s1  $d1  $q1     1-th argument of test instruction
    //   $c2  $s2  $d2  $q2     2-th argument of test instruction
    //   $c3  $s3  $d3  $q3     3-th argument of test instruction
    //   $c4  $s4  $d4  $q4     4-th argument of test instruction
    //   --------------------------------------------------------------
    //        $s5               Temporary               (REG_IDX_TMP)
    //        $s6  $d6          Temporary array address (REG_IDX_ADDR)
    //        $s7  $d7          Workitem ID             (REG_IDX_ID)
    //        $s8  $d8          First index  = id * X1  (REG_IDX_IDX1)
    //        $s9  $d9          Second index = id * X2  (REG_IDX_IDX2)
    //   --------------------------------------------------------------
    //        $s10 $d10         first  vector register  (REG_IDX_VEC)
    //        $s11 $d11         second vector register  (REG_IDX_VEC + 1)
    //        $s12 $d12         third  vector register  (REG_IDX_VEC + 2)
    //        $s13 $d13         fourth vector register  (REG_IDX_VEC + 3)
    //   --------------------------------------------------------------
    //
    //   Other registers are not used 
    //   
private:
    static const unsigned REG_IDX_TMP   = 5;
    static const unsigned REG_IDX_ADDR  = 6;
    static const unsigned REG_IDX_ID    = 7;
    static const unsigned REG_IDX_IDX1  = 8;
    static const unsigned REG_IDX_IDX2  = 9;
    static const unsigned REG_IDX_VEC   = 10;

    //==========================================================================
private:
    static const unsigned OPERAND_IDX_DST = 0;  // Index of destination operand (if any)

    //==========================================================================
private:
    Inst testSample;                            // Instruction generated for this test. It MUST NOT be modified!
    BrigContext *context;                       // Brig context for code generation
    TestDataProvider *provider;                 // Provider of test data

    DirectiveVariable memTestArray;             // Array allocated for testing memory access
    TestDataBundle bundle;                      // Group of test data bundled together

    string testPath;                            // Path to a folder where current test is to be saved
    string testName;                            // Test name (without extension)

    //==========================================================================
public:
    LuaTestGen()
    {
        context = 0;
        provider = 0;
    }

    //==========================================================================
    // Backend Interface Implementation
public:

    // Called to check if tests shall be generated for the specified instruction.
    // If returned value is true, there is at least one test in this group.
    //
    // inst: the instruction which will be generated for this test.
    //       Backend may inspect it but MUST NOT modify it.
    //
    bool startTestGroup(Inst readOnlyInst)
    {
        testSample = readOnlyInst;

        if (testableInst(readOnlyInst) && testableOperands(readOnlyInst))
        {
            // Create a provider of test data for the current instruction.
            // Providers are selected based on data type of each operand.
            // Supported operand types and their test values must be defined
            // for each instruction in HSAILTestGenLuaTestData.h
            provider = getProvider(readOnlyInst);   
            return provider != 0;
        }
        else
        {
             return false;
        }
    }

    // Called when all test for instruction have been generated.
    // This is a good place for backend cleanup
    void endTestGroup()
    {
        // Cleanup: important for proxies
        testSample = Inst();
        delete provider;
        provider = 0;
    }

    // Called to generate data for the next test.
    // Return true if data for next test were generated; false if there are no more test data
    bool genNextTestData()
    {
        assert(provider);
        return provider->nextBundle();
    }

    // Called after current context is created but before generation of test kernel.
    // This is a convenient place for backend to generate auxiliary variables, functions, etc.
    //
    // context:  BRIG context used for test generation.
    //           backend may save this context internally, however, it cannot be used after
    //           endKernelBody is called.
    // path:     path to the folder where tests will be saved, e.g. "d:\TEST\"
    //           It is guaranteed that the path ends with a separator "/" or "\"
    // testName: file name (without extension) which will be used for this test, e.g. "abs_000"
    // ext:      file extension, e.g. ".brig"
    //
    // Return true on success and false if this test shall be skipped.
    //
    bool startTest(BrigContext* ctx, string path, string fileName, string extension)
    {
        if (!bundleTestData()) return false; // emulation failed

        context  = ctx;
        testPath = path;
        testName = fileName;

        createMemTestArray(); // Create an array for testing memory access (if required)
        
        return true;
    }

    // Called just before context destruction
    // This is a good place for backend cleanup
    void endTest()
    {
        // Cleanup: important for proxies
        memTestArray = DirectiveVariable();
        bundle.clear();
        context = 0;
    }

    // This function declares kernel arguments.
    // These arguments are addresses of src, dst and mem arrays.
    // Src arrays are initialized by LUA script with test data.
    // The number of src arrays is the same as the number 
    // of src arguments of instruction being tested.
    // Dst array is used by kernel to save the value in dst register 
    // after execution of test instruction. This array is created only 
    // if the instruction being tested has destination.
    // Mem array is used by kernel to save the value in memory 
    // after execution of test instruction. This array is created only 
    // if the instruction being tested affects memory.
    // Results in dst and/or mem arrays are compared 
    // by LUA script with expected values.
    void defKernelArgs()
    {
        for (int i = provider->getFirstSrcOperandIdx(); i <= provider->getLastOperandIdx(); i++)
        {
            context->emitArg(getModelTypeU(), getSrcArrayName(i, "%"), Brig::BRIG_SEGMENT_KERNARG); 
        }
        if (hasDstOperand())    context->emitArg(getModelTypeU(), getDstArrayName("%"), Brig::BRIG_SEGMENT_KERNARG); 
        if (hasMemoryOperand()) context->emitArg(getModelTypeU(), getMemArrayName("%"), Brig::BRIG_SEGMENT_KERNARG); 
    }

    // Called after test kernel is defined but before generation of first kernel instruction.
    void startKernelBody()
    {
        assert(provider);

        emitCommentSeparator();
        CommentBrig commenter(context);
        emitTestDescription(commenter);

        emitLoadId(); // Load workitem id (used as an index to arrays with test data)
        emitInitCode();

        emitCommentHeader("This is the instruction being tested:");
    }

    // Called just before generation of "ret" instruction for test kernel
    void endKernelBody()
    {
        assert(provider);

        saveTestResults();
        emitCommentSeparator();
        genLuaScript();
    }

    // Called after generation of test instruction.
    // This is the place for backend to create a new test based on the specified instruction.
    // inst: the instruction being tested.
    void makeTest(Inst inst)
    {
        assert(inst);
        assert(provider);
        assert(!bundle.empty());

        // Use first set of test data for HSAIL code.
        // This is not really important as all sets have 
        // the same values for data being embedded into code
        // (imm operands etc).
        bundle.resetPos(); 

        // Generate operands for test instruction based on test values from provider
        for (int i = provider->getFirstOperandIdx(); i <= provider->getLastOperandIdx(); i++)
        {
            Operand operand = inst.operand(i);
            assert(operand);
            
            if (OperandReg reg = operand)               // i-th operand must be either i-th register of the corresponding type...
            {
                inst.operand(i) = getOperandReg(i);
            }
            else if (OperandRegVector reg = operand)    // ... or a vector of registers ...
            {
                inst.operand(i) = getOperandRegVector(i);
            }
            else if (OperandImmed immed = operand)      // ... or an immediate...
            {
                inst.operand(i) = context->emitImm(immed.type(), bundle.getSrcVal(i).conv2b64());
            }
            else if (OperandAddress addr = operand)
            {
                assert(!addr.reg() && addr.offset() == 0);
                inst.operand(i) = getMemTestArrayAddr();
            }
            else if (OperandWavesize ws = operand)      // ... or WaveSize.
            {
                // TestDataProvider does not know operand type for i-th value.
                // The value returned by getSrcValue may be inappropriate for some reason,
                // e.g. LUA may limit wavesize to values in the range [1, 64].
                // Another possible problem is that the current instruction may
                // have several WaveSize operands; in this case they have to be identical.
                //
                // If the value returned by getSrcValue is unsuitable for WaveSize,
                // return from this function with false and reject this test case.
                //
                // Othewise, generate additional LUA directive to set WaveSize to this value.
                
                assert(false); // currently not supported
            }
            else
            {
                assert(false); // currently not supported
            }
        }
    }

    //==========================================================================
    // Kernel code generation
private:

    void emitLoadId()
    {
        emitCommentHeader("Load workitem ID");
        initIdReg();
    }

    // Generate initialization code for all input registers and test variables
    void emitInitCode()
    {
        for (int i = provider->getFirstSrcOperandIdx(); i <= provider->getLastOperandIdx(); ++i)
        {
            Operand operand = testSample.operand(i);
            assert(operand);

            if (OperandReg reg = operand)
            {
                emitCommentHeader("Initialization of input register " + getName(getOperandReg(i)));
                initSrcVal(getOperandReg(i), getSrcArrayIdx(i));
            }
            else if (OperandRegVector reg = operand)
            {
                emitCommentHeader("Initialization of input vector " + getName(getOperandRegVector(i)));
                initSrcVal(getOperandRegVector(i), getSrcArrayIdx(i));
            }
            else if (OperandAddress(operand))
            {
                emitCommentHeader("Initialization of memory");
                initMemTestArray(getSrcArrayIdx(i));
            }
        }

        if (hasMemoryOperand())
        {
            emitCommentHeader("Initialization of index register for memory access");
            initMemTestArrayIndexReg();
        }
    }

    void saveTestResults()
    {
        if (hasDstOperand())
        {
            Operand sampleDst = testSample.operand(OPERAND_IDX_DST);
            if (OperandReg(sampleDst))
            {
                OperandReg dst = getOperandReg(OPERAND_IDX_DST);
                emitCommentHeader("Saving dst register " + getName(dst));
                saveDstVal(dst, getDstArrayIdx());
            }
            else if (OperandRegVector(sampleDst))
            {
                OperandRegVector dst = getOperandRegVector(OPERAND_IDX_DST);
                emitCommentHeader("Saving dst vector " + getName(dst));
                saveDstVal(dst, getDstArrayIdx());
            }
            else
            {
                assert(false);
            }
        }

        if (hasMemoryOperand())
        {
            emitCommentHeader("Saving mem result");
            saveMemTestArray(getMemArrayIdx());
        }
    }

    //==========================================================================
    // Helpers for bundling tests together
private:

    bool bundleTestData()
    {
        Val src[5];
        Val dst;
        Val mem;

        bundle.clear();

        for(;;)
        {            
            for (int i = 0; i < 5; ++i) src[i] = provider->getSrcValue(i);              // Read current set of test data
            
            dst = emulateDstVal(testSample, src[0], src[1], src[2], src[3], src[4]);    // Return an empty value if emulation failed or there is no dst value
            mem = emulateMemVal(testSample, src[0], src[1], src[2], src[3], src[4]);    // Return an empty value if emulation failed or there is no mem value

            if ((!dst.empty() == hasDstOperand()) &&                                    // Check that all expected results have been provided by emulator
                (!mem.empty() == hasMemoryOperand()))                                   // Add to bundle unless emulation failed
            {
                bundle.expand();
                for (unsigned i = 0; i < 5; ++i) bundle.setSrcVal(i, src[i]);
                if (hasDstOperand())             bundle.setDstVal(dst);
                if (hasMemoryOperand())          bundle.setMemVal(mem);
            }
            
            // Request next set of test data for this bundle, if any    
            if (bundle.full() || !provider->next()) break;
        }

        return !bundle.empty();
    }

    //==========================================================================
    // LUA script generation helpers
private:

    // ------------------------------------------------------------------------
    // Printer for src values

    struct LuaSrcPrinter
    { 
    private:
        ostringstream s;
    
    public:
        void setPrecision(unsigned p) { if (p != 0) s << setprecision(p); }
        template<typename T> 
        void operator()(T val, unsigned p = 0, const char* sep = ", ") { 
            setPrecision(p); 
            s << val << sep; 
        }
        string operator()() { 
            string res = s.str();
            return res.substr(0, res.length() - strlen(", ")); // remove last separator
        }
    };

    // ------------------------------------------------------------------------
    // Printer for dst values

    struct LuaDstPrinter
    {
    private:
        ostringstream s;
        string checkName;
        unsigned slot;
        
    public:
        LuaDstPrinter(string name, unsigned firstSlot = 0) : checkName(name), slot(firstSlot) {}
        
    public:
        void setPrecision(unsigned p) { if (p != 0) s << setprecision(p); }
        template<typename T> 
        void operator()(T val, unsigned p = 0) { 
            setPrecision(p); 
            s << "result_array_check_set(" << checkName << ", " << slot++ << ", " << val << ")\n"; 
        }
        string operator()() { return s.str(); }
    };

    // ------------------------------------------------------------------------

    string nan2lua(Val val)
    {
        assert(!val.empty());
        assert(val.isFloat());
        assert(val.isInf() || val.isNan());

        return val.isNan()? "NAN" : val.isPositive()? "INF" : "-INF";
    }

    bool isSignedLuaType(unsigned type) { return isSignedType(type) && getTypeBitSize(type) <= 32; }

    template<class T>
    void val2lua(T& printer, Val v)
    {
        assert(!v.empty());
        
        for (unsigned i = 0; i < v.getDim(); ++i)
        {
            assert(!v[i].isVector());

            Val val = v[i];

            if (val.isFloat())
            {
                if (val.isSpecialFloat()) printer(nan2lua(val));
                else if (val.isX64())     printer(val.f64(), 16);
                else                      printer(val.f32(), 8);
            }
            else if (isSignedLuaType(val.getType()))
            {
                printer(static_cast<s32_t>(val.conv2b32()));    // Subword values are stored as INT32
            }
            else
            {
                                 printer(val.conv2b32());       // Subword values are stored as UINT32
                if (val.isX64()) printer(val.conv2b32hi());     // 64-bit b/s/u values are stored as 2 UINT32
            }
        }
    }

    //==========================================================================
    // LUA script generation
    //
    // Generated script looks like this (details may vary):
    // ----------------------------------------------------------
    //      require "helpers"
    //      
    //      local threads = 1
    //      thread_group = T{0, 0, threads, 1}
    //      
    //      src1 = new_global_array(UINT32, 2)                      // Declaration of an array for storing test results
    //      array_set_all(src1, { 1 })                              // Initialization of array with test data
    //      array_print(src1, "Array with test values for src1")    // Request test data dump
    //      new_arg(REF, src1)                                      // Declaration of this array as kernel argument
    //      
    //      dst = new_global_array(UINT32, 72)                      // Declaration of an array for storing test results
    //      new_arg(REF, dst)                                       // Declaration of this array as kernel argument
    //      dst_check = new_result_array_check(dst)                 // Request Emulator to compare data in this array with expected data
    //      result_array_check_set(dst_check, 0, 0)                 // Declaration of expected data
    //      result_array_check_print(dst_check, "Array with expected dst values")
    //      
    // ----------------------------------------------------------
    //
private:

    void genLuaDesc(ofstream& os)
    { 
        if (enableComments)
        {
            CommentLua commenter;
            emitTestDescription(commenter);
            os << LUA_SEPARATOR << commenter.str() << LUA_SEPARATOR << "\n";
        }
    }

    void genLuaHeader(ofstream& os)
    { 
        os << "require \"helpers\"\n\n"
           << "local threads = " << bundle.getSize() << "\n"
           << "thread_group = T{0, 0, threads, 1}\n\n";
    }

    void defLuaArray(ofstream& os, string name, unsigned type, unsigned dim)
    {
        // Subword values are represented as 32-bit values
        // s64/u64 values are represented as 2 32-bit values because of LUA limitations
        unsigned    typeSize  = getTypeBitSize(type);
        const char* arrayType = isFloatType(type)? (typeSize == 64? "DOUBLE" : "FLOAT") 
                              : isSignedLuaType(type) ?             "INT32"  : "UINT32";
        unsigned    arraySize = isFloatType(type)? 1 : (typeSize == 64? 2 : 1);

        os << name << " = new_global_array(" << arrayType  << ", " << arraySize * bundle.getSize() * dim << ")\n";
    }

    void initLuaArray(ofstream& os, string name, unsigned operandIdx)
    {
        assert(!bundle.empty());

        LuaSrcPrinter printer;
        bundle.resetPos();

        os << "array_set_all(" << name << ", ";

        if (bundle.getSize() == 1)
        {
            val2lua(printer, bundle.getSrcVal(operandIdx));
            os << "{ " << printer() << " }";
        }
        else 
        {
            os << "\n              {";
            for (;;)
            {
                printer("\n                  ", 0, "");
                val2lua(printer, bundle.getSrcVal(operandIdx));
                if (!bundle.next()) break;
            }
            os << printer() 
               << "\n              }\n";
        }

        os << ")\n";
    }

    void defLuaKernelArg(ofstream& os, string name)
    {
        os << "new_arg(REF, " << name << ")\n";
    }

    void defLuaCheckRules(ofstream& os, string checkName, string arrayName, unsigned type)
    {
        assert(type != BRIG_TYPE_F16);
        double precision = getPrecision(testSample);

        os << checkName << " = new_result_array_check(" << arrayName;
        if (precision != 0) os << ", " << precision << ", CM_RELATIVE"; // FIXME: try using CM_ULPS
        os << ")\n";
    }

    void printSrcLuaArray(ofstream& os, string name)
    {
        os << "array_print(" << name << ", \"Array with test values for " << name << "\")\n";
    }

    void printResLuaArray(ofstream& os, string checkName, string valKind)
    {
        os << "result_array_check_print(" << checkName << ", \"Array with expected " << valKind << " values\")\n";
    }

    void defLuaChecks(ofstream& os, string checkName, bool isDst)
    {
        assert(!bundle.empty());

        LuaDstPrinter printer(checkName);
        bundle.resetPos();

        for (;;)
        {
            val2lua(printer, isDst? bundle.getDstVal() : bundle.getMemVal());
            if (!bundle.next()) break;
        }

        os << printer();
    }

    void defSrcLuaArray(ofstream& os, unsigned operandIdx)
    {
        string name = getSrcArrayName(operandIdx);
        defLuaArray(os, name, bundle.getSrcValType(operandIdx), bundle.getSrcValDim(operandIdx));
        initLuaArray(os, name, operandIdx);
        printSrcLuaArray(os, name);
        defLuaKernelArg(os, name);
        os << "\n";
    }

    void defDstLuaArray(ofstream& os)
    {
        defResultLuaArray(os, "dst", "dst_check", getDstArrayName(), bundle.getDstValType(), bundle.getDstValDim(), true);
    }

    void defMemLuaArray(ofstream& os)
    {
        defResultLuaArray(os, "mem", "mem_check", getMemArrayName(), bundle.getMemValType(), bundle.getMemValDim(), false);
    }

    void defResultLuaArray(ofstream& os, string valKind, string checkName, string arrayName, unsigned type, unsigned dim, bool isDst)
    {
        defLuaArray(os, arrayName, type, dim);
        defLuaKernelArg(os, arrayName);
        defLuaCheckRules(os, checkName, arrayName, type);        
        defLuaChecks(os, checkName, isDst);
        printResLuaArray(os, checkName, valKind);
        os << "\n";
    }

    void genLuaScript()
    {
        ofstream os;
        string LUApath = testPath + testName + LUA_FILE_EXT;
        os.open(LUApath.c_str());
        if (os.bad()) throw TestGenError("Failed to create " + LUApath);

        assert(!bundle.empty());

        genLuaDesc(os);
        genLuaHeader(os);

        for (int i = provider->getFirstSrcOperandIdx(); i <= provider->getLastOperandIdx(); ++i)
        {
            defSrcLuaArray(os, i);
        }

        if (hasDstOperand())
        {
            defDstLuaArray(os);
        }

        if (hasMemoryOperand()) 
        {
            defMemLuaArray(os);
        }

        os.close();
    }

    //==========================================================================
    // Access to registers
private:

    OperandReg getTmpReg(unsigned type)  { return context->emitReg(type2b(type),    REG_IDX_TMP);   }
    OperandReg getAddrReg()              { return context->emitReg(getModelTypeB(), REG_IDX_ADDR);  }
    OperandReg getIdReg(unsigned type)   { return context->emitReg(type2b(type),    REG_IDX_ID);    }

    OperandReg getIdxReg(unsigned type, unsigned idx) { if (type == BRIG_TYPE_NONE) type = getModelTypeB(); return context->emitReg(type2b(type), idx); }
    OperandReg getIdxReg1(unsigned type = BRIG_TYPE_NONE) { return getIdxReg(type, REG_IDX_IDX1); }
    OperandReg getIdxReg2(unsigned type = BRIG_TYPE_NONE) { return getIdxReg(type, REG_IDX_IDX2); }
   
    OperandReg getOperandReg(unsigned idx) // Create register for i-th operand of test instruction.  
    {                                      
        assert(0 <= idx && idx <= 4);

        OperandReg reg = testSample.operand(idx); // NB: this register is read-only!

        assert(reg);
        assert(reg.type() == BRIG_TYPE_B1 || reg.type() == BRIG_TYPE_B32 || reg.type() == BRIG_TYPE_B64);

        return context->emitReg(reg.type(), idx); // NB: create register in CURRENT context
    }

    OperandRegVector getOperandRegVector(unsigned idx)  // Create register vector for i-th operand of test instruction.  
    {                                                   
        assert(0 <= idx && idx <= 4);
        assert(OperandRegVector(testSample.operand(idx)));

        OperandRegVector reg = testSample.operand(idx); // NB: this vector is read-only!
        assert(reg.type() == BRIG_TYPE_B32 || reg.type() == BRIG_TYPE_B64);

        return context->emitRegVector(reg.regCount(), reg.type(), REG_IDX_VEC); // NB: create vector in CURRENT context
    }

    //==========================================================================
    // Operations with index registers (used to access array elements)
private:

    void initIdReg()
    {
        // Load workitem id
        context->emitGetWorkItemId(getIdReg(BRIG_TYPE_B32), 0); // Id for 0th dimension

        if (isLargeModel()) // Convert to U64 if necessary
        {
            context->emitCvt(BRIG_TYPE_U64, BRIG_TYPE_U32, getIdReg(BRIG_TYPE_B64), getIdReg(BRIG_TYPE_B32));
        }
    }

    OperandReg loadIndexReg(OperandReg idxReg, unsigned dim, unsigned elemType)
    {
        unsigned addrType = idxReg.type();
        if (elemType == BRIG_TYPE_B1) elemType = BRIG_TYPE_B32; // b1 is a special case, always stored as b32
        context->emitMul(b2u(addrType), idxReg, getIdReg(addrType), dim * getTypeByteSize(elemType));
        return idxReg;
    }

    //==========================================================================
    // Low-level operations with arrays
private:

    OperandReg loadGlobalArrayAddress(OperandReg addrReg, OperandReg indexReg, unsigned arrayIdx)
    {
        assert(addrReg);
        assert(indexReg);
        assert(addrReg.type() == indexReg.type());

        context->emitLd(getModelTypeU(), Brig::BRIG_SEGMENT_KERNARG, addrReg, context->emitAddrRef(getArray(arrayIdx)));
        context->emitAdd(getModelTypeU(), addrReg, addrReg, indexReg);
        return addrReg;
    }

    unsigned getSrcArrayIdx(unsigned idx)
    {
        assert(static_cast<unsigned>(provider->getFirstSrcOperandIdx()) <= idx);
        assert(idx <= static_cast<unsigned>(provider->getLastOperandIdx()));

        return idx - provider->getFirstSrcOperandIdx();
    }

    unsigned getDstArrayIdx()
    {
        assert(hasDstOperand());
        return provider->getLastOperandIdx() - provider->getFirstSrcOperandIdx() + 1;
    }

    unsigned getMemArrayIdx()
    {
        assert(hasMemoryOperand());
        return provider->getLastOperandIdx() - provider->getFirstSrcOperandIdx() + (hasDstOperand()? 2 : 1);
    }

    DirectiveVariable getArray(unsigned idx)
    {
        return HSAIL_ASM::getInputArg(context->getLastKernel(), idx);
    }

    //==========================================================================
    // Operations with src/dst arrays
private:

    void initSrcVal(OperandReg reg, unsigned arrayIdx)
    {
        assert(reg);

        OperandReg indexReg = loadIndexReg(getIdxReg1(), 1, reg.type());
        OperandReg addrReg  = loadGlobalArrayAddress(getAddrReg(), indexReg, arrayIdx);
        OperandAddress addr = context->emitIndirRef(addrReg);
        ldReg(reg.type(), reg, addr);
    }

    void initSrcVal(OperandRegVector vector, unsigned arrayIdx)
    {
        assert(vector);

        unsigned dim      = vector.regCount();
        unsigned elemType = vector.type();

        OperandReg indexReg = loadIndexReg(getIdxReg1(), dim, elemType);
        OperandReg addrReg  = loadGlobalArrayAddress(getAddrReg(), indexReg, arrayIdx);

        for (unsigned i = 0; i < dim; ++i)
        {
            OperandAddress addr = context->emitIndirRef(addrReg, getSlotSize(elemType) * i);
            ldReg(elemType, context->emitReg(elemType, vector.regs(i)), addr);
        }
    }

    void saveDstVal(OperandReg reg, unsigned arrayIdx)
    {
        assert(reg);

        OperandReg indexReg = loadIndexReg(getIdxReg1(), 1, reg.type());
        OperandReg addrReg  = loadGlobalArrayAddress(getAddrReg(), indexReg, arrayIdx);
        OperandAddress addr = context->emitIndirRef(addrReg);
        stReg(reg.type(), reg, addr);
    }

    void saveDstVal(OperandRegVector vector, unsigned arrayIdx)
    {
        assert(vector);

        unsigned dim      = vector.regCount();
        unsigned elemType = vector.type();

        OperandReg indexReg = loadIndexReg(getIdxReg1(), dim, elemType);
        OperandReg addrReg  = loadGlobalArrayAddress(getAddrReg(), indexReg, arrayIdx);

        for (unsigned i = 0; i < dim; ++i)
        {
            OperandAddress addr = context->emitIndirRef(addrReg, getSlotSize(elemType) * i);
            stReg(elemType, context->emitReg(elemType, vector.regs(i)), addr);
        }
    }

    void ldReg(unsigned elemType, OperandReg reg, OperandAddress addr)
    {
        assert(reg);
        assert(addr);

        if (elemType == BRIG_TYPE_B1)
        {
            OperandReg tmpReg = getTmpReg(BRIG_TYPE_B32);
            context->emitLd(type2LdSt(BRIG_TYPE_B32), Brig::BRIG_SEGMENT_GLOBAL, tmpReg, addr);
            context->emitCvt(BRIG_TYPE_B1, BRIG_TYPE_U32, reg, tmpReg);
        }
        else
        {
            context->emitLd(type2LdSt(elemType), Brig::BRIG_SEGMENT_GLOBAL, reg, addr);
        }
    }

    void stReg(unsigned elemType, OperandReg reg, OperandAddress addr)
    {
        assert(reg);
        assert(addr);
       
        if (elemType == BRIG_TYPE_B1)
        {
            OperandReg tmpReg = getTmpReg(BRIG_TYPE_B32);
            context->emitCvt(BRIG_TYPE_U32, BRIG_TYPE_B1, tmpReg, reg);
            context->emitSt(type2LdSt(BRIG_TYPE_B32), Brig::BRIG_SEGMENT_GLOBAL, tmpReg, addr);
        }
        else
        {
            context->emitSt(type2LdSt(elemType), Brig::BRIG_SEGMENT_GLOBAL, reg, addr);
        }
    }

    //==========================================================================
    // Operations with memory test array (required for instructions which access memory)
private:

    bool hasMemoryOperand()
    {
        for (int i = 0; i < 5 && testSample.operand(i); ++i)
        {
            if (OperandAddress(testSample.operand(i))) return true;
        }
        return false;
    }

    void createMemTestArray()
    {                              
        if (hasMemoryOperand()) 
        {
            assert(HSAIL_ASM::getSegment(testSample) != Brig::BRIG_SEGMENT_NONE);
            memTestArray = context->emitSymbol(testSample.type(), getTestArrayName(), HSAIL_ASM::getSegment(testSample), bundle.getSize() * getMaxDim());
        }
    }

    void copyMemTestArray(unsigned arrayIdx, bool isDst) 
    {
        assert(memTestArray);

        unsigned glbAddrType = getModelTypeB();
        unsigned memAddrType = context->getSegAddrTypeB(memTestArray.segment());

        unsigned elemType    = memTestArray.type();
        unsigned slotType    = getSlotTypeB(elemType);
        OperandReg reg       = getTmpReg(slotType);
        unsigned dim         = getMaxDim();

        OperandReg indexReg1 = loadIndexReg(getIdxReg1(glbAddrType), dim, slotType);
        OperandReg indexReg2 = indexReg1; // Reuse first index register if possible
        if (getTypeBitSize(glbAddrType) != getTypeBitSize(memAddrType) ||
            getTypeBitSize(slotType)    != getTypeBitSize(elemType)) 
        {
            indexReg2 = loadIndexReg(getIdxReg2(memAddrType), dim, elemType);
        }

        OperandReg addrReg   = loadGlobalArrayAddress(getAddrReg(), indexReg1, arrayIdx);

        for (unsigned i = 0; i < dim; ++i)
        {
            OperandAddress addr = context->emitIndirRef(addrReg, getSlotSize(elemType) * i);
            if (isDst)
            {
                ldReg(slotType, reg, addr);
                context->emitSt(type2LdSt(elemType), memTestArray.segment(), reg, getMemTestArrayAddr(indexReg2, getTypeByteSize(elemType) * i));
            }
            else
            {
                context->emitLd(type2LdSt(elemType), memTestArray.segment(), reg, getMemTestArrayAddr(indexReg2, getTypeByteSize(elemType) * i));
                stReg(slotType, reg, addr);
            }
        }
    }

    void initMemTestArray(unsigned arrayIdx) { copyMemTestArray(arrayIdx, true); }
    void saveMemTestArray(unsigned arrayIdx) { copyMemTestArray(arrayIdx, false); }

    Operand getMemTestArrayAddr(OperandReg idxReg, unsigned offset)
    {
        assert(memTestArray);
        return context->emitAddrRef(memTestArray, idxReg, offset);
    }

    unsigned getMemTestArrayType() { assert(memTestArray); return memTestArray.type(); }

    //==========================================================================

    void initMemTestArrayIndexReg()
    {
        unsigned memAddrType = context->getSegAddrTypeB(memTestArray.segment());
        unsigned elemType    = memTestArray.type();
        unsigned dim         = getMaxDim();

        loadIndexReg(getIdxReg1(memAddrType), dim, elemType);
    }

    Operand getMemTestArrayAddr()
    {
        assert(memTestArray);
        unsigned memAddrType = context->getSegAddrTypeB(memTestArray.segment());
        return getMemTestArrayAddr(getIdxReg1(memAddrType), 0);
    }

    //==========================================================================
    // Comments generation
private:

    struct CommentBrig 
    { 
        BrigContext* context;
        CommentBrig(BrigContext* ctx) : context(ctx) {}
        void operator()(string s) { context->emitComment("// " + s); }
    };

    struct CommentLua 
    { 
        string res;
        void operator()(string s) { res += LUA_COMMENT + s + "\n"; }
        string str() { return res; }
    };

    template<class T>
    void emitTestDescription(T& comment)
    {
        comment("Test: " + testName);
        comment("");
        comment("Instruction: " + dumpInst(testSample));
        comment("Arguments:");

        assert(!bundle.empty());
        bundle.resetPos(); // dump first set of test data

        for (int i = provider->getFirstSrcOperandIdx(); i <= provider->getLastOperandIdx(); ++i)
        {
            assert(testSample.operand(i));
            comment("    Arg " + index2str(i) + " (" + getOperandKind(i) + "): " + bundle.getSrcVal(i).dump());
        }

        if (hasDstOperand())
        {
            Val dstValue = bundle.getDstVal();
            assert(!dstValue.empty());
            assert(testSample.type() == bundle.getDstValType());
            
            comment("Expected result: " + dstValue.dump());
        }
        
        if (hasMemoryOperand())
        {
            Val memValue = bundle.getMemVal();
            assert(!memValue.empty());

            comment("Expected result in memory: " + memValue.dump());
        }
    }

    string dumpInst(Inst inst)
    {
        HSAIL_ASM::Disassembler disasm(context->getContainer());
        string res = disasm.get(testSample);
        string::size_type pos = res.find_first_of("\t");
        if (pos != string::npos) res = res.substr(0, pos);
        return res;
    }

    string getOperandKind(unsigned i)
    {
        Operand operand = testSample.operand(i);
        return OperandImmed(operand)        ? "imm" : 
               OperandReg(operand)          ? "reg" : 
               OperandRegVector(operand)    ? "vec" : 
               OperandAddress(operand)      ? "mem" : 
               OperandWavesize(operand)     ? "wsz" : 
                                              "???" ;
    }

    void emitComment(string text)
    {
        context->emitComment("//" + text);
    }

    void emitCommentHeader(string text)
    {
        emitCommentSeparator();
        emitComment(" " + text);
        emitComment("");
    }

    void emitCommentSeparator()
    {
        emitComment("");
        emitComment("======================================================");
    }

    //==========================================================================
    // Symbol names
private:

    string getSrcArrayName(unsigned idx, string prefix = "") { return prefix + "src" + index2str(idx); }
    string getDstArrayName(              string prefix = "") { return prefix + "dst"; }
    string getMemArrayName(              string prefix = "") { return prefix + "mem"; }

    const char* getTestArrayName() { return "&var0"; }

    //==========================================================================
    // Helpers
private:

    static bool     isSmallModel()  { return machineModel == MODEL_SMALL; }
    static bool     isLargeModel()  { return machineModel == MODEL_LARGE; }
    static unsigned getModelTypeU() { return isSmallModel() ? BRIG_TYPE_U32 : BRIG_TYPE_U64; }
    static unsigned getModelTypeB() { return isSmallModel() ? BRIG_TYPE_B32 : BRIG_TYPE_B64; }

    static string   getName(OperandReg reg) { return static_cast<SRef>(reg.reg()); }
    static string   getName(OperandRegVector vector) 
    { 
        string res;
        for (unsigned i = 0; i < vector.regCount(); ++i)
        {
            res += (i > 0? ", " : "");
            res += static_cast<SRef>(vector.regs(i));
        }
        return "(" + res + ")";
    }

    static unsigned getTypeBitSize(unsigned type)
    {
        return HSAIL_ASM::getTypeSize(type);
    }

    static unsigned getTypeByteSize(unsigned type)
    {
        return (type == BRIG_TYPE_B1)? 1 : (HSAIL_ASM::getTypeSize(type) / 8);
    }

    static unsigned getSlotSize(unsigned type)
    {
        return (getTypeBitSize(type) <= 32)? 4 : 8;
    }

    static unsigned getSlotTypeB(unsigned type)
    {
        return (getTypeBitSize(type) <= 32)? BRIG_TYPE_B32 : BRIG_TYPE_B64;
    }

    static unsigned type2b(unsigned type)
    {
        return HSAIL_ASM::convType2BitType(type);
    }

    static unsigned b2u(unsigned type)
    {
        using namespace Brig;
        switch(type) 
        {
        case BRIG_TYPE_B8:      return BRIG_TYPE_U8;
        case BRIG_TYPE_B16:     return BRIG_TYPE_U16;
        case BRIG_TYPE_B32:     return BRIG_TYPE_U32;
        case BRIG_TYPE_B64:     return BRIG_TYPE_U64;
        default: 
            assert(false); 
            return BRIG_TYPE_NONE;
        }
    }

    static unsigned type2LdSt(unsigned type) // Convert to type supported by ld/st
    {
        return HSAIL_ASM::isBitType(type)? b2u(type) : type;
    }

    static unsigned getAtomicSrcNum(InstAtomic inst)
    {
        assert(inst);

        unsigned atmOp = inst.atomicOperation();
        return (atmOp == Brig::BRIG_ATOMIC_CAS) ? 3 : (atmOp == Brig::BRIG_ATOMIC_LD) ? 1 : 2;
    }

    static string index2str(unsigned idx)
    {
        ostringstream s;
        s << idx;
        return s.str();
    }

    bool hasDstOperand()
    {
        assert(provider);
        return provider->getDstOperandIdx() >= 0;
    }

    unsigned getMaxDim() { return getMaxDim(testSample); }

    static unsigned getMaxDim(Inst inst)
    {
        assert(inst);

        for (int i = 0; i < 5 && inst.operand(i); ++i)
        {
            OperandRegVector vec = inst.operand(i);
            if (vec) return vec.regCount();
        }
        return 1;
    }

    //==========================================================================
    //==========================================================================
    //==========================================================================
    // This section of code describes limitations on instructions 
    // which could be tested

private:

    // Create a provider of test data for the current instruction.
    // Providers are selected based on data type of each operand.
    // Supported operand types for each instruction must be defined
    // in HSAILTestGenLuaTestData.h
    // If the current instruction is not described or required type
    // is not found, this test will be rejected.
    static TestDataProvider* getProvider(Inst inst)
    {
        assert(inst);

        TestDataProvider* p;

        switch (inst.kind()) 
        {
        case Brig::BRIG_INST_BASIC:
        case Brig::BRIG_INST_MOD:         p = TestDataProvider::getProvider(inst.opcode(), inst.type());                           break;
        case Brig::BRIG_INST_CVT:         p = TestDataProvider::getProvider(inst.opcode(), InstCvt(inst).sourceType());            break;
        case Brig::BRIG_INST_CMP:         p = TestDataProvider::getProvider(inst.opcode(), InstCmp(inst).sourceType());            break;
        case Brig::BRIG_INST_ATOMIC:      p = TestDataProvider::getProvider(inst.opcode(), inst.type(), getAtomicSrcNum(inst));    break;
        case Brig::BRIG_INST_SOURCE_TYPE: p = TestDataProvider::getProvider(inst.opcode(), InstSourceType(inst).sourceType());     break;
        case Brig::BRIG_INST_MEM:         p = TestDataProvider::getProvider(inst.opcode(), InstMem(inst).type());                  break;
        default:                          p = 0; /* other formats are not currently supported */                                   break;
        }

        if (p)
        {
            unsigned maxDim = getMaxDim(inst);

            // By default, tests for source non-immediate operands can be bundled together to speedup testing
            for (int i = p->getFirstSrcOperandIdx(); i <= p->getLastOperandIdx(); ++i) 
            {
                Operand opr = inst.operand(i);
                assert(opr);

                // NB: If there are vector operands, memory operands (if any) must be processed in similar way.
                unsigned dim  = (OperandRegVector(opr) || OperandAddress(opr))? maxDim : 1;
                bool     lock = !group || OperandImmed(opr);
        
                p->registerOperand(i, dim, lock);
            }
            p->reset();
        }

        return p;
    }

    // Check generic limitations on operands.
    static bool testableOperands(Inst inst)
    {
        assert(inst);

        for (int i = 0; i < 5; ++i)
        {
            Operand operand = inst.operand(i);            
            if (!operand) return true;          // no gaps!

            if (OperandAddress addr = operand)
            {
                if (addr.reg() || addr.offset() != 0) return false;
            }
            else if (!OperandReg(operand) && 
                     !OperandRegVector(operand) &&
                     !OperandImmed(operand))
            {
                return false;
            }
        }

        return true;
    }

}; // class LuaTestGen

//==============================================================================
//==============================================================================
//==============================================================================

}; // namespace TESTGEN

#endif // INCLUDED_HSAIL_TESTGEN_LUA_BACKEND_H