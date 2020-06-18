#pragma once
#include <iostream>
#include <windows.h>
#include <system_error>
#include <wincrypt.h>
#include <string>
#include <codecvt>
#include <locale>
#include <stdexcept>

using namespace std;

class interfac
{
private:
	int collog;
	int colpas;
	wchar_t *log;
	wchar_t *pas;
	string login;
	string password;
	void crypt();
	string ValidLogin(string& text);
	string ValidPassword(string& text);
public:
	interfac(string logg, string pass);
	~interfac();
	bool Proverka(int CrPr);
	string Create();
	string Delete();
	string Change();
};

class cipher_error : public std::invalid_argument {
public:
	explicit cipher_error(const std::string& what_arg) :
		std::invalid_argument(what_arg) {}
	explicit cipher_error(const char* what_arg) :
		std::invalid_argument(what_arg) {}
};