#include "ast.hpp"

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <system_error>
#include <utility>

using namespace std;

using namespace llvm;

class CodeGenBlock
{
    public:
        llvm::BasicBlock * block;
        llvm::Value * returnValue;
        std::map<string, llvm::Value*> locals;
        std::map<string, int> types;     // type name string of vars
        std::map<string, bool> isFuncArg;
        std::map<string, int> arraySizes;
};

/*我们在CodeGenContext类来告诉我们生成中间代码的位置。*/

class CodeGenContext
{
private:
    std::vector<CodeGenBlock*> blockStack;

public:
    llvm::LLVMContext llvmContext;
    llvm::IRBuilder<> builder;
    unique_ptr<llvm::Module> theModule;
    llvm::Function* printf;
    llvm::Function* scanf;
    std::map<string, llvm::Value*> globalVars;
   // TypeSystem typeSystem;

    CodeGenContext(): builder(llvmContext){
        theModule = unique_ptr<llvm::Module>(new llvm::Module("main", this->llvmContext));
    }

    llvm::Function* createPrintf();
    llvm::Function* createScanf();
    //创建 函数input() 读入一个int32数，并返回其值
    llvm::Function* createInput();
    llvm::Function* createInputf();
    //创建 函数output(int) 输出int32数，并返回void
    llvm::Function* createOutput();
    llvm::Function* createOutputf();

    llvm::Value* getSymbolValue(string name) const{
        for(auto it=blockStack.rbegin(); it!=blockStack.rend(); it++){
//            cout << "(*it)->locals[" << name << "] = " << (*it)->locals[name] << endl;
            if( (*it)->locals.find(name) != (*it)->locals.end() ){
                return (*it)->locals[name];
            }
        }
        return nullptr;
    }

    int getSymbolType(string name) const{
        for(auto it=blockStack.rbegin(); it!=blockStack.rend(); it++){
//            cout << "(*it)->locals[" << name << "] = " << (*it)->locals[name] << endl;
            if( (*it)->types.find(name) != (*it)->types.end() ){
                return (*it)->types[name];
            }
        }
        return -1;
    }
    void setSymbolValue(string name, llvm::Value* value){
        blockStack.back()->locals[name] = value;
    }

    void setSymbolType(string name, int64_t value){
        blockStack.back()->types[name] = value;
    }

    void setFuncArg(string name, bool value){
        cout << "Set " << name << " as func arg" << endl;
        blockStack.back()->isFuncArg[name] = value;
    }

    llvm::BasicBlock* currentBlock() const{
        return blockStack.back()->block;
    }

    void pushBlock(llvm::BasicBlock * block){
        CodeGenBlock * codeGenBlock = new CodeGenBlock();
        codeGenBlock->block = block;
        codeGenBlock->returnValue = nullptr;
        blockStack.push_back(codeGenBlock);
    }

    void popBlock(){
        CodeGenBlock * codeGenBlock = blockStack.back();
        blockStack.pop_back();
        delete codeGenBlock;
    }

    void setCurrentReturnValue(llvm::Value* value){
        blockStack.back()->returnValue = value;
    }

    llvm::Value* getCurrentReturnValue(){
        return blockStack.back()->returnValue;
    }

    void setArraySize(string name, int value){
        blockStack.back()->arraySizes[name] = value;
    }

    int getArraySize(string name){
        for(auto it=blockStack.rbegin(); it!=blockStack.rend(); it++){
            if( (*it)->arraySizes.find(name) != (*it)->arraySizes.end() ){
                return (*it)->arraySizes[name];
            }
        }
        return blockStack.back()->arraySizes[name];
    }

    void generateCode(ASTNode& root)
    {
        #ifdef YJJDEBUG
            cout << "Generating IR code" << endl;

        #endif
        this->createInput();
        this->createOutput();
        this->createInputf();
        this->createOutputf();
        Value* retValue = root.Codegen(*this);
    }
};

void ObjGen(CodeGenContext & context, const string& filename);
