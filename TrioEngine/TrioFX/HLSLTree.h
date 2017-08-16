#pragma once

#include "CommonHeader.h"

#include "StringPool.h"
#include <vector>
#include <map>


using namespace std;

namespace CuadoFX
{
	enum TRIOFXAPI_DLL HLSLNodeType
	{
		HLSLNodeType_Root,
		HLSLNodeType_Declaration,
		HLSLNodeType_Struct,
		HLSLNodeType_StructField,
		HLSLNodeType_Buffer,
		HLSLNodeType_BufferField,
		HLSLNodeType_Function,
		HLSLNodeType_Argument,
		HLSLNodeType_ExpressionStatement,
		HLSLNodeType_Expression,
		HLSLNodeType_ReturnStatement,
		HLSLNodeType_DiscardStatement,
		HLSLNodeType_BreakStatement,
		HLSLNodeType_ContinueStatement,
		HLSLNodeType_IfStatement,
		HLSLNodeType_ForStatement,
		HLSLNodeType_BlockStatement,
		HLSLNodeType_UnaryExpression,
		HLSLNodeType_BinaryExpression,
		HLSLNodeType_ConditionalExpression,
		HLSLNodeType_CastingExpression,
		HLSLNodeType_LiteralExpression,
		HLSLNodeType_IdentifierExpression,
		HLSLNodeType_ConstructorExpression,
		HLSLNodeType_MemberAccess,
		HLSLNodeType_ArrayAccess,
		HLSLNodeType_FunctionCall,
		HLSLNodeType_StateAssignment,
		HLSLNodeType_SamplerState,
		HLSLNodeType_Pass,
		HLSLNodeType_Technique,
		HLSLNodeType_Attribute,
		HLSLNodeType_Pipeline,
		HLSLNodeType_Stage,

		HLSLNodeType_Pass11,
		HLSLNodeType_Technique11,
		HLSLNodeType_PassShader,
		HLSLNodeType_CompilerShader,
	};

	enum TRIOFXAPI_DLL HLSLBaseType
	{
		HLSLBaseType_Unknown,
		HLSLBaseType_Void,
		HLSLBaseType_Float,
		HLSLBaseType_FirstNumeric = HLSLBaseType_Float,
		HLSLBaseType_Float2,
		HLSLBaseType_Float3,
		HLSLBaseType_Float4,
		HLSLBaseType_Float3x3,
		HLSLBaseType_Float4x4,
		HLSLBaseType_Half,
		HLSLBaseType_Half2,
		HLSLBaseType_Half3,
		HLSLBaseType_Half4,
		HLSLBaseType_Half3x3,
		HLSLBaseType_Half4x4,
		HLSLBaseType_Bool,
		HLSLBaseType_FirstInteger = HLSLBaseType_Bool,
		HLSLBaseType_Int,
		HLSLBaseType_Int2,
		HLSLBaseType_Int3,
		HLSLBaseType_Int4,
		HLSLBaseType_Uint,
		HLSLBaseType_Uint2,
		HLSLBaseType_Uint3,
		HLSLBaseType_Uint4,
		HLSLBaseType_LastInteger = HLSLBaseType_Uint4,
		HLSLBaseType_LastNumeric = HLSLBaseType_Uint4,
		HLSLBaseType_Texture,
		HLSLBaseType_Sampler,           // @@ use type inference to determine sampler type.
		HLSLBaseType_Sampler2D,
		HLSLBaseType_Sampler3D,
		HLSLBaseType_SamplerCube,
		HLSLBaseType_Sampler2DShadow,
		HLSLBaseType_Sampler2DMS,
		HLSLBaseType_Texture2D,
		HLSLBaseType_UserDefined,       // struct

		HLSLBaseType_Count,
		HLSLBaseType_NumericCount = HLSLBaseType_LastNumeric - HLSLBaseType_FirstNumeric + 1
	};

	inline bool IsSamplerType(HLSLBaseType baseType)
	{
		return baseType == HLSLBaseType_Sampler ||
			baseType == HLSLBaseType_Sampler2D ||
			baseType == HLSLBaseType_Sampler3D ||
			baseType == HLSLBaseType_SamplerCube ||
			baseType == HLSLBaseType_Sampler2DShadow ||
			baseType == HLSLBaseType_Sampler2DMS;
	}

	enum TRIOFXAPI_DLL HLSLBinaryOp
	{
		HLSLBinaryOp_And,
		HLSLBinaryOp_Or,
		HLSLBinaryOp_Add,
		HLSLBinaryOp_Sub,
		HLSLBinaryOp_Mul,
		HLSLBinaryOp_Div,
		HLSLBinaryOp_Less,
		HLSLBinaryOp_Greater,
		HLSLBinaryOp_LessEqual,
		HLSLBinaryOp_GreaterEqual,
		HLSLBinaryOp_Equal,
		HLSLBinaryOp_NotEqual,
		HLSLBinaryOp_BitAnd,
		HLSLBinaryOp_BitOr,
		HLSLBinaryOp_BitXor,
		HLSLBinaryOp_Assign,
		HLSLBinaryOp_AddAssign,
		HLSLBinaryOp_SubAssign,
		HLSLBinaryOp_MulAssign,
		HLSLBinaryOp_DivAssign,
	};

	enum TRIOFXAPI_DLL HLSLUnaryOp
	{
		HLSLUnaryOp_Negative,       // -x
		HLSLUnaryOp_Positive,       // +x
		HLSLUnaryOp_Not,            // !x
		HLSLUnaryOp_PreIncrement,   // ++x
		HLSLUnaryOp_PreDecrement,   // --x
		HLSLUnaryOp_PostIncrement,  // x++
		HLSLUnaryOp_PostDecrement,  // x++
		HLSLUnaryOp_BitNot,         // ~x
	};

	enum TRIOFXAPI_DLL HLSLArgumentModifier
	{
		HLSLArgumentModifier_None,
		HLSLArgumentModifier_In,
		HLSLArgumentModifier_Out,
		HLSLArgumentModifier_Inout,
		HLSLArgumentModifier_Uniform,
	};

	enum TRIOFXAPI_DLL HLSLTypeFlags
	{
		HLSLTypeFlag_None = 0,
		HLSLTypeFlag_Const = 0x01,
		HLSLTypeFlag_Static = 0x02,
		//HLSLTypeFlag_Uniform = 0x04,
		//HLSLTypeFlag_Extern = 0x10,
		//HLSLTypeFlag_Volatile = 0x20,
		//HLSLTypeFlag_Shared = 0x40,
		//HLSLTypeFlag_Precise = 0x80,

		HLSLTypeFlag_Input = 0x100,
		HLSLTypeFlag_Output = 0x200,

		// Interpolation modifiers.
		HLSLTypeFlag_Linear = 0x10000,
		HLSLTypeFlag_Centroid = 0x20000,
		HLSLTypeFlag_NoInterpolation = 0x40000,
		HLSLTypeFlag_NoPerspective = 0x80000,
		HLSLTypeFlag_Sample = 0x100000,
	};

	enum TRIOFXAPI_DLL HLSLAttributeType
	{
		HLSLAttributeType_Unroll,
		HLSLAttributeType_Branch,
		HLSLAttributeType_Flatten,
	};


	struct TRIOFXAPI_DLL HLSLNode;
	struct TRIOFXAPI_DLL HLSLRoot;
	struct TRIOFXAPI_DLL HLSLStatement;
	struct TRIOFXAPI_DLL HLSLAttribute;
	struct TRIOFXAPI_DLL HLSLDeclaration;
	struct TRIOFXAPI_DLL HLSLStruct;
	struct TRIOFXAPI_DLL HLSLStructField;
	struct TRIOFXAPI_DLL HLSLBuffer;
	//struct HLSLBufferField;
	struct TRIOFXAPI_DLL HLSLFunction;
	struct TRIOFXAPI_DLL HLSLArgument;
	struct TRIOFXAPI_DLL HLSLExpressionStatement;
	struct TRIOFXAPI_DLL HLSLExpression;
	struct TRIOFXAPI_DLL HLSLBinaryExpression;
	struct TRIOFXAPI_DLL HLSLLiteralExpression;
	struct TRIOFXAPI_DLL HLSLIdentifierExpression;
	struct TRIOFXAPI_DLL HLSLConstructorExpression;
	struct TRIOFXAPI_DLL HLSLFunctionCall;
	struct TRIOFXAPI_DLL HLSLArrayAccess;
	struct TRIOFXAPI_DLL HLSLAttribute;

	struct TRIOFXAPI_DLL HLSLTechnique;
	struct TRIOFXAPI_DLL HLSLTechnique11;
	struct TRIOFXAPI_DLL HLSLPass;
	struct TRIOFXAPI_DLL HLSLPass11;
	struct TRIOFXAPI_DLL HLSLPassShader;
	struct TRIOFXAPI_DLL HLSLCompilerShader;
	

	struct TRIOFXAPI_DLL HLSLType
	{
		explicit HLSLType(HLSLBaseType _baseType = HLSLBaseType_Unknown)
		{
			baseType = _baseType;
			typeName = nullptr;
			array = false;
			arraySize = nullptr;
			flags = 0;
		}
		HLSLBaseType        baseType;
		const char*         typeName;       // For user defined types.
		bool                array;
		HLSLExpression*     arraySize;
		int                 flags;
	};

	inline bool IsSamplerType(const HLSLType & type)
	{
		return IsSamplerType(type.baseType);
	}


	/** Base class for all nodes in the HLSL AST */
	struct TRIOFXAPI_DLL HLSLNode
	{
		HLSLNodeType        nodeType;
		const char*         fileName;
		int                 line;
	};

	struct TRIOFXAPI_DLL HLSLRoot : public HLSLNode
	{
		static const HLSLNodeType s_type = HLSLNodeType_Root;
		HLSLRoot()          { statement = nullptr; }
		HLSLStatement*      statement;          // First statement.
	};

	struct TRIOFXAPI_DLL HLSLStatement : public HLSLNode
	{
		HLSLStatement()
		{
			nextStatement = nullptr;
			attributes = nullptr;
			hidden = false;
		}
		HLSLStatement*      nextStatement;      // Next statement in the block.
		HLSLAttribute*      attributes;
		mutable bool        hidden;
	};

	struct TRIOFXAPI_DLL HLSLAttribute : public HLSLNode
	{
		static const HLSLNodeType s_type = HLSLNodeType_Attribute;
		HLSLAttribute()
		{
			argument = nullptr;
			nextAttribute = nullptr;
		}
		HLSLAttributeType   attributeType;
		HLSLExpression*     argument;
		HLSLAttribute*      nextAttribute;
	};

	struct TRIOFXAPI_DLL HLSLDeclaration : public HLSLStatement
	{
		static const HLSLNodeType s_type = HLSLNodeType_Declaration;
		HLSLDeclaration()
		{
			name = nullptr;
			registerName = nullptr;
			semantic = nullptr;
			nextDeclaration = nullptr;
			assignment = nullptr;
			templateName = nullptr;
		}
		const char*         name;
		HLSLType            type;
		const char*         registerName;       // @@ Store register index?
		const char*         semantic;
		
		HLSLType            templateType;
		const char*         templateName;
		
		HLSLDeclaration*    nextDeclaration;    // If multiple variables declared on a line.
		HLSLExpression*     assignment;
	};

	struct TRIOFXAPI_DLL HLSLStruct : public HLSLStatement
	{
		static const HLSLNodeType s_type = HLSLNodeType_Struct;
		HLSLStruct()
		{
			name = nullptr;
			field = nullptr;
		}
		const char*         name;
		HLSLStructField*    field;              // First field in the structure.
	};

	struct TRIOFXAPI_DLL HLSLStructField : public HLSLNode
	{
		static const HLSLNodeType s_type = HLSLNodeType_StructField;
		HLSLStructField()
		{
			name = nullptr;
			semantic = nullptr;
			sv_semantic = nullptr;
			nextField = nullptr;
			hidden = false;
		}
		const char*         name;
		HLSLType            type;
		const char*         semantic;
		const char*         sv_semantic;
		HLSLStructField*    nextField;      // Next field in the structure.
		bool				hidden;
	};

	/** A cbuffer or tbuffer declaration. */
	struct TRIOFXAPI_DLL HLSLBuffer : public HLSLStatement
	{
		static const HLSLNodeType s_type = HLSLNodeType_Buffer;
		HLSLBuffer()
		{
			name = nullptr;
			registerName = nullptr;
			field = nullptr;
		}
		const char*         name;
		const char*         registerName;
		HLSLDeclaration*    field;
	};

	/** Field declaration inside of a cbuffer or tbuffer */ // @@ Isn't this just a regular declaration?
	/*struct HLSLBufferField : public HLSLNode
	{
	static const HLSLNodeType s_type = HLSLNodeType_BufferField;
	HLSLBufferField()
	{
	name            = nullptr;
	nextField       = nullptr;
	}
	const char*         name;
	HLSLType            type;
	HLSLBufferField*    nextField;      // Next field in the cbuffer.
	};*/

	/** Function declaration */
	struct TRIOFXAPI_DLL HLSLFunction : public HLSLStatement
	{
		static const HLSLNodeType s_type = HLSLNodeType_Function;
		HLSLFunction()
		{
			name = nullptr;
			semantic = nullptr;
			sv_semantic = nullptr;
			statement = nullptr;
			argument = nullptr;
			numArguments = 0;
		}
		const char*         name;
		HLSLType            returnType;
		const char*         semantic;
		const char*         sv_semantic;
		int                 numArguments;
		HLSLArgument*       argument;
		HLSLStatement*      statement;
	};

	/** Declaration of an argument to a function. */
	struct TRIOFXAPI_DLL HLSLArgument : public HLSLNode
	{
		static const HLSLNodeType s_type = HLSLNodeType_Argument;
		HLSLArgument()
		{
			name = nullptr;
			modifier = HLSLArgumentModifier_None;
			semantic = nullptr;
			sv_semantic = nullptr;
			defaultValue = nullptr;
			nextArgument = nullptr;
		}
		const char*             name;
		HLSLArgumentModifier    modifier;
		HLSLType                type;
		const char*             semantic;
		const char*             sv_semantic;
		HLSLExpression*         defaultValue;
		HLSLArgument*           nextArgument;
	};

	/** A expression which forms a complete statement. */
	struct TRIOFXAPI_DLL HLSLExpressionStatement : public HLSLStatement
	{
		static const HLSLNodeType s_type = HLSLNodeType_ExpressionStatement;
		HLSLExpressionStatement()
		{
			expression = nullptr;
		}
		HLSLExpression*     expression;
	};

	struct TRIOFXAPI_DLL HLSLReturnStatement : public HLSLStatement
	{
		static const HLSLNodeType s_type = HLSLNodeType_ReturnStatement;
		HLSLReturnStatement()
		{
			expression = nullptr;
		}
		HLSLExpression*     expression;
	};

	struct TRIOFXAPI_DLL HLSLDiscardStatement : public HLSLStatement
	{
		static const HLSLNodeType s_type = HLSLNodeType_DiscardStatement;
	};

	struct TRIOFXAPI_DLL HLSLBreakStatement : public HLSLStatement
	{
		static const HLSLNodeType s_type = HLSLNodeType_BreakStatement;
	};

	struct TRIOFXAPI_DLL HLSLContinueStatement : public HLSLStatement
	{
		static const HLSLNodeType s_type = HLSLNodeType_ContinueStatement;
	};

	struct TRIOFXAPI_DLL HLSLIfStatement : public HLSLStatement
	{
		static const HLSLNodeType s_type = HLSLNodeType_IfStatement;
		HLSLIfStatement()
		{
			condition = nullptr;
			statement = nullptr;
			elseStatement = nullptr;
		}
		HLSLExpression*     condition;
		HLSLStatement*      statement;
		HLSLStatement*      elseStatement;
	};

	struct TRIOFXAPI_DLL HLSLForStatement : public HLSLStatement
	{
		static const HLSLNodeType s_type = HLSLNodeType_ForStatement;
		HLSLForStatement()
		{
			initialization = nullptr;
			condition = nullptr;
			increment = nullptr;
			statement = nullptr;
		}
		HLSLDeclaration*    initialization;
		HLSLExpression*     condition;
		HLSLExpression*     increment;
		HLSLStatement*      statement;
	};

	struct TRIOFXAPI_DLL HLSLBlockStatement : public HLSLStatement
	{
		static const HLSLNodeType s_type = HLSLNodeType_BlockStatement;
		HLSLBlockStatement()
		{
			statement = nullptr;
		}
		HLSLStatement*      statement;
	};


	/** Base type for all types of expressions. */
	struct TRIOFXAPI_DLL HLSLExpression : public HLSLNode
	{
		static const HLSLNodeType s_type = HLSLNodeType_Expression;
		HLSLExpression()
		{
			nextExpression = nullptr;
		}
		HLSLType            expressionType;
		HLSLExpression*     nextExpression; // Used when the expression is part of a list, like in a function call.
	};

	struct TRIOFXAPI_DLL HLSLUnaryExpression : public HLSLExpression
	{
		static const HLSLNodeType s_type = HLSLNodeType_UnaryExpression;
		HLSLUnaryExpression()
		{
			expression = nullptr;
		}
		HLSLUnaryOp         unaryOp;
		HLSLExpression*     expression;
	};

	struct TRIOFXAPI_DLL HLSLBinaryExpression : public HLSLExpression
	{
		static const HLSLNodeType s_type = HLSLNodeType_BinaryExpression;
		HLSLBinaryExpression()
		{
			expression1 = nullptr;
			expression2 = nullptr;
		}
		HLSLBinaryOp        binaryOp;
		HLSLExpression*     expression1;
		HLSLExpression*     expression2;
	};

	/** ? : construct */
	struct TRIOFXAPI_DLL HLSLConditionalExpression : public HLSLExpression
	{
		static const HLSLNodeType s_type = HLSLNodeType_ConditionalExpression;
		HLSLConditionalExpression()
		{
			condition = nullptr;
			trueExpression = nullptr;
			falseExpression = nullptr;
		}
		HLSLExpression*     condition;
		HLSLExpression*     trueExpression;
		HLSLExpression*     falseExpression;
	};

	struct TRIOFXAPI_DLL HLSLCastingExpression : public HLSLExpression
	{
		static const HLSLNodeType s_type = HLSLNodeType_CastingExpression;
		HLSLCastingExpression()
		{
			expression = nullptr;
		}
		HLSLType            type;
		HLSLExpression*     expression;
	};

	/** Float, integer, boolean, etc. literal constant. */
	struct TRIOFXAPI_DLL HLSLLiteralExpression : public HLSLExpression
	{
		static const HLSLNodeType s_type = HLSLNodeType_LiteralExpression;
		HLSLBaseType        type;   // Note, not all types can be literals.
		union
		{
			bool            bValue;
			float           fValue;
			int             iValue;
		};
	};

	/** An identifier, typically a variable name or structure field name. */
	struct TRIOFXAPI_DLL HLSLIdentifierExpression : public HLSLExpression
	{
		static const HLSLNodeType s_type = HLSLNodeType_IdentifierExpression;
		HLSLIdentifierExpression()
		{
			name = nullptr;
			global = false;
		}
		const char*         name;
		bool                global; // This is a global variable.
	};

	/** float2(1, 2) */
	struct TRIOFXAPI_DLL HLSLConstructorExpression : public HLSLExpression
	{
		static const HLSLNodeType s_type = HLSLNodeType_ConstructorExpression;
		HLSLConstructorExpression()
		{
			argument = nullptr;
		}
		HLSLType            type;
		HLSLExpression*     argument;
	};

	/** object.member **/
	struct TRIOFXAPI_DLL HLSLMemberAccess : public HLSLExpression
	{
		static const HLSLNodeType s_type = HLSLNodeType_MemberAccess;
		HLSLMemberAccess()
		{
			object = nullptr;
			field = nullptr;
		}
		HLSLExpression*     object;
		const char*         field;
	};

	/** array[index] **/
	struct TRIOFXAPI_DLL HLSLArrayAccess : public HLSLExpression
	{
		static const HLSLNodeType s_type = HLSLNodeType_ArrayAccess;
		HLSLArrayAccess()
		{
			array = nullptr;
			index = nullptr;
		}
		HLSLExpression*     array;
		HLSLExpression*     index;
	};

	struct TRIOFXAPI_DLL HLSLFunctionCall : public HLSLExpression
	{
		static const HLSLNodeType s_type = HLSLNodeType_FunctionCall;
		HLSLFunctionCall()
		{
			function = nullptr;
			argument = nullptr;
			numArguments = 0;
		}
		const HLSLFunction* function;
		int                 numArguments;
		HLSLExpression*     argument;
	};

	struct TRIOFXAPI_DLL HLSLStateAssignment : public HLSLNode
	{
		static const HLSLNodeType s_type = HLSLNodeType_StateAssignment;
		HLSLStateAssignment()
		{
			stateName = nullptr;
			sValue = nullptr;
			nextStateAssignment = nullptr;
		}

		const char*             stateName;
		int                     d3dRenderState;
		union {
			int                 iValue;
			float               fValue;
			const char *        sValue;
		};
		HLSLStateAssignment*    nextStateAssignment;
	};

	struct TRIOFXAPI_DLL HLSLSamplerState : public HLSLExpression // @@ Does this need to be an expression? Does it have a type? I guess type is useful.
	{
		static const HLSLNodeType s_type = HLSLNodeType_SamplerState;
		HLSLSamplerState()
		{
			numStateAssignments = 0;
			stateAssignments = nullptr;
		}

		int                     numStateAssignments;
		HLSLStateAssignment*    stateAssignments;
	};

	struct TRIOFXAPI_DLL HLSLPass : public HLSLNode
	{
		static const HLSLNodeType s_type = HLSLNodeType_Pass;
		HLSLPass()
		{
			name = nullptr;
			numStateAssignments = 0;
			stateAssignments = nullptr;
			nextPass = nullptr;
		}

		const char*             name;
		int                     numStateAssignments;
		HLSLStateAssignment*    stateAssignments;
		HLSLPass*               nextPass;
	};

	struct TRIOFXAPI_DLL HLSLPassShader : public HLSLNode
	{
		static const HLSLNodeType s_type = HLSLNodeType_PassShader;
		HLSLPassShader()
		{
			name = nullptr;
			options = nullptr;
			nextShader = nullptr;
		}

		const char*             name;
		HLSLCompilerShader*     options;
		HLSLPassShader*               nextShader;
	};

	struct TRIOFXAPI_DLL HLSLPass11 : public HLSLNode
	{
		static const HLSLNodeType s_type = HLSLNodeType_Pass11;
		HLSLPass11()
		{
			name = nullptr;
			options = nullptr;
			nextPass = nullptr;
			shader = nullptr;
			numPassShader = 0;
		}

		const char*             name;
		HLSLCompilerShader*     options;
		
		HLSLPassShader*               shader;
		int numPassShader;

		HLSLPass11*               nextPass;
	};

	struct TRIOFXAPI_DLL HLSLTechnique : public HLSLStatement
	{
		static const HLSLNodeType s_type = HLSLNodeType_Technique;
		HLSLTechnique()
		{
			name = nullptr;
			numPasses = 0;
			passes = nullptr;
		}

		const char*         name;
		int                 numPasses;
		HLSLPass*           passes;
	};

	struct TRIOFXAPI_DLL HLSLTechnique11 : public HLSLStatement
	{
		static const HLSLNodeType s_type = HLSLNodeType_Technique11;
		HLSLTechnique11()
		{
			name = nullptr;
			numPasses = 0;
			passes = nullptr;
		}

		const char*         name;
		int                 numPasses;
		HLSLPass11*           passes;
	};

	struct TRIOFXAPI_DLL HLSLCompilerShader : public HLSLNode
	{
		static const HLSLNodeType s_type = HLSLNodeType_CompilerShader;
		HLSLCompilerShader()
		{
			profile = nullptr;
			functionCall = nullptr;
			functionCallName = nullptr;
		}
		const char*         profile;
		HLSLFunctionCall*   functionCall;
		const char*   functionCallName;
	};

	struct TRIOFXAPI_DLL HLSLPipeline : public HLSLStatement
	{
		static const HLSLNodeType s_type = HLSLNodeType_Pipeline;
		HLSLPipeline()
		{
			name = nullptr;
			numStateAssignments = 0;
			stateAssignments = nullptr;
		}

		const char*             name;
		int                     numStateAssignments;
		HLSLStateAssignment*    stateAssignments;
	};

	struct TRIOFXAPI_DLL HLSLStage : public HLSLStatement
	{
		static const HLSLNodeType s_type = HLSLNodeType_Stage;
		HLSLStage()
		{
			name = nullptr;
			statement = nullptr;
			inputs = nullptr;
			outputs = nullptr;
		}

		const char*             name;
		HLSLStatement*          statement;
		HLSLDeclaration*        inputs;
		HLSLDeclaration*        outputs;
	};


	/**
	* Abstract syntax tree for parsed HLSL code.
	*/
	class TRIOFXAPI_DLL HLSLTree
	{

	public:

		explicit HLSLTree();
		~HLSLTree();

		/** Adds a string to the string pool used by the tree. */
		const char* AddString(const char* string);

		/** Returns true if the string is contained within the tree. */
		bool GetContainsString(const char* string) const;

		/** Returns the root block in the tree */
		HLSLRoot* GetRoot() const;

		/** Adds a new node to the tree with the specified type. */
		template <class T>
		T* AddNode(const char* fileName, int line)
		{
			HLSLNode* node = new (AllocateMemory(sizeof(T))) T();
			node->nodeType = T::s_type;
			node->fileName = fileName;
			node->line = line;
			return static_cast<T*>(node);
		}

		HLSLFunction * FindFunction(const char * name);
		HLSLDeclaration * FindGlobalDeclaration(const char * name);
		HLSLStruct * FindGlobalStruct(const char * name);
		HLSLTechnique * FindTechnique(const char * name);
		HLSLTechnique11 * FindTechnique11(const char * name);
		HLSLPipeline * FindFirstPipeline();
		HLSLPipeline * FindNextPipeline(HLSLPipeline * current);
		HLSLPipeline * FindPipeline(const char * name);
		HLSLBuffer * FindBuffer(const char * name);
		HLSLTechnique11 * FindFirstTechnique11();

		void PopulateEffectCollections(HLSLTree* root, vector<HLSLTechnique11*> &techniques, vector<HLSLBuffer*> &buffers, vector<HLSLStruct*> &structures);

		bool GetExpressionValue(HLSLExpression * expression, int & value);
		//bool GetExpressionValue(HLSLExpression * expression, float & value);

	private:

		void* AllocateMemory(size_t size);
		void  AllocatePage();

	private:

		static const size_t s_nodePageSize = 1024 * 4;

		struct NodePage
		{
			NodePage*   next;
			char        buffer[s_nodePageSize];
		};

		StringPool      m_stringPool;
		HLSLRoot*       m_root;

		NodePage*       m_firstPage;
		NodePage*       m_currentPage;
		size_t          m_currentPageOffset;

	};



	class TRIOFXAPI_DLL HLSLTreeVisitor
	{
	public:
		virtual void VisitType(HLSLType & type);

		virtual void VisitRoot(HLSLRoot * node);
		virtual void VisitTopLevelStatement(HLSLStatement * node);

		virtual void MarkVisitNode(HLSLNode * node);

		virtual void VisitStatements(HLSLStatement * statement);
		virtual void VisitStatement(HLSLStatement * node);
		virtual void VisitDeclaration(HLSLDeclaration * node);
		virtual void VisitStruct(HLSLStruct * node);
		virtual void VisitStructField(HLSLStructField * node);
		virtual void VisitBuffer(HLSLBuffer * node);
		//virtual void VisitBufferField(HLSLBufferField * node);
		virtual void VisitFunction(HLSLFunction * node);
		virtual void VisitArgument(HLSLArgument * node);
		virtual void VisitExpressionStatement(HLSLExpressionStatement * node);
		virtual void VisitExpression(HLSLExpression * node);
		virtual void VisitReturnStatement(HLSLReturnStatement * node);
		virtual void VisitDiscardStatement(HLSLDiscardStatement * node);
		virtual void VisitBreakStatement(HLSLBreakStatement * node);
		virtual void VisitContinueStatement(HLSLContinueStatement * node);
		virtual void VisitIfStatement(HLSLIfStatement * node);
		virtual void VisitForStatement(HLSLForStatement * node);
		virtual void VisitBlockStatement(HLSLBlockStatement * node);
		virtual void VisitUnaryExpression(HLSLUnaryExpression * node);
		virtual void VisitBinaryExpression(HLSLBinaryExpression * node);
		virtual void VisitConditionalExpression(HLSLConditionalExpression * node);
		virtual void VisitCastingExpression(HLSLCastingExpression * node);
		virtual void VisitLiteralExpression(HLSLLiteralExpression * node);
		virtual void VisitIdentifierExpression(HLSLIdentifierExpression * node);
		virtual void VisitConstructorExpression(HLSLConstructorExpression * node);
		virtual void VisitMemberAccess(HLSLMemberAccess * node);
		virtual void VisitArrayAccess(HLSLArrayAccess * node);
		virtual void VisitFunctionCall(HLSLFunctionCall * node);
		virtual void VisitStateAssignment(HLSLStateAssignment * node);
		virtual void VisitSamplerState(HLSLSamplerState * node);
		virtual void VisitPass(HLSLPass * node);
		virtual void VisitTechnique(HLSLTechnique * node);

		virtual void VisitFunctions(HLSLRoot * root);
		virtual void VisitParameters(HLSLRoot * root);

		HLSLFunction * FindFunction(HLSLRoot * root, const char * name);
		HLSLDeclaration * FindGlobalDeclaration(HLSLRoot * root, const char * name);
		HLSLStruct * FindGlobalStruct(HLSLRoot * root, const char * name);
		HLSLBuffer * FindGlobalStructWithParameterName(HLSLRoot* root, const char * parameterName);
	private:
		
	};


	// Tree transformations:
	extern void PruneTree(HLSLTree* tree, const char* entryName0, const char* entryName1 = nullptr);
	extern void SortTree(HLSLTree* tree);
	extern void GroupParameters(HLSLTree* tree);

}