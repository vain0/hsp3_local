#pragma once

// hsp3ll �� CHsp3Parser �N���X�Əd��

#include <memory>
#include <map>
#include <vector>
#include <string>

#ifdef HSPINSPECT
# include <sstream>
#endif

struct HSPHED;
class CLabel;
class CMemBuf;
class CToken;
class AxCodeInspector;

namespace DInfoCode {
	static unsigned char const
		ChangeContext = 0xFF,   // 255; ���̕����Ɉڍs����B2�A���Ȃ疖����\���B
		SourceFile = 0xFE,      // 254; �\�[�X�t�@�C���w��
		VarName = 0xFD,         // 253; �ϐ����̋L�^
		DebugIdent = 0xFB,      // 251; �f�o�b�O�����ʎq(���x���A��������)�̋L�^
		WideOffset = 0xFC,      // 252; ���̖��߂܂ł�CS�I�t�Z�b�g�l (16bit)
		CodeMin = 0xFB;         // 251: (���ʂȈӖ��ň����R�[�h�̍ŏ��l)
};

#define STRUCTDAT_INDEX_DUMMY ((short)0x8000)

class AxCode
{
	friend AxCodeInspector;
private:
	CToken* tk_;

	std::unique_ptr<HSPHED> hed_buf;
	std::unique_ptr<CMemBuf> cs_buf;
	std::unique_ptr<CMemBuf> ds_buf;
	std::unique_ptr<CMemBuf> ot_buf;
	std::unique_ptr<CMemBuf> di_buf;
	std::unique_ptr<CMemBuf> li_buf;
	std::unique_ptr<CMemBuf> fi_buf;
	std::unique_ptr<CMemBuf> mi_buf;
	std::unique_ptr<CMemBuf> fi2_buf;
	std::unique_ptr<CMemBuf> hpi_buf;

	std::unique_ptr<std::map<std::string, int>> string_literal_table;
	std::unique_ptr<std::map<double, int>> double_literal_table;
	std::unique_ptr<std::vector<int>> working_ot_buf; // �R�[�h�̉�͒��Ƀ��x��(cs�ʒu)�̏����L�����Ă����B���Ƃł܂Ƃ߂� ot_buf �ɏ����o�����
	std::unique_ptr<std::multimap<int, int>> label_reference_table; // �L�[�ł��郉�x��(otindex)���Q�Ƃ��Ă���cs�ʒu�̕\�Botindex�̏d�������Ɏg���B
	std::unique_ptr<std::map<int, int>> otindex_table; //csindex -> new otindex (or -1)

	int cg_varhpi;  // hpi ���錾�����ϐ��^�̐�

	// for Struct
	int	cg_stnum;
	int	cg_stsize;
	int cg_stptr;

public:
	AxCode(CToken* tk);

	HSPHED const& GetHeader() const;
	STRUCTDAT* GetFIBuffer() const;
	size_t GetFICount() const;
	STRUCTPRM* GetMIBuffer() const;


	//		for Code Generator
	int GetCS();
	void PutCS(int type, double value, int exflg);
	void PutCS(int type, int value, int exflg);
	void PutCSSymbol(int label_id, int exflag);
	int PutCSJumpOffsetPlaceholder();
	void SetCS(int csindex, int type, int value);
	void SetCSJumpOffset(int csindex, short offset);
	void SetCSAddExflag(int csindex, int exflag);

	int PutOT(int value);
	void PutOTBuf();
	int PutDS(char *str);
	int PutDSBuf(char *str);
	int PutDSBuf(char *str, int size);
	char *GetDS(int ptr);
	void SetOT(int id, int value);
	int GetOT(int id) const;
	int GetOTCount() const;
	int GetNewOTIndex(int old_otindex) const;
	int GetOldOTIndex(int ot_index) const;

	void PutDI(int dbg_code, int value, int subid);
	void PutDIOffset(int offset);
	void PutDIFinal(CLabel& lb, bool is_debug_compile, bool includes_varnames);
private:
	void PutDIVars(CLabel* lb);
	void PutDILabels(CLabel* lb);
	void PutDIParams(CLabel* lb);
public:
	void PutHPI(short flag, short option, char *libname, char *funcname, int var_type_cnt);
	int PutLIB(int flag, char *name, char* clsname);

	int PutStructParam(short mptype, int extype);
	int PutStructParamTag();
	void PutStructStart();
	int PutStructEnd(char *name, int libindex, int otindex, int funcflag);
	int PutStructEnd(int i, char *name, int libindex, int otindex, int funcflag);
	int PutStructEndDll(char *name, int libindex, int subid, int otindex);
	int PutStructDummy(int label_id);

	void WriteToBuf(CMemBuf& axbuf,
		CToken::HeaderInfo& hi, int mode, int cg_valcnt);

private:
	unsigned short* GetCSBuffer() const;  // codegen������cs�ɏ������ނ���
	int* GetOTBuffer() const;
};

class AxCodeInspector
{
public:
	AxCodeInspector(AxCode& owner, std::shared_ptr<CMemBuf> buf);

#ifdef HSPINSPECT
private:
	void PutCodeSegment();
	void PutFInfoSegment();
	void PutLInfoSegment();
	void PutHpiSegment();

	void PutSegmentTitle(char const* segment_title);

	size_t PutCSElemInspection(int csindex);
	void InspectVar(std::ostringstream& os, int var_index) const;
	void InspectLabel(std::ostringstream& os, int ot_index) const;
	void InspectModcmd(std::ostringstream& os, int fi_index) const;
	void InspectParam(std::ostringstream& os, int mi_index) const;
	void InspectGeneralCommand(std::ostringstream& os, int type, int code) const;

	char const* InspectType(int type) const;

	char const* TryFindIdentName(int type, int opt) const;
	char const* TryFindLabelName(int ot_index) const;
	char const* TryFindParamName(int mi_index) const;
	char const* TryFindVarName(int var_index) const;

private:
	AxCode& ax_;

	std::shared_ptr<CMemBuf> axi_buf;
#endif
};

extern char const* inspectCalcCode(int op);
