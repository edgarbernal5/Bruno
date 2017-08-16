#include "stdafx.h"
#include "GLSLGenerator.h"

#include "StringHelpers.h"
#include "Log.h"
#include "AssertFX.h"

#include "HLSLParser.h"
#include "HLSLTree.h"

#include <stdarg.h>

#pragma warning(disable : 4996)

namespace CuadoFX
{
	static const HLSLType kBoolType(HLSLBaseType_Bool);

	/*
	TODO: actualizar c�digo con el repositorio

	https://github.com/Thekla/hlslparser/
	*/

	// http://www.opengl.org/registry/doc/GLSLangSpec.Full.1.40.08.pdf

	static const char* _builtInSemantics[] =
	{
		"SV_POSITION", "gl_Position",
		//"NORMAL", "gl_Normal",
		"DEPTH", "gl_FragDepth",
	};

	// These are reserved words in GLSL that aren't reserved in HLSL.
	const char* GLSLGenerator::s_reservedWord[] =
	{
		"output",
		"input",
		"mod",
		"mix",
		"fract"
	};

	static const char* GetTypeName(const HLSLType& type)
	{
		switch (type.baseType)
		{
		case HLSLBaseType_Void:         return "void";
		case HLSLBaseType_Float:        return "float";
		case HLSLBaseType_Float2:       return "vec2";
		case HLSLBaseType_Float3:       return "vec3";
		case HLSLBaseType_Float4:       return "vec4";
		case HLSLBaseType_Float3x3:     return "mat3";
		case HLSLBaseType_Float4x4:     return "mat4";
		case HLSLBaseType_Half:         return "float";
		case HLSLBaseType_Half2:        return "vec2";
		case HLSLBaseType_Half3:        return "vec3";
		case HLSLBaseType_Half4:        return "vec4";
		case HLSLBaseType_Half3x3:      return "mat3";
		case HLSLBaseType_Half4x4:      return "mat4";
		case HLSLBaseType_Bool:         return "bool";
		case HLSLBaseType_Int:          return "int";
		case HLSLBaseType_Int2:         return "ivec2";
		case HLSLBaseType_Int3:         return "ivec3";
		case HLSLBaseType_Int4:         return "ivec4";
		case HLSLBaseType_Uint:         return "uint";
		case HLSLBaseType_Uint2:        return "uvec2";
		case HLSLBaseType_Uint3:        return "uvec3";
		case HLSLBaseType_Uint4:        return "uvec4";
		case HLSLBaseType_Texture:      return "texture";
		case HLSLBaseType_Sampler:      return "sampler";
		case HLSLBaseType_Sampler2D:    return "sampler2D";
		case HLSLBaseType_Sampler3D:    return "sampler3D";
		case HLSLBaseType_SamplerCube:  return "samplerCube";
		//case HLSLBaseType_Texture2D:    return "Texture2D";
		case HLSLBaseType_UserDefined:  return type.typeName;
		}
		ASSERT(0);
		return "?";
	}

	static bool GetCanImplicitCast(const HLSLType& srcType, const HLSLType& dstType)
	{
		return srcType.baseType == dstType.baseType;
	}

	static const char* GetBuiltInSemantic(const char* semantic)
	{
		int numBuiltInSemantics = sizeof(_builtInSemantics) / (2 * sizeof(const char*));
		for (int i = 0; i < numBuiltInSemantics; ++i)
		{
			if (String_EqualNoCase(semantic, _builtInSemantics[i * 2 + 0]))
			{
				return _builtInSemantics[i * 2 + 1];
			}
		}
		return nullptr;
	}

	static int GetFunctionArguments(HLSLFunctionCall* functionCall, HLSLExpression* expression[], int maxArguments)
	{
		HLSLExpression* argument = functionCall->argument;
		int numArguments = 0;
		while (argument != nullptr)
		{
			if (numArguments < maxArguments)
			{
				expression[numArguments] = argument;
			}
			argument = argument->nextExpression;
			++numArguments;
		}
		return numArguments;
	}

	GLSLGenerator::GLSLGenerator()
	{
		m_tree = nullptr;
		m_entryName = nullptr;
		m_target = Target_VertexShader;
		m_inAttribPrefix = nullptr;
		m_outAttribPrefix = nullptr;
		m_error = false;
		m_matrixRowFunction[0] = 0;
		m_clipFunction[0] = 0;
		m_tex2DlodFunction[0] = 0;
		m_tex2DbiasFunction[0] = 0;
		m_tex3DlodFunction[0] = 0;
		m_texCUBEbiasFunction[0] = 0;
		m_scalarSwizzle2Function[0] = 0;
		m_scalarSwizzle3Function[0] = 0;
		m_scalarSwizzle4Function[0] = 0;
		m_sinCosFunction[0] = 0;
		m_outputPosition = false;
	}

	bool GLSLGenerator::Generate(const HLSLTree* tree, Target target, const char* entryName)
	{
		m_tree = tree;
		m_entryName = entryName;
		m_target = target;

		bool usesClip = m_tree->GetContainsString("clip");
		bool usesTex2Dlod = m_tree->GetContainsString("tex2Dlod");
		bool usesTex2Dbias = m_tree->GetContainsString("tex2Dbias");
		bool usesTex3Dlod = m_tree->GetContainsString("tex3Dlod");
		bool usestexCUBEbias = m_tree->GetContainsString("texCUBEbias");
		bool usesSinCos = m_tree->GetContainsString("sincos");

		ChooseUniqueName("matrix_row", m_matrixRowFunction, sizeof(m_matrixRowFunction));
		ChooseUniqueName("clip", m_clipFunction, sizeof(m_clipFunction));
		ChooseUniqueName("tex2Dlod", m_tex2DlodFunction, sizeof(m_tex2DlodFunction));
		ChooseUniqueName("tex2Dbias", m_tex2DbiasFunction, sizeof(m_tex2DbiasFunction));
		ChooseUniqueName("tex3Dlod", m_tex3DlodFunction, sizeof(m_tex3DlodFunction));
		ChooseUniqueName("texCUBEbias", m_texCUBEbiasFunction, sizeof(m_texCUBEbiasFunction));

		for (int i = 0; i < s_numReservedWords; ++i)
		{
			ChooseUniqueName(s_reservedWord[i], m_reservedWord[i], sizeof(m_reservedWord[i]));
		}

		ChooseUniqueName("m_scalar_swizzle2", m_scalarSwizzle2Function, sizeof(m_scalarSwizzle2Function));
		ChooseUniqueName("m_scalar_swizzle3", m_scalarSwizzle3Function, sizeof(m_scalarSwizzle3Function));
		ChooseUniqueName("m_scalar_swizzle4", m_scalarSwizzle4Function, sizeof(m_scalarSwizzle4Function));

		ChooseUniqueName("sincos", m_sinCosFunction, sizeof(m_sinCosFunction));

		if (target == Target_VertexShader)
		{
			m_inAttribPrefix = "";
			m_outAttribPrefix = "frag_";
		}
		else
		{
			m_inAttribPrefix = "frag_";
			m_outAttribPrefix = "rast_";
		}

		HLSLRoot* root = m_tree->GetRoot();
		HLSLStatement* statement = root->statement;

		// Find the entry point function.
		HLSLFunction* entryFunction = FindFunction(root, m_entryName);
		if (entryFunction == nullptr)
		{
			Error("Entry point '%s' doesn't exist", m_entryName);
			return false;
		}

		//m_writer.WriteLine(0, "#version 140");
		//m_writer.WriteLine(0, "#version 150");
		m_writer.WriteLine(0, "#version 330 core");
		m_writer.WriteLine(0, "precision highp float;");
		m_writer.WriteLine(0, "precision highp int;\n");
		

		//Comentado por pruebas.
		//// Pragmas for NVIDIA.
		//m_writer.WriteLine(0, "#pragma optionNV(fastmath on)");
		////m_writer.WriteLine(0, "#pragma optionNV(fastprecision on)");
		//m_writer.WriteLine(0, "#pragma optionNV(ifcvt none)");
		//m_writer.WriteLine(0, "#pragma optionNV(inline all)");
		//m_writer.WriteLine(0, "#pragma optionNV(strict on)");
		//m_writer.WriteLine(0, "#pragma optionNV(unroll all)");

		//// Output the special function used to access rows in a matrix.
		//m_writer.WriteLine(0, "vec3 %s(mat3 m, int i) { return vec3( m[0][i], m[1][i], m[2][i] ); }", m_matrixRowFunction);
		//m_writer.WriteLine(0, "vec4 %s(mat4 m, int i) { return vec4( m[0][i], m[1][i], m[2][i], m[3][i] ); }", m_matrixRowFunction);

		// Output the special function used to emulate HLSL clip.
		if (usesClip)
		{
			const char* discard = m_target == Target_FragmentShader ? "discard" : "";
			m_writer.WriteLine(0, "void %s(float x) { if (x < 0.0) %s;  }", m_clipFunction, discard);
			m_writer.WriteLine(0, "void %s(vec2  x) { if (any(lessThan(x, vec2(0.0, 0.0)))) %s;  }", m_clipFunction, discard);
			m_writer.WriteLine(0, "void %s(vec3  x) { if (any(lessThan(x, vec3(0.0, 0.0, 0.0)))) %s;  }", m_clipFunction, discard);
			m_writer.WriteLine(0, "void %s(vec4  x) { if (any(lessThan(x, vec4(0.0, 0.0, 0.0, 0.0)))) %s;  }", m_clipFunction, discard);
		}

		// Output the special function used to emulate tex2Dlod.
		if (usesTex2Dlod)
		{
			m_writer.WriteLine(0, "vec4 %s(sampler2D sampler, vec4 texCoord) { return textureLod(sampler, texCoord.xy, texCoord.w);  }", m_tex2DlodFunction);
		}

		// Output the special function used to emulate tex2Dbias.
		if (usesTex2Dbias)
		{
			if (target == Target_FragmentShader)
			{
				m_writer.WriteLine(0, "vec4 %s(sampler2D sampler, vec4 texCoord) { return texture(sampler, texCoord.xy, texCoord.w);  }", m_tex2DbiasFunction);
			}
			else
			{
				// Bias value is not supported in vertex shader.
				m_writer.WriteLine(0, "vec4 %s(sampler2D sampler, vec4 texCoord) { return texture(sampler, texCoord.xy);  }", m_tex2DbiasFunction);
			}

		}

		// Output the special function used to emulate tex3Dlod.
		if (usesTex3Dlod)
		{
			m_writer.WriteLine(0, "vec4 %s(sampler3D sampler, vec4 texCoord) { return textureLod(sampler, texCoord.xyz, texCoord.w);  }", m_tex3DlodFunction);
		}

		// Output the special function used to emulate texCUBEbias.
		if (usestexCUBEbias)
		{
			if (target == Target_FragmentShader)
			{
				m_writer.WriteLine(0, "vec4 %s(samplerCube sampler, vec4 texCoord) { return texture(sampler, texCoord.xyz, texCoord.w);  }", m_texCUBEbiasFunction);
			}
			else
			{
				// Bias value is not supported in vertex shader.
				m_writer.WriteLine(0, "vec4 %s(samplerCube sampler, vec4 texCoord) { return texture(sampler, texCoord.xyz);  }", m_texCUBEbiasFunction);
			}
		}
		//Comentado por pruebas.
		/*m_writer.WriteLine(0, "vec2  %s(float x) { return  vec2(x, x); }", m_scalarSwizzle2Function);
		m_writer.WriteLine(0, "ivec2 %s(int   x) { return ivec2(x, x); }", m_scalarSwizzle2Function);
		m_writer.WriteLine(0, "uvec2 %s(uint  x) { return uvec2(x, x); }", m_scalarSwizzle2Function);

		m_writer.WriteLine(0, "vec3  %s(float x) { return  vec3(x, x, x); }", m_scalarSwizzle3Function);
		m_writer.WriteLine(0, "ivec3 %s(int   x) { return ivec3(x, x, x); }", m_scalarSwizzle3Function);
		m_writer.WriteLine(0, "uvec3 %s(uint  x) { return uvec3(x, x, x); }", m_scalarSwizzle3Function);

		m_writer.WriteLine(0, "vec4  %s(float x) { return  vec4(x, x, x, x); }", m_scalarSwizzle4Function);
		m_writer.WriteLine(0, "ivec4 %s(int   x) { return ivec4(x, x, x, x); }", m_scalarSwizzle4Function);
		m_writer.WriteLine(0, "uvec4 %s(uint  x) { return uvec4(x, x, x, x); }", m_scalarSwizzle4Function);*/

		if (usesSinCos)
		{
			const char* floatTypes[] = { "float", "vec2", "vec3", "vec4" };
			for (int i = 0; i < 4; ++i)
			{
				m_writer.WriteLine(0, "void %s(%s x, out %s s, out %s c) { s = sin(x); c = cos(x); }", m_sinCosFunction,
					floatTypes[i], floatTypes[i], floatTypes[i]);
			}
		}

		m_visitor.GetVisited().clear();
		m_visitor.setRootNode(m_tree->GetRoot());
		m_visitor.VisitFunction(entryFunction);

		OutputAttributes(entryFunction);
		OutputStatements(0, statement);
		OutputEntryCaller(entryFunction);

		m_tree = nullptr;

		// The GLSL compilers don't check for this, so generate our own error message.
		if (target == Target_VertexShader && !m_outputPosition)
		{
			Error("Vertex shader must output a position");
		}

		return !m_error;

	}

	const char* GLSLGenerator::GetResult() const
	{
		return m_writer.GetResult();
	}

	void GLSLGenerator::OutputExpressionList(HLSLExpression* expression, HLSLArgument* argument)
	{
		int numExpressions = 0;
		while (expression != nullptr)
		{
			if (numExpressions > 0)
			{
				m_writer.Write(", ");
			}

			HLSLType* expectedType = nullptr;
			if (argument != nullptr)
			{
				expectedType = &argument->type;
				argument = argument->nextArgument;
			}

			OutputExpression(expression, expectedType);
			expression = expression->nextExpression;
			++numExpressions;
		}
	}

	void GLSLGenerator::OutputExpression(HLSLExpression* expression, const HLSLType* dstType)
	{

		bool cast = dstType != nullptr && !GetCanImplicitCast(expression->expressionType, *dstType);
		if (expression->nodeType == HLSLNodeType_CastingExpression)
		{
			// No need to include a cast if the expression is already doing it.
			cast = false;
		}

		if (cast)
		{
			OutputDeclaration(*dstType, "");
			m_writer.Write("(");
		}

		if (expression->nodeType == HLSLNodeType_IdentifierExpression)
		{
			HLSLIdentifierExpression* identifierExpression = static_cast<HLSLIdentifierExpression*>(expression);
			const char* name = identifierExpression->name;
			OutputIdentifier(name);
		}
		else if (expression->nodeType == HLSLNodeType_ConstructorExpression)
		{
			HLSLConstructorExpression* constructorExpression = static_cast<HLSLConstructorExpression*>(expression);
			m_writer.Write("%s(", GetTypeName(constructorExpression->type));
			OutputExpressionList(constructorExpression->argument);
			m_writer.Write(")");
		}
		else if (expression->nodeType == HLSLNodeType_CastingExpression)
		{
			HLSLCastingExpression* castingExpression = static_cast<HLSLCastingExpression*>(expression);
			OutputDeclaration(castingExpression->type, "");
			m_writer.Write("(");
			//TO-DO: estar pendientes de cuando se inicializa un struct con 0.
			OutputExpression(castingExpression->expression);
			m_writer.Write(")");
		}
		else if (expression->nodeType == HLSLNodeType_LiteralExpression)
		{
			HLSLLiteralExpression* literalExpression = static_cast<HLSLLiteralExpression*>(expression);
			switch (literalExpression->type)
			{
			case HLSLBaseType_Half:
			case HLSLBaseType_Float:
			{
				// Don't use printf directly so that we don't use the system locale.
				char buffer[64];
				String_FormatFloat(buffer, sizeof(buffer), literalExpression->fValue);
				m_writer.Write("%s", buffer);
			}
				break;
			case HLSLBaseType_Int:
			case HLSLBaseType_Uint:
				m_writer.Write("%d", literalExpression->iValue);
				break;
			case HLSLBaseType_Bool:
				m_writer.Write("%s", literalExpression->bValue ? "true" : "false");
				break;
			default:
				ASSERT(0);
			}
		}
		else if (expression->nodeType == HLSLNodeType_UnaryExpression)
		{
			HLSLUnaryExpression* unaryExpression = static_cast<HLSLUnaryExpression*>(expression);
			const char* op = "?";
			bool pre = true;
			const HLSLType* dstType = nullptr;
			switch (unaryExpression->unaryOp)
			{
			case HLSLUnaryOp_Negative:      op = "-";  break;
			case HLSLUnaryOp_Positive:      op = "+";  break;
			case HLSLUnaryOp_Not:           op = "!";  dstType = &unaryExpression->expressionType; break;
			case HLSLUnaryOp_PreIncrement:  op = "++"; break;
			case HLSLUnaryOp_PreDecrement:  op = "--"; break;
			case HLSLUnaryOp_PostIncrement: op = "++"; pre = false; break;
			case HLSLUnaryOp_PostDecrement: op = "--"; pre = false; break;
			}
			m_writer.Write("(");
			if (pre)
			{
				m_writer.Write("%s", op);
				OutputExpression(unaryExpression->expression, dstType);
			}
			else
			{
				OutputExpression(unaryExpression->expression, dstType);
				m_writer.Write("%s", op);
			}
			m_writer.Write(")");
		}
		else if (expression->nodeType == HLSLNodeType_BinaryExpression)
		{
			HLSLBinaryExpression* binaryExpression = static_cast<HLSLBinaryExpression*>(expression);
			const char* op = "?";
			const HLSLType* dstType1 = nullptr;
			const HLSLType* dstType2 = nullptr;
			switch (binaryExpression->binaryOp)
			{
			case HLSLBinaryOp_Add:          op = " + "; dstType1 = dstType2 = &binaryExpression->expressionType; break;
			case HLSLBinaryOp_Sub:          op = " - "; dstType1 = dstType2 = &binaryExpression->expressionType; break;
			case HLSLBinaryOp_Mul:          op = " * "; break;
			case HLSLBinaryOp_Div:          op = " / "; break;
			case HLSLBinaryOp_Less:         op = " < "; break;
			case HLSLBinaryOp_Greater:      op = " > "; break;
			case HLSLBinaryOp_LessEqual:    op = " <= "; break;
			case HLSLBinaryOp_GreaterEqual: op = " >= "; break;
			case HLSLBinaryOp_Equal:        op = " == "; break;
			case HLSLBinaryOp_NotEqual:     op = " != "; break;
			case HLSLBinaryOp_Assign:       op = " = ";  dstType2 = &binaryExpression->expressionType; break;
			case HLSLBinaryOp_AddAssign:    op = " += "; dstType2 = &binaryExpression->expressionType; break;
			case HLSLBinaryOp_SubAssign:    op = " -= "; dstType2 = &binaryExpression->expressionType; break;
			case HLSLBinaryOp_MulAssign:    op = " *= "; dstType2 = &binaryExpression->expressionType; break;
			case HLSLBinaryOp_DivAssign:    op = " /= "; dstType2 = &binaryExpression->expressionType; break;
			case HLSLBinaryOp_And:          op = " && "; dstType1 = dstType2 = &binaryExpression->expressionType; break;
			case HLSLBinaryOp_Or:           op = " || "; dstType1 = dstType2 = &binaryExpression->expressionType; break;
			default:
				ASSERT(0);
			}
			//m_writer.Write("(");
			OutputExpression(binaryExpression->expression1, dstType1);
			m_writer.Write("%s", op);
			OutputExpression(binaryExpression->expression2, dstType2);
			//m_writer.Write(")");
		}
		else if (expression->nodeType == HLSLNodeType_ConditionalExpression)
		{
			HLSLConditionalExpression* conditionalExpression = static_cast<HLSLConditionalExpression*>(expression);
			m_writer.Write("((");
			OutputExpression(conditionalExpression->condition, &kBoolType);
			m_writer.Write(")?(");
			OutputExpression(conditionalExpression->trueExpression);
			m_writer.Write("):(");
			OutputExpression(conditionalExpression->falseExpression);
			m_writer.Write("))");
		}
		else if (expression->nodeType == HLSLNodeType_MemberAccess)
		{

			HLSLMemberAccess* memberAccess = static_cast<HLSLMemberAccess*>(expression);

			if (memberAccess->object->expressionType.baseType == HLSLBaseType_Half ||
				memberAccess->object->expressionType.baseType == HLSLBaseType_Float ||
				memberAccess->object->expressionType.baseType == HLSLBaseType_Int ||
				memberAccess->object->expressionType.baseType == HLSLBaseType_Uint)
			{
				// Handle swizzling on scalar values.
				int swizzleLength = strlen(memberAccess->field);
				if (swizzleLength == 2)
				{
					m_writer.Write("%s", m_scalarSwizzle2Function);
				}
				else if (swizzleLength == 3)
				{
					m_writer.Write("%s", m_scalarSwizzle3Function);
				}
				else if (swizzleLength == 4)
				{
					m_writer.Write("%s", m_scalarSwizzle4Function);
				}
				//m_writer.Write("(");
				OutputExpression(memberAccess->object);
				//m_writer.Write(")");
			}
			else
			{

				//m_writer.Write("(");
				OutputExpression(memberAccess->object);
				//m_writer.Write(")");

				if (memberAccess->object->expressionType.baseType == HLSLBaseType_Float3x3 ||
					memberAccess->object->expressionType.baseType == HLSLBaseType_Float4x4)
				{
					// Handle HLSL matrix "swizzling".
					// TODO: Properly handle multiple element selection such as _m00_m12
					const char* n = memberAccess->field;
					while (n[0] != 0)
					{
						if (n[0] != '_')
						{
							ASSERT(0);
							break;
						}
						++n;
						char base = '1';
						if (n[0] == 'm')
						{
							base = '0';
							++n;
						}
						if (isdigit(n[0]) && isdigit(n[1]))
						{
							m_writer.Write("[%d][%d]", n[1] - base, n[0] - base);
							n += 2;
						}
						else
						{
							ASSERT(0);
							break;
						}
					}
				}
				else
				{
					m_writer.Write(".%s", memberAccess->field);
				}

			}

		}
		else if (expression->nodeType == HLSLNodeType_ArrayAccess)
		{
			HLSLArrayAccess* arrayAccess = static_cast<HLSLArrayAccess*>(expression);

			if (!arrayAccess->array->expressionType.array &&
				(arrayAccess->array->expressionType.baseType == HLSLBaseType_Float3x3 ||
				arrayAccess->array->expressionType.baseType == HLSLBaseType_Float4x4))
			{
				// GLSL access a matrix as m[c][r] while HLSL is m[r][c], so use our
				// special row access function to convert.
				m_writer.Write("%s(", m_matrixRowFunction);
				OutputExpression(arrayAccess->array);
				m_writer.Write(",");
				OutputExpression(arrayAccess->index);
				m_writer.Write(")");
			}
			else
			{
				OutputExpression(arrayAccess->array);
				m_writer.Write("[");
				OutputExpression(arrayAccess->index);
				m_writer.Write("]");
			}

		}
		else if (expression->nodeType == HLSLNodeType_FunctionCall)
		{
			HLSLFunctionCall* functionCall = static_cast<HLSLFunctionCall*>(expression);

			// Handle intrinsic funtions that are different between HLSL and GLSL.
			bool handled = false;
			const char* functionName = functionCall->function->name;

			if (String_Equal(functionName, "mul"))
			{
				HLSLExpression* argument[2];
				if (GetFunctionArguments(functionCall, argument, 2) != 2)
				{
					Error("mul expects 2 arguments");
					return;
				}
				/*
				//m_writer.Write("((");
				m_writer.Write("(");
				OutputExpression(argument[0], &functionCall->function->argument->type);
				m_writer.Write(") * (");
				OutputExpression(argument[1], &functionCall->function->argument->nextArgument->type);
				//m_writer.Write("))");
				m_writer.Write(")");
				*/

				//m_writer.Write("((");
				m_writer.Write("(");
				OutputExpression(argument[1], &functionCall->function->argument->nextArgument->type);
				m_writer.Write(") * (");
				OutputExpression(argument[0], &functionCall->function->argument->type);
				//m_writer.Write("))");
				m_writer.Write(")");

				handled = true;
			}
			else if (String_Equal(functionName, "saturate"))
			{
				HLSLExpression* argument[1];
				if (GetFunctionArguments(functionCall, argument, 1) != 1)
				{
					Error("saturate expects 1 argument");
					return;
				}
				m_writer.Write("clamp(");
				OutputExpression(argument[0]);
				m_writer.Write(", 0.0, 1.0)");
				handled = true;
			}

			if (!handled)
			{
				OutputIdentifier(functionName);
				m_writer.Write("(");
				OutputExpressionList(functionCall->argument, functionCall->function->argument);
				m_writer.Write(")");
			}
		}
		else
		{
			m_writer.Write("<unknown expression>");
		}

		if (cast)
		{
			/*
			const BaseTypeDescription& srcTypeDesc = _baseTypeDescriptions[expression->expressionType.baseType];
			const BaseTypeDescription& dstTypeDesc = _baseTypeDescriptions[dstType->baseType];

			if (dstTypeDesc.numDimensions == 1 && dstTypeDesc.numComponents > 1)
			{
			// Casting to a vector - pad with 0s
			for (int i = srcTypeDesc.numComponents; i < dstTypeDesc.numComponents; ++i)
			{
			m_writer.Write(", 0");
			}
			}
			*/

			m_writer.Write(")");
		}

	}

	void GLSLGenerator::OutputIdentifier(const char* name)
	{

		// Remap intrinstic functions.
		if (String_Equal(name, "tex2D"))
		{
			name = "texture";
		}
		else if (String_Equal(name, "tex2Dproj"))
		{
			name = "texture2DProj";
		}
		else if (String_Equal(name, "texCUBE"))
		{
			name = "texture";
		}
		else if (String_Equal(name, "clip"))
		{
			name = m_clipFunction;
		}
		else if (String_Equal(name, "tex2Dlod"))
		{
			name = m_tex2DlodFunction;
		}
		else if (String_Equal(name, "texCUBEbias"))
		{
			name = m_texCUBEbiasFunction;
		}
		else if (String_Equal(name, "atan2"))
		{
			name = "atan";
		}
		else if (String_Equal(name, "sincos"))
		{
			name = m_sinCosFunction;
		}
		else if (String_Equal(name, "fmod"))
		{
			// mod is not the same as fmod if the parameter is negative!
			// The equivalent of fmod(x, y) is x - y * floor(x/y)
			// We use the mod version for performance.
			name = "mod";
		}
		else if (String_Equal(name, "lerp"))
		{
			name = "mix";
		}
		else if (String_Equal(name, "frac"))
		{
			name = "fract";
		}
		else
		{
			// The identifier could be a GLSL reserved word (if it's not also a HLSL reserved word).
			name = GetSafeIdentifierName(name);
		}
		m_writer.Write("%s", name);

	}

	void GLSLGenerator::OutputArguments(HLSLArgument* argument)
	{
		int numArgs = 0;
		while (argument != nullptr)
		{
			if (numArgs > 0)
			{
				m_writer.Write(", ");
			}

			switch (argument->modifier)
			{
			case HLSLArgumentModifier_In:
				m_writer.Write("in ");
				break;
			case HLSLArgumentModifier_Inout:
				m_writer.Write("inout ");
				break;
			}

			OutputDeclaration(argument->type, argument->name);
			argument = argument->nextArgument;
			++numArgs;
		}
	}

	void GLSLGenerator::OutputStatements(int indent, HLSLStatement* statement, const HLSLType* returnType)
	{

		while (statement != nullptr)
		{
			bool visited = m_visitor.NodeHasVisited(statement);

			if (!visited){
				statement = statement->nextStatement;
				continue;
			}
			if (statement->nodeType == HLSLNodeType_Declaration)
			{
				HLSLDeclaration* declaration = static_cast<HLSLDeclaration*>(statement);

				// GLSL doesn't seem have texture uniforms, so just ignore them.
				if (declaration->type.baseType != HLSLBaseType_Texture)
				{
					m_writer.BeginLine(indent, declaration->fileName, declaration->line);
					if (indent == 0)
					{
						// At the top level, we need the "uniform" keyword.
						m_writer.Write("uniform ");
					}
					OutputDeclaration(declaration);
					m_writer.EndLine(";");
				}
			}
			else if (statement->nodeType == HLSLNodeType_Struct)
			{
				HLSLStruct* structure = static_cast<HLSLStruct*>(statement);
				m_writer.WriteLine(indent, "struct %s {", structure->name);
				HLSLStructField* field = structure->field;
				while (field != nullptr)
				{
					m_writer.BeginLine(indent + 1, field->fileName, field->line);
					OutputDeclaration(field->type, field->name);
					m_writer.Write(";");
					m_writer.EndLine();
					field = field->nextField;
				}
				m_writer.WriteLine(indent, "};");
			}
			else if (statement->nodeType == HLSLNodeType_Buffer)
			{
				HLSLBuffer* buffer = static_cast<HLSLBuffer*>(statement);
				// Empty uniform blocks cause compilation errors on NVIDIA, so don't emit them.
				if (buffer->field != nullptr)
				{
					HLSLDeclaration* field = buffer->field;
					while (field != nullptr)
					{
						m_writer.BeginLine(indent, field->fileName, field->line);
						m_writer.Write("uniform ");
						OutputDeclaration(field->type, field->name);
						m_writer.Write(";");
						m_writer.EndLine();
						field = (HLSLDeclaration*)field->nextStatement;
					}

					//m_writer.WriteLine(indent, buffer->fileName, buffer->line, "layout (std140) uniform %s {", buffer->name);
					//HLSLDeclaration* field = buffer->field;
					//while (field != nullptr)
					//{
					//	m_writer.BeginLine(indent + 1, field->fileName, field->line);
					//	OutputDeclaration(field->type, field->name);
					//	m_writer.Write(";");
					//	m_writer.EndLine();
					//	field = (HLSLDeclaration*)field->nextStatement;
					//}
					//m_writer.WriteLine(indent, "};");
				}
			}
			else if (statement->nodeType == HLSLNodeType_Function)
			{
				HLSLFunction* function = static_cast<HLSLFunction*>(statement);

				// Check if this is our entry point.
				bool entryPoint = String_Equal(function->name, m_entryName);

				// Use an alternate name for the function which is supposed to be entry point
				// so that we can supply our own function which will be the actual entry point.
				const char* functionName = GetSafeIdentifierName(function->name);
				const char* returnTypeName = GetTypeName(function->returnType);

				m_writer.BeginLine(indent, function->fileName, function->line);
				m_writer.Write("%s %s(", returnTypeName, functionName);

				OutputArguments(function->argument);

				m_writer.Write(") {");
				m_writer.EndLine();

				OutputStatements(indent + 1, function->statement, &function->returnType);
				m_writer.WriteLine(indent, "}");

			}
			else if (statement->nodeType == HLSLNodeType_ExpressionStatement)
			{
				HLSLExpressionStatement* expressionStatement = static_cast<HLSLExpressionStatement*>(statement);
				m_writer.BeginLine(indent, statement->fileName, statement->line);
				OutputExpression(expressionStatement->expression);
				m_writer.EndLine(";");
			}
			else if (statement->nodeType == HLSLNodeType_ReturnStatement)
			{
				HLSLReturnStatement* returnStatement = static_cast<HLSLReturnStatement*>(statement);
				if (returnStatement->expression != nullptr)
				{
					m_writer.BeginLine(indent, returnStatement->fileName, returnStatement->line);
					m_writer.Write("return ");
					OutputExpression(returnStatement->expression, returnType);
					m_writer.EndLine(";");
				}
				else
				{
					m_writer.WriteLine(indent, returnStatement->fileName, returnStatement->line, "return;");
				}
			}
			else if (statement->nodeType == HLSLNodeType_DiscardStatement)
			{
				HLSLDiscardStatement* discardStatement = static_cast<HLSLDiscardStatement*>(statement);
				if (m_target == Target_FragmentShader)
				{
					m_writer.WriteLine(indent, discardStatement->fileName, discardStatement->line, "discard;");
				}
			}
			else if (statement->nodeType == HLSLNodeType_BreakStatement)
			{
				HLSLBreakStatement* breakStatement = static_cast<HLSLBreakStatement*>(statement);
				m_writer.WriteLine(indent, breakStatement->fileName, breakStatement->line, "break;");
			}
			else if (statement->nodeType == HLSLNodeType_ContinueStatement)
			{
				HLSLContinueStatement* continueStatement = static_cast<HLSLContinueStatement*>(statement);
				m_writer.WriteLine(indent, continueStatement->fileName, continueStatement->line, "continue;");
			}
			else if (statement->nodeType == HLSLNodeType_IfStatement)
			{
				HLSLIfStatement* ifStatement = static_cast<HLSLIfStatement*>(statement);
				m_writer.BeginLine(indent, ifStatement->fileName, ifStatement->line);
				m_writer.Write("if (");
				OutputExpression(ifStatement->condition, &kBoolType);
				m_writer.Write(") {");
				m_writer.EndLine();
				OutputStatements(indent + 1, ifStatement->statement, returnType);
				m_writer.WriteLine(indent, "}");
				if (ifStatement->elseStatement != nullptr)
				{
					m_writer.WriteLine(indent, "else {");
					OutputStatements(indent + 1, ifStatement->elseStatement, returnType);
					m_writer.WriteLine(indent, "}");
				}
			}
			else if (statement->nodeType == HLSLNodeType_ForStatement)
			{
				HLSLForStatement* forStatement = static_cast<HLSLForStatement*>(statement);
				m_writer.BeginLine(indent, forStatement->fileName, forStatement->line);
				m_writer.Write("for (");
				OutputDeclaration(forStatement->initialization);
				m_writer.Write("; ");
				OutputExpression(forStatement->condition, &kBoolType);
				m_writer.Write("; ");
				OutputExpression(forStatement->increment);
				m_writer.Write(") {");
				m_writer.EndLine();
				OutputStatements(indent + 1, forStatement->statement, returnType);
				m_writer.WriteLine(indent, "}");
			}
			else if (statement->nodeType == HLSLNodeType_Technique11 || statement->nodeType == HLSLNodeType_Pass11)
			{

			}
			else
			{
				// Unhanded statement type.
				ASSERT(0);
			}

			statement = statement->nextStatement;

		}

	}

	HLSLFunction* GLSLGenerator::FindFunction(HLSLRoot* root, const char* name)
	{
		HLSLStatement* statement = root->statement;
		while (statement != nullptr)
		{
			if (statement->nodeType == HLSLNodeType_Function)
			{
				HLSLFunction* function = static_cast<HLSLFunction*>(statement);
				if (String_Equal(function->name, name))
				{
					return function;
				}
			}
			statement = statement->nextStatement;
		}
		return nullptr;
	}

	HLSLStruct* GLSLGenerator::FindStruct(HLSLRoot* root, const char* name)
	{
		HLSLStatement* statement = root->statement;
		while (statement != nullptr)
		{
			if (statement->nodeType == HLSLNodeType_Struct)
			{
				HLSLStruct* structDeclaration = static_cast<HLSLStruct*>(statement);
				if (String_Equal(structDeclaration->name, name))
				{
					return structDeclaration;
				}
			}
			statement = statement->nextStatement;
		}
		return nullptr;
	}

	void GLSLGenerator::OutputAttribute(const HLSLType& type, const char* semantic, const char* attribType, const char* prefix)
	{
		HLSLRoot* root = m_tree->GetRoot();
		if (type.baseType == HLSLBaseType_UserDefined)
		{
			// If the argument is a struct with semantics specified, we need to
			// grab them.
			HLSLStruct* structDeclaration = FindStruct(root, type.typeName);
			ASSERT(structDeclaration != nullptr);
			HLSLStructField* field = structDeclaration->field;
			int contador = 0;
			while (field != nullptr)
			{
				if (field->semantic != nullptr && GetBuiltInSemantic(field->semantic) == nullptr)
				{
					const char* typeName = GetTypeName(field->type);
					//if (m_target == GLSLGenerator::Target_VertexShader && strcmp(attribType, "in")==0)
					//{
						//m_writer.WriteLine(0, "%s%d) %s %s %s%s;","layout(location = ", contador, attribType, typeName, prefix, field->semantic);
					//	contador++;
					//}
					//else
					{
						m_writer.WriteLine(0, "%s %s %s%s;", attribType, typeName, prefix, field->semantic);
					}
				}
				field = field->nextField;
			}
		}
		else if (semantic != nullptr && GetBuiltInSemantic(semantic) == nullptr)
		{
			const char* typeName = GetTypeName(type);
			m_writer.WriteLine(0, "%s %s %s%s;", attribType, typeName, prefix, semantic);
		}
	}

	void GLSLGenerator::OutputAttributes(HLSLFunction* entryFunction)
	{
		// Write out the input attributes to the shader.
		HLSLArgument* argument = entryFunction->argument;
		while (argument != nullptr)
		{
			OutputAttribute(argument->type, argument->semantic, "in", m_inAttribPrefix);
			//OutputAttribute(argument->type, argument->semantic, "attribute", m_inAttribPrefix);
			
			argument = argument->nextArgument;
		}

		// Write out the output attributes from the shader.
		OutputAttribute(entryFunction->returnType, entryFunction->semantic, "out", m_outAttribPrefix);
	}

	void GLSLGenerator::OutputSetOutAttribute(const char* semantic, const char* resultName)
	{
		const char* builtInSemantic = GetBuiltInSemantic(semantic);
		if (builtInSemantic != nullptr)
		{
			if (String_Equal(builtInSemantic, "gl_Position"))
			{
				// Mirror the y-coordinate when we're outputing from
				// the vertex shader so that we match the D3D texture
				// coordinate origin convention in render-to-texture
				// operations.
				// We also need to convert the normalized device
				// coordinates from the D3D convention of 0 to 1 to the
				// OpenGL convention of -1 to 1.
				m_writer.WriteLine(1, "vec4 temp = %s;", resultName);
				//m_writer.WriteLine(1, "%s = temp * vec4(1,-1,2,1) - vec4(0,0,temp.w,0);", builtInSemantic);
				m_writer.WriteLine(1, "%s = temp * vec4(1,1,2,1) - vec4(0,0,temp.w,0);", builtInSemantic);
				m_outputPosition = true;
			}
			else if (String_Equal(builtInSemantic, "gl_FragDepth"))
			{
				// If the value goes outside of the 0 to 1 range, the
				// fragment will be rejected unlike in D3D, so clamp it.
				m_writer.WriteLine(1, "%s = clamp(float(%s), 0.0, 1.0);", builtInSemantic, resultName);
			}
			else
			{
				m_writer.WriteLine(1, "%s = %s;", builtInSemantic, resultName);
			}
		}
		else
		{
			m_writer.WriteLine(1, "%s%s = %s;", m_outAttribPrefix, semantic, resultName);
		}
	}

	void GLSLGenerator::OutputEntryCaller(HLSLFunction* entryFunction)
	{
		HLSLRoot* root = m_tree->GetRoot();

		m_writer.WriteLine(0, "void main() {");

		// Create local variables for each of the parameters we'll need to pass
		// into the entry point function.
		HLSLArgument* argument = entryFunction->argument;
		while (argument != nullptr)
		{
			m_writer.BeginLine(1);
			OutputDeclaration(argument->type, argument->name);
			m_writer.EndLine(";");

			// Set the value for the local variable.
			if (argument->type.baseType == HLSLBaseType_UserDefined)
			{
				HLSLStruct* structDeclaration = FindStruct(root, argument->type.typeName);
				ASSERT(structDeclaration != nullptr);
				HLSLStructField* field = structDeclaration->field;
				while (field != nullptr)
				{
					if (field->semantic != nullptr)
					{
						const char* builtInSemantic = GetBuiltInSemantic(field->semantic);
						if (builtInSemantic)
						{
							if (m_target != GLSLGenerator::Target_FragmentShader)
								m_writer.WriteLine(1, "%s.%s = %s;", GetSafeIdentifierName(argument->name), GetSafeIdentifierName(field->name), builtInSemantic);
						}
						else
						{
							m_writer.WriteLine(1, "%s.%s = %s%s;", GetSafeIdentifierName(argument->name), GetSafeIdentifierName(field->name), m_inAttribPrefix, field->semantic);
						}
					}
					field = field->nextField;
				}
			}
			else if (argument->semantic != nullptr)
			{
				const char* builtInSemantic = GetBuiltInSemantic(argument->semantic);
				if (builtInSemantic)
				{
					m_writer.WriteLine(1, "%s = %s;", GetSafeIdentifierName(argument->name), builtInSemantic);
				}
				else
				{
					m_writer.WriteLine(1, "%s = %s%s;", GetSafeIdentifierName(argument->name), m_inAttribPrefix, argument->semantic);
				}
			}

			argument = argument->nextArgument;
		}

		const char* resultName = "result";

		// Call the original entry function.
		m_writer.BeginLine(1);
		m_writer.Write("%s %s = %s(", GetTypeName(entryFunction->returnType), resultName, m_entryName);

		int numArgs = 0;
		argument = entryFunction->argument;
		while (argument != nullptr)
		{
			if (numArgs > 0)
			{
				m_writer.Write(", ");
			}
			m_writer.Write("%s", GetSafeIdentifierName(argument->name));
			argument = argument->nextArgument;
			++numArgs;
		}
		m_writer.EndLine(");");

		// Copy values from the result into the out attributes as necessary.
		if (entryFunction->returnType.baseType == HLSLBaseType_UserDefined)
		{
			HLSLStruct* structDeclaration = FindStruct(root, entryFunction->returnType.typeName);
			ASSERT(structDeclaration != nullptr);
			HLSLStructField* field = structDeclaration->field;
			while (field != nullptr)
			{
				char fieldResultName[1024];
				String_Printf(fieldResultName, sizeof(fieldResultName), "%s.%s", resultName, field->name);
				OutputSetOutAttribute(field->semantic, fieldResultName);
				field = field->nextField;
			}
		}
		else if (entryFunction->semantic != nullptr)
		{
			OutputSetOutAttribute(entryFunction->semantic, resultName);
		}

		m_writer.WriteLine(0, "}");
	}

	void GLSLGenerator::OutputDeclaration(HLSLDeclaration* declaration)
	{
		OutputDeclaration(declaration->type, GetSafeIdentifierName(declaration->name));
		if (declaration->assignment != nullptr)
		{
			m_writer.Write(" = ");
			if (declaration->type.array)
			{
				m_writer.Write("%s[]( ", GetTypeName(declaration->type));
				OutputExpressionList(declaration->assignment);
				m_writer.Write(" )");
			}
			else
			{
				OutputExpression(declaration->assignment, &declaration->type);
			}
		}
	}

	void GLSLGenerator::OutputDeclaration(const HLSLType& type, const char* name)
	{
		if (!type.array)
		{
			m_writer.Write("%s %s", GetTypeName(type), GetSafeIdentifierName(name));
		}
		else
		{
			m_writer.Write("%s %s[", GetTypeName(type), GetSafeIdentifierName(name));
			if (type.arraySize != nullptr)
			{
				OutputExpression(type.arraySize);
			}
			m_writer.Write("]");
		}
	}

	void GLSLGenerator::Error(const char* format, ...)
	{
		// It's not always convenient to stop executing when an error occurs,
		// so just track once we've hit an error and stop reporting them until
		// we successfully bail out of execution.
		if (m_error)
		{
			return;
		}
		m_error = true;

		char buffer[1024];
		va_list args;
		va_start(args, format);
		int result = vsnprintf(buffer, sizeof(buffer) - 1, format, args);
		va_end(args);

		Log_Error("%s", buffer);
	}

	const char* GLSLGenerator::GetSafeIdentifierName(const char* name) const
	{
		for (int i = 0; i < s_numReservedWords; ++i)
		{
			if (String_Equal(s_reservedWord[i], name))
			{
				return m_reservedWord[i];
			}
		}
		return name;
	}

	bool GLSLGenerator::ChooseUniqueName(const char* base, char* dst, int dstLength) const
	{
		for (int i = 0; i < 1024; ++i)
		{
			String_Printf(dst, dstLength, "%s%d", base, i);
			if (!m_tree->GetContainsString(dst))
			{
				return true;
			}
		}
		return false;
	}

	GLSLGenerator::HLSLMarkVisitor::HLSLMarkVisitor()
	{
	}

	bool GLSLGenerator::HLSLMarkVisitor::NodeHasVisited(HLSLNode* node)
	{
		map<HLSLNode*, bool>::iterator itm = m_nodesVisited.find(node);
		return itm != m_nodesVisited.end();
	}
	void GLSLGenerator::HLSLMarkVisitor::VisitType(HLSLType & type){
		if (type.baseType == HLSLBaseType_UserDefined){
			HLSLStruct* structe = FindGlobalStruct(m_root, type.typeName);
			VisitStruct(structe);
			m_nodesVisited[structe] = true;
		}
	}
	void GLSLGenerator::HLSLMarkVisitor::MarkVisitNode(HLSLNode * node)
	{
		m_nodesVisited[node] = true;
	}

	void GLSLGenerator::HLSLMarkVisitor::VisitIdentifierExpression(HLSLIdentifierExpression * node)
	{
		m_nodesVisited[node] = true;
		if (node->global)
		{
			HLSLBuffer* structe = FindGlobalStructWithParameterName(m_root, node->name);
			VisitBuffer(structe);
		}
	}
}