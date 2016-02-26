#ifndef __hsp_compiler_api_h
#define __hsp_compiler_api_h

#include <Windows.h>
#include <memory>
#include <string>

typedef BOOL (CALLBACK *DLLFUNC)(int,int,int,int);

struct HspCompilerApi
{
	DLLFUNC hsc_ini;
	DLLFUNC hsc_refname;
	DLLFUNC hsc_objname;
	DLLFUNC hsc_comp;
	DLLFUNC hsc_getmes;
	DLLFUNC hsc_clrmes;
	DLLFUNC hsc_ver;
	DLLFUNC hsc_bye;
	DLLFUNC pack_ini;
	DLLFUNC pack_make;
	DLLFUNC pack_exe;
	DLLFUNC pack_opt;
	DLLFUNC pack_rt;
	DLLFUNC hsc3_getsym;
	DLLFUNC hsc3_make;
	DLLFUNC hsc3_messize;

	//3.0�p�̒ǉ�
	DLLFUNC hsc3_getruntime;
	DLLFUNC hsc3_run;
};

class HspCompilerLoader
{
public:
	HspCompilerLoader();
	~HspCompilerLoader();

	operator bool() const
	{
		return api_ && dll_ && dllflg_ == 1;
	}

	HspCompilerApi const* operator ->() const
	{
		return api_.get();
	}

private:
	DLLFUNC loadFunc(char const* name);

private:
	std::unique_ptr<HspCompilerApi> api_;

	HINSTANCE dll_; // Handle to DLL
	int dllflg_; // DLL uses flag
	std::string errmsg_;
};

#endif
