#pragma once

#include "TrioApiRequisites.h"

#include "CodeWriter.h"
#include "HLSLTree.h"

namespace BrunoFX
{

	class BRUNO_API_EXPORT GLSLGenerator
	{

	public:

		enum Target
		{
			Target_VertexShader,
			Target_FragmentShader,
			Target_GeometryShader,
		};

		enum Version
		{
			Version_110, // OpenGL 2.0
			Version_140, // OpenGL 3.1
			Version_150, // OpenGL 3.2
			Version_100_ES, // OpenGL ES 2.0
			Version_300_ES, // OpenGL ES 3.0
		};

		enum Flags
		{
			Flag_FlipPositionOutput = 1 << 0,
			Flag_EmulateConstantBuffer = 1 << 1,
			Flag_PackMatrixRowMajor = 1 << 2,
			Flag_LowerMatrixMultiplication = 1 << 3,
		};

		struct Options
		{
			unsigned int flags;
			const char* constantBufferPrefix;

			Options()
			{
				flags = 0;
				constantBufferPrefix = "";
			}
		};

		GLSLGenerator();

		bool Generate(HLSLTree* tree, Target target, Version version, const char* entryName, const Options& options = Options());
		const char* GetResult() const;

	private:
		enum AttributeModifier
		{
			AttributeModifier_In,
			AttributeModifier_Out,
		};

		virtual void OutputExpressionList(HLSLExpression* expression, HLSLArgument* argument = nullptr);
		virtual void OutputExpression(HLSLExpression* expression, const HLSLType* dstType = nullptr);
		virtual void OutputIdentifier(const char* name);
		virtual void OutputArguments(HLSLArgument* argument);

		/**
		* If the statements are part of a function, then returnType can be used to specify the type
		* that a return statement is expected to produce so that correct casts will be generated.
		*/
		virtual void OutputStatements(int indent, HLSLStatement* statement, const HLSLType* returnType = nullptr);

		virtual void OutputAttribute(const HLSLType& type, const char* semantic, AttributeModifier modifier);
		virtual void OutputAttributes(HLSLFunction* entryFunction);
		virtual void OutputEntryCaller(HLSLFunction* entryFunction);
		virtual void OutputDeclaration(HLSLDeclaration* declaration);
		virtual void OutputDeclarationType(const HLSLType& type);
		virtual void OutputDeclarationBody(const HLSLType& type, const char* name);
		virtual void OutputDeclaration(const HLSLType& type, const char* name);
		virtual void OutputCast(const HLSLType& type);

		virtual void OutputSetOutAttribute(const char* semantic, const char* resultName);

		void LayoutBuffer(HLSLBuffer* buffer, unsigned int& offset);
		void LayoutBuffer(const HLSLType& type, unsigned int& offset);
		void LayoutBufferElement(const HLSLType& type, unsigned int& offset);
		void LayoutBufferAlign(const HLSLType& type, unsigned int& offset);

		HLSLBuffer* GetBufferAccessExpression(HLSLExpression* expression);
		void OutputBufferAccessExpression(HLSLBuffer* buffer, HLSLExpression* expression, const HLSLType& type, unsigned int postOffset);
		unsigned int OutputBufferAccessIndex(HLSLExpression* expression, unsigned int postOffset);

		void OutputBuffer(int indent, HLSLBuffer* buffer);

		HLSLFunction* FindFunction(HLSLRoot* root, const char* name);
		HLSLStruct* FindStruct(HLSLRoot* root, const char* name);

		void Error(const char* format, ...);

		/** GLSL contains some reserved words that don't exist in HLSL. This function will
		* sanitize those names. */
		const char* GetSafeIdentifierName(const char* name) const;

		/** Generates a name of the format "base+n" where n is an integer such that the name
		* isn't used in the syntax tree. */
		bool ChooseUniqueName(const char* base, char* dst, int dstLength) const;

		const char* GetBuiltInSemantic(const char* semantic, AttributeModifier modifier, int* outputIndex = 0);
		const char* GetAttribQualifier(AttributeModifier modifier);
	private:

		static const int    s_numReservedWords = 7;
		static const char*  s_reservedWord[s_numReservedWords];

		CodeWriter          m_writer;

		HLSLTree*			m_tree;
		const char*         m_entryName;
		Target              m_target;
		Version             m_version;
		bool                m_versionLegacy;
		Options             m_options;

		bool                m_outputPosition;
		int                 m_outputTargets;

		const char*         m_outAttribPrefix;
		const char*         m_inAttribPrefix;

		char                m_matrixRowFunction[64];
		char                m_matrixCtorFunction[64];
		char                m_matrixMulFunction[64];
		char                m_clipFunction[64];
		char                m_tex2DlodFunction[64];
		char                m_tex2DbiasFunction[64];
		char                m_tex2DgradFunction[64];
		char                m_tex3DlodFunction[64];
		char                m_texCUBEbiasFunction[64];
		char                m_texCUBElodFunction[64];
		char                m_scalarSwizzle2Function[64];
		char                m_scalarSwizzle3Function[64];
		char                m_scalarSwizzle4Function[64];
		char                m_sinCosFunction[64];
		char                m_bvecTernary[64];

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

			inline std::map<HLSLNode*, bool>& GetVisited(){
				return m_nodesVisited;
			}

			inline void setRootNode(HLSLRoot* root){
				m_root = root;
			}
			//private:
			
			std::map<HLSLNode*, bool> m_nodesVisited;
			HLSLRoot* m_root;

		};

		HLSLMarkVisitor m_visitor;
	};
}
