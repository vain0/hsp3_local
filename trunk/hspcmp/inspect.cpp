// partial CToken

#ifdef HSPINSPECT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <cassert>

#include <sstream>
#include <algorithm>

#include "../hsp3/hsp3config.h"
#include "../hsp3/hsp3debug.h"
#include "../hsp3/hsp3struct.h"

#include "supio.h"
#include "token.h"
#include "label.h"
#include "tagstack.h"
#include "membuf.h"
#include "strnote.h"
#include "comutil.h"

#include "token.h"

extern char *hsp_prestr[];  // hspcmp.cpp
extern char *hsp_prepp[];

#define assert_sentinel do { assert(false); throw; } while(false)

//treat membuf as _TYPE[]
#define MEMBUF_COUNT(_TYPE, _SELF) ((_SELF).GetSize() / sizeof(_TYPE))
#define MEMBUF_BEGIN(_TYPE, _SELF) (reinterpret_cast<std::add_pointer_t<_TYPE>>((_SELF).GetBuffer()))
#define MEMBUF_END(_TYPE, _SELF)  (&MEMBUF_BEGIN(_TYPE, _SELF)[MEMBUF_COUNT(_TYPE, _SELF)])
#define MEMBUF_RANGE(_TYPE, _SELF) MEMBUF_BEGIN(_TYPE, _SELF), MEMBUF_END(_TYPE, _SELF)

int CToken::SaveAxInspection(char* fname)
{
	return axi_buf ? axi_buf->SaveFile(fname) : -1;
}

void CToken::InspectAxCode()
{
	assert(!axi_buf);
	axi_buf.reset(new CMemBuf());

	Inspect_CodeSegment();
	Inspect_FInfoSegment();
}

void CToken::Inspect_BeginSegment(char const* segment_title)
{
	assert(!!axi_buf);
	axi_buf->PutStrf("[%s]", segment_title);
	axi_buf->PutCR();
}

void CToken::Inspect_CodeSegment()
{
	//todo: �^�u�����ł͂Ȃ������w��ő�������
	Inspect_BeginSegment("CodeSegment");
	axi_buf->PutStr("�ʒu\t�^�C�v\t\t�l\t�P��\t����\t�J���}\t�����\t�l�̈Ӗ�");
	axi_buf->PutCR();

	for ( int i = 0; i < GetCS(); ) {
		i += Inspect_CSElem(i);
	}

	axi_buf->PutCR();
}

int CToken::Inspect_CSElem(int csindex)
{
	auto const cur_cs = &reinterpret_cast<unsigned short*>(cs_buf->GetBuffer())[csindex];

	//analyze single code
	int const c = cur_cs[0];
	int const type = c & CSTYPE;
	bool const exflags[] =
		{ (c & EXFLG_0) != 0, (c & EXFLG_1) != 0, (c & EXFLG_2) != 0, (c & EXFLG_3) != 0 };
	int const value =
		exflags[3] ? *reinterpret_cast<int*>(&cur_cs[1]) : cur_cs[1];
	int code_size =
		exflags[3] ? 3 : 2;

	//Mesf("inspect cs elem #%d (c = %X, type = %d, value = %d)", csindex, c, type, value);

	//if/else�̃X�L�b�v��
	int cmp_dest = -1;
	if ( type == TYPE_CMPCMD ) {
		int const skip_offset = cur_cs[code_size];
		code_size ++;
		cmp_dest = code_size + skip_offset;
	}

	//�o��
	std::ostringstream output;
	output
		<< csindex << '\t'
		<< Inspect_TypeName(type) << '\t'
		<< value << '\t';
	for ( int i = 0; i < 3; ++ i) {
		output << (exflags[i] ? "yes" : "-") << '\t';
	}
	if ( cmp_dest >= 0 ) {
		output << cmp_dest << '\t';
	} else {
		output << '-' << '\t';
	}

	switch ( type ) {
		case TYPE_MARK: {
			output << stringFromCalcCode(value);
			break;
		}
		case TYPE_STRING:
			output << "{\"" << (&ds_buf->GetBuffer()[value]) << "\"}";
			break;
		case TYPE_DNUM:
			output << reinterpret_cast<double*>(&ds_buf->GetBuffer()[value]);
			break;
		case TYPE_INUM:
			output << value;
			break;
		case TYPE_VAR: {
			output << "(var#" << value << ")";
			break;
		}
		case TYPE_LABEL: {
			int const csindex = reinterpret_cast<int*>(ot_buf->GetBuffer())[value];
			output << "*(lb#" << value << ": " << csindex << ")";
			break;
		}
		case TYPE_MODCMD: {
			//todo: STRUCTDAT::prmindex �𒲂ׂāA���̊֐����̑�N�����A�ƕ\�������ق����悢
			output << "(modcmd#" << value << ")";
			break;
		}
		case TYPE_STRUCT: {
			if ( value < 0 ) { output << "thismod"; break; }
			auto const stprm = &reinterpret_cast<STRUCTPRM*>(mi_buf->GetBuffer())[value];
			auto stdat = (stprm->subid >= 0)
				? &reinterpret_cast<STRUCTDAT*>(fi_buf->GetBuffer())[stprm->subid]
				: nullptr;

			// moduletag (newmod �̑�2����)
			if ( stdat && stprm->mptype == MPTYPE_STRUCTTAG ) {
				output << (&ds_buf->GetBuffer()[stdat->nameidx]);
				break;
			}

			// �p�����[�^�G�C���A�X�A�܂��̓��[�J���ϐ�(�����o�ϐ�)
			assert(stprm->mptype == MPTYPE_LOCALVAR || stprm->subid == STRUCTPRM_SUBID_STACK);
			
			//�ǂ̊֐��̃p�����[�^��������
			if ( !stdat ) {
				auto const found = std::find_if(MEMBUF_RANGE(STRUCTDAT, *fi_buf), [&](STRUCTDAT& it) {
					return ((value - it.prmindex) < it.prmmax);
				});
				stdat = (found != MEMBUF_END(STRUCTDAT, *fi_buf)) ? found : nullptr;
			}
			if ( stdat ) {
				char const* funcname = &ds_buf->GetBuffer()[stdat->nameidx];
				output << "(prm#" << value << ": " << funcname << "#" << (value - stdat->prmindex) << ")";
			} else {
				output << "(prm#" << value << ")";
			}
			break;
		}
		default: assert(type >= TYPE_INTCMD);
			if ( type < TYPE_USERDEF ) {
				//(type, value)�ɑΉ�����g�ݍ��݃R�}���h�̖��O����������
				int id = -1;
				for ( id = 0; id < lb->GetNumEntry(); ++id ) {
					if ( lb->GetType(id) == type && lb->GetOpt(id) == value ) break;
				}
				assert( id != lb->GetNumEntry() );
				output << lb->GetName(id);
			} else {
				//todo: #cmd �œo�^���ꂽ�L�[���[�h�̖��O���݂���
				output << "unknown";
			}
			break;
	}

	axi_buf->PutStr(const_cast<char*>(output.str().c_str()));
	axi_buf->PutCR();
	return code_size;
}

char const* CToken::Inspect_TypeName(int type) {
	assert(type >= 0);
	static char const* internalTypeNames[] = {
		"TYPE_MARK", "TYPE_VAR", "TYPE_STRING", "TYPE_DNUM", "TYPE_INUM", "TYPE_STRUCT",
		"TYPE_XLABEL", "TYPE_LABEL", "TYPE_INTCMD", "TYPE_EXTCMD", "TYPE_EXTSYSVAR", "TYPE_CMPCMD", "TYPE_MODCMD",
		"TYPE_INTFUNC", "TYPE_SYSVAR", "TYPE_PROGCMD", "TYPE_DLLFUNC", "TYPE_DLLCTRL",
		"TYPE_USERDEF" };
	if ( type < TYPE_USERDEF ) {
		return internalTypeNames[type];
	} else {
		//todo:�v���O�C���̌��𒴂��Ă��Ȃ����Ƃ�assert������
		return "TYPE_USERDEF";
	}
}

static char const* stringFromMPType(int mptype)
{
	switch ( mptype ) {
		case MPTYPE_VAR: return "var";
		case MPTYPE_STRING: return "str";
		case MPTYPE_DNUM: return "double";
		case MPTYPE_INUM: return "int";
		case MPTYPE_STRUCT: return "struct";
		case MPTYPE_LABEL: return "label";
		case MPTYPE_LOCALVAR: return "local";
		case MPTYPE_ARRAYVAR: return "array";
		case MPTYPE_SINGLEVAR: return "var";
		case MPTYPE_FLOAT: return "float";
		case MPTYPE_STRUCTTAG: return "structtag";
		case MPTYPE_LOCALSTRING: return "str";
		case MPTYPE_MODULEVAR: return "modvar";
		case MPTYPE_PPVAL: return "pval";
		case MPTYPE_PBMSCR: return "bmscr";
		case MPTYPE_PVARPTR: return "var";
		case MPTYPE_IMODULEVAR: return "modinit";
		case MPTYPE_IOBJECTVAR: return "comobj";
		case MPTYPE_LOCALWSTR: return "wstr";
		case MPTYPE_FLEXSPTR: return "sptr";
		case MPTYPE_FLEXWPTR: return "wptr";
		case MPTYPE_PTR_REFSTR: return "prefstr";
		case MPTYPE_PTR_EXINFO: return "pexinfo";
		case MPTYPE_PTR_DPMINFO: return "pdpminfo";  // ( MPTYPE_PTR_DPMINFO ) #func �� 0x20 ���w�肷��Ƒ�l����������ɂȂ�
		case MPTYPE_NULLPTR: return "nullptr";
		case MPTYPE_TMODULEVAR: return "modterm";
		case MPTYPE_NONE:
		default: assert_sentinel;
	}
}

void CToken::Inspect_FInfoSegment()
{
	Inspect_BeginSegment("FInfo");
	std::for_each(MEMBUF_RANGE(STRUCTDAT, *fi_buf), [this](STRUCTDAT& stdat) {
		char const* const name = &ds_buf->GetBuffer()[stdat.nameidx];
		bool const isCType = (stdat.index == STRUCTDAT_INDEX_CFUNC);

		std::ostringstream output;
		output << name;
		if ( isCType ) { output << '('; }
		if ( stdat.funcflag & STRUCTDAT_FUNCFLAG_CLEANUP ) {
			output << "onexit";
		} else {
			//parameters
			for ( int i = 0; i < stdat.prmmax; ++i ) {
				if ( i != 0 ) { output << ", "; }
				auto& stprm = MEMBUF_BEGIN(STRUCTPRM, *mi_buf)[stdat.prmindex];
				output << stringFromMPType(stprm.mptype);
			}
		}
		if ( isCType ) { output << ')'; }
	});
}

#endif

char const* stringFromCalcCode(int op)
{
	static char const* table[] = {
		"+", "-", "*", "/", "\\", "&", "|", "^",
		"=", "!", ">", "<", ">=", "<=", ">>", "<<"
	};
	assert(0 <= op && op < CALCCODE_MAX);
	return table[op];
}
