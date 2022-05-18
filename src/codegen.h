#include "ast.hpp"

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
        std::map<string, std::vector<uint64_t>> arraySizes;
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
/*
    bool isFuncArg(string name) const{

        for(auto it=blockStack.rbegin(); it!=blockStack.rend(); it++){
            if( (*it)->isFuncArg.find(name) != (*it)->isFuncArg.end() ){
                return (*it)->isFuncArg[name];
            }
        }
        return false;
    }
*/
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

    void setArraySize(string name, std::vector<uint64_t> value){
        std::cout << "setArraySize: " << name << ": " << value.size() << endl;
        blockStack.back()->arraySizes[name] = value;
//        cout << "blockStack.back()->arraySizes.size()" << blockStack.back()->arraySizes.size() << endl;
    }

    std::vector<uint64_t> getArraySize(string name){
        for(auto it=blockStack.rbegin(); it!=blockStack.rend(); it++){
            if( (*it)->arraySizes.find(name) != (*it)->arraySizes.end() ){
                return (*it)->arraySizes[name];
            }
        }
        return blockStack.back()->arraySizes[name];
    }
/*
    void PrintSymTable() const{
        cout << "======= Print Symbol Table ========" << endl;
        string prefix = "";
        for(auto it=blockStack.begin(); it!=blockStack.end(); it++){
            for(auto it2=(*it)->locals.begin(); it2!=(*it)->locals.end(); it2++){
                cout << prefix << it2->first << " = " << it2->second << ": " << this->getSymbolType(it2->first) << endl;
            }
            prefix += "\t";
        }
        cout << "===================================" << endl;
    }
*/
    void generateCode(ASTNode& root)
    {
        #ifdef YJJDEBUG
            cout << "Generating IR code" << endl;

        #endif
        this->createPrintf();
        this->createScanf();
        Value* retValue = root.Codegen(*this);
        /*
        std::vector<Type*> sysArgs;
        FunctionType* mainFuncType = FunctionType::get(Type::getVoidTy(this->llvmContext), makeArrayRef(sysArgs), false);
        Function* mainFunc = Function::Create(mainFuncType, GlobalValue::ExternalLinkage, "main");
        BasicBlock* block = BasicBlock::Create(this->llvmContext, "entry");

        pushBlock(block);
        Value* retValue = root.codeGen(*this);
        popBlock();

        cout << "Code generate success" << endl;

        PassManager passManager;
        passManager.add(createPrintModulePass(outs()));
        passManager.run(*(this->theModule.get()));
        */
    }
};