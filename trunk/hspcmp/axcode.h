#pragma once

#include <memory>
#include <map>
#include <vector>
#include <string>

class CMemBuf;
class CToken;
class AxCodeInspector;

class AxCode
{
	friend AxCodeInspector;
private:
	CToken* tk_;

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

	int cg_lastcs;

	// for Struct
	int	cg_stnum;
	int	cg_stsize;
	int cg_stptr;

public:
	AxCode(CToken* tk);

	int GetCS();
	void PutCS(short);
	void PutCS(int type, int value, int exflg);
	void PutCSSymbol(int label_id, int exflag);
	void PutCS(int type, double value, int exflg);
	void SetCS(int csindex, int type, int value);

	int PutOT(int value);
	void PutOTBuf();
	int PutDS(char *str);
	int PutDSBuf(char *str);
	int PutDSBuf(char *str, int size);
	char *GetDS(int ptr);
	void SetOT(int id, int value);
	int GetOT(int id); int GetOTCount();
	int GetNewOTFromOldOT(int old_otindex);

	void PutDI();
	void PutDI(int dbg_code, int value, int subid);
	void PutDIVars(CLabel& lb);
	void PutDILabels(CLabel& lb);
	void PutDIParams(CLabel& lb);

	void PutHPI(short flag, short option, char *libname, char *funcname);
	int PutLIB(int flag, char *name);
	void SetLIBIID(int id, char *clsid);

	int PutStructParam(short mptype, int extype);
	int PutStructParamTag();
	void PutStructStart();
	int PutStructEnd(char *name, int libindex, int otindex, int funcflag);
	int PutStructEnd(int i, char *name, int libindex, int otindex, int funcflag);
	int PutStructEndDll(char *name, int libindex, int subid, int otindex);

	// finalizers
	void WriteToBuf(CMemBuf& axbuf, HSPHED& hsphed,
		CToken::HeaderInfo& hi, int mode, int cg_valcnt, int cg_varhpi);

	//codegen����G�邽�߂̂���
	//todo:�ړI���ƂɍœK�ȃC���^�[�t�F�[�X�ɕύX
	unsigned short* GetCSBuffer() const;  // codegen������cs�ɏ������ނ���; ��ł��������֐��ɒu��������ׂ�
	STRUCTDAT* GetFIBuffer() const;
	STRUCTPRM* GetMIBuffer() const;
	size_t GetFICount() const;
	void AllocFI();
	void PutFI(STRUCTDAT const& st);
	int* GetOTBuffer() const;


};

class AxCodeInspector
{
	//		For inspection
public:
	AxCodeInspector(AxCode& owner);

#ifdef HSPINSPECT
	std::shared_ptr<CMemBuf> GetBuffer() { return axi_buf; }
private:
	void CodeSegment();
	int  CSElem(int csindex);
	void FInfoSegment();
	void LInfoSegment();
	void HpiSegment();

	void AnalyzeDInfo();
	void BeginSegment(char const* segment_title);

	char const* TypeName(int type);

private:
	AxCode& ax_;

	std::shared_ptr<CMemBuf> axi_buf;

	using identTable_t = std::map<int, char const*>;
	std::unique_ptr<identTable_t> inspect_var_names;
	std::unique_ptr<identTable_t> inspect_lab_names;
	std::unique_ptr<identTable_t> inspect_prm_names;
#endif
};
