#pragma once

#include "CommonHeader.h"
#include "CodeWriter.h"
#include "HLSLTree.h"

namespace CuadoFX
{

	class TRIOFXAPI_DLL GLSLGenerator
	{

	public:

		enum Target
		{
			Target_VertexShader,
			Target_FragmentShader,
			Target_GeometryShader,
		};

		explicit GLSLGenerator();

		virtual bool Generate(const HLSLTree* tree, Target target, const char* entryName);
		const char* GetResult() const;

	private:

		virtual void OutputExpressionList(HLSLExpression* expression, HLSLArgument* argument = nullptr);
		virtual void OutputExpression(HLSLExpression* expression, const HLSLType* dstType = nullptr);
		virtual void OutputIdentifier(const char* name);
		virtual void OutputArguments(HLSLArgument* argument);

		/**
		* If the statements are part of a function, then returnType can be used to specify the type
		* that a return statement is expected to produce so that correct casts will be generated.
		*/
		virtual void OutputStatements(int indent, HLSLStatement* statement, const HLSLType* returnType = nullptr);

		virtual void OutputAttribute(const HLSLType& type, const char* semantic, const char* attribType, const char* prefix);
		virtual void OutputAttributes(HLSLFunction* entryFunction);
		virtual void OutputEntryCaller(HLSLFunction* entryFunction);
		virtual void OutputDeclaration(HLSLDeclaration* declaration);
		virtual void OutputDeclaration(const HLSLType& type, const char* name);

		virtual void OutputSetOutAttribute(const char* semantic, const char* resultName);

		HLSLFunction* FindFunction(HLSLRoot* root, const char* name);
		HLSLStruct* FindStruct(HLSLRoot* root, const char* name);

		void Error(const char* format, ...);

		/** GLSL contains some reserved words that don't exist in HLSL. This function will
		* sanitize those names. */
		const char* GetSafeIdentifierName(const char* name) const;

		/** Generates a name of the format "base+n" where n is an integer such that the name
		* isn't used in the syntax tree. */
		bool ChooseUniqueName(const char* base, char* dst, int dstLength) const;

	private:

		static const int    s_numReservedWords = 5;
		static const char*  s_reservedWord[s_numReservedWords];

		CodeWriter          m_writer;

		const HLSLTree*     m_tree;
		const char*         m_entryName;
		Target              m_target;
		bool                m_outputPosition;

		const char*         m_outAttribPrefix;
		const char*         m_inAttribPrefix;

		char                m_matrixRowFunction[64];
		char                m_clipFunction[64];
		char                m_tex2DlodFunction[64];
		char                m_tex2DbiasFunction[64];
		char                m_tex3DlodFunction[64];
		char                m_texCUBEbiasFunction[64];
		char                m_scalarSwizzle2Function[64];
		char                m_scalarSwizzle3Function[64];
		char                m_scalarSwizzle4Function[64];
		char                m_sinCosFunction[64];

		bool                m_error;

		char                m_reservedWord[s_numReservedWords][64];

		class HLSLMarkVisitor : public HLSLTreeVisitor
		{
		public:
			HLSLMarkVisitor();
			void VisitType(HLSLType & type);
			//void VisitStruct(HLSLStruct * node);
			//void VisitStructField(HLSLStructField * node);
			void VisitIdentifierExpression(HLSLIdentifierExpression * node);
			void MarkVisitNode(HLSLNode * node);

			bool NodeHasVisited(HLSLNode* node);

			inline map<HLSLNode*, bool>& GetVisited(){
				return m_nodesVisited;
			}

			inline void setRootNode(HLSLRoot* root){
				m_root = root;
			}
			//private:
			
			map<HLSLNode*, bool> m_nodesVisited;
			HLSLRoot* m_root;

		};

		HLSLMarkVisitor m_visitor;
	};
}
