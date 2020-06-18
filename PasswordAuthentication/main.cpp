#include <iostream>
#include <system_error>
#include <cctype>
#include "interface.h"

using namespace std;

int main(int VuborMenu = 0)
{
	/*locale loc("ru_RU.UTF-8");
	locale::global(loc);*/
	setlocale(LC_ALL, "ru");
	string login, password;
	try
	{
		while (true)
		{

			cout << "Меню программы:\n1) Создание учетной записи.\n2) Аутентификация.\n3) Удаление учетной записи.\n4) Изменение пароля.\n0) Завершение программы.\nВведите номер пункта:  ";
			cin >> VuborMenu;
			if (VuborMenu == 0)
				return 0;
			cout << "Введите логин и пароль:\t";
			cin >> login >> password;
			interfac Polsovatel(login, password);

			switch (VuborMenu)
			{
			case 1:
				cout << Polsovatel.Create() << endl;
				break;
			case 2:
				if (!Polsovatel.Proverka(1))
					cout << "Нет.\n" << endl;
				else
					cout << endl;
				break;
			case 3:
				cout << Polsovatel.Delete() << endl;
				break;
			case 4:
				cout << Polsovatel.Change() << endl;
				break;
			}
		}
	}
	catch (const cipher_error& s)
	{
		cerr << "Ошибка: " << s.what() << endl;
	}
}