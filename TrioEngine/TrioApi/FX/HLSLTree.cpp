#include "stdafx.h"
#include "HLSLTree.h"

#include "StringHelpers.h"

#include "AssertFX.h"

namespace TrioFX
{
	/*
	TODO: actualizar código con el repositorio

	https://github.com/Thekla/hlslparser/
	*/

	HLSLTree::HLSLTree(TrioMem::Allocator* allocator) :
		m_allocator(allocator),
		m_stringPool(allocator)
	{
		m_firstPage = m_allocator->New<NodePage>();
		m_firstPage->next = nullptr;

		m_currentPage = m_firstPage;
		m_currentPageOffset = 0;

		m_root = AddNode<HLSLRoot>(nullptr, 1);
	}

	HLSLTree::~HLSLTree()
	{
		NodePage* page = m_firstPage;
		while (page != nullptr)
		{
			NodePage* next = page->next;
			m_allocator->Delete(page);
			page = next;
		}
	}

	void HLSLTree::AllocatePage()
	{
		NodePage* newPage = m_allocator->New<NodePage>();
		newPage->next = nullptr;
		m_currentPage->next = newPage;
		m_currentPageOffset = 0;
		m_currentPage = newPage;
	}

	const char* HLSLTree::AddString(const char* string)
	{
		return m_stringPool.AddString(string);
	}

	const char* HLSLTree::AddStringFormat(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		const char * string = m_stringPool.AddStringFormatList(format, args);
		va_end(args);
		return string;
	}

	bool HLSLTree::GetContainsString(const char* string) const
	{
		return m_stringPool.GetContainsString(string);
	}

	HLSLRoot* HLSLTree::GetRoot() const
	{
		return m_root;
	}

	void* HLSLTree::AllocateMemory(size_t size)
	{
		if (m_currentPageOffset + size > s_nodePageSize)
		{
			AllocatePage();
		}
		void* buffer = m_currentPage->buffer + m_currentPageOffset;
		m_currentPageOffset += size;
		return buffer;
	}

	// @@ This doesn't do any parameter matching. Simply returns the first function with that name.
	HLSLFunction * HLSLTree::FindFunction(const char * name)
	{
		HLSLStatement * statement = m_root->statement;
		while (statement != nullptr)
		{
			if (statement->nodeType == HLSLNodeType_Function)
			{
				HLSLFunction * function = (HLSLFunction *)statement;
				if (String_Equal(name, function->name))
				{
					return function;
				}
			}

			statement = statement->nextStatement;
		}

		return nullptr;
	}

	HLSLDeclaration* HLSLTree::FindGlobalDeclaration(const char * name, HLSLBuffer ** buffer_out/*=nullptr*/)
	{
		HLSLStatement * statement = m_root->statement;
		while (statement != nullptr)
		{
			if (statement->nodeType == HLSLNodeType_Declaration)
			{
				HLSLDeclaration * declaration = (HLSLDeclaration *)statement;
				if (String_Equal(name, declaration->name))
				{
					if (buffer_out) *buffer_out = nullptr;
					return declaration;
				}
			}
			else if (statement->nodeType == HLSLNodeType_Buffer)
			{
				HLSLBuffer* buffer = (HLSLBuffer*)statement;

				HLSLDeclaration* field = buffer->field;
				while (field != nullptr)
				{
					ASSERT(field->nodeType == HLSLNodeType_Declaration);
					if (String_Equal(name, field->name))
					{
						if (buffer_out) *buffer_out = buffer;
						return field;
					}
					field = (HLSLDeclaration*)field->nextStatement;
				}
			}

			statement = statement->nextStatement;
		}

		if (buffer_out) *buffer_out = nullptr;
		return nullptr;
	}

	HLSLStruct* HLSLTree::FindGlobalStruct(const char * name)
	{
		HLSLStatement * statement = m_root->statement;
		while (statement != nullptr)
		{
			if (statement->nodeType == HLSLNodeType_Struct)
			{
				HLSLStruct * declaration = (HLSLStruct *)statement;
				if (String_Equal(name, declaration->name))
				{
					return declaration;
				}
			}

			statement = statement->nextStatement;
		}

		return nullptr;
	}

	HLSLTechnique * HLSLTree::FindTechnique(const char * name)
	{
		HLSLStatement * statement = m_root->statement;
		while (statement != nullptr)
		{
			if (statement->nodeType == HLSLNodeType_Technique)
			{
				HLSLTechnique * technique = (HLSLTechnique *)statement;
				if (String_Equal(name, technique->name))
				{
					return technique;
				}
			}

			statement = statement->nextStatement;
		}

		return nullptr;
	}

	HLSLTechnique11 * HLSLTree::FindTechnique11(const char * name)
	{
		HLSLStatement * statement = m_root->statement;
		while (statement != nullptr)
		{
			if (statement->nodeType == HLSLNodeType_Technique11)
			{
				HLSLTechnique11 * technique = (HLSLTechnique11 *)statement;
				if (String_Equal(name, technique->name))
				{
					return technique;
				}
			}

			statement = statement->nextStatement;
		}

		return nullptr;
	}

	HLSLTechnique11 * HLSLTree::FindFirstTechnique11()
	{
		HLSLStatement * statement = m_root->statement;
		while (statement != nullptr)
		{
			if (statement->nodeType == HLSLNodeType_Technique11)
			{
				HLSLTechnique11 * technique = (HLSLTechnique11 *)statement;
				
				return technique;
			}

			statement = statement->nextStatement;
		}

		return nullptr;
	}
	HLSLPipeline * HLSLTree::FindFirstPipeline()
	{
		return FindNextPipeline(nullptr);
	}

	HLSLPipeline * HLSLTree::FindNextPipeline(HLSLPipeline * current)
	{
		HLSLStatement * statement = current ? current : m_root->statement;
		while (statement != nullptr)
		{
			if (statement->nodeType == HLSLNodeType_Pipeline)
			{
				return (HLSLPipeline *)statement;
			}

			statement = statement->nextStatement;
		}

		return nullptr;
	}

	HLSLPipeline * HLSLTree::FindPipeline(const char * name)
	{
		HLSLStatement * statement = m_root->statement;
		while (statement != nullptr)
		{
			if (statement->nodeType == HLSLNodeType_Pipeline)
			{
				HLSLPipeline * pipeline = (HLSLPipeline *)statement;
				if (String_Equal(name, pipeline->name))
				{
					return pipeline;
				}
			}

			statement = statement->nextStatement;
		}

		return nullptr;
	}

	HLSLBuffer * HLSLTree::FindBuffer(const char * name)
	{
		HLSLStatement * statement = m_root->statement;
		while (statement != nullptr)
		{
			if (statement->nodeType == HLSLNodeType_Buffer)
			{
				HLSLBuffer * buffer = (HLSLBuffer *)statement;
				if (String_Equal(name, buffer->name))
				{
					return buffer;
				}
			}

			statement = statement->nextStatement;
		}

		return nullptr;
	}

	bool HLSLTree::GetExpressionValue(HLSLExpression * expression, int & value)
	{
		ASSERT(expression != nullptr);

		// Expression must be constant.
		if ((expression->expressionType.flags & HLSLTypeFlag_Const) == 0)
		{
			return false;
		}

		// We are expecting an integer scalar. @@ Add support for type conversion from other scalar types.
		if (expression->expressionType.baseType != HLSLBaseType_Int &&
			expression->expressionType.baseType != HLSLBaseType_Bool)
		{
			return false;
		}

		if (expression->expressionType.array)
		{
			return false;
		}

		if (expression->nodeType == HLSLNodeType_BinaryExpression)
		{
			HLSLBinaryExpression * binaryExpression = (HLSLBinaryExpression *)expression;

			int value1, value2;
			if (!GetExpressionValue(binaryExpression->expression1, value1) ||
				!GetExpressionValue(binaryExpression->expression2, value2))
			{
				return false;
			}

			switch (binaryExpression->binaryOp)
			{
			case HLSLBinaryOp_And:
				value = value1 && value2;
				return true;
			case HLSLBinaryOp_Or:
				value = value1 || value2;
				return true;
			case HLSLBinaryOp_Add:
				value = value1 + value2;
				return true;
			case HLSLBinaryOp_Sub:
				value = value1 - value2;
				return true;
			case HLSLBinaryOp_Mul:
				value = value1 * value2;
				return true;
			case HLSLBinaryOp_Div:
				value = value1 / value2;
				return true;
			case HLSLBinaryOp_Less:
				value = value1 < value2;
				return true;
			case HLSLBinaryOp_Greater:
				value = value1 > value2;
				return true;
			case HLSLBinaryOp_LessEqual:
				value = value1 <= value2;
				return true;
			case HLSLBinaryOp_GreaterEqual:
				value = value1 >= value2;
				return true;
			case HLSLBinaryOp_Equal:
				value = value1 == value2;
				return true;
			case HLSLBinaryOp_NotEqual:
				value = value1 != value2;
				return true;
			case HLSLBinaryOp_BitAnd:
				value = value1 & value2;
				return true;
			case HLSLBinaryOp_BitOr:
				value = value1 | value2;
				return true;
			case HLSLBinaryOp_BitXor:
				value = value1 ^ value2;
				return true;
			case HLSLBinaryOp_Assign:
			case HLSLBinaryOp_AddAssign:
			case HLSLBinaryOp_SubAssign:
			case HLSLBinaryOp_MulAssign:
			case HLSLBinaryOp_DivAssign:
				// IC: These are not valid on non-constant expressions and should fail earlier when querying expression value.
				return false;
			}
		}
		else if (expression->nodeType == HLSLNodeType_UnaryExpression)
		{
			HLSLUnaryExpression * unaryExpression = (HLSLUnaryExpression *)expression;

			if (!GetExpressionValue(unaryExpression->expression, value))
			{
				return false;
			}

			switch (unaryExpression->unaryOp)
			{
			case HLSLUnaryOp_Negative:
				value = -value;
				return true;
			case HLSLUnaryOp_Positive:
				// nop.
				return true;
			case HLSLUnaryOp_Not:
				value = !value;
				return true;
			case HLSLUnaryOp_BitNot:
				value = ~value;
				return true;
			case HLSLUnaryOp_PostDecrement:
			case HLSLUnaryOp_PostIncrement:
			case HLSLUnaryOp_PreDecrement:
			case HLSLUnaryOp_PreIncrement:
				// IC: These are not valid on non-constant expressions and should fail earlier when querying expression value.
				return false;
			}
		}
		else if (expression->nodeType == HLSLNodeType_IdentifierExpression)
		{
			HLSLIdentifierExpression * identifier = (HLSLIdentifierExpression *)expression;

			HLSLDeclaration * declaration = FindGlobalDeclaration(identifier->name);
			if (declaration == nullptr)
			{
				return false;
			}
			if ((declaration->type.flags & HLSLTypeFlag_Const) == 0)
			{
				return false;
			}

			return GetExpressionValue(declaration->assignment, value);
		}
		else if (expression->nodeType == HLSLNodeType_LiteralExpression)
		{
			HLSLLiteralExpression * literal = (HLSLLiteralExpression *)expression;
			
			if (literal->expressionType.baseType == HLSLBaseType_Int) value = literal->iValue;
			else if (literal->expressionType.baseType == HLSLBaseType_Bool) value = (int)literal->bValue;
			else return false;

			return true;
		}

		return false;
	}

	void HLSLTree::PopulateEffectCollections(HLSLTree* root, std::vector<HLSLTechnique11*> &techniques, std::vector<HLSLBuffer*> &buffers, std::vector<HLSLStruct*> &structures, std::vector<TrioFX::HLSLDeclaration*> &textures, std::vector<TrioFX::HLSLDeclaration*> &samplers)
	{
		HLSLStatement* statement = m_root->statement;
		while (statement != nullptr)
		{
			if (statement->nodeType == HLSLNodeType_Technique11)
			{
				HLSLTechnique11* technique = static_cast<HLSLTechnique11*>(statement);

				techniques.push_back(technique);
			}
			else if (statement->nodeType == HLSLNodeType_Buffer)
			{
				HLSLBuffer* buffer = static_cast<HLSLBuffer*>(statement);

				buffers.push_back(buffer);
			}
			else if (statement->nodeType == HLSLNodeType_Struct)
			{
				HLSLStruct* structure = static_cast<HLSLStruct*>(statement);

				structures.push_back(structure);
			}
			else if (statement->nodeType == HLSLNodeType_Declaration)
			{
				HLSLDeclaration* declaration = static_cast<HLSLDeclaration*>(statement);

				if (declaration->type.baseType == HLSLBaseType_SamplerState)
				{
					samplers.push_back(declaration);
				}
				else if (declaration->type.baseType == HLSLBaseType_Texture2D)
				{
					textures.push_back(declaration);
				}
			}

			statement = statement->nextStatement;
		}
	}

	bool HLSLTree::NeedsFunction(const char* name)
	{
		// Early out
		if (!GetContainsString(name))
			return false;

		struct NeedsFunctionVisitor : HLSLTreeVisitor
		{
			const char* name;
			bool result;

			virtual void VisitTopLevelStatement(HLSLStatement * node)
			{
				if (!node->hidden)
					HLSLTreeVisitor::VisitTopLevelStatement(node);
			}

			virtual void VisitFunctionCall(HLSLFunctionCall * node)
			{
				result = result || String_Equal(name, node->function->name);

				HLSLTreeVisitor::VisitFunctionCall(node);
			}
		};

		NeedsFunctionVisitor visitor;
		visitor.name = name;
		visitor.result = false;

		visitor.VisitRoot(m_root);

		return visitor.result;
	}

	int GetVectorDimension(HLSLType & type)
	{
		if (type.baseType >= HLSLBaseType_FirstNumeric &&
			type.baseType <= HLSLBaseType_LastNumeric)
		{
			if (type.baseType == HLSLBaseType_Float || type.baseType == HLSLBaseType_Half || type.baseType == HLSLBaseType_Int || type.baseType == HLSLBaseType_Uint) return 1;
			if (type.baseType == HLSLBaseType_Float2 || type.baseType == HLSLBaseType_Half2 || type.baseType == HLSLBaseType_Int2 || type.baseType == HLSLBaseType_Uint2) return 2;
			if (type.baseType == HLSLBaseType_Float3 || type.baseType == HLSLBaseType_Half3 || type.baseType == HLSLBaseType_Int3 || type.baseType == HLSLBaseType_Uint3) return 3;
			if (type.baseType == HLSLBaseType_Float4 || type.baseType == HLSLBaseType_Half4 || type.baseType == HLSLBaseType_Int4 || type.baseType == HLSLBaseType_Uint4) return 4;

		}
		return 0;
	}

	//int GetDimensionSizeInBytes(const HLSLType & type)
	//{
	//	if (type.baseType >= HLSLBaseType_FirstNumeric &&
	//		type.baseType <= HLSLBaseType_LastNumeric)
	//	{
	//		if (type.baseType == HLSLBaseType_Float || type.baseType == HLSLBaseType_Float2 || type.baseType == HLSLBaseType_Float3 || type.baseType == HLSLBaseType_Float4 ||
	//			type.baseType == HLSLBaseType_Float2x2 || type.baseType == HLSLBaseType_Float3x3 || type.baseType == HLSLBaseType_Float4x4 || type.baseType == HLSLBaseType_Float4x2 ||
	//			type.baseType == HLSLBaseType_Float4x3)
	//			return 4;

	//		if (type.baseType == HLSLBaseType_Int || type.baseType == HLSLBaseType_Int2 || type.baseType == HLSLBaseType_Int3 || type.baseType == HLSLBaseType_Int4 ||
	//			type.baseType == HLSLBaseType_Uint || type.baseType == HLSLBaseType_Uint2 || type.baseType == HLSLBaseType_Uint3 || type.baseType == HLSLBaseType_Uint4)
	//			return 4;

	//		if (type.baseType == HLSLBaseType_Half || type.baseType == HLSLBaseType_Half2 || type.baseType == HLSLBaseType_Half3 || type.baseType == HLSLBaseType_Half4 ||
	//			type.baseType == HLSLBaseType_Half2x2 || type.baseType == HLSLBaseType_Half3x3 || type.baseType == HLSLBaseType_Half4x4 || type.baseType == HLSLBaseType_Half4x2 ||
	//			type.baseType == HLSLBaseType_Half4x3)
	//			return 2;
	//	}
	//	return 0;
	//}

	// Returns dimension, 0 if invalid.
	int HLSLTree::GetExpressionValue(HLSLExpression * expression, float values[4])
	{
		ASSERT(expression != nullptr);

		// Expression must be constant.
		if ((expression->expressionType.flags & HLSLTypeFlag_Const) == 0)
		{
			return 0;
		}

		if (expression->expressionType.baseType == HLSLBaseType_Int ||
			expression->expressionType.baseType == HLSLBaseType_Bool)
		{
			int int_value;
			if (GetExpressionValue(expression, int_value)) {
				for (int i = 0; i < 4; i++) values[i] = (float)int_value;   // @@ Warn if conversion is not exact.
				return 1;
			}

			return 0;
		}
		if (expression->expressionType.baseType >= HLSLBaseType_FirstInteger && expression->expressionType.baseType <= HLSLBaseType_LastInteger)
		{
			// @@ Add support for uints?
			// @@ Add support for int vectors?
			return 0;
		}
		if (expression->expressionType.baseType > HLSLBaseType_LastNumeric)
		{
			return 0;
		}

		// @@ Not supported yet, but we may need it?
		if (expression->expressionType.array)
		{
			return false;
		}

		if (expression->nodeType == HLSLNodeType_BinaryExpression)
		{
			HLSLBinaryExpression * binaryExpression = (HLSLBinaryExpression *)expression;
			int dim = GetVectorDimension(binaryExpression->expressionType);

			float values1[4], values2[4];
			int dim1 = GetExpressionValue(binaryExpression->expression1, values1);
			int dim2 = GetExpressionValue(binaryExpression->expression2, values2);

			if (dim1 == 0 || dim2 == 0)
			{
				return 0;
			}

			if (dim1 != dim2)
			{
				// Brodacast scalar to vector size.
				if (dim1 == 1)
				{
					for (int i = 1; i < dim2; i++) values1[i] = values1[0];
					dim1 = dim2;
				}
				else if (dim2 == 1)
				{
					for (int i = 1; i < dim1; i++) values2[i] = values2[0];
					dim2 = dim1;
				}
				else
				{
					return 0;
				}
			}
			ASSERT(dim == dim1);

			switch (binaryExpression->binaryOp)
			{
			case HLSLBinaryOp_Add:
				for (int i = 0; i < dim; i++) values[i] = values1[i] + values2[i];
				return dim;
			case HLSLBinaryOp_Sub:
				for (int i = 0; i < dim; i++) values[i] = values1[i] - values2[i];
				return dim;
			case HLSLBinaryOp_Mul:
				for (int i = 0; i < dim; i++) values[i] = values1[i] * values2[i];
				return dim;
			case HLSLBinaryOp_Div:
				for (int i = 0; i < dim; i++) values[i] = values1[i] / values2[i];
				return dim;
			default:
				return 0;
			}
		}
		else if (expression->nodeType == HLSLNodeType_UnaryExpression)
		{
			HLSLUnaryExpression * unaryExpression = (HLSLUnaryExpression *)expression;
			int dim = GetVectorDimension(unaryExpression->expressionType);

			int dim1 = GetExpressionValue(unaryExpression->expression, values);
			if (dim1 == 0)
			{
				return 0;
			}
			ASSERT(dim == dim1);

			switch (unaryExpression->unaryOp)
			{
			case HLSLUnaryOp_Negative:
				for (int i = 0; i < dim; i++) values[i] = -values[i];
				return dim;
			case HLSLUnaryOp_Positive:
				// nop.
				return dim;
			default:
				return 0;
			}
		}
		else if (expression->nodeType == HLSLNodeType_ConstructorExpression)
		{
			HLSLConstructorExpression * constructor = (HLSLConstructorExpression *)expression;

			int dim = GetVectorDimension(constructor->expressionType);

			int idx = 0;
			HLSLExpression * arg = constructor->argument;
			while (arg != NULL)
			{
				float tmp[4];
				int n = GetExpressionValue(arg, tmp);
				for (int i = 0; i < n; i++) values[idx + i] = tmp[i];
				idx += n;

				arg = arg->nextExpression;
			}
			ASSERT(dim == idx);

			return dim;
		}
		else if (expression->nodeType == HLSLNodeType_IdentifierExpression)
		{
			HLSLIdentifierExpression * identifier = (HLSLIdentifierExpression *)expression;

			HLSLDeclaration * declaration = FindGlobalDeclaration(identifier->name);
			if (declaration == NULL)
			{
				return 0;
			}
			if ((declaration->type.flags & HLSLTypeFlag_Const) == 0)
			{
				return 0;
			}

			return GetExpressionValue(declaration->assignment, values);
		}
		else if (expression->nodeType == HLSLNodeType_LiteralExpression)
		{
			HLSLLiteralExpression * literal = (HLSLLiteralExpression *)expression;

			if (literal->expressionType.baseType == HLSLBaseType_Float) values[0] = literal->fValue;
			else if (literal->expressionType.baseType == HLSLBaseType_Half) values[0] = literal->fValue;
			else if (literal->expressionType.baseType == HLSLBaseType_Bool) values[0] = literal->bValue;
			else if (literal->expressionType.baseType == HLSLBaseType_Int) values[0] = (float)literal->iValue;  // @@ Warn if conversion is not exact.
			else return 0;

			return 1;
		}

		return 0;
	}

	void HLSLTreeVisitor::VisitType(HLSLType & type)
	{
	}

	void HLSLTreeVisitor::MarkVisitNode(HLSLNode * node)
	{
	}

	void HLSLTreeVisitor::VisitRoot(HLSLRoot * root)
	{
		MarkVisitNode(root);
		HLSLStatement * statement = root->statement;
		while (statement != nullptr) {
			VisitTopLevelStatement(statement);
			statement = statement->nextStatement;
		}
	}

	void HLSLTreeVisitor::VisitTopLevelStatement(HLSLStatement * node)
	{
		MarkVisitNode(node);
		if (node->nodeType == HLSLNodeType_Declaration) {
			VisitDeclaration((HLSLDeclaration *)node);
		}
		else if (node->nodeType == HLSLNodeType_Struct) {
			VisitStruct((HLSLStruct *)node);
		}
		else if (node->nodeType == HLSLNodeType_Buffer) {
			VisitBuffer((HLSLBuffer *)node);
		}
		else if (node->nodeType == HLSLNodeType_Function) {
			VisitFunction((HLSLFunction *)node);
		}
		else if (node->nodeType == HLSLNodeType_Technique) {
			VisitTechnique((HLSLTechnique *)node);
		}
		else {
			//ASSERT(0);
		}
	}

	void HLSLTreeVisitor::VisitStatements(HLSLStatement * statement)
	{
		MarkVisitNode(statement);
		while (statement != nullptr) {
			VisitStatement(statement);
			statement = statement->nextStatement;
		}
	}

	void HLSLTreeVisitor::VisitStatement(HLSLStatement * node)
	{
		MarkVisitNode(node);
		// Function statements
		if (node->nodeType == HLSLNodeType_Declaration) {
			VisitDeclaration((HLSLDeclaration *)node);
		}
		else if (node->nodeType == HLSLNodeType_ExpressionStatement) {
			VisitExpressionStatement((HLSLExpressionStatement *)node);
		}
		else if (node->nodeType == HLSLNodeType_ReturnStatement) {
			VisitReturnStatement((HLSLReturnStatement *)node);
		}
		else if (node->nodeType == HLSLNodeType_DiscardStatement) {
			VisitDiscardStatement((HLSLDiscardStatement *)node);
		}
		else if (node->nodeType == HLSLNodeType_BreakStatement) {
			VisitBreakStatement((HLSLBreakStatement *)node);
		}
		else if (node->nodeType == HLSLNodeType_ContinueStatement) {
			VisitContinueStatement((HLSLContinueStatement *)node);
		}
		else if (node->nodeType == HLSLNodeType_IfStatement) {
			VisitIfStatement((HLSLIfStatement *)node);
		}
		else if (node->nodeType == HLSLNodeType_ForStatement) {
			VisitForStatement((HLSLForStatement *)node);
		}
		else if (node->nodeType == HLSLNodeType_BlockStatement) {
			VisitBlockStatement((HLSLBlockStatement *)node);
		}
		else {
			//ASSERT(0);
		}
	}

	void HLSLTreeVisitor::VisitDeclaration(HLSLDeclaration * node)
	{
		MarkVisitNode(node);
		VisitType(node->type);
		/*do {
		VisitExpression(node->assignment);
		node = node->nextDeclaration;
		} while (node);*/
		if (node->assignment != nullptr) {
			VisitExpression(node->assignment);
		}
		if (node->nextDeclaration != nullptr) {
			VisitDeclaration(node->nextDeclaration);
		}
	}

	void HLSLTreeVisitor::VisitStruct(HLSLStruct * node)
	{
		MarkVisitNode(node);
		HLSLStructField * field = node->field;
		while (field != nullptr) {
			VisitStructField(field);
			field = field->nextField;
		}
	}

	void HLSLTreeVisitor::VisitStructField(HLSLStructField * node)
	{
		MarkVisitNode(node);
		VisitType(node->type);
	}

	void HLSLTreeVisitor::VisitBuffer(HLSLBuffer * node)
	{
		MarkVisitNode(node);
		HLSLDeclaration * field = node->field;
		while (field != nullptr) {
			ASSERT(field->nodeType == HLSLNodeType_Declaration);
			VisitDeclaration(field);
			ASSERT(field->nextDeclaration == nullptr);
			field = (HLSLDeclaration *)field->nextStatement;
		}
	}

	//void HLSLTreeVisitor::VisitBufferField(HLSLBufferField * node)
	//{
	//	MarkVisitNode(node);
	//	VisitType(node->type);
	//}

	void HLSLTreeVisitor::VisitFunction(HLSLFunction * node)
	{
		MarkVisitNode(node);
		VisitType(node->returnType);

		HLSLArgument * argument = node->argument;
		while (argument != nullptr) {
			VisitArgument(argument);
			argument = argument->nextArgument;
		}

		VisitStatements(node->statement);
	}

	void HLSLTreeVisitor::VisitArgument(HLSLArgument * node)
	{
		MarkVisitNode(node);
		VisitType(node->type);
		if (node->defaultValue != nullptr) {
			VisitExpression(node->defaultValue);
		}
	}

	void HLSLTreeVisitor::VisitExpressionStatement(HLSLExpressionStatement * node)
	{
		VisitExpression(node->expression);
	}

	void HLSLTreeVisitor::VisitExpression(HLSLExpression * node)
	{
		MarkVisitNode(node);
		VisitType(node->expressionType);

		if (node->nodeType == HLSLNodeType_UnaryExpression) {
			VisitUnaryExpression((HLSLUnaryExpression *)node);
		}
		else if (node->nodeType == HLSLNodeType_BinaryExpression) {
			VisitBinaryExpression((HLSLBinaryExpression *)node);
		}
		else if (node->nodeType == HLSLNodeType_ConditionalExpression) {
			VisitConditionalExpression((HLSLConditionalExpression *)node);
		}
		else if (node->nodeType == HLSLNodeType_CastingExpression) {
			VisitCastingExpression((HLSLCastingExpression *)node);
		}
		else if (node->nodeType == HLSLNodeType_LiteralExpression) {
			VisitLiteralExpression((HLSLLiteralExpression *)node);
		}
		else if (node->nodeType == HLSLNodeType_IdentifierExpression) {
			VisitIdentifierExpression((HLSLIdentifierExpression *)node);
		}
		else if (node->nodeType == HLSLNodeType_ConstructorExpression) {
			VisitConstructorExpression((HLSLConstructorExpression *)node);
		}
		else if (node->nodeType == HLSLNodeType_MemberAccess) {
			VisitMemberAccess((HLSLMemberAccess *)node);
		}
		else if (node->nodeType == HLSLNodeType_ArrayAccess) {
			VisitArrayAccess((HLSLArrayAccess *)node);
		}
		else if (node->nodeType == HLSLNodeType_FunctionCall) {
			VisitFunctionCall((HLSLFunctionCall *)node);
		}
		else if (node->nodeType == HLSLNodeType_FunctionAccess) {
			VisitFunctionAccess((HLSLFunctionAccess *)node);
		}
		else {
			//ASSERT(0);
		}
	}

	void HLSLTreeVisitor::VisitReturnStatement(HLSLReturnStatement * node)
	{
		MarkVisitNode(node);
		VisitExpression(node->expression);
	}

	void HLSLTreeVisitor::VisitDiscardStatement(HLSLDiscardStatement * node) {}
	void HLSLTreeVisitor::VisitBreakStatement(HLSLBreakStatement * node) {}
	void HLSLTreeVisitor::VisitContinueStatement(HLSLContinueStatement * node) {}

	void HLSLTreeVisitor::VisitIfStatement(HLSLIfStatement * node)
	{
		MarkVisitNode(node);
		VisitExpression(node->condition);
		VisitStatements(node->statement);
		if (node->elseStatement) {
			VisitStatements(node->elseStatement);
		}
	}

	void HLSLTreeVisitor::VisitForStatement(HLSLForStatement * node)
	{
		MarkVisitNode(node);
		if (node->initialization) {
			VisitDeclaration(node->initialization);
		}
		if (node->condition) {
			VisitExpression(node->condition);
		}
		if (node->increment) {
			VisitExpression(node->increment);
		}
		VisitStatements(node->statement);
	}

	void HLSLTreeVisitor::VisitBlockStatement(HLSLBlockStatement * node)
	{
		MarkVisitNode(node);
		VisitStatements(node->statement);
	}

	void HLSLTreeVisitor::VisitUnaryExpression(HLSLUnaryExpression * node)
	{
		MarkVisitNode(node);
		VisitExpression(node->expression);
	}

	void HLSLTreeVisitor::VisitBinaryExpression(HLSLBinaryExpression * node)
	{
		MarkVisitNode(node);
		VisitExpression(node->expression1);
		VisitExpression(node->expression2);
	}

	void HLSLTreeVisitor::VisitConditionalExpression(HLSLConditionalExpression * node)
	{
		MarkVisitNode(node);
		VisitExpression(node->condition);
		VisitExpression(node->falseExpression);
		VisitExpression(node->trueExpression);
	}

	void HLSLTreeVisitor::VisitCastingExpression(HLSLCastingExpression * node)
	{
		MarkVisitNode(node);
		VisitType(node->type);
		VisitExpression(node->expression);
	}

	void HLSLTreeVisitor::VisitLiteralExpression(HLSLLiteralExpression * node)
	{
		MarkVisitNode(node);
	}
	void HLSLTreeVisitor::VisitIdentifierExpression(HLSLIdentifierExpression * node)
	{
		MarkVisitNode(node);
	}

	void HLSLTreeVisitor::VisitConstructorExpression(HLSLConstructorExpression * node)
	{
		MarkVisitNode(node);
		HLSLExpression * argument = node->argument;
		while (argument != nullptr) {
			VisitExpression(argument);
			argument = argument->nextExpression;
		}
	}

	void HLSLTreeVisitor::VisitMemberAccess(HLSLMemberAccess * node)
	{
		MarkVisitNode(node);
		VisitExpression(node->object);
	}

	void HLSLTreeVisitor::VisitArrayAccess(HLSLArrayAccess * node)
	{
		MarkVisitNode(node);
		VisitExpression(node->array);
		VisitExpression(node->index);
	}

	void HLSLTreeVisitor::VisitFunctionCall(HLSLFunctionCall * node)
	{
		MarkVisitNode(node);
		HLSLExpression * argument = node->argument;
		while (argument != nullptr) {
			VisitExpression(argument);
			argument = argument->nextExpression;
		}
	}

	void HLSLTreeVisitor::VisitFunctionAccess(HLSLFunctionAccess * node)
	{
		MarkVisitNode(node);
		VisitExpression(node->object);
		HLSLExpression * argument = node->argument;
		while (argument != nullptr) {
			VisitExpression(argument);
			argument = argument->nextExpression;
		}
	}

	void HLSLTreeVisitor::VisitStateAssignment(HLSLStateAssignment * node) 
	{
		MarkVisitNode(node);
	}

	void HLSLTreeVisitor::VisitSamplerState(HLSLSamplerState * node)
	{
		MarkVisitNode(node);
		HLSLStateAssignment * stateAssignment = node->stateAssignments;
		while (stateAssignment != nullptr) {
			VisitStateAssignment(stateAssignment);
			stateAssignment = stateAssignment->nextStateAssignment;
		}
	}

	void HLSLTreeVisitor::VisitPass(HLSLPass * node)
	{
		MarkVisitNode(node);
		HLSLStateAssignment * stateAssignment = node->stateAssignments;
		while (stateAssignment != nullptr) {
			VisitStateAssignment(stateAssignment);
			stateAssignment = stateAssignment->nextStateAssignment;
		}
	}

	void HLSLTreeVisitor::VisitTechnique(HLSLTechnique * node)
	{
		MarkVisitNode(node);
		HLSLPass * pass = node->passes;
		while (pass != nullptr) {
			VisitPass(pass);
			pass = pass->nextPass;
		}
	}

	void HLSLTreeVisitor::VisitFunctions(HLSLRoot * root)
	{
		HLSLStatement * statement = root->statement;
		while (statement != nullptr) {
			if (statement->nodeType == HLSLNodeType_Function) {
				VisitFunction((HLSLFunction *)statement);
			}

			statement = statement->nextStatement;
		}
	}

	void HLSLTreeVisitor::VisitParameters(HLSLRoot * root)
	{
		HLSLStatement * statement = root->statement;
		while (statement != nullptr) {
			if (statement->nodeType == HLSLNodeType_Declaration) {
				VisitDeclaration((HLSLDeclaration *)statement);
			}

			statement = statement->nextStatement;
		}
	}

	HLSLStruct* HLSLTreeVisitor::FindGlobalStruct(HLSLRoot* root, const char * name)
	{
		HLSLStatement * statement = root->statement;
		while (statement != nullptr)
		{
			if (statement->nodeType == HLSLNodeType_Struct)
			{
				HLSLStruct * declaration = (HLSLStruct *)statement;
				if (String_Equal(name, declaration->name))
				{
					return declaration;
				}
			}

			statement = statement->nextStatement;
		}

		return nullptr;
	}

	HLSLBuffer* HLSLTreeVisitor::FindGlobalStructWithParameterName(HLSLRoot* root, const char * parameterName)
	{
		HLSLStatement * statement = root->statement;
		while (statement != nullptr)
		{
			if (statement->nodeType == HLSLNodeType_Buffer)
			{
				HLSLBuffer * declaration = (HLSLBuffer *)statement;
				HLSLDeclaration * field = declaration->field;
				while (field != nullptr)
				{
					if (String_Equal(parameterName, field->name))
					{
						return declaration;
					}
					field = (HLSLDeclaration *)field->nextStatement;
				}
			}

			statement = statement->nextStatement;
		}

		return nullptr;
	}

	class ResetHiddenFlagVisitor : public HLSLTreeVisitor
	{
	public:
		virtual void VisitTopLevelStatement(HLSLStatement * statement)
		{
			statement->hidden = true;

			if (statement->nodeType == HLSLNodeType_Buffer)
			{
				VisitBuffer((HLSLBuffer*)statement);
			}
		}

		// Hide buffer fields.
		virtual void VisitDeclaration(HLSLDeclaration * node)
		{
			node->hidden = true;
		}
	};

	class MarkVisibleStatementsVisitor : public HLSLTreeVisitor
	{
	public:
		HLSLTree * tree;
		MarkVisibleStatementsVisitor(HLSLTree * tree) : tree(tree) {}

		virtual void VisitFunction(HLSLFunction * node)
		{
			node->hidden = false;
			HLSLTreeVisitor::VisitFunction(node);
		}

		virtual void VisitFunctionCall(HLSLFunctionCall * node)
		{
			HLSLTreeVisitor::VisitFunctionCall(node);

			if (node->function->hidden)
			{
				VisitFunction(const_cast<HLSLFunction*>(node->function));
			}
		}

		virtual void VisitIdentifierExpression(HLSLIdentifierExpression * node)
		{
			HLSLTreeVisitor::VisitIdentifierExpression(node);

			if (node->global)
			{
				HLSLDeclaration * declaration = tree->FindGlobalDeclaration(node->name);
				if (declaration != nullptr && declaration->hidden)
				{
					declaration->hidden = false;
					VisitDeclaration(declaration);
				}
			}
		}

		virtual void VisitType(HLSLType & type)
		{
			if (type.baseType == HLSLBaseType_UserDefined)
			{
				HLSLStruct * globalStruct = tree->FindGlobalStruct(type.typeName);
				if (globalStruct != nullptr)
				{
					globalStruct->hidden = false;
					VisitStruct(globalStruct);
				}
			}
		}

	};


	void PruneTree(HLSLTree* tree, const char* entryName0, const char* entryName1/*=nullptr*/)
	{
		HLSLRoot* root = tree->GetRoot();

		// Reset all flags.
		ResetHiddenFlagVisitor reset;
		reset.VisitRoot(root);

		// Mark all the statements necessary for these entrypoints.
		HLSLFunction* entry = tree->FindFunction(entryName0);
		if (entry != nullptr)
		{
			MarkVisibleStatementsVisitor mark(tree);
			mark.VisitFunction(entry);
		}

		if (entryName1 != nullptr)
		{
			entry = tree->FindFunction(entryName1);
			if (entry != nullptr)
			{
				MarkVisibleStatementsVisitor mark(tree);
				mark.VisitFunction(entry);
			}
		}

		// Mark buffers visible, if any of their fields is visible.
		HLSLStatement * statement = root->statement;
		while (statement != nullptr)
		{
			if (statement->nodeType == HLSLNodeType_Buffer)
			{
				HLSLBuffer* buffer = (HLSLBuffer*)statement;

				HLSLDeclaration* field = buffer->field;
				while (field != nullptr)
				{
					ASSERT(field->nodeType == HLSLNodeType_Declaration);
					if (!field->hidden)
					{
						buffer->hidden = false;
						break;
					}
					field = (HLSLDeclaration*)field->nextStatement;
				}
			}

			statement = statement->nextStatement;
		}
	}


	void SortTree(HLSLTree * tree)
	{
		// Stable sort so that statements are in this order:
		// structs, declarations, functions, techniques.
		// but their relative order is preserved.

		HLSLRoot* root = tree->GetRoot();

		HLSLStatement* structs = nullptr;
		HLSLStatement* lastStruct = nullptr;
		HLSLStatement* constDeclarations = nullptr;
		HLSLStatement* lastConstDeclaration = nullptr;
		HLSLStatement* declarations = nullptr;
		HLSLStatement* lastDeclaration = nullptr;
		HLSLStatement* functions = nullptr;
		HLSLStatement* lastFunction = nullptr;
		HLSLStatement* other = nullptr;
		HLSLStatement* lastOther = nullptr;

		HLSLStatement* statement = root->statement;
		while (statement != nullptr) {
			HLSLStatement* nextStatement = statement->nextStatement;
			statement->nextStatement = nullptr;

			if (statement->nodeType == HLSLNodeType_Struct) {
				if (structs == nullptr) structs = statement;
				if (lastStruct != nullptr) lastStruct->nextStatement = statement;
				lastStruct = statement;
			}
			else if (statement->nodeType == HLSLNodeType_Declaration || statement->nodeType == HLSLNodeType_Buffer) {
				if (statement->nodeType == HLSLNodeType_Declaration && (((HLSLDeclaration *)statement)->type.flags & HLSLTypeFlag_Const)) {
					if (constDeclarations == nullptr) constDeclarations = statement;
					if (lastConstDeclaration != nullptr) lastConstDeclaration->nextStatement = statement;
					lastConstDeclaration = statement;
				}
				else {
					if (declarations == nullptr) declarations = statement;
					if (lastDeclaration != nullptr) lastDeclaration->nextStatement = statement;
					lastDeclaration = statement;
				}
			}
			else if (statement->nodeType == HLSLNodeType_Function) {
				if (functions == nullptr) functions = statement;
				if (lastFunction != nullptr) lastFunction->nextStatement = statement;
				lastFunction = statement;
			}
			else {
				if (other == nullptr) other = statement;
				if (lastOther != nullptr) lastOther->nextStatement = statement;
				lastOther = statement;
			}

			statement = nextStatement;
		}

		// Chain all the statements in the order that we want.
		HLSLStatement * firstStatement = structs;
		HLSLStatement * lastStatement = lastStruct;

		if (constDeclarations != nullptr) {
			if (firstStatement == nullptr) firstStatement = constDeclarations;
			else lastStatement->nextStatement = constDeclarations;
			lastStatement = lastConstDeclaration;
		}

		if (declarations != nullptr) {
			if (firstStatement == nullptr) firstStatement = declarations;
			else lastStatement->nextStatement = declarations;
			lastStatement = lastDeclaration;
		}

		if (functions != nullptr) {
			if (firstStatement == nullptr) firstStatement = functions;
			else lastStatement->nextStatement = functions;
			lastStatement = lastFunction;
		}

		if (other != nullptr) {
			if (firstStatement == nullptr) firstStatement = other;
			else lastStatement->nextStatement = other;
			lastStatement = lastOther;
		}

		root->statement = firstStatement;
	}





	// First and last can be the same.
	void AddStatements(HLSLRoot * root, HLSLStatement * before, HLSLStatement * first, HLSLStatement * last)
	{
		if (before == nullptr) {
			last->nextStatement = root->statement;
			root->statement = first;
		}
		else {
			last->nextStatement = before->nextStatement;
			before->nextStatement = first;
		}
	}

	void AddSingleStatement(HLSLRoot * root, HLSLStatement * before, HLSLStatement * statement)
	{
		AddStatements(root, before, statement, statement);
	}



	// @@ This is very game-specific. Should be moved to pipeline_parser or somewhere else.
	void GroupParameters(HLSLTree * tree)
	{
		// Sort parameters based on semantic and group them in cbuffers.

		HLSLRoot* root = tree->GetRoot();

		HLSLDeclaration * firstPerItemDeclaration = nullptr;
		HLSLDeclaration * lastPerItemDeclaration = nullptr;

		HLSLDeclaration * instanceDataDeclaration = nullptr;

		HLSLDeclaration * firstPerPassDeclaration = nullptr;
		HLSLDeclaration * lastPerPassDeclaration = nullptr;

		HLSLDeclaration * firstPerItemSampler = nullptr;
		HLSLDeclaration * lastPerItemSampler = nullptr;

		HLSLDeclaration * firstPerPassSampler = nullptr;
		HLSLDeclaration * lastPerPassSampler = nullptr;

		HLSLStatement * statementBeforeBuffers = nullptr;

		HLSLStatement* previousStatement = nullptr;
		HLSLStatement* statement = root->statement;
		while (statement != nullptr) {
			HLSLStatement* nextStatement = statement->nextStatement;

			if (statement->nodeType == HLSLNodeType_Struct) // Do not remove this, or it will mess the else clause below.
			{
				statementBeforeBuffers = statement;
			}
			else if (statement->nodeType == HLSLNodeType_Declaration)
			{
				HLSLDeclaration* declaration = (HLSLDeclaration*)statement;

				// We insert buffers after the last const declaration.
				if ((declaration->type.flags & HLSLTypeFlag_Const) != 0)
				{
					statementBeforeBuffers = statement;
				}

				// Do not move samplers or static/const parameters.
				if ((declaration->type.flags & (HLSLTypeFlag_Static | HLSLTypeFlag_Const)) == 0)
				{
					// Unlink statement.
					statement->nextStatement = nullptr;
					if (previousStatement != nullptr) previousStatement->nextStatement = nextStatement;
					else root->statement = nextStatement;

					while (declaration != nullptr)
					{
						HLSLDeclaration* nextDeclaration = declaration->nextDeclaration;

						if (declaration->semantic != nullptr && String_EqualNoCase(declaration->semantic, "PER_INSTANCED_ITEM"))
						{
							ASSERT(instanceDataDeclaration == nullptr);
							instanceDataDeclaration = declaration;
						}
						else
						{
							// Select group based on type and semantic.
							HLSLDeclaration ** first, ** last;
							if (declaration->semantic == nullptr || String_EqualNoCase(declaration->semantic, "PER_ITEM") || String_EqualNoCase(declaration->semantic, "PER_MATERIAL"))
							{
								if (IsSamplerType(declaration->type))
								{
									first = &firstPerItemSampler;
									last = &lastPerItemSampler;
								}
								else
								{
									first = &firstPerItemDeclaration;
									last = &lastPerItemDeclaration;
								}
							}
							else
							{
								if (IsSamplerType(declaration->type))
								{
									first = &firstPerPassSampler;
									last = &lastPerPassSampler;
								}
								else
								{
									first = &firstPerPassDeclaration;
									last = &lastPerPassDeclaration;
								}
							}

							// Add declaration to new list.
							if (*first == nullptr) *first = declaration;
							else (*last)->nextStatement = declaration;
							*last = declaration;
						}

						// Unlink from declaration list.
						declaration->nextDeclaration = nullptr;

						// Reset attributes.
						declaration->registerName = nullptr;
						//declaration->semantic = nullptr;         // @@ Don't do this!

						declaration = nextDeclaration;
					}
				}
			}
			/*else
			{
				if (statementBeforeBuffers == nullptr) {
					// This is the location where we will insert our buffers.
					statementBeforeBuffers = previousStatement;
				}
			}*/

			if (statement->nextStatement == nextStatement) {
				previousStatement = statement;
			}
			statement = nextStatement;
		}


		// Add instance data declaration at the end of the per_item buffer.
		if (instanceDataDeclaration != nullptr)
		{
			if (firstPerItemDeclaration == nullptr) firstPerItemDeclaration = instanceDataDeclaration;
			else lastPerItemDeclaration->nextStatement = instanceDataDeclaration;
		}


		// Add samplers.
		if (firstPerItemSampler != nullptr) {
			AddStatements(root, statementBeforeBuffers, firstPerItemSampler, lastPerItemSampler);
			statementBeforeBuffers = lastPerItemSampler;
		}
		if (firstPerPassSampler != nullptr) {
			AddStatements(root, statementBeforeBuffers, firstPerPassSampler, lastPerPassSampler);
			statementBeforeBuffers = lastPerPassSampler;
		}


		// @@ We are assuming per_item and per_pass buffers don't already exist. @@ We should assert on that.

		if (firstPerItemDeclaration != nullptr)
		{
			// Create buffer statement.
			HLSLBuffer * perItemBuffer = tree->AddNode<HLSLBuffer>(firstPerItemDeclaration->fileName, firstPerItemDeclaration->line - 1);
			perItemBuffer->name = tree->AddString("per_item");
			perItemBuffer->registerName = tree->AddString("b0");
			perItemBuffer->field = firstPerItemDeclaration;

			// Add buffer to statements.
			AddSingleStatement(root, statementBeforeBuffers, perItemBuffer);
			statementBeforeBuffers = perItemBuffer;
		}

		if (firstPerPassDeclaration != nullptr)
		{
			// Create buffer statement.
			HLSLBuffer * perPassBuffer = tree->AddNode<HLSLBuffer>(firstPerPassDeclaration->fileName, firstPerPassDeclaration->line - 1);
			perPassBuffer->name = tree->AddString("per_pass");
			perPassBuffer->registerName = tree->AddString("b1");
			perPassBuffer->field = firstPerPassDeclaration;

			// Add buffer to statements.
			AddSingleStatement(root, statementBeforeBuffers, perPassBuffer);
		}
	}


}
