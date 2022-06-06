#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <fstream>
using namespace std;
typedef long long ll;


ll mosh = 0;
ll nraz = 0;
ll kol_simvol = 0;

string f(ll n) // получение двоичной записи из десятичного числа
{ 
	if (n)
	{
		return f(n >> 1) + to_string(n & 1);  
	}
	return "";
}
string next()  // получение следующего двоичного числа
{
	static ll nu = -1; // так как nu это static переменная, то она сохраняется призакрытии функции 
	nu++;
	string s = f(nu);
	for (ll i = s.size(); i < nraz; i++)
	{
		s = "0" + s;
	}
	return s;
}


struct myhash  // структура хэш-ячейки. Каждая ячейка имеет информационное поле kod и массив ссылок на 256 таких же хэш-таблиц
{
	vector<myhash*> v;
	string kod = "";
};
void hinit(myhash &hah) // инициализация
{
	hah.v.resize(256, NULL);
}
auto ahash(char h) // смысл хэш-таблицы в том, что для перемещению по ней используются не индексы, а все возможные символы.
// т.е. как будто у нас есть массив с такой возможностью обращения: array['h']. Таким образом мы можем хранить некоторую информацию для любого слова,
// просто разделяя его по буквам и уходя всё глубже в хэш-структуру. 
// Эта функция получает из числа номер ячейки массива хэш-ячейки
{
	return ((long long)h + 128);
}
void hadd(myhash &hah, string s) // заносит в хэш-таблицу нужное слово. Если нужных символов нет, то она достраивает таблицу.
// Например, мы вызвали функцию для слова "корова". Мы берём первый сивмол этого слова и проверяем, на что указывает ссылка в нынешней хэш-ячейке.
//допустим, в ячейке 'к' уже имеется следующая хэш-ячейка. Вызываем эту же функцию для этой ячейки, но теперь от слова "орова".
// Если же в массиве в ячейке 'к' указатель указывает на пустоту, то мы создаём новую структуру хэш-ячейки и помещает ссылку на неё в нынешнюю хэш-ячейку
// и вызываем эту же функцию для новой ячейки, но теперь от слова "орова".
// Если же наше слово закончилось, то есть уже конец прохода, это значит, что мы пришли в нужную хэш-ячейку. Добавляем в неё нашу информацию.
{
	if (s == "")     // конец прохода
	{
		hah.kod = next();
		if (hah.kod.size() > nraz)  // это на случай 'alarm', то есть когда мы добавляем ещё один бит
		{
			nraz++;     
		}
	}
	else
	{
		auto ah = ahash(s[0]);
		if (hah.v[ah] != NULL) // если ветка уже есть
		{
			hadd(*hah.v[ah], s.substr(1, s.size() - 1));
		}
		else // создание новой ветки
		{
			myhash* r = new myhash;
			hah.v[ah] = r;
			hinit(*r);
			hadd(*hah.v[ah], s.substr(1, s.size() - 1));
		}
	}
}
bool hchek(myhash &hah, string s) // проверка существования
{
	if (s == "")
	{
		return true;
	}
	else
	{
		auto ah = ahash(s[0]);
		if (hah.v[ah] != NULL)
		{
			return hchek(*hah.v[ah], s.substr(1, s.size() - 1));
		}
		else
		{
			return false;
		}
	}
}
string htake(myhash& hah, string s)// получение информации
{
	if (s == "")
	{
		if (hah.kod.size() < nraz)
		{
			for (ll i = hah.kod.size(); i < nraz; i++)
			{
				hah.kod = "0" + hah.kod;
			}
		}
		return hah.kod;
	}
	else
	{
		auto ah = ahash(s[0]);
		if (hah.v[ah] != NULL)
		{
			return htake(*hah.v[ah], s.substr(1, s.size() - 1));
		}
		else
		{
			cout << "Пытаешься найти код слова, которого нет, а-та-та!";
			return "";
		}
	}
}

vector<pair<string, string>> kodir(myhash &hah, string text) // сам алгоритм лвз
{
	vector<pair<string, string>> zakod;
	ll i = 0;
	string nword = "";
	string nkod = "";

	nword = { text[0] };
	nkod = htake(hah, nword);
	zakod.push_back(make_pair(nword, nkod));
	i++;

	while (i != text.size())
	{
		nword = "";
		nkod = "";

		while (i != text.size())     // ищем новое слово для кодирования
		{
			nword += {text[i]};
			i++;
			if (!hchek(hah, nword))
			{
				nword = nword.substr(0, nword.size() - 1);
				i--;
				break;
			}
		}

		nkod = htake(hah, nword);      //       кодируем
		zakod.push_back(make_pair(nword, nkod));

		string nslov = { nword[0] };
		ll otstup = 2;
		while (true)         // добавляем новое слово в словарь
		{
			nslov = zakod[zakod.size() - otstup].first + nslov;
			if (!hchek(hah, nslov))
			{
				hadd(hah, nslov);
				break;
			}
			else
			{
				otstup++;
			}
		}

	}

	return zakod;
}


//vector<pair<string, string>> zakod;



int main()
{
    setlocale(LC_ALL, "Russian");
	myhash namkod;
	hinit(namkod);

	ifstream f("in.txt");  // этот файл - словарь, то есть все возможные символы
	//    /*
	while (!f.eof())
	{
		string x;
		f >> x;
		mosh++;
	}
	mosh = ceil(log(mosh) / log(2));
	nraz = mosh;
	f.close();
	f.open("in.txt");
	while (!f.eof())
	{
		string x;
		getline(f, x);
		hadd(namkod, x);
	}

	ll tmp;
	cin >> tmp;    // 1 для кодирования, иначе декодирование
	string text = "";
	if (tmp == 1)
	{
		ifstream g("file.txt");   // это файл, откуда берётся текст для кодирования или декодирования
		while (!g.eof())
		{
			string s;
			getline(g, s);
			text += s;
		}
		vector<pair<string, string>> zakod = kodir(namkod, text);
		ofstream q("out.txt");
		for (auto u : zakod)
		{
			kol_simvol += u.second.size();
			q << u.second;
		}
		cout << text.size() << endl;
	}
	else
	{
		cout << "А это мне не захотелось делать. Тут типо хэш создавать не по буквам, а по кодам. Можно просто код в число переводить и в эту ячейку записывать символы.";
		// Да, я декодирование не делал
	}
	cout << kol_simvol;
	//   */

}
