
//
//		Token analysis class ( HSP3 code generator )
//			onion software/onitama 2004/4
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

#include "errormsg.h"

#include <assert.h>
#define assert_sentinel assert(false); throw CGERROR_UNKNOWN;

// コード出力中の、スクリプトの位置を表す文字列
// 返値の有効期限はすぐに切れる
char const* CToken::CG_scriptPositionString() const
{
	static char stt_buf[HSP_MAX_PATH + 100];
	sprintf_s(stt_buf, "#%d[%s]", cg_orgline, cg_orgfile);
	return stt_buf;
}

//プログラムの次を絶対に実行させない命令
// goto, return, break, continue, end, stop
static bool IsTerminateCode(int type, int val)
{
	return (type == TYPE_PROGCMD
		&& (val == 0x00 || val == 0x02 || val == 0x03 || val == 0x06 || val == 0x10 || val == 0x11));
}

//-------------------------------------------------------------
// ConstCode
//-------------------------------------------------------------
CToken::ConstCode const CToken::ConstCode::mark { TYPE_MARK, 0 };

CToken::ConstCode CToken::ConstCode::castTo(int destType) const
{
	assert(type != destType && isConst());
	switch ( destType ) {
		case TYPE_STRING: return ConstCode::makeStr(toString().c_str(), exflag);
		case TYPE_DNUM: {
			switch ( type ) {
				case TYPE_STRING: return ConstCode::makeDouble(static_cast<double>(atof(str)), exflag);
				case TYPE_INUM:   return ConstCode::makeDouble(static_cast<double>(inum), exflag);
			}
		}
		case TYPE_INUM: {
			switch ( type ) {
				case TYPE_STRING: return ConstCode::makeInt( (str[0] == '$') ? htoi(str) : atoi(str),  exflag);
				case TYPE_DNUM:   return ConstCode::makeInt( static_cast<int>(dval), exflag );
			}
		}
		default: assert_sentinel;
	}
}

CToken::ConstCode CToken::CalcCG_evalConstExpr(int op, CToken::ConstCode const& lhs, CToken::ConstCode const& rhs)
{
	if ( lhs.type != rhs.type ) {
		return CalcCG_evalConstExpr(op, lhs, rhs.castTo(lhs.type));
	}

	int const exf = lhs.exflag;
	switch ( lhs.type ) {
		case TYPE_STRING:
			switch ( op ) {
				case CALCCODE_ADD: {
					auto result = std::string(lhs.str) + rhs.str;
					return ConstCode::makeStr(result.c_str(), exf);
				}
				default: Mesf("#文字列に対して演算子 %s は適用できません。", stringFromCalcCode(op));
					throw CGERROR_CALCEXP;
			}
		case TYPE_DNUM:
			switch ( op ) {
				case CALCCODE_ADD:  return ConstCode::makeDouble(lhs.dval + rhs.dval, exf);
				case CALCCODE_SUB:  return ConstCode::makeDouble(lhs.dval - rhs.dval, exf);
				case CALCCODE_MUL:  return ConstCode::makeDouble(lhs.dval *  rhs.dval, exf);
				case CALCCODE_DIV:  return ConstCode::makeDouble(lhs.dval / rhs.dval, exf);
				case CALCCODE_MOD:  return ConstCode::makeDouble(fmod(lhs.dval, rhs.dval), exf);
				case CALCCODE_EQ:   return ConstCode::makeInt(lhs.dval == rhs.dval, exf);
				case CALCCODE_NE:   return ConstCode::makeInt(lhs.dval != rhs.dval, exf);
				case CALCCODE_LT:   return ConstCode::makeInt(lhs.dval <  rhs.dval, exf);
				case CALCCODE_GT:   return ConstCode::makeInt(lhs.dval >  rhs.dval, exf);
				case CALCCODE_LTEQ: return ConstCode::makeInt(lhs.dval <= rhs.dval, exf);
				case CALCCODE_GTEQ: return ConstCode::makeInt(lhs.dval >= rhs.dval, exf);
				default: Mesf("#実数値に対して演算子 %s は適用できません。", stringFromCalcCode(op));
					throw CGERROR_CALCEXP;
			}
		case TYPE_INUM:
			switch ( op ) {
				case CALCCODE_ADD:  return ConstCode::makeInt(lhs.inum + rhs.inum, exf);
				case CALCCODE_SUB:  return ConstCode::makeInt(lhs.inum - rhs.inum, exf);
				case CALCCODE_MUL:  return ConstCode::makeInt(lhs.inum *  rhs.inum, exf);
				case CALCCODE_DIV:  return ConstCode::makeInt(lhs.inum / rhs.inum, exf);
				case CALCCODE_EQ:   return ConstCode::makeInt(lhs.inum == rhs.inum, exf);
				case CALCCODE_NE:   return ConstCode::makeInt(lhs.inum != rhs.inum, exf);
				case CALCCODE_LT:   return ConstCode::makeInt(lhs.inum <  rhs.inum, exf);
				case CALCCODE_GT:   return ConstCode::makeInt(lhs.inum >  rhs.inum, exf);
				case CALCCODE_LTEQ: return ConstCode::makeInt(lhs.inum <= rhs.inum, exf);
				case CALCCODE_GTEQ: return ConstCode::makeInt(lhs.inum >= rhs.inum, exf);
				case CALCCODE_AND:  return ConstCode::makeInt(lhs.inum &  rhs.inum, exf);
				case CALCCODE_OR:   return ConstCode::makeInt(lhs.inum | rhs.inum, exf);
				case CALCCODE_XOR:  return ConstCode::makeInt(lhs.inum ^  rhs.inum, exf);
				case CALCCODE_MOD:  return ConstCode::makeInt(lhs.inum %  rhs.inum, exf);
				case CALCCODE_LR:   return ConstCode::makeInt(lhs.inum << rhs.inum, exf);
				case CALCCODE_RR:   return ConstCode::makeInt(lhs.inum >> rhs.inum, exf);
				default: assert_sentinel;
			}
		default: assert_sentinel;
	}
}

CToken::ConstCode::~ConstCode() {
	if ( type == TYPE_STRING ) free(str);
}
CToken::ConstCode::ConstCode(CToken::ConstCode const& rhs)
	: type(rhs.type), exflag(rhs.exflag)
{
	switch ( rhs.type ) {
		case TYPE_STRING: *this = makeStr(rhs.str, rhs.exflag); break;
		case TYPE_DNUM: dval = rhs.dval; break;
		case TYPE_INUM: inum = rhs.inum; break;
		case TYPE_MARK: break;
		default: assert_sentinel;
	}
}
CToken::ConstCode::ConstCode(CToken::ConstCode&& rhs)
	: type(rhs.type), exflag(rhs.exflag)
{
	switch ( rhs.type ) {
		case TYPE_STRING: str = rhs.str; rhs.str = nullptr; break;
		case TYPE_DNUM: dval = rhs.dval; break;
		case TYPE_INUM: inum = rhs.inum; break;
		case TYPE_MARK: break;
		default: assert(false);
	}
}
CToken::ConstCode& CToken::ConstCode::operator =(CToken::ConstCode&& rhs)
{ new (const_cast<ConstCode*>(this)) ConstCode(std::move(rhs)); return *this; }

CToken::ConstCode CToken::ConstCode::makeStr(char const* str, int exf)
{
	ConstCode self { TYPE_STRING, exf };
	self.str = static_cast<char*>(malloc(strlen(str) + 1));
	strcpy(self.str, str);
	return std::move(self);
}
CToken::ConstCode CToken::ConstCode::makeDouble(double dval, int exf)
{
	ConstCode self { TYPE_DNUM, exf };
	self.dval = dval;
	return std::move(self);
}
CToken::ConstCode CToken::ConstCode::makeInt(int ival, int exf)
{
	ConstCode self { TYPE_INUM, exf };
	self.inum = ival;
	return std::move(self);
}
bool CToken::ConstCode::isConst() const {
	return (type == TYPE_STRING || type == TYPE_DNUM || type == TYPE_INUM);
}
std::string CToken::ConstCode::toString() const {
	static char conv[64];
	switch ( type ) {
		case TYPE_STRING: return str;
		case TYPE_DNUM: sprintf_s(conv, "%f", dval); return conv;
		case TYPE_INUM: sprintf_s(conv, "%d", inum); return conv;
		default: assert_sentinel;
	}
}

//-------------------------------------------------------------
//		Routines
//-------------------------------------------------------------

void CToken::CalcCG_token( void )
{
	GetTokenCG( GETTOKEN_DEFAULT );
	if ( ttype == TK_NONE ) ttype = val;
}

void CToken::CalcCG_token_exprbeg( void )
{
	GetTokenCG( GETTOKEN_EXPRBEG );
	if ( ttype == TK_NONE ) ttype = val;
}

void CToken::CalcCG_token_exprbeg_redo( void )
{
	//		GETTOKEN_EXPRBEG でトークンを取得し直す
	//
	//		GetTokenCG は 文字列リテラルや文字コードリテラルの場合、
	//		cg_ptr のバッファを破壊するので常に取得し直すわけにはいかない
	//
	if ( ttype == TK_NONE ) ttype = val;
	if ( ttype == '-' || ttype == '*' ) {
		cg_ptr = cg_ptr_bak;
		CalcCG_token_exprbeg();
	}
}

static int is_statement_end( int type )
{
	return ( type == TK_SEPARATE )||( type == TK_EOL )||( type == TK_EOF );
}


void CToken::CalcCG_putConstElem(CToken::ConstCode&& self)
{
	if ( CG_optCode() && self.isConst() ) {
		// 定数値は畳み込むかもしれないのでスタックに積んでおく
		stack_calculator->emplace_back(std::move(self));
	} else {
		CalcCG_putCSCalcElem(self);
	}
}

void CToken::CalcCG_putCSCalcElem(CToken::ConstCode const& self)
{
	switch ( self.type ) {
		case TYPE_STRING: PutCS(TYPE_STRING, PutDS(self.str), self.exflag); break;
		case TYPE_DNUM:   PutCS(TYPE_DNUM, self.dval, self.exflag); break;
		case TYPE_INUM:   PutCS(TYPE_INUM, self.inum, self.exflag); break;
		case TYPE_MARK:
		default: assert_sentinel;
	}
	calccount++;
}

// 変数が出現したか、式の終端に達したので、定数式畳み込みをやめる
// 積み込んでいた定数をコードに書き出す
void CToken::CalcCG_ceaseConstFolding()
{
	if ( !CG_optCode() ) return;

	assert(!stack_calculator->empty());
	auto const iterEnd = stack_calculator->end();
	auto iterBegin = iterEnd;
	while ( (iterBegin - 1)->isConst() ) { --iterBegin; }

	for ( auto iter = iterBegin; iter != iterEnd; ++iter) {
		CalcCG_putCSCalcElem(*iter);
	}
	stack_calculator->erase(iterBegin, iterEnd);
}

static int CalcCodeFromMark(int mark)
{
	switch ( mark ) {
		case '+':  return CALCCODE_ADD;
		case '-':  return CALCCODE_SUB;
		case '*':  return CALCCODE_MUL;
		case '/':  return CALCCODE_DIV;
		case '=':  return CALCCODE_EQ;
		case '!':  return CALCCODE_NE;
		case '<':  return CALCCODE_LT;
		case '>':  return CALCCODE_GT;
		case 0x61: return CALCCODE_LTEQ;    // '<='
		case 0x62: return CALCCODE_GTEQ;    // '>='
		case '&':  return CALCCODE_AND;
		case '|':  return CALCCODE_OR;
		case '^':  return CALCCODE_XOR;
		case 0x5c: return CALCCODE_MOD; // '\'
		case 0x63: return CALCCODE_LR;  // '<<'
		case 0x64: return CALCCODE_RR;  // '>>'
		default: throw CGERROR_CALCEXP;
	}
}

void CToken::CalcCG_regmark( int mark )
{
	//		演算子を登録する
	//
	int const op = CalcCodeFromMark(mark);

	if ( stack_calculator->size() >= 2
		&& stack_calculator->back().isConst()
		&& stack_calculator->at(stack_calculator->size() - 2).isConst() ) {
		auto const rhs = std::move(stack_calculator->back()); stack_calculator->pop_back();
		auto const lhs = std::move(stack_calculator->back()); stack_calculator->pop_back();

		// const expr folding
		stack_calculator->emplace_back(CalcCG_evalConstExpr(op, lhs, rhs));

		if ( CG_optInfo() ) {
			auto const& result = stack_calculator->back();
			Mesf("#定数式畳み込み (%s %s %s) → %s %s",
				lhs.toString().c_str(), stringFromCalcCode(op),
				rhs.toString().c_str(), result.toString().c_str(), CG_scriptPositionString());
		}
		return;
	}

	CalcCG_ceaseConstFolding();
	PutCS( TK_NONE, op, texflag ); calccount++;
}

void CToken::CalcCG_factor( void )
{
	cs_lasttype = ttype;
	switch( ttype ) {
	case TK_NUM: // constants
	case TK_DNUM:
	case TK_STRING:
		switch ( ttype ) {
			case TK_NUM:   	CalcCG_putConstElem(ConstCode::makeInt(val, texflag)); break;
			case TK_DNUM:  	CalcCG_putConstElem(ConstCode::makeDouble(val_d, texflag)); break;
			case TK_STRING:	CalcCG_putConstElem(ConstCode::makeStr(cg_str, texflag)); break;
		}
		texflag = 0;
		CalcCG_token();
		break;
	case TK_LABEL:
		CalcCG_ceaseConstFolding();
		GenerateCodeLabel(cg_str, texflag); calccount++;
		//stack_calculator->push(ConstCode { TYPE_LABEL, { GenerateOTIndex(cg_str) }, texflag });
		texflag = 0;
		CalcCG_token();
		
		break;
	case TK_OBJ:
	{
		int const id = SetVarsFixed(cg_str, cg_defvarfix);
		if ( lb->GetType(id) == TYPE_VAR ) {
			if ( lb->GetInitFlag(id) == LAB_INIT_NO ) {
#ifdef JPNMSG
				Mesf("#未初期化の変数 (%s). %s", cg_str, CG_scriptPositionString());
#else
				Mesf( "#Uninitalized variable (%s).", cg_str );
#endif
				if ( hed_cmpmode & CMPMODE_VARINIT ) {
					throw CGERROR_VAR_NOINIT;
				}
				lb->SetInitFlag(id, LAB_INIT_DONE);
			}
		}
		CalcCG_ceaseConstFolding();
		GenerateCodeVAR(id, texflag);
		calccount ++;
		texflag = 0;
		if ( ttype == TK_NONE ) ttype = val;		// CalcCG_token()に合わせるため
		return;
	}
	case TK_SEPARATE:
	case TK_EOL:
	case TK_EOF:
		break;
	default:
		//		カッコの処理
		//
		if ( ttype == '(' ) {
			CalcCG_token_exprbeg();
			CalcCG_start();
			if ( ttype != ')' ) { ttype = TK_CALCERROR; return; }
			CalcCG_token();
		} else {
			//Mesf("#Invalid%d",ttype);
			ttype = TK_CALCERROR;
		}
	}
}

void CToken::CalcCG_unary( void )
{
	//		単項演算子
	//
	int op;
	if ( ttype=='-' ) {
		op=ttype; CalcCG_token_exprbeg();
		if ( is_statement_end(ttype) ) throw CGERROR_CALCEXP;
		CalcCG_unary();
		texflag = 0;
		CalcCG_putConstElem(ConstCode::makeInt(-1, texflag));
		CalcCG_regmark( '*' );
	} else {
		CalcCG_factor();
	}
}

void CToken::CalcCG_muldiv( void )
{
	int op;
	CalcCG_unary();

	while( (ttype=='*')||(ttype=='/')||(ttype=='\\')) {
		op=ttype; CalcCG_token_exprbeg();
		if ( is_statement_end(ttype) ) throw CGERROR_CALCEXP;
		CalcCG_unary();
		CalcCG_regmark( op );
	}
}

void CToken::CalcCG_addsub( void )
{
	int op;
	CalcCG_muldiv();

	while( (ttype=='+')||(ttype=='-')) {
		op=ttype; CalcCG_token_exprbeg();
		if ( is_statement_end(ttype) ) throw CGERROR_CALCEXP;
		CalcCG_muldiv();
		CalcCG_regmark( op );
	}
}


void CToken::CalcCG_shift( void )
{
	int op;
	CalcCG_addsub();

	while( (ttype==0x63)||(ttype==0x64)) {
		op=ttype; CalcCG_token_exprbeg();
		if ( is_statement_end(ttype) ) throw CGERROR_CALCEXP;
		CalcCG_addsub();
		CalcCG_regmark( op );
	}
}


void CToken::CalcCG_compare( void )
{
	int op;
	CalcCG_shift();

	while( 
		(ttype=='<')||(ttype=='>')||(ttype=='=')||(ttype=='!')||
		(ttype==0x61)||(ttype==0x62)) {
		op=ttype; CalcCG_token_exprbeg();
		if ( is_statement_end(ttype) ) throw CGERROR_CALCEXP;
		CalcCG_shift();
		CalcCG_regmark( op );
	}
}


void CToken::CalcCG_bool( void )
{
	int op;
	CalcCG_compare();

	while( (ttype=='&')||(ttype=='|')||(ttype=='^')) {
		op=ttype; CalcCG_token_exprbeg();
		if ( is_statement_end(ttype) ) throw CGERROR_CALCEXP;
		CalcCG_compare();
		CalcCG_regmark( op );
	}
}


void CToken::CalcCG_start( void )
{
	//		entry point
	CalcCG_bool();
}

void CToken::CalcCG( int ex )
{
	//		パラメーターの式を評価する
	//		(結果は逆ポーランドでコードを出力する)
	//
	// 配列や関数があれば再帰的に呼ばれる

	if ( CG_optCode() ) {
		stack_calculator->push_back(ConstCode::mark); // virtual bottom
	}

	texflag = ex;
	cs_lastptr = cs_buf->GetSize();
	calccount = 0;

	CalcCG_token_exprbeg_redo();
	CalcCG_start();

	if ( ttype == TK_CALCERROR ) {
		throw CGERROR_CALCEXP;
	}
	if ( CG_optCode() ) {
		CalcCG_ceaseConstFolding(); // write consts
		assert(!stack_calculator->back().isConst());
		stack_calculator->pop_back(); // remove bottom mark
	}
}

//-----------------------------------------------------------------------------


char *CToken::PickLongStringCG( char *str )
{
	//		指定文字列をmembufへ展開する
	//		( 複数行対応 {"〜"} )
	//
	char *p;
	char *psrc;
	char *ps;
	p = psrc = str;

	while(1) {
		p = PickStringCG2( p, &psrc );
		if ( *psrc != 0 ) break;
		ps = GetLineCG();
		if ( ps == NULL ) throw CGERROR_MULTILINE_STR;
		psrc = ps;
		cg_orgline++;

		//		行の終端にある0を改行に置き換える
		p[0] = 13;
		p[1] = 10;
		p+=2;
	}
	if ( *psrc != '}' ) throw CGERROR_MULTILINE_STR;

	if ( cg_debug ) {
		PutDI( 254, 0, cg_orgline );                // ラインだけをデバッグ情報として登録
	}

	psrc++;
	return psrc;
}


char *CToken::PickStringCG( char *str, int sep )
{
	//		指定文字列をスキップして終端コードを付加する
	//			sep = 区切り文字
	//
	unsigned char *vs;
	unsigned char *pp;
	unsigned char a1;
	vs = (unsigned char *)str;
	pp = vs;

	while(1) {
		a1=*vs;
		if (a1==0) break;
		if (a1==sep) { vs++; break; }
		if (a1==0x5c) {					// '\'チェック
			vs++;
			a1 = tolower( *vs );
			if ( a1 < 32 ) continue;
			switch( a1 ) {
			case 'n':
				*pp++ = 13;
				a1 = 10;
				break;
			case 't':
				a1 = 9;
				break;
			case 'r':
				a1 = 13;
				break;
			}
		}
		if (a1>=129) {					// 全角文字チェック
			if ((a1<=159)||(a1>=224)) {
				*pp++ = a1;
				vs++;
				a1=*vs;
			}
		}
		vs++;
		*pp++ = a1;
	}
	*pp = 0;
	return (char *)vs;
}


char *CToken::PickStringCG2( char *str, char **strsrc )
{
	//		指定文字列をスキップして終端コードを付加する
	//			sep = 区切り文字
	//
	unsigned char *vs;
	unsigned char *pp;
	unsigned char a1;
	vs = (unsigned char *)*strsrc;
	pp = (unsigned char *)str;

	while(1) {
		a1=*vs;
		//Alertf("%d(%c)",a1,a1);
		if (a1==0) break;
		if (a1==0x22) {
			vs++;
			if ( *vs == '}' ) break;
			*pp++ = a1;
			continue;
		}
		if (a1==0x5c) {					// '\'チェック
			vs++;
			a1 = tolower( *vs );
			if ( a1 < 32 ) continue;
			switch( a1 ) {
			case 'n':
				*pp++ = 13;
				a1 = 10;
				break;
			case 't':
				a1 = 9;
				break;
			case 'r':
				a1 = 13;
				break;
			case 0x22:
				a1 = 0x22;
				break;
			}
		}
		if (a1>=129) {					// 全角文字チェック
			if ((a1<=159)||(a1>=224)) {
				*pp++ = a1;
				vs++;
				a1=*vs;
			}
		}
		vs++;
		*pp++ = a1;
	}
	*pp = 0;
	*strsrc = (char *)vs;
	return (char *)pp;
}


char *CToken::GetTokenCG( int option )
{
	cg_ptr_bak = cg_ptr;
	cg_ptr = GetTokenCG( cg_ptr, option );
	return cg_ptr;
}

int CToken::PickNextCodeCG( void )
{
	//		次のコード(１文字を返す)
	//		(終端の場合は0を返す)
	//
	unsigned char *vs;
	unsigned char a1;
	vs = (unsigned char *)cg_ptr;
	if ( vs == NULL ) return 0;
	while(1) {
		a1=*vs;
		if (( a1 != 32 )&&( a1 != '\t' )) {		// space,tab以外か?
			break;
		}
		vs++;
	}
	return (int)a1;
}

char *CToken::GetTokenCG( char *str, int option )
{
	//		stringデータのタイプと内容を返す
	//		(次のptrを返す)
	//		(ttypeにタイプを、val,val_d,cg_strに内容を書き込みます)
	//
	unsigned char *vs;
	unsigned char a1;
	unsigned char a2;
	int a,b,chk,labmode;

	vs = (unsigned char *)str;
	if ( vs==NULL ) {
		ttype = TK_EOF; return NULL;			// already end
	}

	while(1) {
		a1=*vs;
		if (( a1 != 32 )&&( a1 != '\t' )) {		// space,tab以外か?
			break;
		}
		vs++;
	}

	if (a1==0) {								// end
		ttype = TK_EOL;
		return (char *)vs;
		//return GetLineCG();
	}

	if ( a1<0x20 ) {							// 無効なコード
		ttype = TK_ERROR;
		throw CGERROR_UNKNOWN;
	}

	if (a1==0x22) {								// "〜"
		vs++;
		ttype = TK_STRING; cg_str = (char *)vs;
		return PickStringCG( (char *)vs, 0x22 );
	}

	if (a1=='{') {							// {"〜"}
		if (vs[1]==0x22) {
			vs+=2;
			if ( *vs == 0 ) {
				vs = (unsigned char *)GetLineCG();
				cg_orgline++;
			}
			ttype = TK_STRING; cg_str = (char *)vs;
			return PickLongStringCG( (char *)vs );
		}
	}

	if (a1==0x27) {								// '〜'
		char *p;
		vs++; cg_str = (char *)vs;
		p = PickStringCG( (char *)vs, 0x27 );
		ttype = TK_NUM; 
		val = ((unsigned char *)cg_str)[0];
		return p;
	}

	if ((a1==':')||(a1=='{')||(a1=='}')) {		// multi statement
		cg_str=(char *)s2;
		ttype = TK_SEPARATE;
		cg_str[0] = a1;
		cg_str[1] = 0;
		return (char *)vs+1;
	}

	if (a1=='0') {
		a2=tolower( vs[1] );
		if (a2=='x') { vs++;a1='$'; }		// when hex code (0x)
		if (a2=='b') { vs++;a1='%'; }		// when bin code (0b)
	}

	if (a1=='$') {							// when hex code ($)
		vs++;val=0;cg_str=(char *)s2;a=0;
		while(1) {
			a1=toupper(*vs);b=-1;
			if (a1==0) break;
			if ((a1>=0x30)&&(a1<=0x39)) b=a1-0x30;
			if ((a1>=0x41)&&(a1<=0x46)) b=a1-55;
			if (a1=='_') b=-2;
			if (b==-1) break;
			if (b>=0) { cg_str[a++]=(char)a1;val=(val<<4)+b; }
			vs++;
		}
		cg_str[a]=0;
		ttype = TK_NUM;
		return (char *)vs;
	}

	if (a1=='%') {							// when bin code (%)
		vs++;val=0;cg_str=(char *)s2;a=0;
		while(1) {
			a1=*vs;b=-1;
			if (a1==0) break;
			if ((a1>=0x30)&&(a1<=0x31)) b=a1-0x30;
			if (a1=='_') b=-2;
			if (b==-1) break;
			if (b>=0) { cg_str[a++]=(char)a1;val=(val<<1)+b; }
			vs++;
		}
		cg_str[a]=0;
		ttype = TK_NUM;
		return (char *)vs;
	}

	chk=0;
	labmode = 0;
	if (a1<0x30) chk++;
	if ((a1>=0x3a)&&(a1<=0x3f)) chk++;
	if ((a1>=0x5b)&&(a1<=0x5e)) chk++;
	if ((a1>=0x7b)&&(a1<=0x7f)) chk++;

	if ( option & (GETTOKEN_EXPRBEG|GETTOKEN_LABEL) ) {
		if ( a1 == '*' ) {					// ラベル
			a2 = vs[1]; b = 0;
			if (a2<0x30) b++;
			if ((a2>=0x30)&&(a2<=0x3f)) b++;
			if ((a2>=0x5b)&&(a2<=0x5e)) b++;
			if ((a2>=0x7b)&&(a2<=0x7f)) b++;
			if ( b == 0 ) {
				chk = 0;
				labmode = 1;
				vs++; a1 = *vs;
			}
		}
	}

	int is_negative_number = 0;
	if ( option & GETTOKEN_EXPRBEG && a1 == '-' ) {
		is_negative_number = isdigit(vs[1]);
	}

	if ( is_negative_number || isdigit(a1) ) {				// when 0-9 numerical
		a=0;chk=0;
		if ( is_negative_number ) {
			vs ++;
		}
		while(1) {
			a1=*vs;
			if ( option & GETTOKEN_NOFLOAT ) {
				if ((a1<0x30)||(a1>0x39)) break;
			} else {
				if ( a1=='.' ) {
					chk++;if ( chk>1 ) {
						ttype = TK_ERROR;
						throw CGERROR_FLOATEXP;
						return (char *)vs;
					}
				} else {
					if ((a1<0x30)||(a1>0x39)) break;
				}
			}
			s2[a++]=a1;vs++;
		}
		if (( a1=='f' )||( a1=='d' )) { chk = 1; vs++; }
		if ( a1=='e' ) {						// 指数部を取り込む
			chk = 1;
			s2[a++] = 'e';
			vs++;
			a1 = *vs;
			if (( a1=='-' )||( a1=='+' )) {
				s2[a++] = a1;
				vs++;
			}
			while(1) {
				a1=*vs;
				if ((a1<0x30)||(a1>0x39)) break;
				s2[a++]=a1;vs++;
			}
		}

		s2[a]=0;

		switch( chk ) {
		case 1:
			val_d = atof( (char *)s2 );
			if ( is_negative_number ) { val_d = - val_d; }
			ttype = TK_DNUM;
			break;
		default:
			val=atoi_allow_overflow( (char *)s2 );
			if ( is_negative_number ) { val = - val; }
			ttype = TK_NUM;
			break;
		}
		return (char *)vs;
	}

	if (chk) {								// 記号
		vs++;a2=*vs;
		switch( a1 ) {
		case '-':
			if (a2=='>') { vs++;a1=0x65; }	// '->'
			break;
		case '!':
			if (a2=='=') vs++;
			break;
		case '<':
			if (a2=='<') { vs++;a1=0x63; }	// '<<'
			if (a2=='=') { vs++;a1=0x61; }	// '<='
			break;
		case '>':
			if (a2=='>') { vs++;a1=0x64; }	// '>>'
			if (a2=='=') { vs++;a1=0x62; }	// '>='
			break;
		case '=':
			if (a2=='=') { vs++; }			// '=='
			break;
		case '|':
			if (a2=='|') { vs++; }			// '||'
			break;
		case '&':
			if (a2=='&') { vs++; }			// '&&'
			break;
		}
		ttype = TK_NONE;
		val = (int)a1;
		return (char *)vs;
	}

	a=0;
	while(1) {								// シンボル取り出し
		a1=*vs;
		if (a1>=129) {				// 全角文字チェック
			if ((a1<=159)||(a1>=224)) {
				if ( a<OBJNAME_MAX ) {
					s2[a++]=a1;
					vs++;
					a1=*vs;
					s2[a++] = a1; vs++;
				} else {
					vs+=2;
				}
				continue;
			}
		}

		chk=0;
		if (a1<0x30) chk++;
		if ((a1>=0x3a)&&(a1<=0x3f)) chk++;
		if ((a1>=0x5b)&&(a1<=0x5e)) chk++;
		if ((a1>=0x7b)&&(a1<=0x7f)) chk++;
		if ( chk ) break;
		vs++;
		if ( a<OBJNAME_MAX ) s2[a++]=a1;
	}
	s2[a]=0;

	//		シンボル
	//
	if ( labmode ) {
		ttype = TK_LABEL;
	} else {
		ttype = TK_OBJ;
	}
	cg_str = (char *)s2;
	return (char *)vs;
}


char *CToken::GetSymbolCG( char *str )
{
	//		stringデータのシンボル内容を返す
	//
	unsigned char *vs;
	unsigned char a1;
	int a,chk,labmode;

	vs = (unsigned char *)str;
	if ( vs==NULL ) return NULL;			// already end

	while(1) {
		a1=*vs;
		if (( a1 != 32 )&&( a1 != '\t' )) {		// space,tab以外か?
			break;
		}
		vs++;
	}

	if ( a1<0x20 ) {							// 無効なコード
		return NULL;
	}

	chk=0;
	labmode = 0;
	if (a1<0x30) chk++;
	if ((a1>=0x3a)&&(a1<=0x3f)) chk++;
	if ((a1>=0x5b)&&(a1<=0x5e)) chk++;
	if ((a1>=0x7b)&&(a1<=0x7f)) chk++;

	if (chk) {								// 記号
		return NULL;
	}

	a=0;
	while(1) {								// シンボル取り出し
		a1=*vs;
		if (a1>=129) {				// 全角文字チェック
			if ((a1<=159)||(a1>=224)) {
				if ( a<OBJNAME_MAX ) {
					s2[a++]=a1;
					vs++;
					a1=*vs;
					s2[a++] = a1; vs++;
				} else {
					vs+=2;
				}
				continue;
			}
		}

		chk=0;
		if (a1<0x30) chk++;
		if ((a1>=0x3a)&&(a1<=0x3f)) chk++;
		if ((a1>=0x5b)&&(a1<=0x5e)) chk++;
		if ((a1>=0x7b)&&(a1<=0x7f)) chk++;
		if ( chk ) break;
		vs++;
		if ( a<OBJNAME_MAX ) s2[a++]=a1;
	}
	s2[a]=0;
	return (char *)s2;
}


//-----------------------------------------------------------------------------

void CToken::GenerateCodePRM( void )
{
	//		HSP3Codeを展開する(パラメーター)
	//
	int ex;
	ex = 0;
	while(1) {

		if ( ttype == TK_NONE ) {
			if ( val == ',' ) {						// 先頭が','の場合は省略
				if ( ex & EXFLG_2 ) PutCS( TYPE_MARK, '?', EXFLG_2 );
				GetTokenCG( GETTOKEN_DEFAULT );
				ex |= EXFLG_2;
				continue;
			}
		}

		CalcCG( ex );								// 式の評価
		//Mesf( "#count %d", calccount );

		if ( hed_cmpmode & CMPMODE_OPTPRM ) {
			if ( calccount == 1 ) {						// パラメーターが単一項目の時
				switch( cs_lasttype ) {
				case TK_NUM:
				case TK_DNUM:
				case TK_STRING:
					{
					unsigned short *cstmp;
					cstmp = (unsigned short *)( cs_buf->GetBuffer() + cs_lastptr );
					*cstmp |= EXFLG_0;					// 単一項目フラグを立てる
					break;
					}
				default:
					break;
				}
			}
		}

		if ( ttype >= TK_SEPARATE ) break;
		if ( ttype != ',' ) {
			throw CGERROR_CALCEXP;
		}
		GetTokenCG( GETTOKEN_DEFAULT );
		ex |= EXFLG_2;

		if ( ttype >= TK_SEPARATE ) {
			PutCS( TYPE_MARK, '?', EXFLG_2 );
			break;
		}
	}
}


int CToken::GenerateCodePRMF( void )
{
	//		HSP3Codeを展開する(カッコ内のパラメーター)
	//		(戻り値 : exflg)
	//
	int ex;
	ex = 0;
	while(1) {

		if ( ttype == TK_NONE ) {
			if ( val == ')' ) {						// ')'の場合は終了
				return ex;
			}
			if ( val == ',' ) {						// 先頭が','の場合は省略
				if ( ex & EXFLG_2 ) PutCS( TYPE_MARK, '?', EXFLG_2 );
				GetTokenCG( GETTOKEN_DEFAULT );
				ex |= EXFLG_2;
				continue;
			}
		}

		CalcCG( ex );

		if ( ttype >= TK_SEPARATE ) throw CGERROR_PRMEND;

		if ( ttype == ')' ) break;
		if ( ttype != ',' ) {
			throw CGERROR_CALCEXP;
		}
		GetTokenCG( GETTOKEN_DEFAULT );
		ex |= EXFLG_2;
	}
	return 0;
}


void CToken::GenerateCodePRMF2( void )
{
	//		HSP3Codeを展開する('.'から始まる配列内のパラメーター)
	//
	int t;
	int id,ex;
	int tmp;
	ex = 0;
	while(1) {
		if ( ttype >= TK_SEPARATE ) break;

		//Mesf( "(type:%d val:%d) line:%d", ttype, val, cg_orgline );

		switch( ttype ) {
		case TK_NONE:
			if ( val == '(' ) {
				GetTokenCG( GETTOKEN_DEFAULT );
				CalcCG(ex);
				if ( ttype != ')' ) throw CGERROR_CALCEXP;
			} else {
				throw CGERROR_ARRAYEXP;
			}
			GetTokenCG( GETTOKEN_NOFLOAT );
			break;
		case TK_NUM:
			PutCS( TYPE_INUM, val, ex );
			GetTokenCG( GETTOKEN_NOFLOAT );
			break;
		case TK_OBJ:
			id = SetVarsFixed( cg_str, cg_defvarfix );
			t = lb->GetType(id);
			if (( t == TYPE_XLABEL )||( t == TYPE_LABEL )) throw CGERROR_LABELNAME;
			PutCSSymbol( id, ex );
			GetTokenCG( GETTOKEN_DEFAULT );

			if ( ttype == TK_NONE ) {
				if ( val == '(' ) {						// '(' 配列指定
					GetTokenCG( GETTOKEN_DEFAULT );
					PutCS( TYPE_MARK, '(', 0 );
					tmp = GenerateCodePRMF();
					if ( tmp ) 	PutCS( TYPE_MARK, '?', EXFLG_2 );
					PutCS( TYPE_MARK, ')', 0 );
					GetTokenCG( GETTOKEN_DEFAULT );
				}
			}

			//GenerateCodeVAR( id, ex );
			break;
		default:
			throw CGERROR_ARRAYEXP;
		}

		if ( ttype >= TK_SEPARATE ) return;
		if ( ttype != TK_NONE && ttype != '.' ) throw CGERROR_ARRAYEXP;
		if ( val != '.' ) return;

		GetTokenCG( GETTOKEN_NOFLOAT );
		ex |= EXFLG_2;
	}
}


void CToken::GenerateCodePRMF3( void )
{
	//		HSP3Codeを展開する('<'から始まる構造体参照元のパラメーター)
	//
	int id,ex;
	ex = 0;
	GetTokenCG( GETTOKEN_DEFAULT );
	if ( ttype != TK_OBJ ) throw CGERROR_PP_BAD_STRUCT_SOURCE;

	id = SetVarsFixed( cg_str, cg_defvarfix );
	GenerateCodeVAR( id, ex );

	if ( ttype != TK_NONE ) throw CGERROR_PP_BAD_STRUCT_SOURCE;
	if ( val != ']' ) throw CGERROR_PP_BAD_STRUCT_SOURCE;
	GetTokenCG( GETTOKEN_DEFAULT );
}


int CToken::GenerateCodePRMF4( int t )
{
	//		HSP3Codeを展開する(構造体/配列指定パラメーター)
	//
//	int t,id;
	int ex;
	int tmp;

	ex = 0;
	if ( ttype == TK_NONE ) {
		if ( val == '.' ) {
			GetTokenCG( GETTOKEN_NOFLOAT );
/*
			if ( ttype == TK_OBJ ) {
				id = lb->Search( cg_str );
				if ( id < 0 ) throw CGERROR_PP_BAD_STRUCT;
				t = lb->GetType(id);
				if ( t == TYPE_STRUCT ) {
					PutCS( t, lb->GetOpt(id), ex );
					GetTokenCG( GETTOKEN_DEFAULT );
					return GenerateCodePRMF4();
				}
			}
*/
			PutCS( TYPE_MARK, '(', 0 );			// '.' 配列指定
			GenerateCodePRMF2();
			PutCS( TYPE_MARK, ')', 0 );
			return 1;
		}
		if ( val == '(' ) {						// '(' 配列指定
			GetTokenCG( GETTOKEN_DEFAULT );
			PutCS( TYPE_MARK, '(', 0 );
			tmp = GenerateCodePRMF();
			if ( tmp ) 	PutCS( TYPE_MARK, '?', EXFLG_2 );
			PutCS( TYPE_MARK, ')', 0 );
			GetTokenCG( GETTOKEN_DEFAULT );
			return 1;
		}
		if ( t == TYPE_STRUCT ) {
			if ( val == '[' ) {						// '[' ソース指定
				PutCS( TYPE_MARK, '[', 0 );
				GenerateCodePRMF3();
				return 0;
			}
		}
	}
	return 0;
}


void CToken::GenerateCodeMethod( void )
{
	//		HSP3Codeを展開する(->に続くメソッド名)
	//
	int id,ex;
	ex = 0;

	if ( ttype >= TK_SEPARATE ) throw CGERROR_SYNTAX;
	switch( ttype ) {
	case TK_NUM:
		PutCS( TYPE_INUM, val, ex );
		GetTokenCG( GETTOKEN_DEFAULT );
		break;
	case TK_STRING:
		PutCS( TYPE_STRING, PutDS( cg_str ), ex );
		GetTokenCG( GETTOKEN_DEFAULT );
		break;
	case TK_OBJ:
		id = SetVarsFixed( cg_str, cg_defvarfix );
		GenerateCodeVAR( id, ex );
		break;
	default:
		throw CGERROR_SYNTAX;
	}

	ex |= EXFLG_2;
	while(1) {

		if ( ttype == TK_NONE ) {
			if ( val == ',' ) {						// 先頭が','の場合は省略
				if ( ex & EXFLG_2 ) PutCS( TYPE_MARK, '?', EXFLG_2 );
				GetTokenCG( GETTOKEN_DEFAULT );
				ex |= EXFLG_2;
				continue;
			}
		}

		CalcCG( ex );								// 式の評価

		if ( ttype >= TK_SEPARATE ) break;
		if ( ttype != ',' ) {
			throw CGERROR_CALCEXP;
		}
		GetTokenCG( GETTOKEN_DEFAULT );
		ex |= EXFLG_2;
	}

}

#if 0
void CToken::GenerateCodePRMN( void )
{
	//		HSP3Codeを展開する(パラメーター)
	//		(ソースの順番通りに展開する/実験用)
	//
	int i,t,ex;
	ex = 0;
	while(1) {
		if ( ttype >= TK_SEPARATE ) break;

		switch( ttype ) {
		case TK_NONE:
			if ( val == ',' ) {
				if ( ex & EXFLG_2 ) PutCS( TYPE_MARK, '?', ex );
				ex |= EXFLG_2;
			} else {
				PutCS( TYPE_MARK, val, ex );
				ex = 0;
			}
			break;
		case TK_NUM:
			PutCS( TYPE_INUM, val, ex );
			ex = 0;
			break;
		case TK_STRING:
			PutCS( TYPE_STRING, PutDS( cg_str ), ex );
			ex = 0;
			break;
		case TK_DNUM:
			PutCS( TYPE_DNUM, val_d, ex );
			ex = 0;
			break;
		case TK_OBJ:
			i = lb->Search( cg_str );
			if ( i < 0 ) {
				lb->Regist( cg_str, TYPE_VAR, cg_valcnt );
				PutCS( TYPE_VAR, cg_valcnt, ex );
				cg_valcnt++;
			} else {
				t = lb->GetType( i );
				if ( t == TYPE_XLABEL ) t = TYPE_LABEL;
				PutCS( t, lb->GetOpt(i), ex );
			}
			ex = 0;
			break;
		case TK_LABEL:
			GenerateCodeLabel( cg_str, ex );
			ex = 0;
			break;
		default:
			throw CGERROR_SYNTAX;
		}

		GetTokenCG( GETTOKEN_DEFAULT );
	}
}
#endif

int CToken::GenerateOTIndex(char* keyname)
{
	int id, t, i;
	char lname[128];
	char *name;

	name = keyname;
	if ( *name == '@' ) {
		switch ( tolower(name[1]) ) {
			case 'f':
				i = cg_locallabel;
				break;
			case 'b':
				i = cg_locallabel - 1;
				break;
			default:
				throw CGERROR_LABELNAME;
		}
		sprintf(lname, "@l%d", i);				// local label
		name = lname;
	}

	id = lb->Search(name);
	if ( id < 0 ) {									// 仮のラベル
		i = PutOT(-1);
		id = lb->Regist(name, TYPE_XLABEL, i);
	} else {
		t = lb->GetType(id);
		if ( (t != TYPE_XLABEL) && (t != TYPE_LABEL) ) throw CGERROR_LABELEXIST;
	}
	return lb->GetOpt(id);
}
void CToken::GenerateCodeLabel( char *keyname, int ex )
{
	//		HSP3Codeを展開する(ラベル)
	//
	PutCS(TYPE_LABEL, GenerateOTIndex(keyname), ex);
}


void CToken::GenerateCodeVAR( int id, int ex )
{
	//		HSP3Codeを展開する(変数や関数など)
	//		(idはlabel ID)
	//
	int t;
	t = lb->GetType(id);
	if (( t == TYPE_XLABEL )||( t == TYPE_LABEL )) throw CGERROR_LABELNAME;
	//
	PutCSSymbol( id, ex );
	GetTokenCG( GETTOKEN_DEFAULT );

	if ( t == TYPE_SYSVAR ) return;
	GenerateCodePRMF4( t );						// 構造体/配列のチェック
}


void CToken::CheckCMDIF_Set( int mode )
{
	//		set 'if'&'else' command additional code
	//			mode/ 0=if 1=else
	//
	if ( iflev >= CG_IFLEV_MAX ) throw CGERROR_IF_OVERFLOW;

	iftype[iflev] = mode;
	ifptr[iflev] = GetCS();

	cs_buf->Put( (short)0 );

	ifmode[iflev] = GetCS();
	ifscope[iflev] = CG_IFCHECK_LINE;
	ifterm[iflev] = 0;
	iflev++;
	//sprintf(tmp,"#IF BEGIN [L=%d(%d)]\n",cline,iflev);
	//prt(tmp);
}


void CToken::CheckCMDIF_Fin( int mode )
{
	//		finish 'if'&'else' command
	//			mode/ 0=if 1=else
	//
	int a;
	short *p;
	if (iflev==0) return;
finag:
	iflev--;

	a = GetCS() - ifmode[iflev];
	if (mode) {							// when 'else'
		a++;
	}

	if ( ifterm[iflev] == 0 ) {
		ifterm[iflev] = 1;
		p = (short *)cs_buf->GetBuffer();
		p[ ifptr[iflev] ] = (short)a;
	}

	//sprintf(tmp,"#IF FINISH [L=%d(%d)] [skip%d]\n",cline,iflev,a);
	//prt(tmp);

	//Mesf( "lev%d : %d: line%d", iflev, ifscope[iflev], cg_orgline );

	if (mode==0) {
		if ( iflev ) {
			if ( ifscope[iflev-1] == CG_IFCHECK_LINE ) goto finag;
		}
	}
}


void CToken::CheckInternalIF( int opt )
{
	//		内蔵命令生成時チェック
	//
	if (opt) {					// 'else'+offset
		if ( iflev == 0 ) throw CGERROR_ELSE_WO_IF;
		CheckCMDIF_Fin(1);
		CheckCMDIF_Set(1);
		return;
	}
	CheckCMDIF_Set(0);				// normal if
}


void CToken::CheckInternalCMD1( int opt )
{
	//		命令生成時チェック(命令+命令セット)
	//
	int i,t,o;
	if ( ttype != TK_OBJ ) return;
	i = lb->Search( cg_str );
	if ( i < 0 ) return;
	t = lb->GetType(i);
	o = lb->GetOpt(i);
	if ( t != TYPE_PROGCMD ) return;
	PutCS( t, o & 0xffff, 0 );
	GetTokenCG( GETTOKEN_DEFAULT );
}


void CToken::CheckInternalProgCMD( int opt, int orgcs )
{
	//		内蔵プログラム命令生成時チェック
	//
	int i;
	switch(opt) {

	case 0x03:					// repeat break
	case 0x06:					// repeat continue
		if (replev==0) {
			if (opt==0x03) throw CGERROR_BREAK_WO_REPEAT;
			throw CGERROR_CONT_WO_REPEAT;
		}
		i=repend[replev];
		if (i==-1) {
			i = PutOT( -1 );
			repend[replev]=i;
		}
		PutCS( TK_LABEL, i, 0 );
		break;
	case 0x04:					// repeat start
	case 0x0b:					// (foreach)
		if (replev==CG_REPLEV_MAX) throw CGERROR_REPEAT_OVERFLOW;
		replev++;
		i=repend[replev];
		if (i==-1) {
			i=PutOT( -1 );
			repend[replev]=i;
		}
		PutCS( TK_LABEL, i, 0 );
		if ( opt == 0x0b ) {
			PutCS( TYPE_PROGCMD, 0x0c, EXFLG_1 );
			PutCS( TK_LABEL, i, 0 );
		}
		break;
	case 0x05:					// repeat end
		if (replev==0) throw CGERROR_LOOP_WO_REPEAT;
		i = repend[replev];
		if (i!=-1) {
			SetOT( i, GetCS() );
			repend[replev] = -1;
		}
		replev--;
		break;
	case 0x11:					// stop
		i=PutOT( orgcs );
		PutCS( TYPE_PROGCMD, 0, EXFLG_1 );
		PutCS( TYPE_LABEL, i, 0 );
		break;
	case 0x19:					// on
		GetTokenCG( GETTOKEN_DEFAULT );
		CalcCG( 0 );								// 式の評価
		if ( ttype != TK_OBJ ) throw CGERROR_SYNTAX;
		i = lb->Search( cg_str );
		if ( i < 0 ) throw CGERROR_SYNTAX;
		PutCS( lb->GetType(i), lb->GetOpt(i), EXFLG_2 );
		break;

	case 0x08:					// await
		//	await命令の出現をカウントする(HEDINFO_NOMMTIMER自動設定のため)
		if ( hed_autoopt_timer >= 0 ) hed_autoopt_timer++;
		break;

	case 0x09:					// dim
	case 0x0a:					// sdim
	case 0x0d:					// dimtype
	case 0x0e:					// dup
	case 0x0f:					// dupptr
		GetTokenCG( GETTOKEN_DEFAULT );
		if ( ttype == TK_OBJ ) {
			i = SetVarsFixed( cg_str, cg_defvarfix );
			//	変数の初期化フラグをセットする
			lb->SetInitFlag( i, LAB_INIT_DONE );
			//Mesf( "#initflag set [%s]", cg_str );
		}
		cg_ptr = cg_ptr_bak;
		break;
	}
}


void CToken::GenerateCodeCMD( int id )
{
	//		HSP3Codeを展開する(コマンド)
	//		(idはlabel ID)
	//
	int t,opt;
	int orgcs;
	t = lb->GetType(id);
	opt = lb->GetOpt(id);
	orgcs = GetCS();
	PutCSSymbol( id, EXFLG_1 );
	if ( t == TYPE_PROGCMD ) CheckInternalProgCMD( opt, orgcs );
	if ( t == TYPE_CMPCMD ) CheckInternalIF( opt );
	GetTokenCG( GETTOKEN_DEFAULT );

	if ( opt & 0x10000 ) CheckInternalCMD1( opt );

	GenerateCodePRM();
	cg_lastcmd  = CG_LASTCMD_CMD;
	cg_lasttype = t;
	cg_lastval  = opt;
}


void CToken::GenerateCodeLET( int id )
{
	//		HSP3Codeを展開する(代入)
	//		(idはlabel ID)
	//
	int t;
	int mcall;

	t = lb->GetType(id);
	if (( t == TYPE_XLABEL )||( t == TYPE_LABEL )) throw CGERROR_LABELNAME;
	//
	mcall = 0;
	GetTokenCG( GETTOKEN_DEFAULT );

	if (( ttype == TK_NONE )&&( val == 0x65 )) {	// ->が続いているか?
		PutCS( TYPE_PROGCMD, 0x1a, EXFLG_1 );		// 'mcall'コマンドに置き換える
		PutCS( t, lb->GetOpt(id), 0 );				// 変数パラメーター
		GetTokenCG( GETTOKEN_DEFAULT );
		GenerateCodeMethod();						// パラメーター展開
		return;
	}

	PutCS( t, lb->GetOpt(id), EXFLG_1 );			// 通常の変数代入
	GenerateCodePRMF4( t );							// 構造体/配列のチェック

	if ( ttype != TK_NONE ) { throw CGERROR_SYNTAX; }

	int const op = val;
	PutCS( TK_NONE, CalcCodeFromMark(op), 0 );
	texflag = 0;

	cg_lastcmd  = CG_LASTCMD_LET;
	cg_lasttype = TYPE_VAR;
	cg_lastval  = op;

	GetTokenCG( GETTOKEN_DEFAULT );

	switch( op ) {
	case '+':								// '++'
	case '-':								// '--'
		if ( ttype >= TK_SEPARATE ) return;
		if ( ttype == TK_NONE ) {
			if ( val == op ) {
				GetTokenCG( GETTOKEN_DEFAULT );
				if ( ttype >= TK_SEPARATE ) return;
				throw CGERROR_SYNTAX;
			}
		}
		break;
	case '=':								// 変数=prm
		GenerateCodePRM();
		return;
	default:
		break;
	}
	
	if (( ttype == TK_NONE )&&( val == '=' )) {
		GetTokenCG( GETTOKEN_DEFAULT );
	}
	GenerateCodePRM();
}


void CToken::GenerateCodePP_regcmd( void )
{
	//		HSP3Codeを展開する(regcmd)
	//
	char cmd[1024];
	char cmd2[1024];
	cg_pptype = cg_typecnt;
	cmd[0] = 0;

	GetTokenCG( GETTOKEN_DEFAULT );
	switch( ttype ) {
	case TK_STRING:
		strcpy( cmd, cg_str );
		GetTokenCG( GETTOKEN_DEFAULT );
		if ( ttype != TK_NONE ) throw CGERROR_PP_NO_REGCMD;
		if ( val != ',' ) throw CGERROR_PP_NO_REGCMD;
		GetTokenCG( GETTOKEN_DEFAULT );
		if ( ttype != TK_STRING ) throw CGERROR_PP_NO_REGCMD;
		strcpy( cmd2, cg_str );

		GetTokenCG( GETTOKEN_DEFAULT );
		if ( ttype == TK_NONE ) {
			if ( val != ',' ) throw CGERROR_PP_NO_REGCMD;
			GetTokenCG( GETTOKEN_DEFAULT );
			if ( ttype != TK_NUM ) throw CGERROR_PP_NO_REGCMD;
			cg_varhpi+=val;
		}

		PutHPI( HPIDAT_FLAG_TYPEFUNC, 0, cmd2, cmd );
		cg_typecnt++;
		break;
	case TK_NUM:
		PutHPI( HPIDAT_FLAG_SELFFUNC, 0, "", "" );
		cg_pptype = val;
		break;
	default:
		throw CGERROR_PP_NO_REGCMD;
	}
	//Mesf( "#regcmd [%d][%s]",cg_pptype, cmd );
}


void CToken::GenerateCodePP_cmd( void )
{
	//		HSP3Codeを展開する(cmd)
	//
	int id;
	char cmd[1024];
	if ( cg_pptype < 0 ) throw CGERROR_PP_NO_REGCMD;
	GetTokenCG( GETTOKEN_DEFAULT );
	if ( ttype != TK_OBJ ) throw CGERROR_PP_NO_REGCMD;
	strcpy( cmd, cg_str );

	//if ( ttype != TK_NONE ) throw CGERROR_PP_NO_REGCMD;
	//if ( val != ',' ) throw CGERROR_PP_NO_REGCMD;
	//GetTokenCG( GETTOKEN_DEFAULT );

	GetTokenCG( GETTOKEN_DEFAULT );
	if ( ttype != TK_NUM ) throw CGERROR_PP_NO_REGCMD;
	id = val;

	lb->Regist( cmd, cg_pptype, id );
	//Mesf( "#%x:%d [%s]",cg_pptype, id, cmd );
}


void CToken::GenerateCodePP_uselib( void )
{
	//		HSP3Codeを展開する(uselib)
	//
	GetTokenCG( GETTOKEN_DEFAULT );
	cg_libname[0] = 0;
	if ( ttype == TK_STRING ) {
		strncpy( cg_libname, cg_str, 1023 );
	} else if ( ttype < TK_VOID ) {
		throw CGERROR_PP_NAMEREQUIRED;
	}
	cg_libmode = CG_LIBMODE_DLLNEW;
}


void CToken::GenerateCodePP_usecom( void )
{
	//		HSP3Codeを展開する(usecom)
	//
	int i,prmid;
	char libname[1024];
	char clsname[128];
	char iidname[128];

	GetTokenCG( GETTOKEN_DEFAULT );
	if ( ttype != TK_OBJ ) {
		throw CGERROR_PP_NAMEREQUIRED;
	}
	strncpy( libname, cg_str, 1023 );

	i = lb->Search( libname );
	if ( i >= 0 ) {
		throw CGERROR_PP_ALREADY_USE_PARAM;
	}

	GetTokenCG( GETTOKEN_DEFAULT );
	if ( ttype != TK_STRING ) throw CGERROR_PP_BAD_IMPORT_IID;
	strncpy( iidname, cg_str, 127 );

	*clsname = 0;
	GetTokenCG( GETTOKEN_DEFAULT );
	if ( ttype < TK_EOL ) {
		if ( ttype != TK_STRING ) throw CGERROR_PP_BAD_IMPORT_IID;
		strncpy( clsname, cg_str, 127 );
	}

	cg_libindex = PutLIB( LIBDAT_FLAG_COMOBJ, iidname );
	if ( cg_libindex < 0 ) throw CGERROR_PP_BAD_IMPORT_IID;

	SetLIBIID( cg_libindex, clsname );
	cg_libmode = CG_LIBMODE_COM;

	PutStructStart();
	prmid = PutStructEndDll( "*", cg_libindex, STRUCTPRM_SUBID_COMOBJ, -1 );
	lb->Regist( libname, TYPE_DLLCTRL, prmid | TYPE_OFFSET_COMOBJ );

	//Mesf( "#usecom %s [%s][%s]",libname,clsname,iidname );
}


void CToken::GenerateCodePP_func( int deftype )
{
	//		HSP3Codeを展開する(func)
	//
	int warn,i,t,subid,otflag;
	int ref;
	char fbase[1024];
	char fname[1024];

	GetTokenCG( GETTOKEN_DEFAULT );
	if ( ttype != TK_OBJ ) throw CGERROR_PP_NAMEREQUIRED;
	strncpy( fbase, cg_str, 1023 );

	ref = -1;
	if ( CG_optCode() &&( tmp_lb != NULL )) {		// プリプロセス情報から最適化を行なう
		i = tmp_lb->Search( fbase );
		if ( i >= 0 ) {
			ref = tmp_lb->GetReference( i );
			//Mesf( "#func %s [use%d]", fbase, ref );
		}
	}

	warn = 0;
	otflag = deftype;
	GetTokenCG( GETTOKEN_DEFAULT );

	if ( ttype == TK_OBJ ) {
		if ( strcmp( cg_str, "onexit" ) == 0 ) {
			otflag |= STRUCTDAT_OT_CLEANUP;
			GetTokenCG( GETTOKEN_DEFAULT );
		}
	}

	if ( ref == 0 && (otflag & STRUCTDAT_OT_CLEANUP) == 0 ) {
		if ( CG_optInfo() ) {
#ifdef JPNMSG
			Mesf("#未参照のDll関数を削除(%s) %s", fbase, CG_scriptPositionString());
#else
			Mesf( "#Delete func %s", fbase );
#endif
		}
		return;
	}

	if ( cg_libmode == CG_LIBMODE_DLLNEW ) {							// 初回はDLL名を登録する
		cg_libindex = PutLIB( LIBDAT_FLAG_DLL, cg_libname );
		cg_libmode = CG_LIBMODE_DLL;
	}
	if ( cg_libmode != CG_LIBMODE_DLL ) throw CGERROR_PP_NO_USELIB;

	switch( ttype ) {
	case TK_OBJ:
		sprintf( fname,"_%s@16",cg_str );
		warn = 1;
		break;
	case TK_STRING:
		strncpy( fname, cg_str, 1023 );
		break;
	case TK_NONE:
		if ( val == '*' ) break;
		throw CGERROR_PP_BAD_IMPORT_NAME;
	default:
		throw CGERROR_PP_BAD_IMPORT_NAME;
	}
	GetTokenCG( GETTOKEN_DEFAULT );

	PutStructStart();
	if ( ttype == TK_NUM ) {
		int p1,p2,p3,p4,c1;
		warn = 1;
		p1 = p2 = p3 = p4 = MPTYPE_INUM;
		c1 = val & 3;
		if ( c1 == 1 ) p1 = MPTYPE_PVARPTR;
		if ( c1 == 2 ) p1 = MPTYPE_PBMSCR;
		if ( c1 == 3 ) {
			if (( val & 0x80 )==0 ) throw CGERROR_PP_INCOMPATIBLE_IMPORT;
			p1 = MPTYPE_PPVAL;
		}
		if ( val & 4 ) p2 = MPTYPE_LOCALSTRING;
		if ( val & 0x10 ) p4 = MPTYPE_PTR_REFSTR;
		if ( val & 0x20 ) p4 = MPTYPE_PTR_DPMINFO;
		if ( val & 0x100 ) otflag |= STRUCTDAT_OT_CLEANUP;
		if ( val & 0x200 ) {
			if ( (val&3) != 2 ) throw CGERROR_PP_INCOMPATIBLE_IMPORT;
			p1 = MPTYPE_PTR_EXINFO;
			p2 = p3 = MPTYPE_NULLPTR;
			if (( val & 0x30 ) == 0 ) p4 = MPTYPE_NULLPTR;
		}
//		if ( val & 0x220 ) throw CGERROR_PP_INCOMPATIBLE_IMPORT;

//		Mesf("#oldfunc %d,%d,%d,%d",p1,p2,p3,p4);

		PutStructParam( p1, STRUCTPRM_SUBID_STID );
		PutStructParam( p2, STRUCTPRM_SUBID_STID );
		PutStructParam( p3, STRUCTPRM_SUBID_STID );
		PutStructParam( p4, STRUCTPRM_SUBID_STID );

	} else {
		while(1) {
			if ( ttype >= TK_EOL ) break;
			if ( ttype != TK_OBJ ) throw CGERROR_PP_WRONG_PARAM_NAME;
			t = GetParameterFuncTypeCG( cg_str );
			if ( t == MPTYPE_NONE ) throw CGERROR_PP_WRONG_PARAM_NAME;
			PutStructParam( t, STRUCTPRM_SUBID_STID );
			GetTokenCG( GETTOKEN_DEFAULT );

			if ( ttype >= TK_EOL ) break;
			if ( ttype != TK_NONE ) throw CGERROR_PP_WRONG_PARAM_NAME;
			if ( val != ',' ) throw CGERROR_PP_WRONG_PARAM_NAME;
			GetTokenCG( GETTOKEN_DEFAULT );
		}
	}

	i = lb->Search( fbase );
	if ( i >= 0 ) {
		throw CGERROR_PP_ALREADY_USE_FUNCNAME;
	}
	subid = STRUCTPRM_SUBID_DLL;
	if ( warn ) {
		subid = STRUCTPRM_SUBID_OLDDLL;
		//Mesf( "Warning:Old func expression [%s]", fbase );
	}
	i = PutStructEndDll( fname, cg_libindex, subid, otflag );
	lb->Regist( fbase, TYPE_DLLFUNC, i );

	//Mesf( "#func [%s][%s][%d]",fbase, fname, i );
}


void CToken::GenerateCodePP_comfunc( void )
{
	//		HSP3Codeを展開する(comfunc)
	//
	int i,t,subid,imp_index;
	char fbase[1024];

	if ( cg_libmode != CG_LIBMODE_COM ) throw CGERROR_PP_NO_USECOM;

	GetTokenCG( GETTOKEN_DEFAULT );
	if ( ttype != TK_OBJ ) throw CGERROR_PP_NAMEREQUIRED;
	strncpy( fbase, cg_str, 1023 );

	GetTokenCG( GETTOKEN_DEFAULT );

	if ( ttype != TK_NUM ) {
		throw CGERROR_PP_BAD_IMPORT_INDEX;
	}
	imp_index = val;

	GetTokenCG( GETTOKEN_DEFAULT );

	PutStructStart();
	PutStructParam( MPTYPE_IOBJECTVAR, STRUCTPRM_SUBID_STID );

	while(1) {
		if ( ttype >= TK_EOL ) break;
		if ( ttype != TK_OBJ ) throw CGERROR_PP_WRONG_PARAM_NAME;
		t = GetParameterFuncTypeCG( cg_str );
		if ( t == MPTYPE_NONE ) throw CGERROR_PP_WRONG_PARAM_NAME;
		PutStructParam( t, STRUCTPRM_SUBID_STID );
		GetTokenCG( GETTOKEN_DEFAULT );

		if ( ttype >= TK_EOL ) break;
		if ( ttype != TK_NONE ) throw CGERROR_PP_WRONG_PARAM_NAME;
		if ( val != ',' ) throw CGERROR_PP_WRONG_PARAM_NAME;
		GetTokenCG( GETTOKEN_DEFAULT );
	}

	i = lb->Search( fbase );
	if ( i >= 0 ) {
		throw CGERROR_PP_ALREADY_USE_TAGNAME;
	}
	subid = STRUCTPRM_SUBID_COMOBJ;
	i = PutStructEndDll( "*", cg_libindex, subid, imp_index );
	lb->Regist( fbase, TYPE_DLLCTRL, i | TYPE_OFFSET_COMOBJ );

	//Mesf( "#comfunc [%s][%d][%d]",fbase, imp_index, i );
}


int CToken::GetParameterTypeCG( char *name )
{
	//		パラメーター名を認識する(deffunc)
	//
	if ( !strcmp( cg_str,"int" ) ) return MPTYPE_INUM;
	if ( !strcmp( cg_str,"var" ) ) return MPTYPE_SINGLEVAR;
	if ( !strcmp( cg_str,"val" ) ) { 
#ifdef JPNMSG
		Mesf("警告:古いdeffunc表記(%s) %s", cg_orgline, name, CG_scriptPositionString());
#else
		Mesf( "Warning:Old deffunc expression at %d.[%s]", cg_orgline, name );
#endif
		return MPTYPE_SINGLEVAR;
	}
	if ( !strcmp( cg_str,"str" ) ) return MPTYPE_LOCALSTRING;
	if ( !strcmp( cg_str,"double" ) ) return MPTYPE_DNUM;
	if ( !strcmp( cg_str,"label" ) ) return MPTYPE_LABEL;
	if ( !strcmp( cg_str,"local" ) ) return MPTYPE_LOCALVAR;
	if ( !strcmp( cg_str,"array" ) ) return MPTYPE_ARRAYVAR;
	if ( !strcmp( cg_str,"modvar" ) ) return MPTYPE_MODULEVAR;
	if ( !strcmp( cg_str,"modinit" ) ) return MPTYPE_IMODULEVAR;
	if ( !strcmp( cg_str,"modterm" ) ) return MPTYPE_TMODULEVAR;

	return MPTYPE_NONE;
}


int CToken::GetParameterStructTypeCG( char *name )
{
	//		パラメーター名を認識する(struct)
	//
	if ( !strcmp( cg_str,"int" ) ) return MPTYPE_INUM;
	if ( !strcmp( cg_str,"var" ) ) return MPTYPE_LOCALVAR;
	if ( !strcmp( cg_str,"str" ) ) return MPTYPE_LOCALSTRING;
	if ( !strcmp( cg_str,"double" ) ) return MPTYPE_DNUM;
	if ( !strcmp( cg_str,"label" ) ) return MPTYPE_LABEL;
	if ( !strcmp( cg_str,"float" ) ) return MPTYPE_FLOAT;
	return MPTYPE_NONE;
}


int CToken::GetParameterFuncTypeCG( char *name )
{
	//		パラメーター名を認識する(func)
	//
	if ( !strcmp( cg_str,"int" ) ) return MPTYPE_INUM;
	if ( !strcmp( cg_str,"var" ) ) return MPTYPE_PVARPTR;
	if ( !strcmp( cg_str,"str" ) ) return MPTYPE_LOCALSTRING;
	if ( !strcmp( cg_str,"double" ) ) return MPTYPE_DNUM;
//	if ( !strcmp( cg_str,"label" ) ) return MPTYPE_LABEL;
	if ( !strcmp( cg_str,"float" ) ) return MPTYPE_FLOAT;
	if ( !strcmp( cg_str,"pval" ) ) return MPTYPE_PPVAL;
	if ( !strcmp( cg_str,"bmscr" ) ) return MPTYPE_PBMSCR;

	if ( !strcmp( cg_str,"comobj" ) ) return MPTYPE_IOBJECTVAR;
	if ( !strcmp( cg_str,"wstr" ) ) return MPTYPE_LOCALWSTR;

	if ( !strcmp( cg_str,"sptr" ) ) return MPTYPE_FLEXSPTR;
	if ( !strcmp( cg_str,"wptr" ) ) return MPTYPE_FLEXWPTR;

	if ( !strcmp( cg_str,"prefstr" ) ) return MPTYPE_PTR_REFSTR;
	if ( !strcmp( cg_str,"pexinfo" ) ) return MPTYPE_PTR_EXINFO;
	if ( !strcmp( cg_str,"nullptr" ) ) return MPTYPE_NULLPTR;

//	if ( !strcmp( cg_str,"hwnd" ) ) return MPTYPE_PTR_HWND;
//	if ( !strcmp( cg_str,"hdc" ) ) return MPTYPE_PTR_HDC;
//	if ( !strcmp( cg_str,"hinst" ) ) return MPTYPE_PTR_HINST;

	return MPTYPE_NONE;
}


int CToken::GetParameterResTypeCG( char *name )
{
	//		戻り値のパラメーター名を認識する(defcfunc)
	//
	if ( !strcmp( cg_str,"int" ) ) return MPTYPE_INUM;
	if ( !strcmp( cg_str,"str" ) ) return MPTYPE_STRING;
	if ( !strcmp( cg_str,"double" ) ) return MPTYPE_DNUM;
	if ( !strcmp( cg_str,"label" ) ) return MPTYPE_LABEL;
	if ( !strcmp( cg_str,"float" ) ) return MPTYPE_FLOAT;
	return MPTYPE_NONE;
}


#define GET_FI_SIZE() ((int)(fi_buf->GetSize() / sizeof(STRUCTDAT)))
#define GET_FI(n) (((STRUCTDAT *)fi_buf->GetBuffer()) + (n))
#define STRUCTDAT_INDEX_DUMMY ((short)0x8000)


void CToken::GenerateCodePP_deffunc0( int is_command )
{
	//		HSP3Codeを展開する(deffunc / defcfunc)
	//
	int i,t,ot,prmid,subid;
	int index;
	int funcflag;
	int regflag;
	int prep;
	char funcname[1024];
	STRUCTPRM *prm;
	STRUCTDAT *st;

	prep = 0;
	GetTokenCG( GETTOKEN_DEFAULT );
	if ( ttype != TK_OBJ ) throw CGERROR_PP_NAMEREQUIRED;

	if ( is_command && !strcmp( cg_str,"prep" ) ) {				// プロトタイプ宣言
		prep = 1;
		GetTokenCG( GETTOKEN_DEFAULT );
		if ( ttype != TK_OBJ ) throw CGERROR_PP_NAMEREQUIRED;
	}

	strncpy( funcname, cg_str, 1023 );

	for(i=0;i<cg_localcur;i++) {
		lb->SetFlag( cg_localstruct[i], -1 );		// 以前に指定されたパラメーター名を削除する
	}
	cg_localcur = 0;
	funcflag = 0;
	regflag = 1;

	index = -1;
	int label_id = lb->Search( funcname );
	if ( label_id >= 0 ) {
		if ( lb->GetType(label_id) != TYPE_MODCMD ) throw CGERROR_PP_ALREADY_USE_FUNC;
		index = lb->GetOpt(label_id);
		if ( index >= 0 && GET_FI(index)->index != STRUCTDAT_INDEX_DUMMY ) {
			throw CGERROR_PP_ALREADY_USE_FUNC;
		}
	}

	PutStructStart();
	while(1) {
		GetTokenCG( GETTOKEN_DEFAULT );
		if ( ttype >= TK_EOL ) break;
		if ( ttype != TK_OBJ ) throw CGERROR_PP_WRONG_PARAM_NAME;

		if ( is_command && !strcmp( cg_str,"onexit" ) ) {
			funcflag |= STRUCTDAT_FUNCFLAG_CLEANUP;
			break;
		}

		t = GetParameterTypeCG( cg_str );
		if ( t == MPTYPE_NONE ) throw CGERROR_PP_WRONG_PARAM_NAME;
		if (( t == MPTYPE_MODULEVAR )||( t == MPTYPE_IMODULEVAR )||( t == MPTYPE_TMODULEVAR )) {
			//	モジュール名指定
			GetTokenCG( GETTOKEN_DEFAULT );
			if ( ttype != TK_OBJ ) throw CGERROR_PP_WRONG_PARAM_NAME;
			i = lb->Search( cg_str );
			if ( i < 0 ) throw CGERROR_PP_BAD_STRUCT;
			if ( lb->GetType(i) != TYPE_STRUCT ) throw CGERROR_PP_BAD_STRUCT;
			prm = (STRUCTPRM *)mi_buf->GetBuffer();
			subid = prm[ lb->GetOpt(i) ].subid;
			//Mesf( "%s:struct%d",cg_str,subid );
			if ( t == MPTYPE_IMODULEVAR ) {
				if ( prm[ lb->GetOpt(i) ].offset != -1 ) throw CGERROR_PP_MODINIT_USED;
				prm[ lb->GetOpt(i) ].offset = GET_FI_SIZE();
				regflag = 0;
			}
			if ( t == MPTYPE_TMODULEVAR ) {
				st = (STRUCTDAT *)fi_buf->GetBuffer();
				if ( st[ subid ].otindex != 0 ) throw CGERROR_PP_MODTERM_USED;
				st[ subid ].otindex = GET_FI_SIZE();
				regflag = 0;
			}
			prmid = PutStructParam( t, subid );
			GetTokenCG( GETTOKEN_DEFAULT );

		} else {
			prmid = PutStructParam( t, STRUCTPRM_SUBID_STACK );
			//Mesf( "%d:type%d",prmid,t );

			GetTokenCG( GETTOKEN_DEFAULT );
			if ( ttype == TK_OBJ ) {
				//	引数のエイリアス
				i = lb->Search( cg_str );
				if ( i >= 0 ) {
					throw CGERROR_PP_ALREADY_USE_PARAM;
				}
				i = lb->Regist( cg_str, TYPE_STRUCT, prmid );
				cg_localstruct[ cg_localcur++ ] = i;
				GetTokenCG( GETTOKEN_DEFAULT );
			}
		}

		if ( ttype >= TK_EOL ) break;
		if ( ttype != TK_NONE ) throw CGERROR_PP_WRONG_PARAM_NAME;
		if ( val != ',' ) throw CGERROR_PP_WRONG_PARAM_NAME;
	}

	ot = PutOT( GetCS() );
	if ( index == -1 ) {
		index = GET_FI_SIZE();
		fi_buf->PreparePtr( sizeof(STRUCTDAT) );
		if ( regflag ) {
			lb->Regist( funcname, TYPE_MODCMD, index );
		}
	}
	if ( label_id >= 0 ) {
		lb->SetOpt( label_id, index );
	}
	int dat_index = is_command ? STRUCTDAT_INDEX_FUNC : STRUCTDAT_INDEX_CFUNC;
	PutStructEnd( index, funcname, dat_index, ot, funcflag );

	cg_evaluatable = true;
}


void CToken::GenerateCodePP_deffunc( void )
{
	GenerateCodePP_deffunc0( 1 );
}


void CToken::GenerateCodePP_defcfunc( void )
{
	GenerateCodePP_deffunc0( 0 );
}


void CToken::GenerateCodePP_module( void )
{
	//		HSP3Codeを展開する(module)
	//
	int i,ref;
	char *modname;
	GetTokenCG( GETTOKEN_DEFAULT );
	if ( ttype != TK_OBJ ) throw CGERROR_PP_NAMEREQUIRED;
	modname = cg_str;

	if ( CG_optCode() &&( tmp_lb != NULL )) {		// プリプロセス情報から最適化を行なう
		i = tmp_lb->Search( modname );
		if ( i >= 0 ) {
			ref = tmp_lb->GetReference( i );
			if ( ref == 0 ) {
				cg_flag = CG_FLAG_DISABLE;
				if ( CG_optInfo() ) {
#ifdef JPNMSG
					Mesf("#未使用のモジュールを削除(%s) %s", modname, CG_scriptPositionString());
#else
					Mesf( "#Delete module %s", modname );
#endif
				}
				return;
			}
		}
	}

}


void CToken::GenerateCodePP_struct( void )
{
	//		HSP3Codeを展開する(struct)
	//
	int i,t,prmid;
	char funcname[1024];
	GetTokenCG( GETTOKEN_DEFAULT );
	if ( ttype != TK_OBJ ) throw CGERROR_PP_NAMEREQUIRED;
	strncpy( funcname, cg_str, 1023 );
	i = lb->Search( funcname );
	if ( i >= 0 ) {
		throw CGERROR_PP_ALREADY_USE_PARAM;
	}

	PutStructStart();
	prmid = PutStructParamTag();					// modinit用のTAG
	lb->Regist( funcname, TYPE_STRUCT, prmid );
	//Mesf( "%d:%s",prmid, funcname );

	while(1) {
		GetTokenCG( GETTOKEN_DEFAULT );
		if ( ttype >= TK_EOL ) break;
		if ( ttype != TK_OBJ ) throw CGERROR_PP_WRONG_PARAM_NAME;
		t = GetParameterStructTypeCG( cg_str );
		if ( t == MPTYPE_NONE ) throw CGERROR_PP_WRONG_PARAM_NAME;
		prmid = PutStructParam( t, STRUCTPRM_SUBID_STID );
		//Mesf( "%d:type%d",prmid,t );

		GetTokenCG( GETTOKEN_DEFAULT );
		if ( ttype != TK_OBJ ) throw CGERROR_PP_WRONG_PARAM_NAME;

		i = lb->Search( cg_str );
		if ( i >= 0 ) {
			throw CGERROR_PP_ALREADY_USE_PARAM;
		}
		lb->Regist( cg_str, TYPE_STRUCT, prmid );

		GetTokenCG( GETTOKEN_DEFAULT );
		if ( ttype >= TK_EOL ) break;
		if ( ttype != TK_NONE ) throw CGERROR_PP_WRONG_PARAM_NAME;
		if ( val != ',' ) throw CGERROR_PP_WRONG_PARAM_NAME;
	}
	PutStructEnd( funcname, STRUCTDAT_INDEX_STRUCT, 0, 0 );
}


void CToken::GenerateCodePP_defvars( int fixedvalue )
{
	//		HSP3Codeを展開する(defint,defdouble,defnone)
	//
	int id;
	int prms;
	prms = 0;
	while(1) {
		GetTokenCG( GETTOKEN_DEFAULT );
		if ( ttype >= TK_EOL ) break;
		if ( ttype != TK_OBJ ) throw CGERROR_WRONG_VARIABLE;
		id = SetVarsFixed( cg_str, fixedvalue );
		if ( lb->GetType(id) != TYPE_VAR ) {
			throw CGERROR_WRONG_VARIABLE;
		}
		prms++;
		//Mesf( "name:%s(%d) fixed:%d", cg_str, id, fixedvalue );

		GetTokenCG( GETTOKEN_DEFAULT );
		if ( ttype >= TK_EOL ) break;
		if ( ttype != TK_NONE ) throw CGERROR_PP_WRONG_PARAM_NAME;
		if ( val != ',' ) throw CGERROR_PP_WRONG_PARAM_NAME;
	}

	if ( prms == 0 ) {
		cg_defvarfix = fixedvalue;
	}
}


int CToken::SetVarsFixed( char *varname, int fixedvalue )
{
	//		変数の固定型を設定する
	//
	int id;
	id = lb->Search( varname );
	if ( id < 0 ) {
		id = lb->Regist( varname, TYPE_VAR, cg_valcnt );
		cg_valcnt++;
	}
	lb->SetForceType( id, fixedvalue );
	return id;
}


void CToken::GenerateCodePP( char *buf )
{
	//		HSP3Codeを展開する(プリプロセスコマンド)
	//
	int i;
	GetTokenCG( GETTOKEN_DEFAULT );					// 最初の'#'を読み飛ばし
	GetTokenCG( GETTOKEN_DEFAULT );

	if ( ttype == TK_NONE ) {						// プリプロセッサから渡される行情報
		if ( val != '#' ) throw CGERROR_UNKNOWN;
		GetTokenCG( GETTOKEN_DEFAULT );
		if ( ttype != TK_NUM ) throw CGERROR_UNKNOWN;
		cg_orgline = val;
		GetTokenCG( GETTOKEN_DEFAULT );
		if ( ttype == TK_STRING ) {
			strcpy ( cg_orgfile, cg_str );
			if ( cg_debug ) {
				i = PutDSBuf( cg_str );
				PutDI( 254, i, cg_orgline );				// ファイル名をデバッグ情報として登録
			}
		} else {
			if ( cg_debug ) {
				PutDI( 254, 0, cg_orgline );				// ラインだけをデバッグ情報として登録
			}
		}
		//Mesf( "#%d [%s]",cg_orgline, cg_str );
		return;
	}

	if ( ttype != TK_OBJ ) {								// その他はエラー
		throw CGERROR_PP_SYNTAX;
	}

	if ( !strcmp( cg_str,"global" ) ) { cg_flag = CG_FLAG_ENABLE; return; }

	if ( cg_flag != CG_FLAG_ENABLE ) {						// 最適化による出力抑制
		return;
	}

	if ( !strcmp( cg_str,"regcmd" ) ) { GenerateCodePP_regcmd(); return; }
	if ( !strcmp( cg_str,"cmd" ) ) { GenerateCodePP_cmd(); return; }
	if ( !strcmp( cg_str,"uselib" ) ) { GenerateCodePP_uselib(); return; }
	if ( !strcmp( cg_str,"func" ) ) { GenerateCodePP_func( STRUCTDAT_OT_STATEMENT ); return; }
	if ( !strcmp( cg_str,"cfunc" ) ) { GenerateCodePP_func( STRUCTDAT_OT_FUNCTION ); return; }
	if ( !strcmp( cg_str,"deffunc" ) ) { GenerateCodePP_deffunc(); return; }
	if ( !strcmp( cg_str,"defcfunc" ) ) { GenerateCodePP_defcfunc(); return; }
	if ( !strcmp( cg_str,"module" ) ) { GenerateCodePP_module(); return; }
	if ( !strcmp( cg_str,"struct" ) ) { GenerateCodePP_struct(); return; }
	if ( !strcmp( cg_str,"usecom" ) ) { GenerateCodePP_usecom(); return; }
	if ( !strcmp( cg_str,"comfunc" ) ) { GenerateCodePP_comfunc(); return; }
	if ( !strcmp( cg_str,"defint" ) ) { GenerateCodePP_defvars( LAB_TYPEFIX_INT ); return; }
	if ( !strcmp( cg_str,"defdouble" ) ) { GenerateCodePP_defvars( LAB_TYPEFIX_DOUBLE ); return; }
	if ( !strcmp( cg_str,"defnone" ) ) { GenerateCodePP_defvars( LAB_TYPEFIX_NONE ); return; }
}


int CToken::GenerateCodeSub( void )
{
	//		文字列(１行単位)からHSP3Codeを展開する
	//		(エラー発生時は例外が発生します)
	//
	int i,t;
//	char tmp[512];

	cg_errline = line;

	if ( cg_ptr == NULL ) return TK_EOF;

	if ( *cg_ptr == '#' ) {
		GenerateCodePP( cg_ptr );
		return TK_EOL;
	}

	if ( cg_flag != CG_FLAG_ENABLE ) return TK_EOL;				// 最適化による出力抑制

//	while(1) {
//		if ( cg_ptr!=NULL ) Mes( cg_ptr );
		GetTokenCG( GETTOKEN_LABEL );
		if ( ttype >= TK_SEPARATE ) return ttype;

		switch( ttype ) {
//		case TK_NONE:
//			sprintf( tmp,"#cod:%d",val );
//			Mes( tmp );
//			break;
//		case TK_NUM:
//			sprintf( tmp,"#num:%d",val );
//			Mes( tmp );
//			break;
//		case TK_STRING:
//			sprintf( tmp,"#str:%s",cg_str );
//			Mes( tmp );
//			break;
//		case TK_DNUM:
//			sprintf( tmp,"#dbl:%f",val_d );
//			Mes( tmp );
//			break;
		case TK_OBJ:
			cg_lastcmd = CG_LASTCMD_LET;
			i = lb->Search( cg_str );

			if ( !cg_evaluatable ) {
				cg_evaluatable = (lb->GetType(i) == TYPE_PROGCMD && lb->GetOpt(i) == 0x005); // loop
				if ( !cg_evaluatable ) {
					if ( CG_optInfo() ) Mesf("#実行されないコードの抑制 %s", CG_scriptPositionString());
					return TK_EOL;
				}
			}

			if ( i < 0 ) {
				//Mesf( "[%s][%d]",cg_str, cg_valcnt );
				i = SetVarsFixed( cg_str, cg_defvarfix );
				lb->SetInitFlag( i, LAB_INIT_DONE );		//	変数の初期化フラグをセットする
				GenerateCodeLET( i );
			} else {
				t = lb->GetType( i );
				switch( t ) {
				case TYPE_VAR:
				case TYPE_STRUCT:
					GenerateCodeLET( i );
					break;
				case TYPE_LABEL:
				case TYPE_XLABEL:
					throw CGERROR_LABELNAME;
					break;
				default:
					GenerateCodeCMD( i );
					if ( IsTerminateCode(cg_lasttype, cg_lastval) ) { cg_evaluatable = false; }
					break;
				}
			}
//			sprintf( tmp,"#obj:%s (%d)",cg_str,i );
//			Mes( tmp );
			break;
		case TK_LABEL:
			cg_evaluatable = true;

			//Mesf( "#lab:%s",cg_str );
			if ( *cg_str == '@') {
				sprintf(cg_str, "@l%d", cg_locallabel );				// local label
				cg_locallabel++;
			}

			i = lb->Search( cg_str );
			if ( i >= 0 ) {
				LABOBJ *lab;
				lab = lb->GetLabel( i );
				if ( lab->type != TYPE_XLABEL ) throw CGERROR_LABELEXIST;
				SetOT( lb->GetOpt(i), GetCS() );
				lab->type = TYPE_LABEL; 
			} else {
				i = lb->Regist( cg_str, TYPE_LABEL, GetOTCount() );
				PutOT( GetCS() );
			}
			GetTokenCG( GETTOKEN_DEFAULT );
			break;
		default:
			throw CGERROR_SYNTAX;
		}
//	}

	if ( ttype < TK_SEPARATE ) throw CGERROR_SYNTAX;
	return ttype;
}


char *CToken::GetLineCG( void )
{
	//		vs_wpから１行を取得する
	//
	char *pp;
	unsigned char *p;
	unsigned char a1;
	p = cg_wp;
	if ( p == NULL ) return NULL;

	pp = (char *)p;
	a1 = *p;
	if ( a1 == 0 ) { cg_wp = NULL; return NULL; }
	while(1) {
		a1=*p;
		if (a1>=129) {						// 全角文字チェック
			if ((a1<=159)||(a1>=224)) {
				p++;
				if ( *p >= 32 ) { p++; continue; }
			}
		}
		if ( a1 == 0 ) break;
		if ( a1 == 13 ) {
			*p = 0; p++; line++;
			if ( *p == 10 ) { *p = 0; p++; }
			break;
		}
		if ( a1 == 10 ) { *p=0; p++; line++; break; }
		p++;
	}
	cg_wp = p;
	return pp;
}


int CToken::GenerateCodeBlock( void )
{
	//		プロック単位でHSP3Codeを展開する
	//		(エラー発生時は例外が発生します)
	//
	int res,id,ff;
	char a1;
	char *p;
	res = GenerateCodeSub();
	if ( res == TK_EOF ) return res;
	if ( res == TK_EOL ) {
		cg_ptr = GetLineCG();
		if ( iflev ) {
			if ( ifscope[iflev-1] == CG_IFCHECK_LINE ) CheckCMDIF_Fin(0);			// 'if' jump support
			cg_evaluatable = true;
		}
		if ( cg_debug ) PutDI();
		cg_orgline++;
	}
	if ( res == TK_SEPARATE ) {
		a1 = cg_str[0];
		if (a1=='{') {					// when '{'
			if ( iflev == 0 ) throw CGERROR_BLOCKEXP;
			if ( ifscope[iflev-1] == CG_IFCHECK_SCOPE ) throw CGERROR_BLOCKEXP;
			ifscope[iflev-1] = CG_IFCHECK_SCOPE;
		} else if (a1=='}') {			// when '}'
			if ( iflev == 0 ) throw CGERROR_BLOCKEXP;
			if ( ifscope[iflev-1] != CG_IFCHECK_SCOPE ) throw CGERROR_BLOCKEXP;
			cg_evaluatable = true;

			ff = 0;
			p = GetTokenCG( cg_ptr, GETTOKEN_DEFAULT );

			if ( ttype == TK_EOL ) {										// 次行のコマンドがelseかどうか調べる
				if ( cg_wp != NULL ) {
					p = GetSymbolCG( (char *)cg_wp );
					if ( p != NULL ) {
						id = lb->Search( p );
						if ( id >= 0 ) {
							if (( lb->GetType(id)==TYPE_CMPCMD )&&( lb->GetOpt(id)==1 )) {
								ff = 1;
							}
						}
					}
				}
			} else if ( ttype == TK_OBJ ) {									// 次のコマンドがelseかどうか調べる
				id = lb->Search( cg_str );
				if ( id >= 0 ) {
					if (( lb->GetType(id)==TYPE_CMPCMD )&&( lb->GetOpt(id)==1 )) {
						//ifscope[iflev-1] = CG_IFCHECK_LINE;					// line scope on	
						ff = 1;
					}
				}
			}
			if ( ff == 0 ) CheckCMDIF_Fin(0);
		}
	}
	return res;
}


void CToken::RegisterFuncLabels( void )
{
	//		プリプロセス時のラベル情報から関数を定義
	//
	if ( tmp_lb == NULL ) return;
	int len = tmp_lb->GetCount();
	for( int i = 0; i < len; i ++ ) {
		if ( tmp_lb->GetType(i) == LAB_TYPE_PPMODFUNC && tmp_lb->GetFlag(i) >= 0 ) {
			char *name = tmp_lb->GetName(i);
			if ( lb->Search(name) >= 0 ) {
				throw CGERROR_PP_ALREADY_USE_FUNC;
			}
			int id = lb->Regist( name, TYPE_MODCMD, -1 );
			lb->SetData2( id, (char *)&i, sizeof i );
		}
	}
}


int CToken::GenerateCodeMain( CMemBuf *buf )
{
	//		ソースをHSP3Codeに展開する
	//		(ソースのバッファを書き換えるので注意)
	//
	int a;
	line = 0;
	cg_flag = CG_FLAG_ENABLE;
	cg_evaluatable = true;
	cg_valcnt = 0;
	cg_typecnt = HSP3_TYPE_USER;
	cg_pptype = -1;
	cg_iflev = 0;
	cg_wp = (unsigned char *)buf->GetBuffer();
	cg_ptr = GetLineCG();
	cg_orgfile[0] = 0;
	cg_libindex = -1;
	cg_libmode = CG_LIBMODE_NONE;
	cg_lastcs = 0;
	cg_lastcmd = CG_LASTCMD_NONE;
	cg_localcur = 0;
	cg_locallabel = 0;
	cg_varhpi = 0;
	cg_defvarfix = LAB_TYPEFIX_NONE;

	iflev=0;
	replev=0;
	std::fill(repend, &repend[CG_REPLEV_MAX], -1);

	try {
		RegisterFuncLabels();

		while(1) {
			if ( GenerateCodeBlock() == TK_EOF ) break;
		}

		cg_errline = -1;				// エラーの行番号は該当なし

		//		コンパイル後の後始末チェック
		if ( replev != 0 ) throw CGERROR_LOOP_NOTFOUND;

		//		ラベル未処理チェック
		int errend;
		errend = 0;
		for( a=0; a<lb->GetCount(); a++ ) {
			if ( lb->GetType(a) == TYPE_XLABEL ) {
				Mesf( "#ラベルが存在しません [%s]",lb->GetName(a) );
				errend++;
			}
		}
		
		//		関数未処理チェック
		for( a=0; a<GET_FI_SIZE(); a++ ) {
			if ( GET_FI(a)->index == STRUCTDAT_INDEX_DUMMY ) {
				Mesf( "#関数が存在しません [%s]", lb->GetName(GET_FI(a)->otindex) );
				errend++;
			}
		}
		
		//      ブレース対応チェック 
		if ( iflev > 0 ) { 
				Mesf( "#波括弧が閉じられていません。" );
				errend ++;
		}

		if ( errend ) throw CGERROR_FATAL;
	}
	catch ( CGERROR code ) {
		return (int)code;
	}

	return 0;
}


void CToken::SetCS(int csindex, int type, int value)
{
	// コードセグメントに上書きする
	// bit 数は固定されているので注意

	assert(0 <= csindex && csindex < GetCS());
	unsigned short* const cscode = &reinterpret_cast<unsigned short*>(cs_buf->GetBuffer())[csindex];
	auto const _type = *cscode & CSTYPE;
	auto const _exf = *cscode &~ CSTYPE;
	assert(0 <= _type && _type < TYPE_USERDEF + cg_varhpi);

	*cscode = type | _exf;
	if ( _exf & EXFLG_3 ) {
		*reinterpret_cast<int*>(cscode + 1) = value;
	} else {
		assert(static_cast<unsigned int>(value) < 0x10000);  // when 16 bit encode
		cscode[1] = value;
	}
}

void CToken::PutCS( int type, int value, int exflg )
{
	//		Register command code
	//		(HSP ver3.3以降用)
	//			type=0-0xfff ( -1 to debug line info )
	//			val=16,32bit length supported
	//
	assert(type != TYPE_XLABEL);
	if ( CG_optShort() && type == TYPE_LABEL ) {
		label_reference_table->insert({ value, GetCS() });
		//Mesf("#ラベル参照 (cs#%d -> ot#%d)", GetCS(), value);
	}

	int a;
	unsigned int v;
	v=(unsigned int)value;

	a=( type & CSTYPE ) | exflg;
	if ( v<0x10000 ) {						// when 16bit encode
		cs_buf->Put( (short)(a) );
		cs_buf->Put( (short)(v) );
	}
	else {									// when 32bit encode
		cs_buf->Put( (short)(EXFLG_3 | a) );
		cs_buf->Put( (int)value );
	}
}


void CToken::PutCS( int type, double value, int exflg )
{
	//		Register command code (double)
	//
	int i = ds_buf->GetSize();
	bool needsToPutDS = true;

	// double literal pool
	if ( CG_optCode() ) {
		auto const it = double_literal_table->find(value);
		if ( it != double_literal_table->end() ) {
			i = it->second;
			needsToPutDS = false;
			if ( CG_optInfo() ) { Mesf("#double_literal_pool(%f) %s", value, CG_scriptPositionString()); }
		} else {
			double_literal_table->insert({ value, i });
		}
	}

	if ( needsToPutDS ) {
		ds_buf->Put(value);
	}
	PutCS( type, i, exflg );
}


void CToken::PutCSSymbol( int label_id, int exflag )
{
	//		まだ定義されていない関数の呼び出しがあったら仮登録する
	//
	int type = lb->GetType(label_id);
	int value = lb->GetOpt(label_id);
	if ( type == TYPE_MODCMD && value == -1 ) {
		int id = *(int *)lb->GetData2(label_id);
		tmp_lb->AddReference( id );
		
		STRUCTDAT st = { STRUCTDAT_INDEX_DUMMY };
		st.otindex = label_id;
		value = GET_FI_SIZE();
		fi_buf->PutData( &st, sizeof(STRUCTDAT) );
		lb->SetOpt( label_id, value );
	}
	if ( exflag & EXFLG_1 && type != TYPE_VAR && type != TYPE_STRUCT ) {
		value &= 0xffff;
	}
	PutCS( type, value, exflag );
}


int CToken::GetCS( void )
{
	//		Get current CS index
	//
	return ( cs_buf->GetSize() )>>1;
}


int CToken::PutDS( char *str )
{
	//		Register strings to data segment (script string)
	//

	// output as UTF8 format
	if ( cg_utf8out ) {
		int i = ds_buf->GetSize();
		char* p = ExecSCNV( str, SCNV_OPT_SJISUTF8 );
		size_t size = strlen(p) + 1;
		ds_buf->PutData( p, static_cast<int>(size) );
		return i;
	}

	return PutDSBuf(str);
}


int CToken::PutDSBuf( char *str )
{
	//		Register strings to data segment (direct)
	//
	int i;
	i = ds_buf->GetSize();

	// string literal pool
	// todo: UTF8に対応。そのまま適用できるか調べるか、または対応できるようにする
	if ( CG_optCode() ) {
		auto const it = string_literal_table->find(str);
		if ( it != string_literal_table->end() ) {
			i = it->second;
			if ( CG_optInfo() ) Mesf("#string_literal_pool {\"%s\"} %s", str, CG_scriptPositionString());
			return i;
		} else {
			string_literal_table->insert({ std::string(str), i });
		}
}

	ds_buf->PutStr( str );
	ds_buf->Put( (char)0 );
	return i;
}

int CToken::PutDSBuf( char *str, int size )
{
	//		Register strings to data segment (direct)
	//
	int i;
	i = ds_buf->GetSize();
	ds_buf->PutData( str, size );
	return i;
}


int CToken::PutOT( int value )
{
	// ラベル(cs位置参照)オブジェクトを確保し、そのIDを返す
	// 参照先のcs位置が未確定の場合 (if文のジャンプ先など)、value < 0 で登録される。
	//
	int const i = GetOTCount();
	working_ot_buf->push_back(value);
	return i;
}


void CToken::SetOT( int id, int value )
{
	// ラベル(cs位置参照)オブジェクトの参照先を確定させる
	//
	assert(0 <= id && id < (int)working_ot_buf->size());
	
	auto& ref = working_ot_buf->at(id);
	assert(ref < 0); // 未確定でなければならない

	ref = value;
}

int CToken::GetOT(int id)
{
	assert(0 <= id && id < (int)working_ot_buf->size());
	return working_ot_buf->at(id);
}
int CToken::GetOTCount() { return working_ot_buf->size(); }

void CToken::PutOTBuf()
{
	// OTバッファを書き出す
	// 最適化: ラベルの重複を取り除き、otindex を割り振りなおす

	assert(ot_buf->GetSize() == 0);
	int ot_count = 0;

	for (int i = 0; i < static_cast<int>(working_ot_buf->size()); ++ i) {
		auto const csindex = working_ot_buf->at(i);
		assert(csindex >= 0);  // すべてのラベルが確定済みのはず

		if ( CG_optShort() ) {
			//同じ参照先をもつ既存ラベルをみつける
			int const new_ot_index = GetNewOTFromOldOT(i);
			if ( new_ot_index >= 0 ) {
				//Mesf("#重複ラベルを発見 (otindex = %d -> %d; csindex = %d)", i, new_ot_index, csindex);
				continue;  // skip output
			} else {
				otindex_table->insert({ csindex, ot_count });
			}
		}
		ot_buf->Put(csindex); ot_count ++;
	}

	if ( CG_optShort() ) {
		// cs からの参照を書き換える
		for ( auto kv : *label_reference_table ) {
			int const new_ot_index = GetNewOTFromOldOT(kv.first);
			assert(new_ot_index >= 0);
			if ( kv.second != new_ot_index ) {
				//Mesf("#ラベル参照の書き換え (cs#%d, ot#%d -> %d)", kv.second, kv.first, new_ot_index);
				SetCS(kv.second, TYPE_LABEL, new_ot_index);
			}
		}
		// deffunc/defcfunc の呼び出し先を書き換える
		for ( int i = 0; i < GET_FI_SIZE(); ++i ) {
			auto const stdat = GET_FI(i);
			if ( stdat->index == STRUCTDAT_INDEX_FUNC || stdat->index == STRUCTDAT_INDEX_CFUNC ) {
				int const new_ot_index = GetNewOTFromOldOT(stdat->otindex);
				//Mesf("#ラベル参照の書き換え (stdat#%d, ot#%d -> %d)", i, stdat->otindex, new_ot_index);
				stdat->otindex = new_ot_index;
			}
		}
	}
}

int CToken::GetNewOTFromOldOT(int old_otindex)
{
	if ( !CG_optShort() ) return old_otindex;

	assert(0 <= old_otindex && old_otindex < (int)working_ot_buf->size());
	auto iter = otindex_table->find(working_ot_buf->at(old_otindex));
	return (iter != otindex_table->end()) ? iter->second : -1;
};

void CToken::PutDI( void )
{
	//		Debug code register
	//
	//		mem_di formats :
	//			0-250           = offset to old mcs
	//			252,x(16)       = big offset
	//			253,x(24),y(16) = used val (x=mds ptr.)
	//			254,x(24),y(16) = new filename accepted (x=mds ptr.)
	//			255             = end of debug data
	//
	int ofs;
	ofs=(int)( GetCS() - cg_lastcs );
	if ( ofs <= 250 ) {
		di_buf->Put( (unsigned char)ofs );
	} else {
		di_buf->Put( (unsigned char)252 );
		di_buf->Put( (unsigned char)(ofs) );
		di_buf->Put( (unsigned char)(ofs>>8) );
	}
	cg_lastcs = GetCS();
}


void CToken::PutDI( int dbg_code, int a, int subid )
{
	//		special Debug code register
	//			in : -1=end of code
	//				254=(a=file ds ptr./subid=line num.)
	//
	if (dbg_code<0) {
		di_buf->Put( (unsigned char)255 );
		di_buf->Put( (unsigned char)255 );
	} else {
		di_buf->Put( (unsigned char)dbg_code );
		di_buf->Put( (unsigned char)(a) );
		di_buf->Put( (unsigned char)(a>>8) );
		di_buf->Put( (unsigned char)(a>>16) );
		di_buf->Put( (unsigned char)(subid) );
		di_buf->Put( (unsigned char)(subid>>8) );
	}
}


void CToken::PutDIVars( void )
{
	//		Debug info register for vals
	//
	int a,i,id;
	LABOBJ *lab;
	char vtmpname[256];
	char *p;

	id = 0;
	strcpy( vtmpname, "I_" );

	for( a=0; a<lb->GetNumEntry(); a++ ) {
		lab = lb->GetLabel(a);
		if ( lab->type == TK_OBJ ) {
			switch( lab->typefix ) {
			case LAB_TYPEFIX_INT:
				vtmpname[0] = 'I';
				p = vtmpname;
				strcpy( p+2, lab->name );
				break;
			case LAB_TYPEFIX_DOUBLE:
				vtmpname[0] = 'D';
				p = vtmpname;
				strcpy( p+2, lab->name );
				break;
			case LAB_TYPEFIX_NONE:
			default:
				p = lab->name;
				break;
			}
			i = PutDSBuf( p );
			PutDI( 253, i, lab->opt );
		}
	}
}


// ラベル名の情報を出力する
// 識別子表(lb)は古い ot_index を用いて書かれているので注意
void CToken::PutDILabels( void )
{
	int const num = GetOTCount();  // old ot_index の数
	std::vector<int> table;
	table.resize(num, -1);  // 無効値で初期化

	//識別子表からラベル名を抽出し、ラベルの名前として登録する
	for (int i = 0; i < lb->GetNumEntry(); i++) {
		if (lb->GetType(i) == TYPE_LABEL) {
			int id = lb->GetOpt(i);
			table[id] = i;
		}
	}

	//ラベル名のリストを書き出す
	di_buf->Put((unsigned char)255);
	for (int i = 0; i < num; i ++) {
		if (table[i] == -1) continue;
		char *name = lb->GetName(table[i]);
		int dsPos = PutDSBuf(name);
		PutDI(251, dsPos, GetNewOTFromOldOT(i));
	}
}


// 引数名の情報を出力する
void CToken::PutDIParams( void )
{
	di_buf->Put((unsigned char)255);
	for (int i = 0; i < lb->GetNumEntry(); i++) {
		if (lb->GetType(i) == TYPE_STRUCT) {
			int id = lb->GetOpt(i);
			if (id < 0) continue;
			char *name = lb->GetName(i);
			int dsPos = PutDSBuf(name);
			PutDI(251, dsPos, id);
		}
	}
}


char *CToken::GetDS( int ptr )
{
	int i;
	char *p;
	i = ds_buf->GetSize();
	if ( ptr >= i ) return NULL;
	p = ds_buf->GetBuffer();
	p += ptr;
	return p;
}


/*
	rev 54
	mingw : warning : i は未初期化で使用されうる
	に対処。
*/

int CToken::PutLIB( int flag, char *name )
{
	int a,i = -1,p;
	LIBDAT lib;
	LIBDAT *l;
	p = li_buf->GetSize() / sizeof(LIBDAT);
	l = (LIBDAT *)li_buf->GetBuffer();

	if ( flag == LIBDAT_FLAG_DLL ) {
		if ( *name != 0 ) {
			for( a=0; a<p; a++ ) {
				if ( l->flag == flag ) {
					if ( strcmp( GetDS( l->nameidx ), name )==0 ) {
						return a;
					}
				}
				l++;
			}
			i = PutDSBuf( name );
		} else {
			i = -1;
		}
	}
	if ( flag == LIBDAT_FLAG_COMOBJ ) {
		COM_GUID guid;
		if ( ConvertIID( &guid, name ) ) return -1;
		i = PutDSBuf( (char *)&guid, sizeof(COM_GUID) );
	}

	lib.flag = flag;
	lib.nameidx = i;
	lib.hlib = NULL;
	lib.clsid = -1;
	li_buf->PutData( &lib, sizeof(LIBDAT) );
	//Mesf( "LIB#%d:%s",flag,name );

	return p;
}


void CToken::SetLIBIID( int id, char *clsid )
{
	LIBDAT *l;
	l = (LIBDAT *)li_buf->GetBuffer();
	l += id;
	if ( *clsid == 0 ) {
		l->clsid = -1;
	} else {
		l->clsid = PutDSBuf( clsid );
	}
}


int CToken::PutStructParam( short mptype, int extype )
{
	int size;
	int i;
	STRUCTPRM prm;

	i = mi_buf->GetSize() / sizeof(STRUCTPRM);

	prm.mptype = mptype;
	if ( extype == STRUCTPRM_SUBID_STID ) {
		prm.subid  = (short)GET_FI_SIZE();
	} else {
		prm.subid = extype;
	}
	prm.offset = cg_stsize;

	size = 0;
	switch( mptype ) {
	case MPTYPE_INUM:
	case MPTYPE_STRUCT:
		size = sizeof(int);
		break;
	case MPTYPE_LOCALVAR:
		size = sizeof(PVal);
		break;
	case MPTYPE_DNUM:
		size = sizeof(double);
		break;
	case MPTYPE_FLOAT:
		size = sizeof(float);
		break;
	case MPTYPE_LOCALSTRING:
	case MPTYPE_STRING:
	case MPTYPE_LABEL:
	case MPTYPE_PPVAL:
	case MPTYPE_PBMSCR:
	case MPTYPE_PVARPTR:
	case MPTYPE_IOBJECTVAR:
	case MPTYPE_LOCALWSTR:
	case MPTYPE_FLEXSPTR:
	case MPTYPE_FLEXWPTR:
	case MPTYPE_PTR_REFSTR:
	case MPTYPE_PTR_EXINFO:
	case MPTYPE_PTR_DPMINFO:
	case MPTYPE_NULLPTR:
		size = sizeof(char *);
		break;
	case MPTYPE_SINGLEVAR:
	case MPTYPE_ARRAYVAR:
		size = sizeof(MPVarData);
		break;
	case MPTYPE_MODULEVAR:
	case MPTYPE_IMODULEVAR:
	case MPTYPE_TMODULEVAR:
		size = sizeof(MPModVarData);
		break;
	default:
		return i;
	}
	cg_stsize += size;
	cg_stnum++;
	mi_buf->PutData( &prm, sizeof(STRUCTPRM) );
	return i;
}


int CToken::PutStructParamTag( void )
{
	int i;
	STRUCTPRM prm;

	i = mi_buf->GetSize() / sizeof(STRUCTPRM);

	prm.mptype = MPTYPE_STRUCTTAG;
	prm.subid  = (short)GET_FI_SIZE();
	prm.offset = -1;

	cg_stnum++;
	mi_buf->PutData( &prm, sizeof(STRUCTPRM) );
	return i;
}


void CToken::PutStructStart( void )
{
	cg_stnum = 0;
	cg_stsize = 0;
	cg_stptr = mi_buf->GetSize() / sizeof(STRUCTPRM);
}


int CToken::PutStructEnd( int i, char *name, int libindex, int otindex, int funcflag )
{
	//		STRUCTDATを登録する(モジュール用)
	//
	STRUCTDAT st;
	st.index = libindex;
	st.nameidx = PutDSBuf( name );
	st.subid = i;
	st.prmindex = cg_stptr;
	st.prmmax = cg_stnum;
	st.funcflag = funcflag;
	st.size = cg_stsize;
	if ( otindex < 0 ) {
		st.otindex = 0;
		st.subid = otindex;
	} else {
		st.otindex = otindex;
	}
	*GET_FI(i) = st;
	//Mesf( "#%d : %s(LIB%d) prm%d size%d ot%d", i, name, libindex, cg_stnum, cg_stsize, otindex );
	return i;
}


int CToken::PutStructEnd( char *name, int libindex, int otindex, int funcflag )
{
	int i = GET_FI_SIZE();
	fi_buf->PreparePtr( sizeof(STRUCTDAT) );
	return PutStructEnd( i, name, libindex, otindex, funcflag );
}

int CToken::PutStructEndDll( char *name, int libindex, int subid, int otindex )
{
	//		STRUCTDATを登録する(DLL用)
	//
	int i;
	STRUCTDAT st;
	i = GET_FI_SIZE();
	st.index = libindex;
	if ( name[0] == '*' ) {
		st.nameidx = -1;
	} else {
		st.nameidx = PutDSBuf( name );
	}
	st.subid = subid;
	st.prmindex = cg_stptr;
	st.prmmax = cg_stnum;
	st.proc = NULL;
	st.size = cg_stsize;
	st.otindex = otindex;
	fi_buf->PutData( &st, sizeof(STRUCTDAT) );
	//Mesf( "#%d : %s(LIB%d) prm%d size%d ot%d", i, name, libindex, cg_stnum, cg_stsize, otindex );
	return i;
}


void CToken::PutHPI( short flag, short option, char *libname, char *funcname )
{
	HPIDAT hpi;
	hpi.flag = flag;
	hpi.option = option;
	hpi.libname = PutDSBuf( libname );
	hpi.funcname = PutDSBuf( funcname );
	hpi.libptr = NULL;
	hpi_buf->PutData( &hpi, sizeof(HPIDAT) );
}

int CToken::GenerateCode( char *fname, char *oname, int mode )
{
	CMemBuf srcbuf;
	if ( srcbuf.PutFile( fname ) < 0 ) {
		Mes( "#No file." );
		return -1;
	}
	return GenerateCode( &srcbuf, oname, mode );
}

int CToken::GenerateCode( CMemBuf *srcbuf, char *oname, int mode )
{
	//		ファイルをHSP3Codeに展開する
	//		mode			Debug code (0=off 1=on)
	//
	int i,orgcs,res;
	int adjsize;
	CMemBuf optbuf;				// オプション文字列用バッファ
	CMemBuf bakbuf;				// プリプロセッサソース保存用バッファ

	cs_buf = new CMemBuf;
	ds_buf = new CMemBuf;
	ot_buf = new CMemBuf;
	di_buf = new CMemBuf;
	li_buf = new CMemBuf;
	fi_buf = new CMemBuf;
	mi_buf = new CMemBuf;
	fi2_buf = new CMemBuf;
	hpi_buf = new CMemBuf;

	if ( CG_optCode() ) {
		string_literal_table.reset(new std::map<std::string, int>());
		double_literal_table.reset(new std::map<double, int>());
		stack_calculator.reset(new std::decay_t<decltype(*stack_calculator)>());

		if ( CG_optShort() ) {
			otindex_table.reset(new std::decay_t<decltype(*otindex_table)>());
			label_reference_table.reset(new std::decay_t<decltype(*label_reference_table)>());
		}
	}
	working_ot_buf.reset(new std::decay_t<decltype(*working_ot_buf)>());

	bakbuf.PutStr( srcbuf->GetBuffer() );				// プリプロセッサソースを保存する

	cg_debug = mode & COMP_MODE_DEBUG;
	cg_utf8out = mode & COMP_MODE_UTF8;
	cg_putvars = hed_cmpmode & CMPMODE_PUTVARS;
	res = GenerateCodeMain( srcbuf );
	if ( res ) {
		//		エラー終了
		char tmp[512];
		CStrNote note;
		CMemBuf srctmp;
		Mesf( "%s(%d) : error %d : %s (%d行目)", cg_orgfile, cg_orgline, res, cg_geterror((CGERROR)res), cg_orgline );
		if ( cg_errline > 0 ) {
			note.Select( bakbuf.GetBuffer() );
			note.GetLine( tmp, cg_errline-1, 510 );
			Mesf( "--> %s",tmp );
		}
	} else {
		//		正常終了
		CMemBuf axbuf;
		HSPHED hsphed;
		int sz_hed, sz_opt, cs_size, ds_size, ot_size, di_size;
		int li_size, fi_size, mi_size, fi2_size, hpi_size;

		//終端命令
		if ( CG_optCode() && !cg_evaluatable ) {
			if ( CG_optInfo() ) { Mes("#終端コード省略"); }
		} else {
			orgcs = GetCS();
			PutCS(TYPE_PROGCMD, 0x11, EXFLG_1);  // stop
			i = PutOT(orgcs);
			PutCS(TYPE_PROGCMD, 0, EXFLG_1);
			PutCS(TYPE_LABEL, i, 0);
		}

		PutOTBuf();
		if ( cg_debug ) {
			PutDI();
		}
		if (( cg_debug )||( cg_putvars )) {
			PutDIVars();
		}
		if ( cg_debug ) {
			PutDILabels();
			PutDIParams();
		}
		PutDI( -1, 0, 0 );								// デバッグ情報終端

		sz_hed = sizeof(HSPHED);
		memset( &hsphed, 0, sz_hed );
		hsphed.bootoption = 0;
		hsphed.runtime = 0;

		if ( hed_option & HEDINFO_RUNTIME ) {
			optbuf.PutStr( hed_runtime );
		}
		sz_opt = optbuf.GetSize();
		if ( sz_opt ) {
			while(1) {
				adjsize = ( sz_opt + 15 ) & 0xfff0;
				if ( adjsize == sz_opt ) break;
				optbuf.Put( (char)0 );
				sz_opt = optbuf.GetSize();
			}
			hsphed.bootoption |= HSPHED_BOOTOPT_RUNTIME;
			hsphed.runtime = sz_hed;
			sz_hed += sz_opt;
		}

		//		デバッグウインドゥ表示
		if ( mode & COMP_MODE_DEBUGWIN ) hsphed.bootoption |= HSPHED_BOOTOPT_DEBUGWIN;
		//		起動オプションの設定
		if (hed_autoopt_timer >= 0) {
			// awaitが使用されていない場合はマルチメディアタイマーを無効にする(自動設定)
			if (hed_autoopt_timer == 0) hsphed.bootoption |= HSPHED_BOOTOPT_NOMMTIMER;
		} else {
			// 設定されたオプションに従ってマルチメディアタイマーを無効にする
			if (hed_option & HEDINFO_NOMMTIMER) hsphed.bootoption |= HSPHED_BOOTOPT_NOMMTIMER;
		}

		if (hed_option & HEDINFO_NOGDIP) hsphed.bootoption |= HSPHED_BOOTOPT_NOGDIP;		// GDI+による描画を無効にする
		if (hed_option & HEDINFO_FLOAT32) hsphed.bootoption |= HSPHED_BOOTOPT_FLOAT32;		// 実数を32bit floatとして処理する
		if (hed_option & HEDINFO_ORGRND) hsphed.bootoption |= HSPHED_BOOTOPT_ORGRND;		// 標準の乱数発生を使用する

		cs_size = cs_buf->GetSize();
		ds_size = ds_buf->GetSize();
		ot_size = ot_buf->GetSize();
		di_size = di_buf->GetSize();

		li_size = li_buf->GetSize();
		fi_size = fi_buf->GetSize();
		mi_size = mi_buf->GetSize();
		fi2_size = fi2_buf->GetSize();
		hpi_size = hpi_buf->GetSize();

		hsphed.h1='H';
		hsphed.h2='S';
		hsphed.h3='P';
		hsphed.h4='3';
		hsphed.version = 0x0350;						// version3.5
		hsphed.max_val = cg_valcnt;						// max count of VAL Object
		hsphed.allsize  = sz_hed + cs_size + ds_size + ot_size + di_size;
		hsphed.allsize += li_size + fi_size + mi_size + fi2_size + hpi_size;

		hsphed.pt_cs = sz_hed;							// ptr to Code Segment
		hsphed.max_cs = cs_size;						// size of CS
		hsphed.pt_ds = sz_hed + cs_size;				// ptr to Data Segment
		hsphed.max_ds = ds_size;						// size of DS
		hsphed.pt_ot = hsphed.pt_ds + ds_size;			// ptr to Object Temp
		hsphed.max_ot = ot_size;						// size of OT
		hsphed.pt_dinfo = hsphed.pt_ot + ot_size;		// ptr to Debug Info
		hsphed.max_dinfo = di_size;						// size of DI

		hsphed.pt_linfo = hsphed.pt_dinfo + di_size;	// ptr to Debug Info
		hsphed.max_linfo = li_size;						// size of LINFO

		hsphed.pt_finfo = hsphed.pt_linfo + li_size;	// ptr to Debug Info
		hsphed.max_finfo = fi_size;						// size of FINFO

		hsphed.pt_minfo = hsphed.pt_finfo + fi_size;	// ptr to Debug Info
		hsphed.max_minfo = mi_size;						// size of MINFO

		hsphed.pt_finfo2 = hsphed.pt_minfo + mi_size;	// ptr to Debug Info
		hsphed.max_finfo2 = fi2_size;					// size of FINFO2

		hsphed.pt_hpidat = hsphed.pt_finfo2 + fi2_size;	// ptr to Debug Info
		hsphed.max_hpi = hpi_size;						// size of HPIDAT
		hsphed.max_varhpi = cg_varhpi;					// Num of Vartype Plugins

		hsphed.pt_sr = sizeof(HSPHED);					// ptr to Option Segment
		hsphed.max_sr = sz_opt;							// size of Option Segment
		hsphed.opt1 = 0;
		hsphed.opt2 = 0;


		axbuf.PutData( &hsphed, sizeof(HSPHED) );
		if ( sz_opt ) axbuf.PutData( optbuf.GetBuffer(), sz_opt );

		if ( cs_size ) axbuf.PutData( cs_buf->GetBuffer(), cs_size );
		if ( ds_size ) axbuf.PutData( ds_buf->GetBuffer(), ds_size );
		if ( ot_size ) axbuf.PutData( ot_buf->GetBuffer(), ot_size );
		if ( di_size ) axbuf.PutData( di_buf->GetBuffer(), di_size );

		if ( li_size ) axbuf.PutData( li_buf->GetBuffer(), li_size );
		if ( fi_size ) axbuf.PutData( fi_buf->GetBuffer(), fi_size );
		if ( mi_size ) axbuf.PutData( mi_buf->GetBuffer(), mi_size );
		if ( fi2_size ) axbuf.PutData( fi2_buf->GetBuffer(), fi2_size );
		if ( hpi_size ) axbuf.PutData( hpi_buf->GetBuffer(), hpi_size );

		res = axbuf.SaveFile( oname );
		if ( res<0 ) {
#ifdef JPNMSG
			Mes( "#出力ファイルを書き込めません" );
#else
			Mes( "#Can't write output file." );
#endif
		} else {
			int n_mod, n_hpi;
			n_hpi = hpi_buf->GetSize() / sizeof(HPIDAT);
			n_mod = fi_buf->GetSize() / sizeof(STRUCTDAT);
			Mesf( "#Code size (%d) String data size (%d) param size (%d)",cs_size,ds_size,mi_buf->GetSize() );
			Mesf( "#Vars (%d) Labels (%d) Modules (%d) Libs (%d) Plugins (%d)",cg_valcnt,ot_size>>2,n_mod,li_size,n_hpi );
			Mesf( "#No error detected. (total %d bytes)",hsphed.allsize );
			res = 0;
		}

#ifdef HSPINSPECT
		if ( hed_cmpmode & CMPMODE_AXIOUT ) {
			InspectAxCode();
		}
#endif
	}

	delete hpi_buf;
	delete fi2_buf;
	delete mi_buf;
	delete fi_buf;
	delete li_buf;

	delete di_buf;
	delete ot_buf;
	delete ds_buf;
	delete cs_buf;

	return res;
}

