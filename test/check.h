#pragma once
#include<string>
namespace chk
{
	void isTrue(bool flag);
	void isTrue(bool flag, std::wstring message);
	void isTrue(bool flag, std::string  message);
}

//#ifdef __cplusplus
//extern "C" {
//#endif  /* __cplusplus */
//
//	_CRTIMP void __cdecl _wassert(_In_z_ const wchar_t * _Message, _In_z_ const wchar_t *_File, _In_ unsigned _Line);
//
//#ifdef __cplusplus
//}
//#endif  /* __cplusplus */
//
//#define assert(_Expression) (void)( (!!(_Expression)) || (_wassert(_CRT_WIDE(#_Expression), _CRT_WIDE(__FILE__), __LINE__), 0) )

