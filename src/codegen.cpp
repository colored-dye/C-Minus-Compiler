#include "ast.hpp"
#include "codegen.h"

llvm::Value* ASTProgram::Codegen(CodeGenContext& context)
{
    llvm::Value* last = nullptr;
    for(auto it = this->declList.begin(); it!=this->declList.end(); it++){
        last = (*it)->Codegen(context);
    }
    return last;
}

llvm::Value* ASTFactor:: Codegen(CodeGenContext& context)
{
    if (this->isInt)
    {  
        #ifdef YJJDEBUG
            cout << "Generating Integer: " << this->numInt << endl;
        #endif
        return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context.llvmContext), this->numInt, true);
    }
    else if (this->isReal)
    {
        #ifdef YJJDEBUG
            cout << "Generating Float: " << this->numReal << endl;
        #endif
        return llvm::ConstantFP::get(llvm::Type::getDoubleTy(context.llvmContext), this->numReal); 
    }
    else if (this->expr != nullptr)
    {
        return this->expr->Codegen(context);
    }
    else if (this->var != nullptr)
    {
        return this->var->Codegen(context);
    }
    else if (this->callExpr != nullptr)
    {
        return this->callExpr->Codegen(context);
    }
    #ifdef YJJDEBUG
        cout << "Can not handle when generate Factor" <<endl;
    #endif
    return nullptr;
}

llvm::Value* ASTVar::Codegen(CodeGenContext& context)
{
    #ifdef YJJDEBUG
        cout << "Generating identifier Var " << this->id << endl;
    #endif
    llvm::Value* value = context.getSymbolValue(this->id);
    if( !value )
    {
        return LogErrorV("Unknown variable name " + this->id);
    }
    /*这个地方好像是数组 先不管*/
    /*
    if( value->getType()->isPointerTy() )
    {
        auto arrayPtr = context.builder.CreateLoad(value, "arrayPtr");
        if( arrayPtr->getType()->isArrayTy() )
        {
            cout << "(Array Type)" << endl;
//            arrayPtr->setAlignment(16);
            std::vector<llvm::Value*> indices;
            indices.push_back(ConstantInt::get(context.typeSystem.intTy, 0, false));
            auto ptr = context.builder.CreateInBoundsGEP(value, indices, "arrayPtr");
            return ptr;
        }
    }
    */  
    return context.builder.CreateLoad(value->getType()->getPointerElementType(),value,"");

}

llvm::Value* ASTTerm::Codegen(CodeGenContext& context)
{

    #ifdef YJJDEBUG
        cout << "Generating * / expression" << endl;
    #endif

    /*如果没有多个factor 那么就只有term->factor，直接调用factor的codegen*/
    if (this->areMultipleFactors == false)
    {
        return this->firstFactor->Codegen(context);
    }
    else
    {
        /*暂时都当整型，不支持浮点*/

        llvm::Value* L = this->firstFactor->Codegen(context);    
        auto opit = this->mulOpList.begin();
        auto factorit = this->factorList.begin();
        llvm::Value* R =nullptr;

        while (opit != this->mulOpList.end())
        {
            R = (*factorit)->Codegen(context);
            switch ((*opit))
            {
            case ASTMUL:
                {
                    L = context.builder.CreateMul(L, R, "multmp");
                    break;
                }

            case ASTDIV:
                {
                    L = context.builder.CreateSDiv(L, R, "divtmp");
                    break;
                }
            default:
                #ifdef YJJDEBUG
                    cout << "can not handle the mulOP!" << endl;
                #endif
                break;
            }
            opit++;
            factorit++;
        }
        return L;
    }
    return nullptr;
}

llvm::Value* ASTAddExpr::Codegen(CodeGenContext& context)
{
    #ifdef YJJDEBUG
        cout << "Generating + -  expression" << endl;
    #endif

    /*如果没有多个factor 那么就只有term->factor，直接调用factor的codegen*/
    if (this->areMultipleTerms == false)
    {
        return this->firstTerm->Codegen(context);
    }
    else
    {
        /*暂时都当整型，不支持浮点*/

        llvm::Value* L = this->firstTerm->Codegen(context);    
        auto opit = this->addOpList.begin();
        auto termit = this->termList.begin();
        llvm::Value* R =nullptr;

        while (opit != this->addOpList.end())
        {
            R = (*termit)->Codegen(context);
            switch ((*opit))
            {
            case ASTADD:
                {
                    L = context.builder.CreateAdd(L, R, "addtmp");
                    break;
                }

            case ASTMINUS:
                {
                    L = context.builder.CreateSub(L, R, "subtmp");
                    break;
                }
            default:
                #ifdef YJJDEBUG
                    cout << "can not handle the addOP!" << endl;
                #endif
                break;
            }
            opit++;
            termit++;
        }
        return L;
    }
    return nullptr;
}

llvm::Value* ASTSimpleExpr::Codegen(CodeGenContext& context)
{
 #ifdef YJJDEBUG
        cout << "Generating relation expression" << endl;
    #endif

    /*如果没有多个操作数*/
    if (this->haveRightAddExpr == false)
    {
        return this->leftAddExpr->Codegen(context);
    }
    else
    {
        /*暂时都当整型，不支持浮点*/
        llvm::Value* L = this->leftAddExpr->Codegen(context);    
        llvm::Value* R = this->rightAddExpr->Codegen(context);
        switch (this->relOp)
        {
            /* <= */
        case ASTLE:
            {
                return context.builder.CreateICmpSLE(L, R, "cmptmp");
            }
            break;
            /* < */
        case ASTLT:
            {
                return context.builder.CreateICmpULT(L, R, "cmptmp");
            }
            break;
            /* > */
        case ASTGT:
            {
                return context.builder.CreateICmpSGT(L, R, "cmptmp");
            }
            break;
            /* >= */
        case ASTGE:
            {
               return context.builder.CreateICmpSGE(L, R, "cmptmp");
            }
            break;
            /* == */
        case ASTEQ:
            {
               return context.builder.CreateICmpEQ(L, R, "cmptmp");
            }
            break;
            /* != */
        case ASTNE:
            {
                return context.builder.CreateICmpNE(L, R, "cmptmp");
            }
            break;   
        default:
            #ifdef YJJDEBUG
                cout << "can not handle relation op" << endl;
            #endif
            break;
        }
       
    }
    return nullptr;
}

llvm::Value* ASTExpr::Codegen(CodeGenContext& context)
{
    #ifdef YJJDEBUG
        cout << "Generating expr expression" << endl;
    #endif
    /* expr -> simpleexpr*/
    if (this->isAssignStmt == false)
    {
        return this->simpleExpr->Codegen(context);
    }
    else /* expr -> var = expr*/
    {
        #ifdef YJJDEBUG
            cout << "Generating assignment "<< endl;
        #endif
        Value* dst = context.getSymbolValue(this->var->GetId());
        int dstType = context.getSymbolType(this->var->GetId());
        //string dstTypeStr = dstType->name;
        if( !dst )
        {
            return LogErrorV("Undeclared variable");
        }
        Value* exp = exp = this->expr->Codegen(context);
        if (!exp)
            return LogErrorV("no legal rhs");
       // cout << "dst typeid = " << TypeSystem::llvmTypeToStr(context.typeSystem.getVarType(dstTypeStr)) << endl;
     //  cout << "exp typeid = " << TypeSystem::llvmTypeToStr(exp) << endl;

     //   exp = context.typeSystem.cast(exp, context.typeSystem.getVarType(dstTypeStr), context.currentBlock());
        //exp =  CastInst::Create(,);
        context.builder.CreateStore(exp, dst);
        return dst;
    }
    return nullptr;
}

llvm::Value* ASTCompoundStmt::Codegen(CodeGenContext& context)
{
    cout << "Generating block" << endl;
    Value* last = nullptr;
    for (auto it = this->declList.begin();it!=this->declList.end();it++)
        last = (*it)->Codegen(context);
    for(auto it=this->stmtList.begin(); it!=this->stmtList.end(); it++){
        last = (*it)->Codegen(context);
    }
    return last;
}

llvm::Value* ASTVarDecl::Codegen(CodeGenContext& context)
{
    //不管数组
    #ifdef YJJDEBUG
        cout << "Generating variable declaration of " << this->GetTypeSpec() << " " << this->GetId() << endl;
    #endif
    if (this->isGlobal)
    {
        llvm::Type* ty= nullptr;
        if (this->GetTypeSpec() == ASTINT)
            ty = Type::getInt32Ty(context.llvmContext);
        else if (this->GetTypeSpec() == ASTREAL)
            ty = Type::getFloatTy(context.llvmContext);
        llvm::Value *globalVar = new llvm::GlobalVariable(
                *context.theModule,  ty, false, llvm::GlobalVariable::PrivateLinkage,0,this->GetId());
       // context.theModule->getGlobalVariable()
       return globalVar;
    }
    else
    {
        llvm::Type* ty = nullptr;
        Value* inst = nullptr;
        if (this->GetTypeSpec() == ASTINT)
        {
            ty = Type::getInt32Ty(context.llvmContext);
        }
        else if (this->GetTypeSpec() == ASTREAL)
        {
            ty = Type::getFloatTy(context.llvmContext);
        }
        
        inst = context.builder.CreateAlloca(ty);
        context.setSymbolType(this->GetId(), this->GetTypeSpec());
        context.setSymbolValue(this->GetId(), inst);

        return inst;
    }
    return nullptr;

}

llvm::Value* ASTFunDecl::Codegen(CodeGenContext& context)
{
    #ifdef YJJDEBUG
         cout << "Generating function declaration of " << this->id << endl;
    #endif
    //参数 不管array
    std::vector<Type*> argTypes;
    for (auto it = this->paramList.begin();it!=this->paramList.end();it++)
    {
        if ((*it)->GetTypeSpec() == ASTINT)
            argTypes.push_back(Type::getInt32Ty(context.llvmContext));
        else if ((*it)->GetTypeSpec() == ASTREAL)
            argTypes.push_back(Type::getFloatTy(context.llvmContext));
    }

    //返回值 不管array
    Type* retType = nullptr;
    if(this->typeSpec == ASTVOID)
        retType = nullptr;
    else if ( this->typeSpec == ASTINT)
        retType = Type::getInt32Ty(context.llvmContext);
    else if ( this->typeSpec == ASTREAL)
        retType = Type::getFloatTy(context.llvmContext);

    //定义函数
    FunctionType* functionType = FunctionType::get(retType, argTypes, false);
    Function* function = Function::Create(functionType, GlobalValue::ExternalLinkage, this->id, context.theModule.get());

    //函数体
    BasicBlock* basicBlock = BasicBlock::Create(context.llvmContext, "entry", function, nullptr);
    context.builder.SetInsertPoint(basicBlock);
    context.pushBlock(basicBlock);
    
    // declare function params
    auto origin_arg = this->paramList.begin();
    for (auto &Arg : function->args())
    {
        //设置id
        Arg.setName((*origin_arg)->GetId());
        Value* argAlloc;
        argAlloc = (*origin_arg)->Codegen(context);
        context.builder.CreateStore(&Arg, argAlloc, false);
        context.setSymbolValue((*origin_arg)->GetId(), argAlloc);
        context.setSymbolType((*origin_arg)->GetId(), (*origin_arg)->GetTypeSpec());
        context.setFuncArg((*origin_arg)->GetId(), true);
        origin_arg++;
    }
    this->compoundStmt->Codegen(context);
    if( context.getCurrentReturnValue() )
    {
        context.builder.CreateRet(context.getCurrentReturnValue());
    } 
    else
    {
        return LogErrorV("Function block return value not founded");
    }
    context.popBlock();
    return function;
}

llvm::Value* ASTParam::Codegen(CodeGenContext& context)
{
        //不管数组
    #ifdef YJJDEBUG
        cout << "Generating param declaration of a function" << this->GetTypeSpec() << " " << this->GetId() << endl;
    #endif
    
    llvm::Type* ty = nullptr;
    Value* inst = nullptr;
    if (this->GetTypeSpec() == ASTINT)
    {
        ty = Type::getInt32Ty(context.llvmContext);
    }
    else if (this->GetTypeSpec() == ASTREAL)
    {
        ty = Type::getFloatTy(context.llvmContext);
    }
    
    inst = context.builder.CreateAlloca(ty);
    context.setSymbolType(this->GetId(), this->GetTypeSpec());
    context.setSymbolValue(this->GetId(), inst);

    return inst;
}

llvm::Value* ASTReturnStmt::Codegen(CodeGenContext& context)
{
    #ifdef YJJDEBUG
        cout << "Generating return statement" << endl;
    #endif
    Value* returnValue = this->expr->Codegen(context);
    context.setCurrentReturnValue(returnValue);
    return returnValue;
}

llvm::Value* ASTCall::Codegen(CodeGenContext& context)
{
    #ifdef YJJDEBUG
        cout << "Generating method call of " << this->GetId() << endl;
    #endif
    Function * calleeF = context.theModule->getFunction(this->GetId());
    if( !calleeF )
    {
        LogErrorV("Function name not found");
    }
    if( calleeF->arg_size() != this->argList.size() )
    {
        LogErrorV("Function arguments size not match, calleeF=" + std::to_string(calleeF->size()) + ", this->arguments=" + std::to_string(this->argList.size()) );
    }
    std::vector<Value*> argsv;
    for(auto it=this->argList.begin(); it!=this->argList.end(); it++){
        argsv.push_back((*it)->Codegen(context));
        if( !argsv.back() ){        // if any argument codegen fail
            return nullptr;
        }
    }
    return context.builder.CreateCall(calleeF, argsv, "calltmp");
}

llvm::Value* ASTSelectStmt::Codegen(CodeGenContext& context)
{
    return nullptr;
}
llvm::Value* ASTWhileStmt::Codegen(CodeGenContext& context)
{
    return nullptr;
}
llvm::Value* ASTForStmt::Codegen(CodeGenContext& context)
{
    return nullptr;
}

std::unique_ptr<ASTNode*> LogError(const char *str) {
    fprintf(stderr, "LogError: %s\n", str);
    return nullptr;
}
llvm::Value *LogErrorV(string str){
    return LogErrorV(str.c_str());
}

llvm::Value *LogErrorV(const char *str) {
    LogError(str);
    return nullptr;
}

llvm::Function* CodeGenContext::createPrintf()
{
    std::vector<llvm::Type*> arg_types;
    arg_types.push_back(builder.getInt8PtrTy());
    auto printf_type = llvm::FunctionType::get(builder.getInt32Ty(), llvm::makeArrayRef(arg_types), true);
    auto func = llvm::Function::Create(printf_type, llvm::Function::ExternalLinkage, llvm::Twine("printf"), this->theModule.get());
    func->setCallingConv(llvm::CallingConv::C);
    return func;
}

llvm::Function* CodeGenContext::createScanf()
{
    auto scanf_type = llvm::FunctionType::get(builder.getInt32Ty(), true);
    auto func = llvm::Function::Create(scanf_type, llvm::Function::ExternalLinkage, llvm::Twine("scanf"), this->theModule.get());
    func->setCallingConv(llvm::CallingConv::C);
    return func;
}