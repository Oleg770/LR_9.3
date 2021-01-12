#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <list>
#include <sstream>

/*
		Є текстовий файл, що містить список службовців. Для кожного
		службовця вказано:
		1. прізвище і ініціали
		2. назву посади
		3. рік прийому на роботу
		4. оклад
		 Написати програму, що виконує наступні дії:
		* Створює перегляд
		* Поповнює список
		----------------------------
		** вилучення службовця
		** редагування службовця
		**сортування за прізвищем
		**сортування за окладом
		**сортування за роком прийому
		-------------------------------
		* Здійснює виведення на екран інформації про службовця,
		  прізвище якого введене з клавіатури
*/

using namespace std;


// Работник ------
struct Employee {
	int ind; // в деяких випадках потрібно номер в таблиці, щоб вказати на даного службовця
	string Name; // прізвище і ініціали
	string specialty; // назва посади
	int Year = 0; // рік прийому на роботу
	double Salary; // оклад

	Employee() {};
	// читає одну строку в поля структури
	void LoadFromStream(istream& in);
	// записує поля структури в строку в потік
	void SaveToStream(ostream& out);
	void PrintFormat(int i); // вивід на екран 
	void Print();
};

// список работников
struct PList {
	list<Employee> dt;
	void LoadFromFile(string fname);
	void SaveToFile(string fname);
	void Print();
	void AddNew();
	// пошук по прізвищу
	vector<Employee> FindByName(string name);
	void DeleteEmployee();
	void EditEmployee();
	void Sort();

};

void Employee::Print()
{
	cout << Name << "," << specialty << "," << Year << "," << Salary << endl;
}

// удалят передние и задние пробелы
void DeleteSpase(string& str)
{
	str.erase(str.find_last_not_of(' ') + 1);
	str.erase(0, str.find_first_not_of(' '));
}


void PList::AddNew()
{
	cout << "Enter fields: Name, specialty, year of hiring, salary\n";
	cout << "Example: Mironova M.I, doctor, 1998, 11000\n";
	Employee em;
	em.LoadFromStream(cin);
	dt.push_back(em);
}

void PList::SaveToFile(string fname)
{
	ofstream f(fname, ios::out);
	for (auto p : dt)p.SaveToStream(f);
	f.close();

}

void PList::LoadFromFile(string fname)
{
	fstream file(fname, ios::in);
	while (file.good()) {
		Employee em;
		em.LoadFromStream(file);
		if (file.eof() || em.Year == 0)break;
		dt.push_back(em);
	}
	cout << "Loaded " << dt.size() << " Employees\n";

}

// считываение одной строки файла в поля структуры Employee
// зчитування може бути як з файла так і з екрана, дивлячись що
// ми поставимо замість in
void Employee::LoadFromStream(istream& in)
{
	char bf[1000];

	if (in.peek() == '\n') { in.get(); } // если попал предыдущий символ конца строки, считываем его в пустоту и он исчезает

	in.getline(bf, 1000, ','); // берем часть строки из файла (потока) до запятой
	if (bf[0] == '\n' || bf[0] == 0) { Year = 0; return; } // если пустой список - нет слов
	Name = bf;
	DeleteSpase(Name); // убираем пробелы спереди и сзади, если есть


	in.getline(bf, 1000, ','); // берем часть строки из файла (потока) до запятой
	specialty = bf;
	DeleteSpase(specialty); // убираем пробелы спереди и сзади, если есть

	in.getline(bf, 1000, ','); // берем часть строки из файла (потока) до запятой
	string wrd = bf;
	DeleteSpase(wrd); // убираем пробелы спереди и сзади, если есть
	Year = stoi(wrd); // раскодировуем слова в числа, где нужно

	in.getline(bf, 1000); // берем часть строки из файла (потока) до конца строки
	wrd = bf;
	DeleteSpase(wrd); // убираем пробелы спереди и сзади, если есть
	Salary = stod(wrd); // раскодировуем слова в числа, где нужно
}

// вывод одного екземпляра структуры p или в строку файла или на єкран
// это смотря что мы поставим в параметр out. Если поставим файловый поток, то
// выведется (довавится) в файл, если это будет cout, то на стандартный вывод - экран
void Employee::SaveToStream(ostream& out)
{
	out << Name << ',' << specialty << ',' << Year << ',' << Salary << endl;
}

void Employee::PrintFormat(int i)
{
	// форматированный вывод одной структуры (под вид таблицы)
	cout << setfill(' ') << left;
	cout << "| " << setw(5) << i; // номер в таблиці
	cout << "| " << setw(23) << Name;
	cout << "| " << setw(15) << specialty;
	cout << "| " << setw(6) << Year;
	cout << "| " << setw(7) << Salary << '|' << endl;

}

void PList::Print()
{
	if (!dt.size()) { cout << "Empty\n"; return; }
	cout << "-------------------------------------------------------------------\n";
	cout << "| N    |   name                 |   specialty    | year  | salary |" << endl; // названия колонок (заголовки)
	cout << "-------------------------------------------------------------------\n";
	int i = 1;
	for (auto p : dt)p.PrintFormat(i++);
	cout << "-------------------------------------------------------------------\n";

}

void LoadFromFile(PList& PL)
{
	cout << "Load data from file.\n";
	cout << "Enter file name: ";
	string fname;
	cin >> fname;
	PL.LoadFromFile(fname);
}

void ClearDB(PList& L)
{
	cout << "Data will be delete only in memory.\n";
	cout << "Data in file will not be delete.\n";
	cout << "You are sure to delete the data ?\n";
	cout << "  Yes - y, No - n :";
	string str; cin >> str;
	if (str == "y") {
		L.dt.clear();
		cout << "Data deleted!\n";
	}

}

// пошук по прізвищу, результатів може бути декілька
// повертає вектор із знайдених службовців
vector<Employee> PList::FindByName(string name)
{
	vector<Employee> finded;
	int ind = 0;
	for (auto p : dt) {
		ind++;
		if (p.Name.find(name) != string::npos)
		{
			p.ind = ind; finded.push_back(p);
		}
	}
	return finded;
}

// для пошуку можна вводити частину Прізвища, або декілька літер
// буде шукатись їх наявність в полі прізвища
void FindEmployeeByName(PList& L)
{
	cout << " Find employee by name\n";
	cout << "Enter name: ";
	string name; cin >> name;
	vector<Employee> emp = L.FindByName(name);
	if (!emp.size()) { cout << "Not found\n"; return; }
	cout << "-------------------------------------------------------------------\n";
	cout << "| N    |   name                 |   specialty    | year  | salary |" << endl; // названия колонок (заголовки)
	cout << "-------------------------------------------------------------------\n";
	for (auto p : emp)p.PrintFormat(p.ind);
	cout << "-------------------------------------------------------------------\n";

}

void PList::DeleteEmployee()
{
	cout << "Delete. Enter employee numer:"; int iDel; cin >> iDel;
	if (iDel <= 0 || iDel > dt.size()) { cout << "bad input!\n"; return; }
	list<Employee>::iterator it;
	it = dt.begin();
	iDel--;
	int i = 0;
	for (; i < iDel; i++)it++;
	cout << "Deleted: " << i + 1 << ". " << it->Name << endl;
	dt.erase(it);

}

void PList::EditEmployee()
{
	cout << "Edit. Enter employee numer:"; int iDel; cin >> iDel;
	if (iDel <= 0 || iDel > dt.size()) { cout << "bad input!\n"; return; }
	list<Employee>::iterator it;
	it = dt.begin();
	iDel--;
	int i = 0;
	for (; i < iDel; i++)it++;
	cout << "Current data:\n";
	it->Print();
	cout << "Enter new data:\n";
	it->LoadFromStream(cin);
}

void PList::Sort()
{
	cout << "Sort by Name:\t1)\n";
	cout << "Sort by Year:\t2)\n";
	cout << "Sort by Salary:\t3)\n";
	cout << "Enter numer: "; int fild; cin >> fild;
	switch (fild)
	{
	case 1:dt.sort([](Employee a, Employee b) {return a.Name < b.Name; });
		cout << "Sorted by name!\n";
		break;

	case 2:dt.sort([](Employee a, Employee b) {return a.Year < b.Year; });
		cout << "Sorted by Year!\n";
		break;

	case 3:dt.sort([](Employee a, Employee b) {return a.Salary < b.Salary; });
		cout << "Sorted by Salary!\n";
		break;
	}
}

int main()
{
	PList PL;
	PL.LoadFromFile("emp.txt");
	cout << "data from file emp.txt:\n";
	PL.Print();

	int choose = -1;
	string fname;
	while (choose != 0) {
		cout << "\n 1) Add data from file\t\t2) Add new employee\
				\ 3) Save to file\t\t4) Print DB\
				\n 5) Clear DB\t\t\t6) Find by name\
				\n 7) Delete employee\t\t8) Edit employee\
				\ 9) Sort\t\t\t0) Quit" << "\n\n";

		cin >> choose;
		switch (choose) {
		case 1: LoadFromFile(PL); break;
		case 2: PL.AddNew(); break;
		case 3:
			cout << "Enter file name:";
			cin >> fname;
			PL.SaveToFile(fname);
			break;
		case 4: PL.Print(); break;
		case 5: ClearDB(PL); break; // очистить список (все стереть)
		case 6: FindEmployeeByName(PL); break;
		case 7: PL.DeleteEmployee(); break;
		case 8: PL.EditEmployee(); break;
		case 9: PL.Sort(); break;
		case 0: return 0;
		}
	}

	cout << "\n\n"; system("pause"); return 0;

}