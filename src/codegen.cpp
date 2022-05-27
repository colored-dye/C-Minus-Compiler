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
        return llvm::ConstantFP::get(llvm::Type::getFloatTy(context.llvmContext), this->numReal); 
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
        value = context.theModule->getGlobalVariable(this->id, true);
        cout<<"herevar"<<endl;
        if (!value)
            return LogErrorV("Unknown variable name " + this->id);
    }
    
    if( this->HaveSubscript())
    {
        llvm::Value* indexValue = this->subscript->Codegen(context);
        if(value->getType()->getPointerElementType()->isPointerTy())
        {
            #ifdef YJJDEBUG
                cout << "meet a pointer" <<endl;
            #endif
            auto arrayPtr = context.builder.CreateLoad(value->getType()->getPointerElementType(),value, "arrayPtr");
            std::vector<Value*> indices;
            indices.push_back(indexValue);
            llvm::Value * varPtr = context.builder.CreateInBoundsGEP(arrayPtr->getType()->getPointerElementType(),arrayPtr, indices, "tmpvar");
            return context.builder.CreateLoad(varPtr->getType()->getPointerElementType(), varPtr, "tmpvar");
        }
        else
        {
            std::vector<Value*> indices;
            indices.push_back(context.builder.getInt32(0));
            indices.push_back(indexValue);
            llvm::Value * varPtr = context.builder.CreateInBoundsGEP(value->getType()->getPointerElementType(),value, indices, "tmpvar");
            return context.builder.CreateLoad(varPtr->getType()->getPointerElementType(), varPtr, "tmpvar");
        }
    }
    /*
    if( value->getType()->isPointerTy() ){
        auto arrayPtr = context.builder.CreateLoad(value->getType()->getPointerElementType(),value, "arrayPtr");
        if( arrayPtr->getType()->isArrayTy() ){
           // cout << "(Array Type)" << endl;
            std::vector<Value*> indices;
            indices.push_back(context.builder.getInt32(0));
            auto ptr = context.builder.CreateInBoundsGEP(value->getType()->getPointerElementType(),value, indices, "arrayPtr");
            return ptr;
        }
    }
    */
    cout << "WHO AM I" <<endl;
    
    if(value->getType()->getPointerElementType()->isArrayTy())
    {
        #ifdef YJJDEBUG
            cout << "meet a pointer without sub" <<endl;
        #endif
        std::vector<Value*> indices;
        indices.push_back(context.builder.getInt32(0));
        indices.push_back(context.builder.getInt32(0));
        llvm::Value * varPtr = context.builder.CreateInBoundsGEP(value->getType()->getPointerElementType(),value, indices, "tmpvar");
        return varPtr;
    }
   // cout << "FUCK！" <<endl;
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
        /*已经支持浮点 不支持数组*/

        llvm::Value* L = this->firstFactor->Codegen(context);    
        auto opit = this->mulOpList.begin();
        auto factorit = this->factorList.begin();
        llvm::Value* R =nullptr;

        while (opit != this->mulOpList.end())
        {
            R = (*factorit)->Codegen(context);
            bool fp =false;
            if( (L->getType()->getTypeID() == Type::FloatTyID) || (R->getType()->getTypeID() == Type::FloatTyID) )
            {  // type upgrade
                fp = true;
                if( (R->getType()->getTypeID() != Type::FloatTyID) )
                {
                    R = context.builder.CreateUIToFP(R, Type::getFloatTy(context.llvmContext), "ftmp");
                }
                if( (L->getType()->getTypeID() != Type::FloatTyID) )
                {
                    L = context.builder.CreateUIToFP(L, Type::getFloatTy(context.llvmContext), "ftmp");
                }
            }

            switch ((*opit))
            {
            case ASTMUL:
                {
                    if (fp)
                        L = context.builder.CreateFMul(L, R, "mulftmp");
                    else
                        L = context.builder.CreateMul(L, R, "multmp");
                    break;
                }

            case ASTDIV:
                {
                    if (fp)
                        L = context.builder.CreateFDiv(L, R, "divftmp");
                    else
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
        /*已经支持浮点 不支持数组*/

        llvm::Value* L = this->firstTerm->Codegen(context);    
        auto opit = this->addOpList.begin();
        auto termit = this->termList.begin();
        llvm::Value* R =nullptr;

        while (opit != this->addOpList.end())
        {
            R = (*termit)->Codegen(context);
            bool fp =false;
            if( (L->getType()->getTypeID() == Type::FloatTyID) || (R->getType()->getTypeID() == Type::FloatTyID) )
            {  // type upgrade
                fp = true;
                if( (R->getType()->getTypeID() != Type::FloatTyID) )
                {
                    R = context.builder.CreateUIToFP(R, Type::getFloatTy(context.llvmContext), "ftmp");
                }
                if( (L->getType()->getTypeID() != Type::FloatTyID) )
                {
                    L = context.builder.CreateUIToFP(L, Type::getFloatTy(context.llvmContext), "ftmp");
                }
            }
            switch ((*opit))
            {
            case ASTADD:
                {
                    if (fp)
                        L = context.builder.CreateFAdd(L, R, "addtmp");
                    else
                        L = context.builder.CreateAdd(L, R, "addtmp");
                    break;
                }

            case ASTMINUS:
                {
                    if (fp)
                        L = context.builder.CreateFSub(L, R, "subftmp");
                    else
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
        llvm::Value* ret = nullptr;
        /*暂时都当整型，不支持浮点*/
        llvm::Value* L = this->leftAddExpr->Codegen(context);    
        llvm::Value* R = this->rightAddExpr->Codegen(context);
        bool fp =false;
        if( (L->getType()->getTypeID() == Type::FloatTyID) || (R->getType()->getTypeID() == Type::FloatTyID) )
        {  // type upgrade
            fp = true;
            if( (R->getType()->getTypeID() != Type::FloatTyID) )
            {
                R = context.builder.CreateUIToFP(R, Type::getFloatTy(context.llvmContext), "ftmp");
            }
            if( (L->getType()->getTypeID() != Type::FloatTyID) )
            {
                L = context.builder.CreateUIToFP(L, Type::getFloatTy(context.llvmContext), "ftmp");
            }
        }
        switch (this->relOp)
        {

            /* <= */
        case ASTLE:
            {
                if (fp)
                    ret = context.builder.CreateFCmpOLE(L, R, "cmpftmp");
                else 
                    ret = context.builder.CreateICmpSLE(L, R, "cmptmp");
            }
            break;
            /* < */
        case ASTLT:
            {
                if (fp)
                    ret = context.builder.CreateFCmpOLT(L, R, "cmpftmp");
                else
                    ret = context.builder.CreateICmpULT(L, R, "cmptmp");
            }
            break;
            /* > */
        case ASTGT:
            {
                if (fp)
                    ret = context.builder.CreateFCmpOGT(L, R, "cmpftmp");
                else
                    ret = context.builder.CreateICmpSGT(L, R, "cmptmp");
            }
            break;
            /* >= */
        case ASTGE:
            {
                if (fp)
                    ret = context.builder.CreateFCmpOGE(L, R, "cmpftmp");
                else
                    ret = context.builder.CreateICmpSGE(L, R, "cmptmp");
            }
            break;
            /* == */
        case ASTEQ:
            {
                if (fp)
                    ret = context.builder.CreateFCmpOEQ(L, R, "cmpftmp");
                else
                    ret = context.builder.CreateICmpEQ(L, R, "cmptmp");
            }
            break;
            /* != */
        case ASTNE:
            {
                if (fp)
                    ret = context.builder.CreateFCmpONE(L, R, "cmpftmp");
                else
                    ret = context.builder.CreateICmpNE(L, R, "cmptmp");
            }
            break;   
        default:
            #ifdef YJJDEBUG
                cout << "can not handle relation op" << endl;
            #endif
            break;
        }
       return context.builder.CreateUIToFP(ret, Type::getFloatTy(context.llvmContext),"booltmp");
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
        
        if (this->var->HaveSubscript())
        {
          //  cout << "unfinished" << endl;
            llvm::Value * value = context.getSymbolValue(this->var->GetId());
             if( !value )
            {
                value = context.theModule->getGlobalVariable(this->var->GetId(), true);
                #ifdef YJJDEBUG
                    cout<<"global here"<<endl;
                #endif
                if (!value)
                    return LogErrorV("Undeclared variable");
            }
            Value* exp = exp = this->expr->Codegen(context);
            if (!exp)
                return LogErrorV("no legal rhs");

            llvm::Value* indexValue = this->var->subscript->Codegen(context);
            if(value->getType()->getPointerElementType()->isPointerTy())
            {
                #ifdef YJJDEBUG
                    cout << "meet a pointer" <<endl;
                #endif
                auto arrayPtr = context.builder.CreateLoad(value->getType()->getPointerElementType(),value, "arrayPtr");
                std::vector<Value*> indices;
                indices.push_back(indexValue);
                llvm::Value * varPtr = context.builder.CreateInBoundsGEP(arrayPtr->getType()->getPointerElementType(),arrayPtr, indices, "tmpvar");
                context.builder.CreateStore(exp, varPtr);
                return context.builder.CreateLoad(varPtr->getType()->getPointerElementType(), varPtr, "tmpvar");
            }
            else
            {
                std::vector<Value*> indices;
                indices.push_back(context.builder.getInt32(0));
                indices.push_back(indexValue);
                llvm::Value * varPtr = context.builder.CreateInBoundsGEP(value->getType()->getPointerElementType(),value, indices, "tmpvar");
                context.builder.CreateStore(exp, varPtr);
                return context.builder.CreateLoad(varPtr->getType()->getPointerElementType(), varPtr, "tmpvar");
            }
        }
        else
        {
            Value* dst = context.getSymbolValue(this->var->GetId());
            int dstType = -1;
            if (dst)
                dstType =  context.getSymbolType(this->var->GetId());
            //string dstTypeStr = dstType->name;
            if( !dst )
            {
                dst = context.theModule->getGlobalVariable(this->var->GetId(), true);
                #ifdef YJJDEBUG
                    cout<<"here"<<endl;
                #endif
                if (!dst)
                    return LogErrorV("Undeclared variable");
            }
            Value* exp = exp = this->expr->Codegen(context);
            if (!exp)
                return LogErrorV("no legal rhs");
            context.builder.CreateStore(exp, dst);
            return dst;
        }
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
 //   if( context.getCurrentReturnValue() )
  //  {
   //     context.builder.CreateRet(context.getCurrentReturnValue());
  //  } 
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
        if (this->isArray)
        {
       //     context.setArraySize(this->GetId(), this->arrayLength);
            Value* arraySizeValue = ConstantInt::get(Type::getInt32Ty(context.llvmContext), this->arrayLength, true);
           
            llvm::ArrayType* arrayType = nullptr;
            if (this->typeSpec == ASTINT)
                arrayType = ArrayType::get(Type::getInt32Ty(context.llvmContext), this->arrayLength);
            else if (this->typeSpec == ASTREAL)
                arrayType = ArrayType::get(Type::getFloatTy(context.llvmContext), this->arrayLength);

            llvm::GlobalVariable *globalVar = new llvm::GlobalVariable(
                *context.theModule,  arrayType, false, llvm::GlobalVariable::ExternalLinkage,0,this->GetId());
            std::vector<llvm::Constant*> const_array_elems;
            for (int i=0;i<this->GetArrayLength();i++)
            {
                if (this->GetTypeSpec() == ASTINT)
                    const_array_elems.push_back(llvm::ConstantInt::get(llvm::Type::getInt32Ty(context.llvmContext), 0));
                else
                    const_array_elems.push_back(llvm::ConstantFP::get(llvm::Type::getFloatTy(context.llvmContext), 0.0));
            }
            llvm::Constant* const_array = llvm::ConstantArray::get(arrayType, const_array_elems);
            globalVar->setInitializer(const_array);

            return globalVar;
        }
        else
        {
            llvm::GlobalVariable *globalVar = new llvm::GlobalVariable(
                *context.theModule,  ty, false, llvm::GlobalVariable::ExternalLinkage,0,this->GetId());
            if (ty == Type::getInt32Ty(context.llvmContext))
                globalVar->setInitializer(llvm::ConstantInt::get(llvm::Type::getInt32Ty(context.llvmContext), 0));
            else
                globalVar->setInitializer(llvm::ConstantFP::get(llvm::Type::getFloatTy(context.llvmContext), 0.0));
            return globalVar;
        }
       // context.theModule->getGlobalVariable()

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
        

        if (this->isArray)
        {
            context.setArraySize(this->GetId(), this->arrayLength);
            Value* arraySizeValue = ConstantInt::get(Type::getInt32Ty(context.llvmContext), this->arrayLength, true);
            llvm::ArrayType* arrayType = nullptr;
            if (this->typeSpec == ASTINT)
                arrayType = ArrayType::get(Type::getInt32Ty(context.llvmContext), this->arrayLength);
            else if (this->typeSpec == ASTREAL)
                arrayType = ArrayType::get(Type::getFloatTy(context.llvmContext), this->arrayLength);
            inst = context.builder.CreateAlloca(arrayType, arraySizeValue, "arraytmp");
        }
        else
        {
            inst = context.builder.CreateAlloca(ty);
        }
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
            if ((*it)->IsArray())
                argTypes.push_back(PointerType::get(Type::getInt32Ty(context.llvmContext),0));
            else
                argTypes.push_back(Type::getInt32Ty(context.llvmContext));
        else if ((*it)->GetTypeSpec() == ASTREAL)
            if ((*it)->IsArray())
                argTypes.push_back(PointerType::get(Type::getFloatTy(context.llvmContext),0));
            else
                argTypes.push_back(Type::getFloatTy(context.llvmContext));
    }
        // if( arg->type->isArray ){
            //argTypes.push_back(PointerType::get(context.typeSystem.getVarType(arg->type->name), 0));


    //返回值 不管array
    Type* retType = nullptr;
    if(this->typeSpec == ASTVOID)
        retType = Type::getVoidTy(context.llvmContext);
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
        if ((*origin_arg)->IsArray())
        {
            if ((*origin_arg)->GetTypeSpec() == ASTINT)
                argAlloc = context.builder.CreateAlloca(PointerType::get(Type::getInt32Ty(context.llvmContext), 0));
            else if ((*origin_arg)->GetTypeSpec() == ASTREAL)
                argAlloc = context.builder.CreateAlloca(PointerType::get(Type::getFloatTy(context.llvmContext), 0));
        }
        else
            argAlloc = (*origin_arg)->Codegen(context);

        context.builder.CreateStore(&Arg, argAlloc, false);
        context.setSymbolValue((*origin_arg)->GetId(), argAlloc);
        context.setSymbolType((*origin_arg)->GetId(), (*origin_arg)->GetTypeSpec());
        context.setFuncArg((*origin_arg)->GetId(), true);
        origin_arg++;
    }
    this->compoundStmt->Codegen(context);
  ///  cout<<"U!!! SUCK!!!!!!!!"<<endl;
    if( !context.getCurrentReturnValue() )
    {
        #ifdef YJJDEBUG
            cout << "!!!!!end block has no ret" << endl;
        if ( this->typeSpec == ASTINT)
            context.builder.CreateRet(
                ConstantInt::get(llvm::Type::getInt32Ty(context.llvmContext), 0, true));
        else if ( this->typeSpec == ASTREAL)
            context.builder.CreateRet(
                ConstantInt::get(llvm::Type::getFloatTy(context.llvmContext), 0.0, true));
        else if ( this->typeSpec == ASTVOID)
            context.builder.CreateRetVoid();
        #endif
    } 
  //  cout<<"SUCK!!!!!!!!"<<endl;
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
    if (returnValue)
    {
        context.setCurrentReturnValue(returnValue);
        context.builder.CreateRet(context.getCurrentReturnValue());
    }
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
    //cout<<"FUCK U ALL"<<endl;
    return context.builder.CreateCall(calleeF, argsv, "");
}

llvm::Value* ASTSelectStmt::Codegen(CodeGenContext& context)
{
    #ifdef YJJDEBUG
        cout << "Generating if statement" << endl;
    #endif
    Value* condValue = this->expr->Codegen(context);
   // condValue = context.builder.CreateCast()
//    llvm::ConstantFP::get(llvm::Type::getFloatTy(context.llvmContext), this->numReal)
    if( !condValue )
        return nullptr;
    if (condValue->getType() == llvm::Type::getInt32Ty(context.llvmContext))
    {
        #ifdef YJJDEBUG
            cout << "Meet a int32 condition in if " << endl;
        #endif
        condValue = context.builder.CreateICmpNE(
            condValue, llvm::ConstantInt::get(llvm::Type::getInt32Ty(context.llvmContext), 0) , "ifcond");
    }
    else 
    {
        condValue = context.builder.CreateFCmpONE(
            condValue, llvm::ConstantFP::get(llvm::Type::getFloatTy(context.llvmContext), 0.0) , "ifcond");
    }

    Function* theFunction = context.builder.GetInsertBlock()->getParent();      // the function where if statement is in

    BasicBlock *thenBB = BasicBlock::Create(context.llvmContext, "then", theFunction);
    BasicBlock *elseBB = BasicBlock::Create(context.llvmContext, "else");
    BasicBlock *mergeBB = BasicBlock::Create(context.llvmContext, "ifcont");

    if( this->haveElse)
    {
        context.builder.CreateCondBr(condValue, thenBB, elseBB);
    } 
    else
    {
        context.builder.CreateCondBr(condValue, thenBB, mergeBB);
    }

    context.builder.SetInsertPoint(thenBB);

    context.pushBlock(thenBB);

    this->trueStmt->Codegen(context);

    context.popBlock();

    thenBB = context.builder.GetInsertBlock();

    if( thenBB->getTerminator() == nullptr )
    {      
        context.builder.CreateBr(mergeBB);
    }

    if( this->haveElse )
    {
        theFunction->getBasicBlockList().push_back(elseBB);    //
        context.builder.SetInsertPoint(elseBB);            //

        context.pushBlock(thenBB);

        this->falseStmt->Codegen(context);

        context.popBlock();

        context.builder.CreateBr(mergeBB);
    }

    theFunction->getBasicBlockList().push_back(mergeBB);        //
    context.builder.SetInsertPoint(mergeBB);  
    return nullptr;
}
llvm::Value* ASTWhileStmt::Codegen(CodeGenContext& context)
{
    Function* theFunction = context.builder.GetInsertBlock()->getParent();

    BasicBlock *block = BasicBlock::Create(context.llvmContext, "whileloop", theFunction);
    BasicBlock *after = BasicBlock::Create(context.llvmContext, "whilecont");

    ASTExpr* delc = nullptr;
    ASTExpr* expr = nullptr;
    // execute the initial
    Value* condValue = this->expr->Codegen(context);
    if( !condValue )
        return nullptr;
    if (condValue->getType() == llvm::Type::getFloatTy(context.llvmContext))
    {
        #ifdef YJJDEBUG
            cout << "while condition value is float" << endl;
        #endif
        condValue = context.builder.CreateFCmpONE(
        condValue, llvm::ConstantFP::get(llvm::Type::getFloatTy(context.llvmContext), 0.0) , "whilecond");
    }
    else if (condValue->getType() == llvm::Type::getInt32Ty(context.llvmContext))
    {
        #ifdef YJJDEBUG
            cout << "while condition value is int" << endl;
        #endif
        condValue = context.builder.CreateICmpNE(
        condValue, llvm::ConstantInt::get(llvm::Type::getInt32Ty(context.llvmContext), 0) , "whilecond");
    }

    // fall to the block
    context.builder.CreateCondBr(condValue, block, after);

    context.builder.SetInsertPoint(block);

    context.pushBlock(block);

    this->stmt->Codegen(context);

    context.popBlock();
    condValue = this->expr->Codegen(context);
    // execute the again or stop
    if (condValue->getType() == llvm::Type::getFloatTy(context.llvmContext))
    {
        #ifdef YJJDEBUG
            cout << "while condition value is float" << endl;
        #endif
        condValue = context.builder.CreateFCmpONE(
        condValue, llvm::ConstantFP::get(llvm::Type::getFloatTy(context.llvmContext), 0.0) , "whilecond");
    }
    else if (condValue->getType() == llvm::Type::getInt32Ty(context.llvmContext))
    {
        #ifdef YJJDEBUG
            cout << "while condition value is int" << endl;
        #endif
        condValue = context.builder.CreateICmpNE(
        condValue, llvm::ConstantInt::get(llvm::Type::getInt32Ty(context.llvmContext), 0) , "whilecond");
    }
    context.builder.CreateCondBr(condValue, block, after);

    // insert the after block
    theFunction->getBasicBlockList().push_back(after);
    context.builder.SetInsertPoint(after);

    return nullptr;
}
llvm::Value* ASTForStmt::Codegen(CodeGenContext& context)
{
    Function* theFunction = context.builder.GetInsertBlock()->getParent();

    BasicBlock *block = BasicBlock::Create(context.llvmContext, "forloop", theFunction);
    BasicBlock *after = BasicBlock::Create(context.llvmContext, "forcont");

    ASTExpr* delc = nullptr;
    ASTExpr* expr = nullptr;
    // execute the initial
    if( this->haveForParam1)
    {
        delc = new ASTExpr(this->var1,this->expr1);
        delc->Codegen(context);
    }

    Value* condValue = this->expr2->Codegen(context);
    if( !condValue )
        return nullptr;
    
    condValue = context.builder.CreateFCmpONE(
        condValue, llvm::ConstantFP::get(llvm::Type::getFloatTy(context.llvmContext), 0.0) , "forcond");

    // fall to the block
    context.builder.CreateCondBr(condValue, block, after);

    context.builder.SetInsertPoint(block);

    context.pushBlock(block);

    this->stmt->Codegen(context);

    context.popBlock();

    // do increment
    if( this->haveForParam3 )
    {
        expr = new ASTExpr(this->var3,this->expr3);
        expr->Codegen(context);
    }

    // execute the again or stop
    condValue = this->expr2->Codegen(context);
    condValue = context.builder.CreateFCmpONE(
        condValue, llvm::ConstantFP::get(llvm::Type::getFloatTy(context.llvmContext), 0.0) , "forcond");
    context.builder.CreateCondBr(condValue, block, after);

    // insert the after block
    theFunction->getBasicBlockList().push_back(after);
    context.builder.SetInsertPoint(after);

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
    auto printf_type = llvm::FunctionType::get(builder.getInt32Ty(), arg_types, true);
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

llvm::Function* CodeGenContext::createInput()
{
    std::vector<llvm::Type*> arg_types;
   // arg_types.push_back(builder.getInt8PtrTy());
    Type* retType = Type::getVoidTy(this->llvmContext);
    retType = Type::getInt32Ty(this->llvmContext);
    auto input_type = llvm::FunctionType::get(retType, arg_types, false);
   // this->theModule->getOrInsertFunction("input", input_type);
 //   Function *fun_pow = cast<Function>(this->t->getOrInsertFunction("pow", pow_type));
    auto func = llvm::Function::Create(input_type, llvm::Function::ExternalLinkage, llvm::Twine("input"), this->theModule.get());
  //  func->setCallingConv(llvm::CallingConv::C);
    #ifdef YJJDEBUG
        cout << "generate input done" <<endl;
    #endif
    return nullptr;
}
llvm::Function* CodeGenContext::createInputf()
{
    std::vector<llvm::Type*> arg_types;
   // arg_types.push_back(builder.getInt8PtrTy());
    Type* retType = Type::getVoidTy(this->llvmContext);
    retType = Type::getFloatTy(this->llvmContext);
    auto input_type = llvm::FunctionType::get(retType, arg_types, false);
   // this->theModule->getOrInsertFunction("input", input_type);
 //   Function *fun_pow = cast<Function>(this->t->getOrInsertFunction("pow", pow_type));
    auto func = llvm::Function::Create(input_type, llvm::Function::ExternalLinkage, llvm::Twine("inputf"), this->theModule.get());
  //  func->setCallingConv(llvm::CallingConv::C);
    #ifdef YJJDEBUG
        cout << "generate input done" <<endl;
    #endif
    return nullptr;
}
llvm::Function* CodeGenContext::createOutput()
{
    std::vector<llvm::Type*> arg_types;
    arg_types.push_back(Type::getInt32Ty(this->llvmContext));
    Type* retType = Type::getVoidTy(this->llvmContext);
    #ifdef YJJDEBUG
        cout << "generate output not done" <<endl;
    #endif
    auto input_type = llvm::FunctionType::get(retType, arg_types, false);
    #ifdef YJJDEBUG
        cout << "generate output ing" <<endl;
    #endif
    auto func = llvm::Function::Create(input_type, llvm::Function::ExternalLinkage, llvm::Twine("output"), this->theModule.get());
  //  func->setCallingConv(llvm::CallingConv::C);
    #ifdef YJJDEBUG
        cout << "generate output done" <<endl;
    #endif
    return func;
}
llvm::Function* CodeGenContext::createOutputf()
{
    std::vector<llvm::Type*> arg_types;
    arg_types.push_back(Type::getFloatTy(this->llvmContext));
    Type* retType = Type::getVoidTy(this->llvmContext);
    #ifdef YJJDEBUG
        cout << "generate output not done" <<endl;
    #endif
    auto input_type = llvm::FunctionType::get(retType, arg_types, false);
    #ifdef YJJDEBUG
        cout << "generate output ing" <<endl;
    #endif
    auto func = llvm::Function::Create(input_type, llvm::Function::ExternalLinkage, llvm::Twine("outputf"), this->theModule.get());
  //  func->setCallingConv(llvm::CallingConv::C);
    #ifdef YJJDEBUG
        cout << "generate output done" <<endl;
    #endif
    return func;
}

void ObjGen(CodeGenContext & context, const string& filename )
{
    // Initialize the target registry etc.
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();

    auto targetTriple = sys::getDefaultTargetTriple();
    context.theModule->setTargetTriple(targetTriple);

    std::string error;
    auto Target = TargetRegistry::lookupTarget(targetTriple, error);

    if( !Target ){
        errs() << error;
        return;
    }

    auto CPU = "generic";
    auto features = "";

    TargetOptions opt;
    auto RM = Optional<Reloc::Model>();
    auto theTargetMachine = Target->createTargetMachine(targetTriple, CPU, features, opt, RM);

    context.theModule->setDataLayout(theTargetMachine->createDataLayout());
    context.theModule->setTargetTriple(targetTriple);

    std::error_code EC;
    raw_fd_ostream dest(filename.c_str(), EC, sys::fs::OF_None);
//    raw_fd_ostream dest(filename.c_str(), EC, sys::fs::F_None);
//    formatted_raw_ostream formattedRawOstream(dest);

    legacy::PassManager pass;
    auto FileType = CGFT_ObjectFile;
    // auto fileType = TargetMachine::CGFT_ObjectFile;
    if( theTargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType) ){
        errs() << "theTargetMachine can't emit a file of this type";
        return;
    }

    pass.run(*context.theModule.get());
    dest.flush();

   // outs() << "Object code wrote to " << filename.c_str() << "\n";

}