#include "interface.h"

std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> codec;

HKEY hkey;
DWORD Buf = 0;
PBYTE mean = 0;

interfac::interfac(string logg, string pass)
{
	login = ValidLogin(logg);
	password = ValidPassword(pass);
}

interfac::~interfac()
{
	delete[] log;
	delete[] pas; 
}

string interfac::Create()
{
	if (Proverka(2))
	{
		RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\ChessHashes", 0, NULL, REG_OPTION_VOLATILE, KEY_WRITE, NULL, &hkey, NULL);
		if ((RegSetValueEx(hkey, log, 0, REG_SZ, (BYTE*)pas, sizeof(pas)*colpas / 2) != ERROR_SUCCESS))
			throw cipher_error(string("Не удалось создать учетную запись.\n"));
		else
			return "Вы успешно создали учетную запись.\n";
	}
	else
		throw cipher_error(string("Данный логин занят.\n"));
}

bool interfac::Proverka(int CrPr)
{
	switch (CrPr)
	{
	case 1:
		crypt();
		if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\ChessHashes", 0, KEY_QUERY_VALUE, &hkey) != ERROR_SUCCESS)
			return false;
		RegQueryValueEx(hkey, log, 0, 0, 0, &Buf);
		mean = new BYTE[Buf];
		if (RegQueryValueEx(hkey, log, 0, 0, mean, &Buf) != ERROR_SUCCESS)
			return false;
		else
		{
			int k = 0;
			for (int i = 0; i < Buf / 2; i++)
			{
				if (pas[i] != mean[k]) {
					cout << "Нет.\n";
					return false;
				}
				k = k + 2;
			}
			cout << "Да.\n";
			delete[] mean;
			RegCloseKey(hkey);
			return true;
		}
	case 2:
		crypt();
		if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\ChessHashes", 0, KEY_QUERY_VALUE, &hkey) != ERROR_SUCCESS)
			if (RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\ChessHashes", 0, NULL, REG_OPTION_VOLATILE, KEY_WRITE, NULL, &hkey, NULL) != ERROR_SUCCESS)
				return false;
			else
				return true;
		else
		{
			if (RegQueryValueEx(hkey, log, 0, 0, 0, 0) != ERROR_SUCCESS)
				return true;
			else
				return false;
		}
	}
}

string interfac::Delete()
{
	if (Proverka(1))
	{
		if (RegDeleteKeyValueW(HKEY_CURRENT_USER, L"Software\\ChessHashes", log) == ERROR_SUCCESS)
			return "Удаление учетной записи прошло успешно.\n";
		else
			throw cipher_error(string("Не удалось удалить учетную запись.\n"));
	}
	else
		return "Нет.\n";
}

string interfac::Change()
{
	if (Proverka(1))
	{
		if (RegDeleteKeyValueW(HKEY_CURRENT_USER, L"Software\\ChessHashes", log) == ERROR_SUCCESS)
		RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\ChessHashes", 0, NULL, REG_OPTION_VOLATILE, KEY_WRITE, NULL, &hkey, NULL);
		cout << "Введите новый пароль: ";
		string password1;
		cin >> password1;
		password = ValidPassword(password1);
		crypt();
		if ((RegSetValueEx(hkey,log, 0, REG_SZ, (BYTE*)pas, sizeof(pas)*colpas / 2) == ERROR_SUCCESS))
			return "Пароль успешно изменен.\n";
		else
			throw cipher_error(string( "Что-то пошло не так.\n"));
	}
	else
		return "Нет.\n";
}

void interfac::crypt()
{
	string HashStr;
	unsigned long int len = password.size();
	for (int i = 0; i < password.size(); i++)
	{
		len += (int)password[i];
	}
	static constexpr int BUF_SIZE = 1024 * 4;
	const char HEX_DIG[16] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };
	BYTE * buf = new BYTE[BUF_SIZE];
	HCRYPTPROV hCryptProv;
	HCRYPTHASH hHash;

	if (!CryptAcquireContext(&hCryptProv, nullptr, nullptr, PROV_DSS, 0))  //crypto context open?
		throw std::runtime_error("Can\'t acquire cryptocontext");
	if (!CryptCreateHash(hCryptProv, CALG_SHA, 0, 0, &hHash))               //hash handle created?
		throw std::runtime_error("Can\'t create hash");
	if (!CryptHashData(hHash, buf, len, 0))    //calc hash for data portion
		throw std::runtime_error("Can\'n calc hash");
	len = BUF_SIZE;
	if (!CryptGetHashParam(hHash, HP_HASHVAL, buf, &len, 0))
		throw std::runtime_error("Can\' get hash data");
	for (unsigned int i = 0; i < len; i++) {
		HashStr.push_back(HEX_DIG[buf[i] >> 4]);    //high forth bits
		HashStr.push_back(HEX_DIG[buf[i] & 0xf]);   //low forth bits
	}
	wstring ppas = codec.from_bytes(HashStr);
	wstring plog= codec.from_bytes(login);
	pas = new wchar_t[ppas.size()+1];
	log = new wchar_t[login.size()+1];
	colpas = ppas.size();
	for (int i = 0; i < ppas.size(); i++)
	{
		pas[i] = ppas[i];
	}
	pas[ppas.size()] = L'\0';
	for (int i = 0; i < login.size(); i++)
	{
		log[i] = login[i];
	}
	log[login.size()] = L'\0';
	CryptDestroyHash(hHash);            //destroy hash handle
	CryptReleaseContext(hCryptProv, 0);  //close crypto context
	delete[] buf;
}

string interfac::ValidPassword(string& text)
{
	if (text.empty())
		throw cipher_error(string("Пустая строка "));
	for (auto & c : text)
	{
		if ((c > 'z' || c < 'A') && (c < '0' || c>'9'))
			throw cipher_error(string("Недопустимый текст - " + text));
	}
	if (text.size()<6)
		throw cipher_error(string("Слишком короткий пароль - " + text));
	return text;
}

string interfac::ValidLogin(string& text)
{
	if (text.empty())
		throw cipher_error(string("Пустая строка "));
	for (auto & c : text)
	{
		if ((c > 'z' || c < 'A') && (c < '0' || c>'9'))
			throw cipher_error(string("Недопустимый текст - " + text));
	}
	if (text.size() < 6)
		throw cipher_error(string("Слишком короткий логин - " + text));
	return text;
}