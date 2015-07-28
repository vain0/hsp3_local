
//
//		Token analysis class
//			onion software/onitama 2002/2
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>

#include "../hsp3/hsp3config.h"
#include "supio.h"
#include "token.h"
#include "label.h"
#include "tagstack.h"
#include "membuf.h"
#include "strnote.h"
#include "ahtobj.h"

#define s3size 0x8000

//-------------------------------------------------------------
//		Routines
//-------------------------------------------------------------

void CToken::Mes( char *mes )
{
	//		���b�Z�[�W�o�^
	//
	errbuf->PutStr( mes );
	errbuf->PutStr( "\r\n" );
}


void CToken::Mesf( char *format, ...)
{
	//		���b�Z�[�W�o�^
	//		(�t�H�[�}�b�g�t��)
	//
	char textbf[1024];
	va_list args;
	va_start(args, format);
	vsprintf(textbf, format, args);
	va_end(args);
	errbuf->PutStr( textbf );
	errbuf->PutStr( "\r\n" );
}


void CToken::Error( char *mes )
{
	//		�G���[���b�Z�[�W�o�^
	//
	char tmp[256];
	sprintf( tmp, "#Error:%s\r\n", mes );
	errbuf->PutStr( tmp );
}


void CToken::LineError( char *mes, int line, char const *fname )
{
	//		�G���[���b�Z�[�W�o�^(line/filename)
	//
	char tmp[256];
	sprintf( tmp, "#Error:%s in line %d [%s]\r\n", mes, line, fname );
	errbuf->PutStr( tmp );
}


void CToken::SetErrorBuf( CMemBuf *buf )
{
	//		�G���[���b�Z�[�W�o�b�t�@�o�^
	//
	errbuf = buf;
}


void CToken::SetPackfileOut( CMemBuf *pack )
{
	//		packfile�o�̓o�b�t�@�o�^
	//
	packbuf = pack;
	packbuf->PutStr( ";\r\n;\tsource generated packfile\r\n;\r\n" );
}


void CToken::SetError( char *mes )
{
	//		�G���[���b�Z�[�W���o�^
	//
	strcpy( errtmp, mes );
}


int CToken::AddPackfile( char *name, int mode )
{
	//		packfile�o��
	//			0=name/1=+name/2=other
	//
	CStrNote note;
	int i,max;
	char packadd[1024];
	char tmp[1024];
	char *s;

	strcpy( packadd, name );
	strcase( packadd );
	if ( mode<2 ) {
		note.Select( packbuf->GetBuffer() );
		max = note.GetMaxLine();
		for( i=0;i<max;i++ ) {
			note.GetLine( tmp, i );
			s = tmp;if ( *s=='+' ) s++;
			if ( tstrcmp( s, packadd )) return -1;
		}
		if ( mode==1 ) packbuf->PutStr( "+" );
	}
	packbuf->PutStr( packadd );
	packbuf->PutStr( "\r\n" );
	return 0;
}


//-------------------------------------------------------------
//		Interfaces
//-------------------------------------------------------------

CToken::CToken( void )
	: lb(new CLabel)
	, tmp_lb(nullptr)
	, tstack(new CTagStack)
	, filename_table(new std::set<std::string>())
{
	s3 = (unsigned char *)malloc( s3size );
	hed_info.cmpmode = CMPMODE_OPTCODE | CMPMODE_OPTPRM | CMPMODE_SKIPJPSPC;
	errbuf = NULL;
	packbuf = NULL;
	ahtmodel = NULL;
	ahtbuf = NULL;
	scnvbuf = NULL;
	ResetCompiler();
}


CToken::~CToken( void )
{
	if ( scnvbuf!=NULL ) InitSCNV(-1);

	free( s3 ); s3 = NULL;
//	buffer = NULL;
}


std::shared_ptr<CLabel> CToken::GetLabelInfo( void )
{
	//		���x�������o��
	return lb;
}


void CToken::SetLabelInfo( std::shared_ptr<CLabel> lbinfo )
{
	//		���x�����ݒ�
	//
	tmp_lb = lbinfo;
}


void CToken::ResetCompiler( void )
{
//	buffer = buf;
//	wp = (unsigned char *)buf;
	line = 1;
	fpbit = 256.0;
	incinf = 0;
	ppswlev = 0;
	ppswctx = { true, false, LineMode::On };
	SetModuleName( "" ); modgc = 0;
	search_path[0] = 0;
	lb->Reset();
	fileadd = 0;

	//		reset header info
	hed_info.option = 0;
	hed_info.runtime[0] = 0;
	hed_info.autoopt_timer = 0;
}


void CToken::SetAHT( AHTMODEL *aht )
{
	ahtmodel = aht;
}


void CToken::SetAHTBuffer( CMemBuf *aht )
{
	ahtbuf = aht;
}


void CToken::SetLook( char *buf )
{
	wp = (unsigned char *)buf;
}


char *CToken::GetLook( void )
{
	return (char *)wp;
}


char *CToken::GetLookResult( void )
{
	return (char *)s2;
}


int CToken::GetLookResultInt( void )
{
	return val;
}


void CToken::Pickstr( void )
{
	//		Strings pick sub
	//
	int a=0;
	unsigned char a1;
	while(1) {

pickag:
		a1=(unsigned char)*wp;
		if (a1>=0x81) {
			if (a1<0xa0) {				// s-jis code
				s3[a++]=a1;wp++;
				s3[a++]=*wp;wp++;
				continue;
			}
			else if (a1>=0xe0) {		// s-jis code2
				s3[a++]=a1;wp++;
				s3[a++]=*wp;wp++;
				continue;
			}
		}

		if (a1==0x5c) {					// '\' extra control
			wp++;a1=tolower(*wp);
			switch(a1) {
				case 'n':
					s3[a++]=13;a1=10;
					break;
				case 't':
					a1=9;
					break;
				case 'r':
					s3[a++]=13;wp++;
					goto pickag;
				case 0x22:
					s3[a++]=a1;wp++;
					goto pickag;
			}
		}
		if (a1==0) { wp=NULL;break; }
#ifdef HSPLINUX
		if (a1==10) {
			wp++;
			line++;
			break;
		}
#endif
		if (a1==13) {
			wp++;if ( *wp==10 ) wp++;
			line++;
			break;
		}
		if (a1==0x22) {
			wp++;
			if ( *wp == 0 ) wp=NULL;
			break;
		}
		s3[a++]=a1;wp++;
	}
	s3[a]=0;
}


char *CToken::Pickstr2( char *str )
{
	//		Strings pick sub '�`'
	//
	unsigned char *vs;
	unsigned char *pp;
	unsigned char a1;
	vs = (unsigned char *)str;
	pp = s3;

	while(1) {
		a1=*vs;
		if (a1==0) break;
		if (a1==0x27) { vs++; break; }
		if (a1==0x5c) {					// '\'�`�F�b�N
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
		if (a1>=129) {					// �S�p�����`�F�b�N
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

int CToken::CheckModuleName( char *name )
{
	int a;
	unsigned char a1;

	a = 0;
	auto p = (unsigned char *)name;
	while(1) {								// normal object name
		a1=*p;
		if (a1==0) { return 0; }
		if (is_mark_char(a1)) break;
		if (a1>=129) {						// �S�p�����`�F�b�N
			if (a1<=159) { p++;a1=*p; }
			else if (a1>=224) { p++;a1=*p; }
		}
		p++;
	}
	return -1;
}


int CToken::GetToken( void )
{
	//
	//	get new word from wp ( result:s3 )
	//			result : word type
	//
	int rval;
	int a,b;
	int minmode;
	unsigned char a1,a2,an;
	int fpflag;
	int *fpival;
	int ft_bak;

	if (wp==NULL) return TK_NONE;

	a = 0;
	minmode = 0;
	rval=TK_OBJ;

	wp = skip_blanks(wp, skipsJaSpaces());
	a1 = *wp;

	if (a1==0) { wp=NULL;return TK_NONE; }		// End of Source
	if (a1==13) {					// Line Break
		wp++;if (*wp==10) wp++;
		line++;
		return TK_NONE;
	}
	if (a1==10) {					// Unix Line Break
		wp++;
		line++;
		return TK_NONE;
	}

	//	Check Extra Character
	if (is_mark_char(a1)) rval=TK_NONE;

	if (a1==':' || a1 == '{' || a1 == '}') {   // multi statement
		wp++;
		return TK_SEPARATE;
	}

	if (a1=='0') {
		a2=wp[1];
		if (a2=='x') { wp++;a1='$'; }		// when hex code (0x)
		if (a2=='b') { wp++;a1='%'; }		// when bin code (0b)
	}
	if (a1=='$') {							// when hex code ($)
		wp++;val=0;
		while(1) {
			a1=toupper(*wp);b=-1;
			if (a1==0) { wp=NULL;break; }
			if (isdigit(a1)) b=a1-'0';
			if (isupper(a1)) b=a1-'A';
			if (a1=='_') b=-2;
			if (b==-1) break;
			if (b>=0) { s3[a++]=a1;val=(val<<4)+b; }
			wp++;
		}
		s3[a]=0;
		return TK_NUM;
	}

	if (a1=='%') {							// when bin code (%)
		wp++;val=0;
		while(1) {
			a1=*wp;b=-1;
			if (a1==0) { wp=NULL;break; }
			if (a1 == '0' || a1 == '1') b = a1 - '0';
			if (a1=='_') b=-2;
			if (b==-1) break;
			if (b>=0) { s3[a++]=a1;val=(val<<1)+b; }
			wp++;
		}
		s3[a]=0;
		return TK_NUM;
	}
/*
	if (a1=='-') {							// minus operator (-)
		wp++;an=*wp;
		if ((an<0x30)||(an>0x39)) {
			s3[0]=a1;s3[1]=0;
			return a1;
		}
		minmode++;
		a1=an;						// �������l�Ȃ�΂��̂܂܌p��
	}
*/		
	if (isdigit(a1)) {			// when 0-9 numerical
		fpflag = 0;
		ft_bak = 0;
		unsigned char const* wp_bak;
		while(1) {
			a1=*wp;
			if (a1==0) { wp=NULL;break; }
			if (a1=='.') {
				if ( fpflag ) {
					break;
				}
				a2=*(wp+1);
				if (!isdigit(a2)) break;
				wp_bak = wp;
				ft_bak = a;
				fpflag = 3;
				//fpflag = -1;
				s3[a++]=a1;wp++;
				continue;
			}
			if (!isdigit(a1)) break;
			s3[a++]=a1;
			wp++;
		}
		s3[a]=0;
		if ( wp != NULL ) {
			if ( *wp=='k' ) { fpflag=1;wp++; }
			if ( *wp=='f' ) { fpflag=2;wp++; }
			if ( *wp=='d' ) { fpflag=3;wp++; }
			if ( *wp=='e' ) { fpflag=4;wp++; }
		}

		if ( fpflag<0 ) {				// �����l�łȂ����́u.�v�܂łŏI���
			s3[ft_bak] = 0;
			wp = wp_bak;
			fpflag = 0;
		}

		switch( fpflag ) {
		case 0:					// �ʏ�̐���
			val=atoi_allow_overflow((char *)s3);
			if ( minmode ) val=-val;
			break;
		case 1:					// int�Œ菬��
			val_d = atof( (char *)s3 );
			val = (int)( val_d * fpbit );
			if ( minmode ) val=-val;
			break;
		case 2:					// int�`����float�l��Ԃ�
			val_f = (float)atof( (char *)s3 );
			if ( minmode ) val_f=-val_f;
			fpival = (int *)&val_f;
			val = *fpival;
			break;
		case 4:					// double�l(�w���\�L)
			s3[a++]='e';
			a1 = *wp;
			if (( a1=='-' )||( a1=='+' )) {
				s3[a++] = a1;
				wp++;
			}
			while(1) {
				a1=*wp;
				if ((a1<0x30)||(a1>0x39)) break;
				s3[a++]=a1;wp++;
			}
			s3[a]=0;
		case 3:					// double�l
			val_d = atof( (char *)s3 );
			if ( minmode ) val_d=-val_d;
			return TK_DNUM;
		}
		return TK_NUM;
	}

	if (a1==0x22) {							// when "string"
		wp++;Pickstr();
		return TK_STRING;
	}

	if (a1==0x27) {							// when 'char'
		wp++;
		wp = (unsigned char *)Pickstr2( (char *)wp );
		val=*(unsigned char *)s3;
		return TK_NUM;
	}

	if (rval==TK_NONE) {					// token code
		wp++;an=*wp;
		if (a1=='!') {
			if (an=='=') wp++;
		}
/*
		else if (a1=='<') {
			if (an=='<') { wp++;a1=0x63; }	// '<<'
			if (an=='=') { wp++;a1=0x61; }	// '<='
		}
		else if (a1=='>') {
			if (an=='>') { wp++;a1=0x64; }	// '>>'
			if (an=='=') { wp++;a1=0x62; }	// '>='
		}
*/
		else if (a1=='=') {
			if (an=='=') { wp++; }			// '=='
		}
		else if (a1=='|') {
			if (an=='|') { wp++; }			// '||'
		}
		else if (a1=='&') {
			if (an=='&') { wp++; }			// '&&'
		}
		s3[0]=a1;s3[1]=0;
		return a1;
	}

	while(1) {								// normal object name
		a1=*wp;
		if (a1==0) { wp=NULL;break; }
		if (is_mark_char(a1)) break;
		if ( a>=OBJNAME_MAX ) break;

		if (a1>=129) {						// �S�p�����`�F�b�N

#ifdef HSPWIN
			if ( hed_info.cmpmode & CMPMODE_SKIPJPSPC ) {
				if ( a1 == 0x81 ) {
					if ( wp[1] == 0x40 ) {	// �S�p�X�y�[�X�͏I�[�Ƃ��ď���
						break;
					}
				}
			}
#endif
			if (a1<=159) { s3[a++]=a1;wp++;a1=*wp; }
			else if (a1>=224) { s3[a++]=a1;wp++;a1=*wp; }
		}
		s3[a++]=a1;wp++;
	}
	s3[a]=0;
	return TK_OBJ;
}


int CToken::PeekToken( void )
{
	// �߂��̂� wp �̂݁B
	// s3, val, val_f, val_d �Ȃǂ͖߂���Ȃ�
	auto const wp_bak = wp;
	int result = GetToken();
	wp = wp_bak;
	return result;
}

//-----------------------------------------------------------------------------

void CToken::Calc_token( void )
{
	lasttoken = (char *)wp;
	ttype = GetToken();
}

void CToken::Calc_factor( CALCVAR &v )
{
	CALCVAR v1;
	int id,type;
	char *ptr_dval;
	if ( ttype==TK_NUM ) {
		v=(CALCVAR)val;
		Calc_token();
		return;
	}
	if ( ttype==TK_DNUM ) {
		v=(CALCVAR)val_d;
		Calc_token();
		return;
	}
	if ( ttype==TK_OBJ ) {
		id = lb->Search( (char *)s3 );
		if ( id == -1 ) { ttype=TK_CALCERROR; return; }
		type = lb->GetType( id );
		if ( type != LAB_TYPE_PPVAL ) { ttype=TK_CALCERROR; return; }
			ptr_dval = lb->GetData2( id );
			if ( ptr_dval == NULL ) {
				v = (CALCVAR)lb->GetOpt( id );
			} else {
				v = *(CALCVAR *)ptr_dval;
			}
		Calc_token();
		return;
	}
	if( ttype!='(' ) { ttype=TK_ERROR; return; }
	Calc_token();
	Calc_start(v1); 
	if( ttype!=')' ) { ttype=TK_CALCERROR; return; }
	Calc_token();
	v=v1;
}

void CToken::Calc_unary( CALCVAR &v )
{
	CALCVAR v1;
	int op;
	if ( ttype=='-' ) {
		op=ttype; Calc_token();
		Calc_unary(v1);
		v1 = -v1;
	} else {
		Calc_factor(v1);
	}
	v=v1;
}

void CToken::Calc_muldiv( CALCVAR &v )
{
	CALCVAR v1,v2;
	int op;
	Calc_unary(v1);
	while( (ttype=='*')||(ttype=='/')||(ttype==0x5c)) {
		op=ttype; Calc_token();
		Calc_unary(v2);
		if (op=='*') v1*=v2;
		else if (op=='/') {
			if ( (int)v2==0 ) { ttype=TK_CALCERROR; return; }
			v1/=v2;
		} else if (op==0x5c) {
			if ( (int)v2==0 ) { ttype=TK_CALCERROR; return; }
			v1 = fmod( v1, v2 );
		}
	}
	v=v1;
}

void CToken::Calc_addsub( CALCVAR &v )
{
	CALCVAR v1,v2;
	int op;
	Calc_muldiv(v1);
	while( (ttype=='+')||(ttype=='-')) {
		op=ttype; Calc_token();
		Calc_muldiv(v2);
		if (op=='+') v1+=v2;
		else if (op=='-') v1-=v2;
	}
	v=v1;
}


void CToken::Calc_compare( CALCVAR &v )
{
	CALCVAR v1,v2;
	int v1i,v2i,op;
	Calc_addsub(v1);
	while( (ttype=='<')||(ttype=='>')||(ttype=='=')) {
		op=ttype; Calc_token();
		if (op=='=') {
			Calc_addsub(v2);
			v1i = v1==v2;
			v1=(CALCVAR)v1i; continue;
		}
		if (op=='<') {
			if ( ttype=='=' ) {
				Calc_token();Calc_addsub(v2);
				v1i=(v1<=v2); v1=(CALCVAR)v1i; continue;
			}
			if ( ttype=='<' ) {
				Calc_token();Calc_addsub(v2);
				v1i = (int)v1;
				v2i = (int)v2;
				v1i<<=v2i;
				v1=(CALCVAR)v1i; continue;
			}
			Calc_addsub(v2);
			v1i=(v1<v2);
			v1=(CALCVAR)v1i; continue;
		}
		if (op=='>') {
			if ( ttype=='=' ) {
				Calc_token();Calc_addsub(v2);
				v1i=(v1>=v2);
				v1=(CALCVAR)v1i; continue;
			}
			if ( ttype=='>' ) {
				Calc_token();Calc_addsub(v2);
				v1i = (int)v1;
				v2i = (int)v2;
				v1i>>=v2i;
				v1=(CALCVAR)v1i; continue;
			}
			Calc_addsub(v2);
			v1i=(v1>v2);
			v1=(CALCVAR)v1i; continue;
		}
		v1=(CALCVAR)v1i;
	}
	v=v1;
}


void CToken::Calc_bool2( CALCVAR &v )
{
	CALCVAR v1,v2;
	int v1i,v2i;
	Calc_compare(v1);
	while( ttype=='!') {
		Calc_token();
		Calc_compare(v2);
		v1i = (int)v1;
		v2i = (int)v2;
		v1i = v1i != v2i;
		v1=(CALCVAR)v1i;
	}
	v=v1;
}


void CToken::Calc_bool( CALCVAR &v )
{
	CALCVAR v1,v2;
	int op,v1i,v2i;
	Calc_bool2(v1);
	while( (ttype=='&')||(ttype=='|')||(ttype=='^')) {
		op=ttype; Calc_token();
		Calc_bool2(v2);
		v1i = (int)v1;
		v2i = (int)v2;
		if (op=='&') v1i&=v2i;
		else if (op=='|') v1i|=v2i;
		else if (op=='^') v1i^=v2i;
		v1=(CALCVAR)v1i;
	}
	v=v1;
}


void CToken::Calc_start( CALCVAR &v )
{
	//		entry point
	Calc_bool(v);
}

int CToken::Calc( CALCVAR &val )
{
	CALCVAR v;
	Calc_token();
	Calc_start( v );
	if ( ttype == TK_CALCERROR ) {
		SetError("abnormal calculation");
		return -1;
	}
	if ( wp==NULL ) { val = v; return 0; }
	if ( *wp==0 ) { val = v; return 0; }
	SetError("expression syntax error");
	return -1;
}

//-----------------------------------------------------------------------------

bool CToken::skipsJaSpaces() const {
#ifdef HSPWIN
	return (hed_info.cmpmode & CMPMODE_SKIPJPSPC) != 0;
#else
	return false;
#endif
}

char *CToken::ExpandStr( char *str, int opt )
{
	//		�w�蕶�����membuf�֓W�J����
	//			opt:0=�s���܂ŃX�L�b�v/1="�܂�/2='�܂�
	//
	int a;
	unsigned char *vs;
	unsigned char a1;
	unsigned char sep;
	vs = (unsigned char *)str;
	a = 0;
	sep = 0;
	if (opt==1) sep=0x22;
	if (opt==2) sep=0x27;
	s3[a++]=sep;

	while(1) {
		a1=*vs;
		if (a1==0) break;
		if (a1==sep) { vs++;break; }
		if ((a1<32)&&(a1!=9)) break;
		s3[a++]=a1;vs++;
		if (a1==0x5c) {					// '\'�`�F�b�N
			s3[a++] = *vs++;
		}
		if (a1>=129) {					// �S�p�����`�F�b�N
			if ((a1<=159)||(a1>=224)) {
				s3[a++] = *vs++;
			}
		}
	}
	s3[a++]=sep;
	s3[a]=0;
	if ( opt!=0 ) {
		if (wrtbuf!=NULL) wrtbuf->PutData( s3, a );
	}
	return (char *)vs;
}


char *CToken::ExpandAhtStr( char *str )
{
	//		�R�����g��W�J����
	//		( ;;�ɑ���AHT�w�蕶����p )
	//
	unsigned char *vs;
	unsigned char a1;
	vs = (unsigned char *)str;

	while(1) {
		a1=*vs;
		if (a1==0) break;
		if ((a1<32)&&(a1!=9)) break;
		vs++;
	}
	return (char *)vs;
}


char *CToken::ExpandStrEx( char *str )
{
	//		�w�蕶�����membuf�֓W�J����
	//		( �����s�Ή� {"�`"} )
	//
	int a;
	unsigned char *vs;
	unsigned char a1;
	vs = (unsigned char *)str;
	a = 0;
	//s3[a++]=0x22;

	while(1) {
		a1=*vs;
		if (a1==0) {
			//s3[a++]=13; s3[a++]=10;
			break;
		}
		if (a1==13) {
			s3[a++]=0x5c; s3[a++]='n';
			vs++;
			if (*vs==10) { vs++; }
			continue;
		}
#ifdef HSPLINUX
		if (a1==10) {
			s3[a++]=0x5c; s3[a++]='n';
			vs++;
			continue;
		}
#endif
//		if ((a1<32)&&(a1!=9)) break;
		if (a1==0x22) {
			if (vs[1]=='}') {
				s3[a++]=0x22; s3[a++]='}';
				mulstr=LineMode::On; vs+=2; break;
			}
			s3[a++]=0x5c; s3[a++]=0x22;
			vs++;
			continue;
		}
		s3[a++]=a1;vs++;
		if (a1==0x5c) {					// '\'�`�F�b�N
			if (*vs>=32) { s3[a++] = *vs; vs++; }
		}
		if (a1>=129) {					// �S�p�����`�F�b�N
			if ((a1<=159)||(a1>=224)) {
				s3[a++] = *vs++;
			}
		}
	}
	//s3[a++]=0x22;
	s3[a]=0;
	if (wrtbuf!=NULL) { wrtbuf->PutData( s3, a ); }
	return (char *)vs;
}


char *CToken::ExpandStrComment( char *str, int opt )
{
	//		/*�`*/ �R�����g��W�J����
	//
	int a;
	unsigned char *vs;
	unsigned char a1;
	vs = (unsigned char *)str;
	a = 0;

	while(1) {
		a1=*vs;
		if (a1==0) {
			//s3[a++]=13; s3[a++]=10;
			break;
		}
		if (a1=='*') {
			if (vs[1]=='/') {
				mulstr=LineMode::On; vs+=2; break;
			}
			vs++;
			continue;
		}
		vs++;
		if (a1>=129) {					// �S�p�����`�F�b�N
			if ((a1<=159)||(a1>=224)) vs++;
		}
	}
	s3[a]=0;
	if ( opt==0 ) if (wrtbuf!=NULL) wrtbuf->PutData( s3, a );
	return (char *)vs;
}


char *CToken::ExpandHex( char *str, int *val )
{
	//		16�i���������membuf�֓W�J����
	//
	int a,b,num;
	unsigned char *vs;
	unsigned char a1;
	vs = (unsigned char *)str;

	s3[0]='$'; a=1; num=0;
	while(1) {
		a1=toupper(*vs);b=-1;
		if ((a1>=0x30)&&(a1<=0x39)) b=a1-0x30;
		if ((a1>=0x41)&&(a1<=0x46)) b=a1-55;
		if (a1=='_') b=-2;
		if (b==-1) break;
		if (b>=0) { s3[a++]=a1;num=(num<<4)+b; }
		vs++;
	}
	s3[a]=0;
	if (wrtbuf!=NULL) wrtbuf->PutData( s3, a );
	*val = num;
	return (char *)vs;
}


char *CToken::ExpandBin( char *str, int *val )
{
	//		2�i���������membuf�֓W�J����
	//
	int a,b,num;
	unsigned char *vs;
	unsigned char a1;
	vs = (unsigned char *)str;

	s3[0]='%'; a=1; num=0;
	while(1) {
			a1=*vs;b=-1;
			if ((a1>=0x30)&&(a1<=0x31)) b=a1-0x30;
			if (a1=='_') b=-2;
			if (b==-1) break;
			if (b>=0) { s3[a++]=a1;num=(num<<1)+b; }
			vs++;
		}
	s3[a]=0;
	if (wrtbuf!=NULL) wrtbuf->PutData( s3, a );
	return (char *)vs;
}


char *CToken::ExpandToken(char *str, int *type, int ppmode)
{
	//		string�f�[�^��membuf�֓W�J����
	//			ppmode : 0=�ʏ�A1=�v���v���Z�b�T��
	//
	auto vs = (unsigned char *)str;
	if ( vs == NULL ) {
		*type = TK_EOF; return NULL;			// already end
	}

	unsigned char a1 = *vs;
	if ( a1 == 0 ) {							// end
		*type = TK_EOF;
		return NULL;
	}
	if ( a1 == 10 ) {							// Unix���s
		vs++;
		if ( wrtbuf != NULL ) wrtbuf->PutStr("\r\n");
		*type = TK_EOL;
		return (char *)vs;
	}
	if ( a1 == 13 ) {							// ���s
		vs++; if ( *vs == 10 ) vs++;
		if ( wrtbuf != NULL ) wrtbuf->PutStr("\r\n");
		*type = TK_EOL;
		return (char *)vs;
	}
	if ( a1 == ';' ) {							// �R�����g
		*type = TK_VOID;
		*vs = 0;
		vs++;
		if ( *vs == ';' ) {
			vs++;
			if ( ahtmodel != NULL ) {
				ahtkeyword = (char *)vs;
			}
		}
		return ExpandStr((char *)vs, 0);
	}
	if ( a1 == '/' ) {							// C�R�����g
		if ( vs[1] == '/' ) {
			*type = TK_VOID;
			*vs = 0;
			return ExpandStr((char *)vs + 2, 0);
		}
		if ( vs[1] == '*' ) {
			mulstr = LineMode::Comment;
			*type = TK_VOID;
			*vs = 0;
			return ExpandStrComment((char *)vs + 2, 0);
		}
	}
	if ( a1 == 0x22 ) {							// "�`"
		*type = TK_STRING;
		return ExpandStr((char *)vs + 1, 1);
	}
	if ( a1 == 0x27 ) {							// '�`'
		*type = TK_STRING;
		return ExpandStr((char *)vs + 1, 2);
	}
	if ( a1 == '{' ) {							// {"�`"}
		if ( vs[1] == 0x22 ) {
			if ( wrtbuf != NULL ) wrtbuf->PutStr("{\"");
			mulstr = LineMode::String;
			*type = TK_STRING;
			return ExpandStrEx((char *)vs + 2);
		}
	}

	if ( a1 == '0' ) {
		unsigned char const a2 = vs[1];
		if ( a2 == 'x' ) { vs++; a1 = '$'; }		// when hex code (0x)
		if ( a2 == 'b' ) { vs++; a1 = '%'; }		// when bin code (0b)
	}
	if ( a1 == '$' ) {							// when hex code ($)
		int a;
		*type = TK_OBJ;
		return ExpandHex((char *)vs + 1, &a);
	}

	if ( a1 == '%' ) {							// when bin code (%)
		int a;
		*type = TK_OBJ;
		return ExpandBin((char *)vs + 1, &a);
	}

	if ( a1 < 0x30 ) {							// space,tab
		*type = TK_CODE;
		vs++;
		if ( wrtbuf != NULL ) wrtbuf->Put((char)a1);
		return (char *)vs;
	}

#ifdef HSPWIN
	if ( skipsJaSpaces() ) {
		if ( a1 == 0x81 && vs[1] == 0x40 ) {	// �S�p�X�y�[�X�𔼊p�X�y�[�X�ɕϊ�����
			*type = TK_CODE;
			vs += 2;
			if ( wrtbuf != NULL ) {
				wrtbuf->Put((char)0x20);
			}
			return (char *)vs;
		}
	}
#endif

	bool const is_mark = is_mark_char(a1);
	if ( is_mark ) {
		vs++;
		if ( wrtbuf != NULL ) wrtbuf->Put((char)a1);		// �L��
		*type = a1;
		return (char *)vs;
	}

	if ( isdigit(a1) ) {			// when 0-9 numerical
		int a = 0;
		int flcnt = 0;
		while ( 1 ) {
			a1 = *vs;
			if ( a1 == '.' ) {
				flcnt++; if ( flcnt > 1 ) break;
			} else {
				if ( (a1 < 0x30) || (a1>0x39) ) break;
			}
			s2[a++] = a1; vs++;
		}
		if ( (a1 == 'k') || (a1 == 'f') || (a1 == 'd') ) { s2[a++] = a1; vs++; }
		if ( a1 == 'e' ) {
			s2[a++] = a1; vs++;
			a1 = *vs;
			if ( (a1 == '-') || (a1 == '+') ) {
				s2[a++] = a1;
				vs++;
			}
			while ( 1 ) {
				a1 = *vs;
				if ( (a1 < 0x30) || (a1>0x39) ) break;
				s2[a++] = a1; vs++;
			}
		}

		s2[a] = 0;
		if ( wrtbuf != NULL ) wrtbuf->PutData(s2, a);
		*type = TK_OBJ;
		return (char *)vs;
	}

	/*
		if ( ppmode ) {					// �v���v���Z�b�T����#���܂߂ăL�[���[�h�Ƃ���
		s2[a++]='#';
		}
		*/

	//		���p�X�y�[�X�̌��o
	//
#ifdef HSPWIN
	if ( !skipsJaSpaces() ) {
		if ( strncmp((char *)s2, "�@", 2) == 0 ) {
			SetError("SJIS space code error");
			*type = TK_ERROR; return (char *)vs;
		}
	}
#endif


	//	 �V���{�����o��
	//
	int a = 0;
	while ( 1 ) {
		a1 = *vs;
		//if ((a1>='A')&&(a1<='Z')) a1+=0x20;		// to lower case

		if ( a1 >= 129 ) {				// �S�p�����`�F�b�N
#ifdef HSPWIN
			if ( skipsJaSpaces() && a1 == 0x81 && vs[1] == 0x40 ) {	// �S�p�X�y�[�X�͏I�[�Ɣ��f
				break;
			}
#endif
			if ( (a1 <= 159) || (a1 >= 224) ) {
				if ( a < OBJNAME_MAX ) {
					s2[a++] = a1;
					vs++;
					a1 = *vs;
					//if (a1>=32) { s2[a++] = a1; vs++; }
					s2[a++] = a1; vs++;
				} else {
					vs += 2;
				}
				continue;
			}
		}

		if ( is_mark_char(a1) ) break;
		vs++;

		//		if ( a1=='@' ) if ( *vs==0 ) {
		//			vs_modbrk = s2+a;
		//		}
		if ( a < OBJNAME_MAX ) s2[a++] = a1;

	}
	s2[a] = 0;

	if ( *s2 == '@' ) {
		if ( wrtbuf != NULL ) wrtbuf->PutData(s2, a);
		*type = TK_CODE;
		return (char *)vs;
	}

	//		�V���{������
	//
	char fixname[256];
	strcase2((char *)s2, fixname);

	//	if ( vs_modbrk != NULL ) *vs_modbrk = 0;
	FixModuleName((char *)s2);
	AddModuleName(fixname);

	int const id = lb->SearchLocal((char *)s2, fixname);
	if ( id != -1 ) {
		int errcode;
		switch ( lb->GetType(id) ) {
			case LAB_TYPE_PPVAL: {
				//		const�}�N���W�J
				char cnvstr[80];
				char* const ptr_dval = lb->GetData2(id);
				if ( ptr_dval == NULL ) {
#ifdef HSPWIN
					_itoa(lb->GetOpt(id), cnvstr, 10);
#else
					sprintf( cnvstr, "%d", lb->GetOpt(id) );
#endif
				} else {
#ifdef HSPWIN
					_gcvt(*(CALCVAR *)ptr_dval, 64, cnvstr);
#else
					sprintf( cnvstr, "%f", *(CALCVAR *)ptr_dval );
#endif
				}
				errcode = ReplaceLineBuf(str, (char *)vs, cnvstr, 0, NULL);
				break;
			}
			case LAB_TYPE_PPINTMAC:
				//		�����}�N��
				//
				if ( ppmode ) {			//	�v���v���Z�b�T���͂��̂܂ܓW�J
					if ( wrtbuf != NULL ) {
						FixModuleName((char *)s2);
						wrtbuf->PutStr((char *)s2);
					}
					*type = TK_OBJ;
					return (char *)vs;
				}
				// fall through
			case LAB_TYPE_PPMAC: {
				//		�}�N���W�J
				//
				auto const vs_bak = skip_blanks(vs, false);
				a1 = *vs_bak;
				int const opt = lb->GetOpt(id);
				if ( (a1 == '=') && ((opt & PRM_MASK) != 0) ) { // �}�N���ɑ�����悤�Ƃ����ꍇ�̃G���[
					SetError("Reserved word syntax error");
					*type = TK_ERROR; return (char *)vs;
				}
				char* const macptr = lb->GetData(id);  // maybe nullptr
				errcode = ReplaceLineBuf(str, (char *)vs, macptr, opt, (MACDEF *)lb->GetData2(id));
				break;
			}
			case LAB_TYPE_PPDLLFUNC:
				//		���W���[�����t���W�J�L�[���[�h
				if ( wrtbuf != NULL ) {
					//				AddModuleName( (char *)s2 );
					if ( lb->GetEternal(id) ) {
						FixModuleName((char *)s2);
						wrtbuf->PutStr((char *)s2);
					} else {
						wrtbuf->PutStr(fixname);
					}
				}
				*type = TK_OBJ;
				if ( *modname == 0 ) {
					lb->AddReference(id);
				} else {
					int i;
					i = lb->Search(GetModuleName());
					if ( lb->SearchRelation(id, i) == 0 ) {
						lb->AddRelation(id, i);
					}
				}
				return (char *)vs;

			case LAB_TYPE_COMVAR:
				//		COM�L�[���[�h��W�J
				if ( wrtbuf != NULL ) {
					if ( lb->GetEternal(id) ) {
						FixModuleName((char *)s2);
						wrtbuf->PutStr((char *)s2);
					} else {
						wrtbuf->PutStr(fixname);
					}
				}
				*type = TK_OBJ;
				lb->AddReference(id);
				return (char *)vs;

			case LAB_TYPE_PPMODFUNC:
			default:
				//		�ʏ�L�[���[�h�͂��̂܂ܓW�J
				if ( wrtbuf != NULL ) {
					FixModuleName((char *)s2);
					wrtbuf->PutStr((char *)s2);
				}
				*type = TK_OBJ;
				lb->AddReference(id);
				return (char *)vs;
		}
		if ( errcode ) { *type = TK_ERROR; return str; }
		*type = TK_OBJ;
		return str;
	}

	//		�o�^����Ă��Ȃ��L�[���[�h��W�J
	//
	if ( wrtbuf != NULL ) {
	//		AddModuleName( (char *)s2 );
		if ( strcmp((char *)s2, fixname) ) {
			//	���Œ�`����Ă���֐��̌Ăяo���̂��߂�
			//	���W���[������@�����Ă��Ȃ����ʎq�̈ʒu���L�^����
			undefined_symbol_t sym;
			sym.pos = wrtbuf->GetSize();
			sym.len_include_modname = (int)strlen(fixname);
			sym.len = (int)strlen((char *)s2);
			undefined_symbols.push_back(sym);
		}
		wrtbuf->PutStr(fixname);
	//		wrtbuf->Put( '?' );
	}
	*type = TK_OBJ;
	return (char *)vs;
}


char *CToken::SkipLine( char *str, int *pline )
{
	//		str������s�܂ł��X�L�b�v����
	//		( �s���Ɂu\�v�Ŏ��s��ڑ� )
	//
	unsigned char *vs;
	unsigned char a1;
	unsigned char a2;
	vs = (unsigned char *)str;
	a2=0;
	while(1) {
		a1=*vs;
		if (a1==0) break;
		if (a1==13) {
			pline[0]++;
			vs++;if ( *vs==10 ) vs++;
			if ( a2!=0x5c ) break;
			continue;
		}
		if (a1==10) {
			pline[0]++;
			vs++;
			if ( a2!=0x5c ) break;
			continue;
		}
		if ((a1<32)&&(a1!=9)) break;
		vs++;a2=a1;
	}
	return (char *)vs;
}


char *CToken::SendLineBuf( char *str )
{
	//		�P�s���̃f�[�^��linebuf�ɓ]��
	//
	char *p;
	char *w;
	char a1;
	p = str;
	w = linebuf;
	while(1) {
		a1 = *p;if ( a1==0 ) break;
		p++;
		if ( a1 == 10 ) break;
		if ( a1 == 13 ) {
			if ( *p==10 ) p++;
			break;
		}
		*w++=a1;
	}
	*w=0;
	return p;
}


#define IS_CHAR_HEAD(str, pos) \
	is_sjis_char_head((unsigned char *)(str), (int)((pos) - (unsigned char *)(str)))

char *CToken::SendLineBufPP( char *str, int *lines )
{
	//		�P�s���̃f�[�^��linebuf�ɓ]��
	//			(�s����'\'�͌p�� lines�ɍs����Ԃ�)
	//
	unsigned char *p;
	unsigned char *w;
	unsigned char a1,a2;
	int ln;
	p = (unsigned char *)str;
	w = (unsigned char *)linebuf;
	a2 = 0; ln =0;
	while(1) {
		a1 = *p;if ( a1==0 ) break;
		p++;
		if ( a1 == 10 ) {
			if ( a2==0x5c && IS_CHAR_HEAD(str, p - 2) ) {
				ln++; w--; a2=0; continue;
			}
			break;
		}
		if ( a1 == 13 ) {
			if ( a2==0x5c && IS_CHAR_HEAD(str, p - 2) ) {
				if ( *p==10 ) p++;
				ln++; w--; a2=0; continue;
			}
			if ( *p==10 ) p++;
			break;
		}
		*w++=a1; a2=a1;
	}
	*w=0;
	*lines = ln;
	return (char *)p;
}

#undef IS_CHAR_HEAD


char *CToken::ExpandStrComment2( char *str )
{
	//		"*/" �ŏI�[���Ă��Ȃ��ꍇ�� NULL ��Ԃ�
	//
	LineMode const mulstr_bak = mulstr;
	mulstr = LineMode::Comment;
	char *result = ExpandStrComment( str, 1 );
	if ( mulstr == LineMode::Comment ) {
		result = NULL;
	}
	mulstr = mulstr_bak;
	return result;
}

int CToken::ReplaceLineBuf(char *str1, char *str2, char const* repl, int opt, MACDEF *macdef)
{
	//		linebuf�̃L�[���[�h��u������
	//		(linetmp��j�󂵂܂�)
	//			str1 : �u���������L�[���[�h�擪(linebuf��)
	//			str2 : �u���������L�[���[�h��ptr(linebuf��)
	//			repl : �u�������L�[���[�h
	//			macopt : �}�N���Y���̐�
	//
	//		return : 0=ok/1=error
	//
	unsigned char dummy[4] = "";
	unsigned char const* prm[MacroArityMax];
	unsigned char const* prme[MacroArityMax];

	int const macopt = (opt & PRM_MASK);  // �A���e�B
	bool const ctype = ((opt & PRM_FLAG_CTYPE) != 0);

	strcpy(linetmp, str2);
	wp = (unsigned char *)linetmp;

	if ( macopt > 0 || ctype ) {
		// wp ���w���X�N���v�g����}�N�������������o��
		// i �Ԗڂ̎������ɂȂ鎚���(������)���A�͈� (prm[i], prme[i]) �ŕ\��

		int i = 0;
		unsigned char const* p = wp;
		int type = GetToken();
		if ( ctype ) {
			if ( type != '(' ) { SetError("C-Type macro syntax error"); return 4; }
			p = wp; type = GetToken();
		}
		if ( type != TK_NONE ) {
			int kakko = 0;  // () �̃l�X�g���x��
			bool is_top_of_arg = true;    // �����̓����H
			wp = (unsigned char *)p;
			prm[0] = p;

			while ( 1 ) {
				p = wp; type = GetToken();
				if ( type == ';' || type == '}'
					|| (type == '/' && *wp == '/') ) {
					type = TK_SEPARATE;
				} else if ( type == '/' && *wp == '*' ) {
					char* const start = (char *)wp - 1;
					char* const end = ExpandStrComment2(start + 2);
					if ( end == NULL ) {	// �͈̓R�����g�����̍s�܂ő����Ă���
						type = TK_SEPARATE;
					} else {
						wp = (unsigned char *)end;
					}
				}

				if ( is_top_of_arg ) {
					is_top_of_arg = false;
					prm[i] = p;
					if ( type == TK_NONE ) {
						prme[i++] = p;
						break;
					}
				}
				if ( type == TK_SEPARATE ) {
					wp = (unsigned char *)p;
					prme[i++] = wp;
					break;
				}
				if ( wp == NULL ) {
					prme[i++] = NULL; break;
				}
				if ( type == ',' && kakko == 0 ) {	// �����̋�؂�
					prme[i] = p;
					is_top_of_arg = true; i++;
				}
				if ( type == '(' ) kakko++;
				if ( type == ')' ) {
					if ( ctype && kakko == 0 ) { // ctype macro �̕���
						prme[i++] = p;
						wp = skip_blanks(p, false);
						*const_cast<unsigned char*>(wp) = ' ';		// ')'��space��
						break;
					}
					kakko--;
				}
			}
		}

		if ( i > macopt ) {
			// �umacro()�v�̏ꍇ������
			if ( !(ctype && (i == 1) && (macopt == 0) && (prm[0] == prme[0])) ) {
				SetError("too many macro parameter"); return 3;
			}
		}
		for ( ; i < macopt; i++ ) {		// �ȗ��p�����[�^��⊮
			prm[i] = prme[i] = dummy;
		}
	}

	// �ȍ~�A�}�N���̓W�J����

	char const* const last = reinterpret_cast<char const*>(wp);  // �}�N���̈��������ɂ���X�N���v�g������
	char mactmp[128];     // �W�J��̕�����̌`���Ɏg���ꎞ�o�b�t�@
	int tagid = 0x10000;  // %t... �Ŏw�肳��Ă���^�O��id
	char* w = str1;       // �u���������̐擪
	wp = reinterpret_cast<unsigned char const*>((repl ? repl : ""));

	while ( wp != nullptr ) {
		if ( w >= linetmp ) { SetError("macro buffer overflow"); return 4; }
		unsigned char const a1 = *wp++; if ( a1 == 0 ) break;
		if ( a1 != '%' ) {
			*w++ = a1;
			continue;
		}

		// �}�N������(%num)�����W�J�}�N���̓W�J
		if ( *wp == '%' ) { *w++ = '%'; wp++; continue; }  // �G�X�P�[�v�V�[�P���X
		switch ( GetToken() ) {
			case TK_OBJ: {			// ����R�}���h���x������
				char const* macbuf = "";  // �W�J��̕�������w���|�C���^ (nullptr �Ȃ��̓G���[)
				switch ( tolower(*s3)) {
					case 't': {  // %t�^�O��
						char* const tagname = reinterpret_cast<char*>(&s3[1]);
						if ( strlen(tagname) > TAGSTK_TAGSIZE ) { Mesf("#Warning: too long macro tag name (%s)", tagname); }
						tagid = tstack->GetTagID(tagname);
						break;
					}
					case 'i':
						tstack->GetTagUniqueName(tagid, mactmp);
						tstack->PushTag(tagid, mactmp);
						switch ( s3[1] ) {
							case '\0': macbuf = mactmp; break;
							case '0': break;
							default: macbuf = nullptr; break;
						}
						break;
					case 's': {
						int const id_prm = (int)(s3[1] - '0') - 1;  // %sN -> %N ���v�b�V��
						if ( !(0 <= id_prm && id_prm < macopt) ) { macbuf = nullptr; break; }
						unsigned char const* p = prm[id_prm];
						unsigned char const* endp = prme[id_prm];
						if ( p == endp ) {				// �l�ȗ���
							char* macbuf2 = macdef->data + macdef->index[id_prm];
							p = reinterpret_cast<unsigned char const*>(macbuf2);
							endp = nullptr;
						}
						char* w2 = mactmp;
						while ( p != endp ) {  // %num�}�N���W�J
							unsigned char const a1 = *p++; if ( a1 == 0 ) break;
							*w2++ = a1;
						}
						*w2 = 0;
						tstack->PushTag(tagid, mactmp);
						break;
					}
					case 'n':
						tstack->GetTagUniqueName(tagid, mactmp);
						if ( s3[1] != '\0' ) macbuf = nullptr;
						break;
					case 'p': {
						unsigned char const stklevel =
							(s3[1] == '\0' ? 0 : (s3[1] - '0'));  // 1���̐��l�̂݁A�ȗ����� 0
						if ( !(0 <= stklevel && stklevel < 10) ) { macbuf = nullptr; break; }
						macbuf = tstack->LookupTag(tagid, stklevel);
						break;
					}
					case 'o': {
						auto const buf = tstack->PopTag(tagid);
						switch ( s3[1] ) {
							case '\0': macbuf = buf; break;
							case '0': break;
							default: macbuf = nullptr; break;
						}
						break;
					}
					case 'c':
						mactmp[0] = 0x0d; mactmp[1] = 0x0a; mactmp[2] = 0;
						macbuf = mactmp;
						break;
					default:
						macbuf = nullptr;
						break;
				}
				if ( macbuf == nullptr ) {
					sprintf(mactmp, "macro syntax error (%%%s) [%s]", s3, tstack->GetTagName(tagid));
					SetError(mactmp); return 2;
				}
				while ( 1 ) {					//mactmp�W�J
					unsigned char const a1 = *macbuf++; if ( a1 == 0 ) break;
					*w++ = a1;
				}
				if ( wp != NULL ) { wp = skip_blanks(wp, false); }
				break;
			}
			case TK_NUM: {
				int const id_prm = val - 1;
				if ( !(0 <= id_prm && id_prm < macopt) ) {
					SetError("illegal macro parameter"); return 2;
				}
				unsigned char const* p = prm[id_prm];
				unsigned char const* endp = prme[id_prm];
				if ( p == endp ) {				// �l�ȗ���
					auto const macbuf = macdef->data + macdef->index[id_prm];
					if ( *macbuf == 0 ) { SetError("no default parameter"); return 5; }
					p = reinterpret_cast<unsigned char*>(macbuf);
					endp = nullptr;
				}
				while ( p != endp ) {	// %num�}�N���W�J
					unsigned char const a1 = *p++; if ( a1 == 0 ) break;
					*w++ = a1;
				}
				break;
			}
			default: { SetError("macro parameter invalid"); return 1; }
		}
	}
	*w = 0;
	if ( last != NULL ) {
		if ( w + strlen(last) + 1 >= linetmp ) { SetError("macro buffer overflow"); return 4; }
		strcpy(w, last);
	}
	return 0;
}


ppresult_t CToken::PP_SwitchStart( int sw )
{
	if ( ppswlev == 0 ) { ppswctx.is_enabled = true; ppswctx.lmode = LineMode::On; }
	if ( ppswlev >= SWSTACK_MAX ) {
		SetError("#if nested too deeply");
		return PPRESULT_ERROR;
	}
	ppswctx_stack[ppswlev] = ppswctx;
	ppswlev++;
	ppswctx.in_else = false;
	if ( ppswctx.is_enabled ) {
		ppswctx.lmode = (sw == 0) ? LineMode::Off : LineMode::On;
		mulstr = ppswctx.lmode;
		if ( mulstr == LineMode::Off ) ppswctx.is_enabled = false;
	}
	return PPRESULT_SUCCESS;
}


ppresult_t CToken::PP_SwitchEnd( void )
{
	if ( ppswlev == 0 ) {
		SetError("#endif without #if");
		return PPRESULT_ERROR;
	}
	ppswlev--;
	ppswctx = ppswctx_stack[ppswlev];
	if ( ppswctx.is_enabled ) mulstr = ppswctx.lmode;
	return PPRESULT_SUCCESS;
}


ppresult_t CToken::PP_SwitchReverse( void )
{
	if ( ppswlev == 0 ) {
		SetError("#else without #if");
		return PPRESULT_ERROR;
	}
	if ( ppswctx.in_else ) {
		SetError("#else after #else");
		return PPRESULT_ERROR;
	}
	if ( ppswctx_stack[ppswlev - 1].is_enabled ) {	// ��̃X�^�b�N���L���̂Ƃ��̂ݗL����
		PPSwitchCtx const new_ctx = {
			!ppswctx.is_enabled,
			true,
			((ppswctx.lmode == LineMode::On) ? LineMode::Off : LineMode::On)
		};
		ppswctx = new_ctx;
		mulstr = new_ctx.lmode;
	}
	return PPRESULT_SUCCESS;
}


ppresult_t CToken::PP_IncludeImpl( bool is_addition )
{
	char* const word = (char *)s3;
	int add_bak;
	if ( GetToken() != TK_STRING ) {
		if ( is_addition ) {
			SetError("invalid addition suffix");
		} else {
			SetError("invalid include suffix");
		}
		return PPRESULT_ERROR;
	}

	incinf++;
	if ( incinf > INCLUDE_LEVEL_MAX ) {
		SetError("too many include level");
		return PPRESULT_ERROR;
	}
	RegistExtMacro("__include_level__", incinf);

	char tmp_spath[HSP_MAX_PATH];
	strcpy( tmp_spath, search_path );
	if ( is_addition ) add_bak = SetAdditionMode( 1 );
	int const res = ExpandFile( wrtbuf, word, word );
	if ( is_addition ) SetAdditionMode( add_bak );
	strcpy( search_path, tmp_spath );

	incinf--;
	if (res) {
		if ( is_addition && res == -1 ) return PPRESULT_SUCCESS;
		return PPRESULT_ERROR;
	}
	RegistExtMacro("__include_level__", incinf);

	return PPRESULT_INCLUDED;
}


ppresult_t CToken::PP_Const(void)
{
	//		#const���
	//
	enum class ConstType { Indeterminate, Double, Int }
		valuetype = ConstType::Indeterminate;
	int glmode = 0;

	char* const word = (char *)s3;
	if ( GetToken() != TK_OBJ ) {
		char strtmp[512];
		sprintf( strtmp,"invalid symbol [%s]", word );
		SetError( strtmp ); return PPRESULT_ERROR;
	}

	strcase( word );
	if (tstrcmp(word,"global")) {		// global macro
		if ( GetToken() != TK_OBJ ) {
			SetError( "bad global syntax" ); return PPRESULT_ERROR;
		}
		glmode=1;
		strcase( word );
	}

	//type-determinant keyword
	if ( tstrcmp(word, "double") ) { valuetype = ConstType::Double; }
	else if ( tstrcmp(word, "int") ) { valuetype = ConstType::Int; }
	if (valuetype != ConstType::Indeterminate) {
		if ( GetToken() != TK_OBJ ) {
			SetError( "bad #const syntax" ); return PPRESULT_ERROR;
		}
		strcase(word);
	}

	char keyword[256];
	strcpy( keyword, word );
	if ( glmode ) FixModuleName( keyword ); else AddModuleName( keyword );
	int const res = lb->Search( keyword );
	if ( res != -1 ) { return SetErrorSymbolOverloading(keyword, res); }
	
	CALCVAR cres;
	if ( Calc(cres) ) return PPRESULT_ERROR;

	//		AHT keyword check
	if ( ahtkeyword != NULL ) {

		if ( ahtbuf != NULL ) {						// AHT�o�͎�
			AHTPROP *prop;
			CALCVAR dbval;
			prop = ahtmodel->GetProperty( keyword );
			if ( prop != NULL ) {
				int const id = lb->Regist( keyword, LAB_TYPE_PPVAL, prop->GetValueInt() );
				if ( cres != floor( cres ) ) {
					dbval = prop->GetValueDouble();
					lb->SetData2( id, (char *)(&dbval), sizeof(CALCVAR) );
				}
				if ( glmode ) lb->SetEternal( id );
				return PPRESULT_SUCCESS;
			}
		} else {									// AHT�ǂݏo����
			if ( cres != floor( cres ) ) {
				ahtmodel->SetPropertyDefaultDouble( keyword, (double)cres );
			} else {
				ahtmodel->SetPropertyDefaultInt( keyword, (int)cres );
			}
			if ( ahtmodel->SetAHTPropertyString( keyword, ahtkeyword ) ) {
				SetError( "AHT parameter syntax error" ); return PPRESULT_ERROR;
			}
		}
	}


	int const id = lb->Regist( keyword, LAB_TYPE_PPVAL, (int)cres );
	if ( valuetype == ConstType::Double || (valuetype == ConstType::Indeterminate && cres != floor( cres )) ) {
		lb->SetData2( id, (char *)(&cres), sizeof(CALCVAR) );
	}
	if ( glmode ) lb->SetEternal( id );

	return PPRESULT_SUCCESS;
}


ppresult_t CToken::PP_Enum( void )
{
	//		#enum���
	//
	int glmode = 0;
	char* const word = (char *)s3;
	if ( GetToken() != TK_OBJ ) {
		char strtmp[512];
		sprintf( strtmp,"invalid symbol [%s]", word );
		SetError( strtmp ); return PPRESULT_ERROR;
	}

	strcase( word );
	if (tstrcmp(word,"global")) {		// global macro
		if ( GetToken() != TK_OBJ ) {
			SetError( "bad global syntax" ); return PPRESULT_ERROR;
		}
		glmode=1;
		strcase( word );
	}

	char keyword[256];
	strcpy( keyword, word );
	if ( glmode ) FixModuleName( keyword ); else AddModuleName( keyword );
	int const label_id = lb->Search( keyword );
	if ( label_id != -1 ) { return SetErrorSymbolOverloading(keyword, label_id); }

	if ( GetToken() == '=' ) {
		CALCVAR cres;
		if ( Calc( cres ) ) return PPRESULT_ERROR;
		enumgc = (int)cres;
	}

	int const value = enumgc++;
	int const id = lb->Regist(keyword, LAB_TYPE_PPVAL, value);
	if ( glmode ) lb->SetEternal( id );
	return PPRESULT_SUCCESS;
}


/*
	rev 54
	mingw : warning : ��r�͏�Ɂc
	�ɑΏ��B
*/

char *CToken::CheckValidWord( void )
{
	//		�s���܂łɃR�����g�����邩���ׂ�
	//			( return : �L��������̐擪�|�C���^ )
	//
	char *res;
	char *p;
	char *p2;
	unsigned char a1;
	int qqflg, qqchr;
	res = (char *)wp;
	if ( res == NULL ) return res;
	qqflg = 0;
	p = res;
	while(1) {
		a1 = *p;
		if ( a1==0 ) break;


		if ( qqflg==0 ) {						// �R�����g�����t���O
		
			if ( a1==0x22 ) { qqflg=1; qqchr=a1; }
			if ( a1==0x27 ) { qqflg=1; qqchr=a1; }
			if ( a1==';' ) {						// �R�����g
				*p = 0; break;
			}
			if ( a1=='/' ) {						// C�R�����g
				if (p[1]=='/') {
					*p = 0; break;
				}
				if (p[1]=='*') {
					mulstr = LineMode::Comment;
					p2 = ExpandStrComment( (char *)p+2, 1 );
					while(1) {
						if ( p>=p2 ) break;
						*p++=' ';			// �R�����g������space��
					}
					continue;
				}
			}
		} else {								// �����񒆂̓R�����g��������
			if (a1==0x5c) {							// '\'�`�F�b�N
				p++; a1 = *p;
				if ( a1>=32 ) p++;
				continue;
			}
			if ( a1==qqchr ) qqflg=0;
		}
		
		if (a1>=129) {					// �S�p�����`�F�b�N
			if ((a1<=159)||(a1>=224)) {
				p++;
			}
		}
		p++;
	}
	return res;
}


ppresult_t CToken::PP_Define(void)
{
	//		#define���
	//
	char strtmp[512];

	int glmode = 0;
	bool ctype = false;
	char* const word = (char *)s3;
	if ( GetToken() != TK_OBJ ) {
		sprintf(strtmp, "invalid symbol [%s]", word);
		SetError(strtmp); return PPRESULT_ERROR;
	}

	strcase(word);
	if ( tstrcmp(word, "global") ) {		// global macro
		if ( GetToken() != TK_OBJ ) {
			SetError("bad macro syntax"); return PPRESULT_ERROR;
		}
		glmode = 1;
		strcase(word);
	}
	if ( tstrcmp(word, "ctype") ) {		// C-type macro
		if ( GetToken() != TK_OBJ ) {
			SetError("bad macro syntax"); return PPRESULT_ERROR;
		}
		ctype = true;
		strcase(word);
	}
	char keyword[256];
	strcpy(keyword, word);
	if ( glmode ) FixModuleName(keyword); else AddModuleName(keyword);
	{
		int const label_id = lb->Search(keyword);
		if ( label_id != -1 ) { return SetErrorSymbolOverloading(keyword, label_id); }
	}

	if ( wp == nullptr || *wp != '(' ) { // no parameters
		char* wdata = CheckValidWord();

		//		AHT keyword check
		if ( ahtkeyword != NULL ) {
			if ( ahtbuf != NULL ) {						// AHT�o�͎�
				AHTPROP *prop;
				prop = ahtmodel->GetProperty(keyword);
				if ( prop != NULL ) wdata = prop->GetOutValue();
			} else {									// AHT�ǂݍ��ݎ�
				AHTPROP *prop;
				prop = ahtmodel->SetPropertyDefault(keyword, wdata);
				if ( ahtmodel->SetAHTPropertyString(keyword, ahtkeyword) ) {
					SetError("AHT parameter syntax error"); return PPRESULT_ERROR;
				}
				if ( prop->ahtmode & AHTMODE_OUTPUT_RAW ) {
					ahtmodel->SetPropertyDefaultStr(keyword, wdata);
				}
			}
		}

		int const id = lb->Regist(keyword, LAB_TYPE_PPMAC, (ctype ? PRM_FLAG_CTYPE : 0));
		lb->SetData(id, wdata);
		if ( glmode ) lb->SetEternal(id);

		return PPRESULT_SUCCESS;
	}

	wp++;

	//		�p�����[�^��`�擾
	//
	MACDEF *macdef = (MACDEF *)linetmp;
	macdef->data[0] = 0;
	int macptr = 1;				// �f�t�H���g�}�N���f�[�^�Q�ƃI�t�Z�b�g
	int prms = 0; // �}�N���̉������̐� (arity)
	
	enum { ExpectingParam = 0, ExpectingDefaultOrDelimiter, ExpectingParamDefault }
		ctx = ExpectingParam;

	unsigned char a1;
	while ( 1 ) {
		if ( wp == NULL ) goto bad_macro_param_expr;
		a1 = *wp++;
		if ( a1 == ')' ) {
			if ( ctx == ExpectingParam ) goto bad_macro_param_expr;
			prms++;
			break;
		}
		switch ( a1 ) {
			case 9:
			case 32:
				break;
			case ',':
				if ( ctx == ExpectingParam ) goto bad_macro_param_expr;
				prms++; ctx = ExpectingParam;
				break;
			case '%': {
				if ( ctx != ExpectingParam ) goto bad_macro_param_expr;
				int const type = GetToken();
				if ( !(type == TK_NUM && val == (prms + 1)) ) goto bad_macro_param_expr;
				ctx = ExpectingDefaultOrDelimiter;
				macdef->index[prms] = 0;			// �f�t�H���g(�����l�Ȃ�)
				break;
			}
			case '=':
				if ( ctx != ExpectingDefaultOrDelimiter ) goto bad_macro_param_expr;
				ctx = ExpectingParamDefault;
				macdef->index[prms] = macptr;		// �����l�|�C���^�̐ݒ�
				switch ( GetToken() ) {
					case TK_NUM:
#ifdef HSPWIN
						_itoa(val, word, 10);
#else
						sprintf(word, "%d", val);
#endif
						break;
					case TK_DNUM:
						strcpy(word, (char *)s3);
						break;
					case TK_STRING:
						sprintf(strtmp, "\"%s\"", word);
						strcpy(word, strtmp);
						break;
					case TK_OBJ:
						break;
					case '-': {
						switch ( GetToken() ) {
							case TK_NUM:
								//_itoa( val, word, 10 );
								sprintf(word, "-%d", val); break;
							case TK_DNUM:
								sprintf(strtmp, "-%s", s3);
								strcpy(word, strtmp);
								break;
							default: SetError("bad default value"); return PPRESULT_ERROR;
						}
						break;
					}
					default: SetError("bad default value"); return PPRESULT_ERROR;
				}

				{
					char* const macbuf = (macdef->data) + macptr;
					size_t const len = strlen(word);
					strcpy(macbuf, word);
					macptr += len + 1;
				}
				break;
			default:
				goto bad_macro_param_expr;
		}
	}

	//		skip space,tab code
	if ( wp == NULL ) a1 = 0; else {
		wp = skip_blanks(wp, false);
		a1 = *wp;
	}
	if ( a1 == 0 ) { SetError("macro contains no data"); return PPRESULT_ERROR; }
	if ( ctype ) prms |= PRM_FLAG_CTYPE;

	//		�f�[�^��`
	int const label_id = lb->Regist(keyword, LAB_TYPE_PPMAC, prms);
	char* const wdata = CheckValidWord();
	lb->SetData(label_id, wdata);
	lb->SetData2(label_id, (char *)macdef, macptr + sizeof(macdef->index));
	if ( glmode ) lb->SetEternal(label_id);

	//sprintf( keyword,"[%d]-[%s]",id,wdata );Alert( keyword );
	return PPRESULT_SUCCESS;

bad_macro_param_expr:
	SetError("bad macro parameter expression");
	return PPRESULT_ERROR;
}



ppresult_t CToken::PP_DeffuncImpl( int mode, bool is_ctype, bool is_modfunc )
{
	//		#deffunc���
	// mode (1: modinit, 2: modterm, 0: deffunc/defcfunc/modfunc/modcfunc)
	// mode != 0 �̂Ƃ� is_ctype, is_modfunc �͎g��Ȃ��B
	char fixname[OBJNAME_MAX + 2];

	char* word = (char *)s3;
	char* mod = GetModuleName();
	char* implicit_modvar_type = nullptr; // ���W���[���ϐ����󂯎��Öق̈����^�C�v
	int id = -1; // ��`�����R�}���h�̎��ʎq�̃��x��ID
	int glmode = 0;

	if ( mode == 0 ) { // normal command
		int t = GetToken();
		if ( t == TK_OBJ ) {
			strcase( word );
			if (tstrcmp(word,"local")) {		// local option
				if ( *mod == 0 ) { SetError("module name not found"); return PPRESULT_ERROR; }
				glmode = 1;
				t = GetToken();
			}
		} else { SetError("invalid func name"); return PPRESULT_ERROR; }

		strcase2( word, fixname );
		
		int const label_id = lb->Search( fixname );
		if ( label_id != -1 ) {
			if ( lb->GetFlag(label_id) != LAB_TYPE_PP_PREMODFUNC ) {
				return SetErrorSymbolOverloading(fixname, label_id);
			}
			id = label_id;
		}

		if ( glmode ) AddModuleName( fixname );

		auto const directive_name = is_ctype ? "defcfunc" : "deffunc"; // #modfunc �� #deffunc �ŏo��
		wrtbuf->PutStrf("#%s %s ", directive_name, fixname);

		if ( id == -1 ) {
			id = lb->Regist(fixname, LAB_TYPE_PPMODFUNC, 0);
			if ( glmode == 0 ) lb->SetEternal( id );
			if ( *mod != 0 ) { lb->AddRelation( mod, id ); }		// ���W���[�����x���Ɉˑ���ǉ�
		} else {
			lb->SetFlag(id, LAB_TYPE_PPMODFUNC);
		}

		if ( is_modfunc ) {
			implicit_modvar_type = "modvar ";
		}
	} else { // ctor or dtor
		assert(mode == 1 || mode == 2);
		implicit_modvar_type = (mode == 1 ? "modinit" : "modterm");
		wrtbuf->PutStrf( "#deffunc %s ", (mode == 1 ? "__init" : "__term")  );
		strcpy_s(fixname, implicit_modvar_type);
		AddModuleName(fixname);
	}

	{
		std::shared_ptr<char> funcname_literal { to_hsp_string_literal(fixname), free };
		RegistExtMacro("__func__", funcname_literal.get());
	}

	if ( implicit_modvar_type != nullptr ) {
		if ( *mod == 0 ) { SetError("module name not found"); return PPRESULT_ERROR; }
		wrtbuf->PutStr(implicit_modvar_type);
		wrtbuf->Put(' ');
		wrtbuf->PutStr(mod);
		if ( wp != NULL ) wrtbuf->Put(',');
	}

	for(;;) {
		// �������^�C�v
		int t = GetToken();
		if ( wp == NULL ) break;
		if ( t == TK_OBJ ) {
			wrtbuf->PutStr( word );

			if ( mode == 0 && !is_ctype ) {
				strcase(word);
				if ( tstrcmp(word, "onexit") ) { // onexit�͎Q�ƍς݂ɂ���
					lb->AddReference(id);
				}
			}
		} else {
			goto deffunc_funcparam_error;
		}

		// �G�C���A�X���ʎq
		t = GetToken();
		if ( wp == NULL ) break;
		if ( t == TK_OBJ ) {
			strcase2( word, fixname );
			AddModuleName( fixname );
			wrtbuf->Put( ' ' );
			wrtbuf->PutStr( fixname );

			t = GetToken();
		}
		if ( t != ',' ) { goto deffunc_funcparam_error; }

		wrtbuf->Put( ',' );
	}

	//wrtbuf->PutStr( linebuf );
	wrtbuf->PutCR();
	return PPRESULT_WROTE_LINE;

deffunc_funcparam_error:
	SetError("invalid func param");
	return PPRESULT_ERROR;
}


ppresult_t CToken::PP_Struct( void )
{
	//		#struct���
	//
	char *word;
	int i;
	int id,res,glmode;
	char keyword[256];
	char tagname[256];
	char strtmp[0x4000];
	glmode = 0;
	word = (char *)s3;
	if ( GetToken() != TK_OBJ ) {
		sprintf( strtmp,"invalid symbol [%s]", word );
		SetError( strtmp ); return PPRESULT_ERROR;
	}

	strcase( word );
	if (tstrcmp(word,"global")) {		// global macro
		if ( GetToken() != TK_OBJ ) {
			SetError( "bad global syntax" ); return PPRESULT_ERROR;
		}
		glmode=1;
		strcase( word );
	}

	strcpy( tagname, word );
	if ( glmode ) FixModuleName( tagname ); else AddModuleName( tagname );
	res = lb->Search(tagname);
	if ( res != -1 ) { return SetErrorSymbolOverloading(tagname, res); }
	id = lb->Regist( tagname, LAB_TYPE_PPDLLFUNC, 0 );
	if ( glmode ) lb->SetEternal( id );

	wrtbuf->PutStrf( "#struct %s ",tagname );

	while(1) {

		i = GetToken();
		if ( wp == NULL ) break;
		if ( i != TK_OBJ ) { SetError("invalid struct param"); return PPRESULT_ERROR; }
		wrtbuf->PutStr( word );
		wrtbuf->Put( ' ' );

		i = GetToken();
		if ( i != TK_OBJ ) { SetError("invalid struct param"); return PPRESULT_ERROR; }

		sprintf( keyword,"%s_%s", tagname, word );
		if ( glmode ) FixModuleName( keyword ); else AddModuleName( keyword );
		res = lb->Search(keyword);
		if ( res != -1 ) { return SetErrorSymbolOverloading(keyword, res); }
		id = lb->Regist( keyword, LAB_TYPE_PPDLLFUNC, 0 );
		if ( glmode ) lb->SetEternal( id );
		wrtbuf->PutStr( keyword );

		i = GetToken();
		if ( wp == NULL ) break;
		if ( i != ',' ) {
			SetError("invalid struct param"); return PPRESULT_ERROR;
		}
		wrtbuf->Put( ',' );

	}

	wrtbuf->PutCR();
	return PPRESULT_WROTE_LINE;
}


ppresult_t CToken::PP_FuncImpl( char const *name )
{
	//		#func���
	//
	char *word = (char *)s3;
	int const t = GetToken();
	if ( t != TK_OBJ ) { SetError("invalid func name"); return PPRESULT_ERROR; }

	int glmode = 0;
	strcase( word );
	if (tstrcmp(word,"global")) {		// global macro
		if ( GetToken() != TK_OBJ ) {
			SetError( "bad global syntax" ); return PPRESULT_ERROR;
		}
		glmode=1;
	}
	if ( glmode ) FixModuleName( word ); else AddModuleName( word );

	//AddModuleName( word );
	int const i = lb->Search(word); if ( i != -1 ) { SetErrorSymbolOverloading(word, i); return PPRESULT_ERROR; }
	int const id = lb->Regist( word, LAB_TYPE_PPDLLFUNC, 0 );
	if ( glmode ) lb->SetEternal( id );
	//
	wrtbuf->PutStrf( "#%s %s%s",name, word, (char *)wp );
	wrtbuf->PutCR();
	//
	return PPRESULT_WROTE_LINE;
}


ppresult_t CToken::PP_Cmd()
{
	//		#cmd���
	//
	char *word = (char *)s3;
	int i = GetToken();
	if ( i != TK_OBJ ) { SetError("invalid func name"); return PPRESULT_ERROR; }
	i = lb->Search(word); if ( i != -1 ) { SetErrorSymbolOverloading(word, i); return PPRESULT_ERROR; }

	int const id = lb->Regist( word, LAB_TYPE_PPINTMAC, 0 );		// �����}�N���Ƃ��Ē�`
	strcat( word, "@hsp" );
	lb->SetData( id, word );
	lb->SetEternal( id );

	//AddModuleName( word );
	//id = lb->Regist( word, LAB_TYPE_PPDLLFUNC, 0 );
	//lb->SetEternal( id );
	//
	wrtbuf->PutStrf( "#cmd %s%s", word, (char *)wp );
	wrtbuf->PutCR();
	//
	return PPRESULT_WROTE_LINE;
}


ppresult_t CToken::PP_Usecom( void )
{
	//		#usecom���
	//
	int i, id;
	int glmode;
	char *word;
	word = (char *)s3;
	i = GetToken();
	if ( i != TK_OBJ ) { SetError("invalid COM symbol name"); return PPRESULT_ERROR; }

	glmode = 0;
	strcase( word );
	if (tstrcmp(word,"global")) {		// global macro
		if ( GetToken() != TK_OBJ ) {
			SetError( "bad global syntax" ); return PPRESULT_ERROR;
		}
		glmode=1;
	}

	i = lb->Search(word); if ( i != -1 ) { SetErrorSymbolOverloading(word, i); return PPRESULT_ERROR; }
	if ( glmode ) FixModuleName( word ); else AddModuleName( word );
	id = lb->Regist( word, LAB_TYPE_COMVAR, 0 );
	if ( glmode ) lb->SetEternal( id );
	//
	wrtbuf->PutStrf( "#usecom %s%s",word, (char *)wp );
	wrtbuf->PutCR();
	//
	return PPRESULT_WROTE_LINE;
}


ppresult_t CToken::PP_Module( void )
{
	//		#module���
	//
	char* const word = (char *)s3; // ���W���[������\���g�[�N��������
	bool fl = false; // ���W���[�����̎擾�ɐ����������H

	int i = GetToken();
	if (( i == TK_OBJ )||( i == TK_STRING )) fl = true;
	if ( i == TK_NONE ) { sprintf( word, "M%d", modgc ); modgc++; fl = true; }
	if ( !fl ) { SetError("invalid module name"); return PPRESULT_ERROR; }

	if ( !IsGlobalMode() ) { SetError("not in global mode"); return PPRESULT_ERROR; }
	if ( CheckModuleName( word ) ) {
		SetError("bad module name"); return PPRESULT_ERROR;
	}
	char tagname[MODNAME_MAX + 1];
	sprintf( tagname, "%.*s", MODNAME_MAX, word );
	int res = lb->Search( tagname );if ( res != -1 ) {
		SetErrorSymbolOverloading(tagname, res); return PPRESULT_ERROR;
	}
	int id = lb->Regist( tagname, LAB_TYPE_PPDLLFUNC, 0 );
	lb->SetEternal( id );
	SetModuleName( tagname );

	wrtbuf->PutStrf( "#module %s",tagname );
	wrtbuf->PutCR();
	wrtbuf->PutStrf( "goto@hsp *_%s_exit",tagname );
	wrtbuf->PutCR();
	{
		std::shared_ptr<char> tagname_literal { to_hsp_string_literal(tagname), free };
		RegistExtMacro("__module__", tagname_literal.get());
	}

	if ( PeekToken() != TK_NONE ) {
	  wrtbuf->PutStrf( "#struct %s ",tagname );
	  while(1) {
		i = GetToken();
		if ( i != TK_OBJ ) { SetError("invalid module param"); return PPRESULT_ERROR; }
		AddModuleName( word );
		res = lb->Search(word); if ( res != -1 ) {
			SetErrorSymbolOverloading(word, res); return PPRESULT_ERROR; 
		}
		id = lb->Regist( word, LAB_TYPE_PPDLLFUNC, 0 );
		wrtbuf->PutStr( "var " );
		wrtbuf->PutStr( word );

		i = GetToken();
		if ( wp == NULL ) break;
		if ( i != ',' ) {
			SetError("invalid module param"); return PPRESULT_ERROR;
		}
		wrtbuf->Put( ',' );
	  }
	  wrtbuf->PutCR();
	}

	return PPRESULT_WROTE_LINES;
}


ppresult_t CToken::PP_Global( void )
{
	//		#global���
	//
	if ( IsGlobalMode() ) { SetError("already in global mode"); return PPRESULT_ERROR; }
	//
	wrtbuf->PutStr( "#global" );
	wrtbuf->PutCR();
	wrtbuf->PutStrf( "*_%s_exit",GetModuleName() );
	wrtbuf->PutCR();
	SetModuleName("");
	RegistExtMacro("__module__", "\"\"");
	RegistExtMacro("__func__", "");
	return PPRESULT_WROTE_LINES;
}


ppresult_t CToken::PP_Aht( void )
{
	//		#aht���
	//
	int i;
	char tmp[512];
	if ( ahtmodel == NULL ) return PPRESULT_SUCCESS;
	if ( ahtbuf != NULL ) return PPRESULT_SUCCESS;					// AHT�o�͎��͖�������

	i = GetToken();
	if ( i != TK_OBJ ) {
		SetError("invalid AHT option name"); return PPRESULT_ERROR;
	}
	strcpy2( tmp, (char *)s3, 512 );
	i = GetToken();
	if (( i != TK_STRING )&&( i != TK_NUM )) {
		SetError("invalid AHT option value"); return PPRESULT_ERROR;
	}
	ahtmodel->SetAHTOption( tmp, (char *)s3 );

	return PPRESULT_SUCCESS;
}


ppresult_t CToken::PP_Ahtout( void )
{
	//		#ahtout���
	//
	if ( ahtmodel == NULL ) return PPRESULT_SUCCESS;
	if ( ahtbuf == NULL ) return PPRESULT_SUCCESS;
	if ( wp == NULL ) return PPRESULT_SUCCESS;

	ahtbuf->PutStr( (char *)wp );
	ahtbuf->PutCR();
	return PPRESULT_SUCCESS;
}


ppresult_t CToken::PP_Ahtmes( void )
{
	//		#ahtmes���
	//
	int i;
	int addprm;

	if ( ahtmodel == NULL ) return PPRESULT_SUCCESS;
	if ( ahtbuf == NULL ) return PPRESULT_SUCCESS;
	if ( wp == NULL ) return PPRESULT_SUCCESS;
	addprm = 0;

	while(1) {

		if ( wp == NULL ) break;

		i = GetToken();
		if ( i == TK_NONE ) break;
		if (( i != TK_OBJ )&&( i != TK_NUM )&&( i != TK_STRING )) {
			SetError("illegal ahtmes parameter"); return PPRESULT_ERROR;
		}
		ahtbuf->PutStr( (char *)s3 );

		if ( wp == NULL ) {	addprm = 0; break; }

		i = GetToken();
		if ( i != '+' ) { SetError("invalid ahtmes format"); return PPRESULT_ERROR; }
		addprm++;

	}
	if ( addprm == 0 ) ahtbuf->PutCR();
	return PPRESULT_SUCCESS;
}


ppresult_t CToken::PP_PackImpl( bool encrypts )
{
	//		#pack,#epack���
	//			(mode:0=normal/1=encrypt)
	if ( packbuf!=NULL ) {
		int i = GetToken();
		if ( i != TK_STRING ) {
			SetError("invalid pack name"); return PPRESULT_ERROR;
		}
		AddPackfile( (char *)s3, (encrypts ? 1 : 0) );
	}
	return PPRESULT_SUCCESS;
}


ppresult_t CToken::PP_PackOpt( void )
{
	//		#packopt���
	//
	if ( packbuf!=NULL ) {
		int i = GetToken();

		if ( i != TK_OBJ ) {
			SetError("illegal option name"); return PPRESULT_ERROR;
		}

		char optname[1024];
		strncpy( optname, (char *)s3, 128 );
		i = GetToken();
		if (( i != TK_OBJ )&&( i != TK_NUM )&&( i != TK_STRING )) {
			SetError("illegal option parameter"); return PPRESULT_ERROR;
		}

		char tmp[1024];
		sprintf( tmp, ";!%s=%s", optname, (char *)s3 );
		AddPackfile( tmp, 2 );
	}
	return PPRESULT_SUCCESS;
}

static int ParseCmpOptName(const char* optname)
{
	static char const* OptionNames[]  = { "ppout",       "optcode",      "case",       "optinfo",        "varname",       "varinit",       "optprm",       "skipjpspc",       "optcode_short",  "axiout" };
	static int         OptionValues[] = { CMPMODE_PPOUT, CMPMODE_OPTCODE, CMPMODE_CASE, CMPMODE_OPTINFO, CMPMODE_PUTVARS, CMPMODE_VARINIT, CMPMODE_OPTPRM, CMPMODE_SKIPJPSPC, CMPMODE_OPTSHORT, CMPMODE_AXIOUT };
	for ( int i = 0; i < sizeof(OptionValues) / sizeof(int); ++i ) {
		if ( tstrcmp(optname, OptionNames[i]) ) {
			return OptionValues[i];
		}
	}
	return CMPMODE_ERROR;
}

ppresult_t CToken::PP_CmpOpt( void )
{
	//		#cmpopt���
	//
	int i = GetToken();
	if ( i != TK_OBJ ) {
		SetError("illegal option name"); return PPRESULT_ERROR;
	}
	char optname[1024];
	strcase2( (char *)s3, optname );

	i = GetToken();
	if ( i != TK_NUM ) {
		SetError("illegal option parameter"); return PPRESULT_ERROR;
	}

	i = ParseCmpOptName(optname);
	if ( i == CMPMODE_ERROR ) {
		SetError("illegal option name"); return PPRESULT_ERROR;
	}

	if ( val ) {
		hed_info.cmpmode |= i;
	} else {
		hed_info.cmpmode &= ~i;
	}
	//Alertf("%s(%d)",optname,val);
	//wrtbuf->PutCR();
	return PPRESULT_SUCCESS;
}


ppresult_t CToken::PP_RuntimeOpt( void )
{
	//		#runtime���
	//
	int const i = GetToken();
	if ( i != TK_STRING ) {
		SetError("illegal runtime name"); return PPRESULT_ERROR;
	}
	strncpy(hed_info.runtime, (char *)s3, sizeof(hed_info.runtime));
	hed_info.runtime[sizeof(hed_info.runtime) - 1] = '\0';

	if ( packbuf!=NULL ) {
		char tmp[1024];
		sprintf( tmp, ";!runtime=%s.hrt", hed_info.runtime );
		AddPackfile( tmp, 2 );
	}

	hed_info.option |= HEDINFO_RUNTIME;
	return PPRESULT_SUCCESS;
}



ppresult_t CToken::PP_BootOpt(void)
{
	//		#bootopt���
	//

	int i = GetToken();
	if (i != TK_OBJ) {
		SetError("illegal option name"); return PPRESULT_ERROR;
	}
	char optname[1024];
	strcase2((char *)s3, optname);

	i = GetToken();
	if (i != TK_NUM) {
		SetError("illegal option parameter"); return PPRESULT_ERROR;
	}

	i = 0;
	if (tstrcmp(optname, "notimer")) {			// No MMTimer sw
		i = HEDINFO_NOMMTIMER;
		hed_info.autoopt_timer = -1;
	}
	if (tstrcmp(optname, "nogdip")) {			// No GDI+ sw
		i = HEDINFO_NOGDIP;
	}
	if (tstrcmp(optname, "float32")) {			// float32 sw
		i = HEDINFO_FLOAT32;
	}
	if (tstrcmp(optname, "orgrnd")) {			// standard random sw
		i = HEDINFO_ORGRND;
	}

	if (i == 0) {
		SetError("illegal option name"); return PPRESULT_ERROR;
	}

	if (val) {
		hed_info.option |= i;
	}
	else {
		hed_info.option &= ~i;
	}
	return PPRESULT_SUCCESS;
}




void CToken::PreprocessCommentCheck( char *str )
{
	int qmode = 0;
	unsigned char *vs = (unsigned char *)str;
	while(1) {
		unsigned char a1=*vs++;
		if (a1==0) break;
		if ( qmode == 0 ) {
			if (( a1 == ';' )&&( *vs == ';' )) {
				vs++;
				ahtkeyword = (char *)vs;
			}
		}
		if (a1==0x22) qmode^=1;
		if (a1>=129) {					// �S�p�����`�F�b�N
			if ((a1<=159)||(a1>=224)) {
				vs++;
			}
		}
	}
}


ppresult_t CToken::PreprocessNM( char *str )
{
	//		�v���v���Z�X�̎��s(�}�N���W�J�Ȃ�)
	//
	char *word;
	int id,type;
	ppresult_t res;
	char fixname[128];

	word = (char *)s3;
	wp = (unsigned char *)str;

	if ( ahtmodel != NULL ) {
		PreprocessCommentCheck( str );
	}

	type = GetToken();
	if ( type != TK_OBJ ) return PPRESULT_UNKNOWN_DIRECTIVE;

	//		�\�[�X�����R���g���[��
	//
	bool const is_ifdef = tstrcmp(word,"ifdef") != 0;
	bool const is_ifndef = tstrcmp(word,"ifndef") != 0;
	if ( is_ifdef || is_ifndef ) {		// generate control
		if ( mulstr == LineMode::Off ) {
			res = PP_SwitchStart( 0 );
		} else {
			res = PPRESULT_ERROR; type = GetToken();
			if ( type == TK_OBJ ) {
				strcase2( word, fixname );
				AddModuleName( fixname );
				id = lb->SearchLocal( word, fixname );

				//id = lb->Search( word );
				res = PP_SwitchStart( is_ifdef ^ (id == -1) );
			}
		}
		return res;
	}
	if (tstrcmp(word,"else")) {			// generate control
		return PP_SwitchReverse();
	}
	if (tstrcmp(word,"endif")) {		// generate control
		return PP_SwitchEnd();
	}

	//		����ȍ~��#off���Ɏ��s���܂���
	//
	if ( mulstr == LineMode::Off ) { return PPRESULT_UNKNOWN_DIRECTIVE; }

	if (tstrcmp(word,"define")) {		// keyword define
		return PP_Define();
	}

	if (tstrcmp(word,"undef")) {		// keyword terminate
		if ( GetToken() != TK_OBJ ) {
			SetError("invalid symbol");
			return PPRESULT_ERROR;
		}

		strcase2( word, fixname );
		AddModuleName( fixname );
		id = lb->SearchLocal( word, fixname );

		//id = lb->Search( word );
		if ( id >= 0 ) {
			lb->SetFlag( id, -1 );
		}
		return PPRESULT_SUCCESS;
	}

	return PPRESULT_UNKNOWN_DIRECTIVE;
}


ppresult_t CToken::Preprocess( char *str )
{
	//		�v���v���Z�X�̎��s
	//
	char *word;
	int type,a;
	ppresult_t res;
	CALCVAR cres;

	word = (char *)s3;
	wp = (unsigned char *)str;
	type = GetToken();
	if ( type != TK_OBJ ) return PPRESULT_SUCCESS;

	//		�\�[�X�����R���g���[��
	//
	if (tstrcmp(word,"if")) {			// generate control
		if ( mulstr == LineMode::Off ) {
			res = PP_SwitchStart( 0 );
		} else {
			res = PPRESULT_SUCCESS;
			if ( Calc(cres)==0 ) {
				a = (int)cres;
				res = PP_SwitchStart(a);
			} else res=PPRESULT_ERROR;
		}
		return res;
	}

	//		����ȍ~��#off���Ɏ��s���܂���
	//
	if ( mulstr == LineMode::Off ) { return PPRESULT_SUCCESS; }

	//		�R�[�h�����R���g���[��
	//
	if ( !pp_functable ) {
		pp_functable.reset(new std::decay_t<decltype(*pp_functable)> {
			{ "include",  &CToken::PP_Include },
			{ "addition", &CToken::PP_Addition },
			{ "const",    &CToken::PP_Const },
			{ "enum",     &CToken::PP_Enum },
			{ "module",   &CToken::PP_Module },
			{ "global",   &CToken::PP_Global },
			{ "deffunc",  &CToken::PP_Deffunc },
			{ "defcfunc", &CToken::PP_Defcfunc },
			{ "modfunc",  &CToken::PP_Modfunc },
			{ "modcfunc", &CToken::PP_Modcfunc },
			{ "modinit",  &CToken::PP_Modinit },
			{ "modterm",  &CToken::PP_Modterm },
			{ "struct",   &CToken::PP_Struct },
			{ "func",     &CToken::PP_Func },
			{ "cfunc",    &CToken::PP_CFunc },
			{ "cmd",      &CToken::PP_Cmd },
			{ "comfunc",  &CToken::PP_Comfunc },
			{ "aht",      &CToken::PP_Aht },
		});
	}
	auto iter = pp_functable->find(word);
	if ( iter != pp_functable->end() ) {
		return (this->*(iter->second))();

	} else {
		//		�o�^�L�[���[�h�ȊO�̓R���p�C���ɓn��
		//
		//Mesf("#not preprocessed directive (%s)", word);
		wrtbuf->Put((char)'#');
		wrtbuf->PutStr(linebuf);
		wrtbuf->PutCR();
		return PPRESULT_WROTE_LINE;
	}
}


int CToken::ExpandTokens( char *vp, CMemBuf *buf, int *lineext, int is_preprocess_line )
{
	//		�}�N����W�J
	//
	*lineext = 0;				// 1�s->�����s�Ƀ}�N���W�J���ꂽ��?
	int macloop = 0;			// �}�N���W�J�������[�v�`�F�b�N�p�J�E���^
	while(1) {
		if ( mulstr == LineMode::Off ) {			// �P�s����
			if ( wrtbuf!=NULL ) wrtbuf->PutCR();	// �s��CR/LF��ǉ�
			break;
		}

		// {"�`"}�̏���
		//
		if ( mulstr == LineMode::String ) {
			wrtbuf = buf;
			vp = ExpandStrEx( vp );
			if ( *vp!=0 ) continue;
		}

		// /*�`*/�̏���
		//
		if ( mulstr == LineMode::Comment ) {
			vp = ExpandStrComment( vp, 0 );
			if ( *vp!=0 ) continue;
		}

		char* const vp_bak = vp;
		int type;
		vp = ExpandToken( vp, &type, is_preprocess_line );
		if ( type < 0 ) {
			return type;
		}
		if ( type == TK_EOL ) { (*lineext)++; }
		if ( type == TK_EOF ) {
			if ( wrtbuf!=NULL ) wrtbuf->PutCR();	// �s��CR/LF��ǉ�
			break;
		}
		if ( vp_bak == vp ) {
			macloop++;
			if ( macloop > MACRO_LOOP_MAX ) {
				SetError("Endless macro loop");
				return -1;
			}
		}
	}
	return 0;
}

int CToken::ExpandLine( CMemBuf *buf, CMemBuf *src, char const *refname )
{
	//		string�f�[�^��membuf�֓W�J���� (�s����)
	//
	char *p = src->GetBuffer();
	int pline = 1;
	enumgc = 0;
	mulstr = LineMode::On;
	*errtmp = 0;

	while(1) {
		RegistExtMacro( "__line__", pline );			// �s�ԍ��}�N�����X�V

		p = skip_blanks(p, skipsJaSpaces());

		if ( *p==0 ) break;					// �I��(EOF)
		ahtkeyword = NULL;					// AHT�L�[���[�h�����Z�b�g����

		bool const is_preprocess_line =
			(*p == '#')
			&& mulstr != LineMode::String
			&& mulstr != LineMode::Comment;

		//		�s�f�[�^��linebuf�ɓW�J
		int mline;
		if ( is_preprocess_line ) {
			p = SendLineBufPP( p + 1, &mline );// �s���܂ł����o��('\'�p��)
			wrtbuf = NULL;
		} else {
			p = SendLineBuf( p );			// �s���܂ł����o��
			mline = 0;
			wrtbuf = buf;
		}

//		Mesf("%d:%s", pline, src->GetFileName() );
//		sprintf( mestmp,"%d:%s:%s(%d)", pline, src->GetFileName(), linebuf, is_preprocess_line );
//		Alert( mestmp );
//		buf->PutStr( mestmp );

		//		�}�N���W�J�O�ɏ��������v���v���Z�b�T
		if ( is_preprocess_line ) {
			ppresult_t const res = PreprocessNM( linebuf );
			if ( res == PPRESULT_ERROR ) {
				LineError( errtmp, pline, refname );
				return 1;
			}
			if ( res == PPRESULT_SUCCESS ) {			// �v���v���Z�b�T�ŏ������ꂽ��
				mline++;
				pline += mline;
				for (int i = 0; i < mline; i++) {
					buf->PutCR();
				}
				continue;
			}
			assert( res == PPRESULT_UNKNOWN_DIRECTIVE );
		}

//		if ( wrtbuf!=NULL ) {
//			char ss[64];
//			sprintf( ss,"__%d:",pline );
//			wrtbuf->PutStr( ss );
//		}

		//		�}�N����W�J
		int count_lines_expanded;	// �W�J��̍s��
		int const res = ExpandTokens(linebuf, buf, &count_lines_expanded, is_preprocess_line);
		if ( res ) {
			LineError( errtmp, pline, refname );
			return res;
		}

		//		�v���v���Z�b�T����
		if ( is_preprocess_line ) {
			wrtbuf = buf;
			ppresult_t res = Preprocess( linebuf );
			if ( res == PPRESULT_INCLUDED ) {			// include��̏���
				pline += 1+mline;

				std::shared_ptr<char> fname_literal { to_hsp_string_literal(refname), free };
				RegistExtMacro( "__file__", fname_literal.get() );			// �t�@�C�����}�N�����X�V

				wrtbuf = buf;
				wrtbuf->PutStrf( "##%d %s\r\n", pline-1, fname_literal );
				continue;
			}
			if ( res == PPRESULT_WROTE_LINES ) {			// �v���v���Z�X�ōs����������̏���
				pline += mline;
				wrtbuf->PutStrf( "##%d\r\n", pline );
				pline++;
				continue;
			}
			if ( res == PPRESULT_ERROR ) {
				LineError( errtmp, pline, refname );
				return 1;
			}
			pline += 1+mline;
			if ( res != PPRESULT_WROTE_LINE ) mline++;
			for (int i = 0; i < mline; i++) {
				buf->PutCR();
			}
			assert( res == PPRESULT_SUCCESS || res == PPRESULT_WROTE_LINE );
			continue;
		}

		//		�}�N���W�J��ɍs�����ς�����ꍇ�̏���
		pline += 1+mline;
		if ( count_lines_expanded != mline ) {
			wrtbuf->PutStrf( "##%d\r\n", pline );
		}
	}
	return 0;
}


int CToken::ExpandFile( CMemBuf *buf, char *fname, char const *refname )
{
	//		�\�[�X�t�@�C����membuf�֓W�J����
	// �R���p�C�����璼�ڌĂ΂��A�v���v���Z�X�����̃G���g���[�|�C���g
	// #include ����ċA�I�ɌĂ΂��

	char purename[HSP_MAX_PATH]; // fname�̃t�@�C��������
	char foldername[HSP_MAX_PATH]; // fname�̃f�B���N�g������
	CMemBuf fbuf;

	getpath( fname, purename, 8 );
	getpath( fname, foldername, 32 );
	if ( *foldername != 0 ) strcpy( search_path, foldername );

	if ( fbuf.PutFile( fname ) < 0 ) {
		char cname[HSP_MAX_PATH]; 
		strcpy( cname, common_path );strcat( cname, purename );
		if ( fbuf.PutFile( cname ) < 0 ) {
			strcpy( cname, search_path );strcat( cname, purename );
			if ( fbuf.PutFile( cname ) < 0 ) {
				strcpy( cname, common_path );strcat( cname, search_path );strcat( cname, purename );
				if ( fbuf.PutFile( cname ) < 0 ) {
					if ( fileadd == 0 ) {
						Mesf( "#Source file not found.[%s]",purename );
					}
					return -1;
				}
			}
		}
	}
	fbuf.Put( (char)0 );

	if ( fileadd ) {
		Mesf( "#Use file [%s]",purename );
	}

	std::shared_ptr<char> fname_literal { to_hsp_string_literal(refname), free };
	RegistExtMacro( "__file__", fname_literal.get() );			// �t�@�C�����}�N�����X�V

	buf->PutStrf( "##0 %s\r\n", fname_literal.get() );

	char refname_copy[HSP_MAX_PATH];
	strcpy(refname_copy, refname);
	int res = ExpandLine( buf, &fbuf, refname_copy );

	if ( res == 0 ) {
		//		�v���v���Z�X��`�F�b�N
		//
		int const count_error_tags = tstack->StackCheck( linebuf );
		if ( count_error_tags > 0 ) {
			Mesf("#%d unresolved macro(s).[%s]", count_error_tags, refname_copy);
			Mes( linebuf );
			res = -2; // fatal error
		}
	}
	
	if ( res ) {
		Mes( "#Fatal error reported." );
		return -2;
	}
	return 0;
}


int CToken::SetAdditionMode( int mode )
{
	//		Addition�ɂ��t�@�C���ǉ����[�h�ݒ�(1=on/0=off)
	//
	int i;
	i = fileadd;
	fileadd = mode;
	return i;
}


void CToken::SetCommonPath( char *path )
{
	if ( path==NULL ) { common_path[0]=0; return; }
	strcpy( common_path, path );
}


void CToken::FinishPreprocess( CMemBuf *buf )
{
	//	���Œ�`���ꂽ�֐�������ꍇ�A����ɏ���������
	//
	//	���̊֐��ł� foo@modname �� foo �ɏ���������Ȃǃo�b�t�@�T�C�Y���������Ȃ�ύX�����s��Ȃ�
	//
	int read_pos = 0;
	int write_pos = 0;
	size_t len = undefined_symbols.size();
	char *p = buf->GetBuffer();
	for ( size_t i = 0; i < len; i ++ ) {
		undefined_symbol_t sym = undefined_symbols[i];
		int pos = sym.pos;
		int len_include_modname = sym.len_include_modname;
		int len = sym.len;
		int id;
		memmove( p + write_pos, p + read_pos, pos - read_pos );
		write_pos += pos - read_pos;
		read_pos = pos;
		// @modname �����������O�̊֐������݂����炻��ɏ�������
		p[pos+len] = '\0';
		id = lb->Search( p + pos );
		if ( id >= 0 && lb->GetType(id) == LAB_TYPE_PPMODFUNC ) {
			memmove( p + write_pos, p + pos, len );
			write_pos += len;
			read_pos += len_include_modname;
		}
		p[pos+len] = '@';
	}
	memmove( p + write_pos, p + read_pos, buf->GetSize() - read_pos );
	buf->ReduceSize( buf->GetSize() - (read_pos - write_pos) );
}


int CToken::LabelRegist( char **list, int mode )
{
	//		���x������o�^
	//
	if ( mode ) {
		return lb->RegistList( list, "@hsp" );
	}
	return lb->RegistList( list, "" );
}


int CToken::LabelRegist2( char **list )
{
	//		���x������o�^(�}�N��)
	//
	return lb->RegistList2( list, "@hsp" );
}


int CToken::LabelRegist3( char **list )
{
	//		���x������o�^(�F�����p)
	//
	return lb->RegistList3( list );
}


int CToken::RegistExtMacroPath( char *keyword, char *str )
{
	//		�}�N�����O������o�^(path�p)
	//
	int id, res;
	char path[1024];
	char mm[512];
	unsigned char *p;
	unsigned char *src;
	unsigned char a1;

	p = (unsigned char *)path;
	src = (unsigned char *)str;
	while(1) {
		a1 = *src++;
		if ( a1 == 0 ) break;
		if ( a1 == 0x5c ) {	*p++=a1; }		// '\'�`�F�b�N
		if ( a1>=129 ) {					// �S�p�����`�F�b�N
			if (a1<=159) { *p++=a1;a1=*src++; }
			else if (a1>=224) { *p++=a1;a1=*src++; }
		}
		*p++ = a1;
	}
	*p = 0;	

	strcpy( mm, keyword );
	FixModuleName( mm );
	res = lb->Search( mm );if ( res != -1 ) {	// ���łɂ���ꍇ�͏㏑��
		lb->SetData( res, path );
		return -1;
	}
	//		�f�[�^��`
	id = lb->Regist( mm, LAB_TYPE_PPMAC, 0 );
	lb->SetData( id, path );
	lb->SetEternal( id );
	return 0;
}


int CToken::RegistExtMacro( char *keyword, char *str )
{
	//		�}�N�����O������o�^
	//
	int id, res;
	char mm[512];
	strcpy( mm, keyword );
	FixModuleName( mm );
	res = lb->Search( mm );if ( res != -1 ) {	// ���łɂ���ꍇ�͏㏑��
		lb->SetData( res, str );
		return -1;
	}
	//		�f�[�^��`
	id = lb->Regist( mm, LAB_TYPE_PPMAC, 0 );
	lb->SetData( id, str );
	lb->SetEternal( id );
	return 0;
}


int CToken::RegistExtMacro( char *keyword, int val )
{
	//		�}�N�����O������o�^(���l)
	//
	int id, res;
	char mm[512];
	strcpy( mm, keyword );
	FixModuleName( mm );
	res = lb->Search( mm );if ( res != -1 ) {	// ���łɂ���ꍇ�͏㏑��
		lb->SetOpt( res, val );
		return -1;
	}
	//		�f�[�^��`
	id = lb->Regist( mm, LAB_TYPE_PPVAL, val );
	lb->SetEternal( id );
	return 0;
}


int CToken::LabelDump( CMemBuf *out, int option )
{
	//		�o�^����Ă��郉�x������errbuf�ɓW�J
	//
	lb->DumpHSPLabel( linebuf, option, LINEBUF_MAX - 256 );
	out->PutStr( linebuf );
	return 0;
}


void CToken::SetModuleName( char *name )
{
	//		���W���[������ݒ�
	//
	if ( *name==0 ) {
		modname[0] = 0; return;
	}
	sprintf( modname, "@%.*s", MODNAME_MAX, name );
	strcase( modname+1 );
}


char *CToken::GetModuleName( void )
{
	//		���W���[�������擾
	//
	if ( *modname == 0 ) return modname;
	return modname+1;
}


void CToken::AddModuleName( char *str )
{
	//		�L�[���[�h�Ƀ��W���[������t��(���W���[���ˑ����x���p)
	//
	unsigned char a1;
	unsigned char *wp;
	wp=(unsigned char *)str;
	while(1) {
		a1=*wp;
		if (a1==0) break;
		if (a1=='@') {
			a1=wp[1];
			if (a1==0) *wp=0;
			return;
		}
		if (a1>=129) wp++;
		wp++;
	}
	if ( *modname==0 ) return;
	strcpy( (char *)wp, modname );
}


void CToken::FixModuleName( char *str )
{
	//		�L�[���[�h�̃��W���[�����𐳋K��(���W���[����ˑ����x���p)
	//
//	char *wp;
//	wp = str + ( strlen(str)-1 );
//	if ( *wp=='@' ) *wp=0;

	unsigned char a1;
	unsigned char *wp;
	wp=(unsigned char *)str;
	while(1) {
		a1=*wp;
		if (a1==0) break;
		if (a1=='@') {
			a1=wp[1];
			if (a1==0) *wp=0;
			return;
		}
		if (a1>=129) wp++;
		wp++;
	}
	
}


int CToken::IsGlobalMode( void )
{
	//		���W���[����(0)���A�O���[�o��(1)����Ԃ�
	//
	if ( *modname==0 ) return 1;
	return 0;
}


int CToken::GetLabelBufferSize( void )
{
	//		���x���o�b�t�@�T�C�Y�𓾂�
	//
	return lb->GetSymbolSize();
}


void CToken::InitSCNV( int size )
{
	//		�����R�[�h�ϊ��̏�����
	//		(size<0�̏ꍇ�̓�������j��)
	//
	if ( scnvbuf != NULL ) {
		free( scnvbuf );
		scnvbuf = NULL;
	}
	if ( size <= 0 ) return;
	scnvbuf = (char *)malloc(size);
	scnvsize = size;
}


char *CToken::ExecSCNV( char *srcbuf, int opt )
{
	//		�����R�[�h�ϊ�
	//
	//int ressize;
	int size;

	if ( scnvbuf == NULL ) InitSCNV( SCNVBUF_DEFAULTSIZE );

	size = (int)strlen( srcbuf );
	switch( opt ) {
	case SCNV_OPT_NONE:
		strcpy( scnvbuf, srcbuf );
		break;
	case SCNV_OPT_SJISUTF8:
#ifdef HSPWIN
		ConvSJis2Utf8( srcbuf, scnvbuf, scnvsize );
#else
		strcpy( scnvbuf, srcbuf );
#endif
		break;
	default:
		*scnvbuf = 0;
		break;
	}

	return scnvbuf;
}

ppresult_t CToken::SetErrorSymbolOverloading(char* keyword, int labelId)
{
	// ���ʎq�̑��d��`�Ɋւ���G���[

	char strtmp[0x100];
	sprintf( strtmp,"symbol in use [%s]", keyword );
	SetError(strtmp);
	return PPRESULT_ERROR;
}


