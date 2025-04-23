#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstring>
#include <iomanip>
#include <fstream>
#include <clocale>

using namespace std;


class Date {
private:
    int day;
    int month;
    int year;

public:
    Date(int d = 1, int m = 1, int y = 2000) : day(d), month(m), year(y) {}

    friend istream& operator>>(istream& in, Date& dt) {
        in >> dt.day >> dt.month >> dt.year;
        return in;
    }

    friend ostream& operator<<(ostream& out, const Date& dt) {
        out << dt.day << "." << dt.month << "." << dt.year;
        return out;
    }

    void saveToFile(ofstream& file) const {
        file << day << " " << month << " " << year << endl;
    }

    void loadFromFile(ifstream& file) {
        file >> day >> month >> year;
    }
};

class Man {
private:
    char name[50];
    char surname[50];

public:
    Man(const char* n = "", const char* s = "") {
        strncpy(name, n, 49);
        name[49] = '\0';
        strncpy(surname, s, 49);
        surname[49] = '\0';
    }

    friend istream& operator>>(istream& in, Man& m) {
        cout << "Введите имя и фамилию: ";
        in.ignore();
        in.getline(m.name, 50, ' ');
        in.getline(m.surname, 50);
        return in;
    }

    friend ostream& operator<<(ostream& out, const Man& m) {
        out << m.name << " " << m.surname;
        return out;
    }

    bool operator==(const Man& other) const {
        return strcmp(name, other.name) == 0 && strcmp(surname, other.surname) == 0;
    }

    void saveToFile(ofstream& file) const {
        file << name << " " << surname << endl;
    }

    void loadFromFile(ifstream& file) {
        file.getline(name, 50, ' ');
        file.getline(surname, 50);
    }
};

// Базовый класс Common
class Common {
public:
    virtual ~Common() {}
    virtual void input() = 0;
    virtual void output() const = 0;
    virtual bool compare(const Man& target) const = 0;
    virtual void saveToFile(ofstream& file) const = 0;
    virtual void loadFromFile(ifstream& file) = 0;
};

// Класс Staff
class Staff : public Common {
private:
    Man employee;
    double salary;

public:
    void input() override {
        cout << "\n--- Добавление сотрудника ---" << endl;
        cin >> employee;
        cout << "Введите зарплату: ";
        cin >> salary;
    }

    void output() const override {
        cout << "| Сотрудник | " << setw(20) << left << employee
            << " | Зарплата: " << setw(10) << salary << " |" << endl;
    }

    bool compare(const Man& target) const override {
        return employee == target;
    }

    void saveToFile(ofstream& file) const override {
        file << "Staff\n";
        employee.saveToFile(file);
        file << salary << endl;
    }

    void loadFromFile(ifstream& file) override {
        employee.loadFromFile(file);
        file >> salary;
    }
};

class Worker1 : public Common {
private:
    Man employee;
    char profession[50];
    Date employmentDate;

public:
    void input() override {
        cout << "\n--- Добавление рабочего ---" << endl;
        cin >> employee;
        cout << "Введите профессию: ";
        cin.ignore();
        cin.getline(profession, 50);
        cout << "Введите дату приема (дд мм гггг): ";
        cin >> employmentDate;
    }

    void output() const override {
        cout << "| Рабочий   | " << setw(20) << left << employee
            << " | Профессия: " << setw(15) << profession
            << " | Дата: " << employmentDate << " |" << endl;
    }

    bool compare(const Man& target) const override {
        return employee == target;
    }

    void saveToFile(ofstream& file) const override {
        file << "Worker1\n";
        employee.saveToFile(file);
        file << profession << endl;
        employmentDate.saveToFile(file);
    }

    void loadFromFile(ifstream& file) override {
        employee.loadFromFile(file);
        file.getline(profession, 50);
        employmentDate.loadFromFile(file);
    }
};

void saveData(Common** arr, int size, const char* filename) {
    ofstream file(filename);
    if (!file) {
        cerr << "Ошибка открытия файла для записи!" << endl;
        return;
    }

    file << size << endl;
    for (int i = 0; i < size; ++i) {
        arr[i]->saveToFile(file);
    }
    file.close();
    cout << "Данные сохранены в файл: " << filename << endl;
}

void loadData(Common**& arr, int& size, const char* filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Ошибка открытия файла для чтения!" << endl;
        return;
    }

    file >> size;
    arr = new Common * [size];

    for (int i = 0; i < size; ++i) {
        string type;
        file >> type;

        if (type == "Staff") {
            Staff* obj = new Staff();
            obj->loadFromFile(file);
            arr[i] = obj;
        }
        else if (type == "Worker1") {
            Worker1* obj = new Worker1();
            obj->loadFromFile(file);
            arr[i] = obj;
        }
    }
    file.close();
    cout << "Данные загружены из файла: " << filename << endl;
}

void displayAll(Common** staffArr, int staffSize, Common** worker1Arr, int worker1Size) {
    if (staffSize == 0 && worker1Size == 0) {
        cout << "\nНет данных для отображения." << endl;
        return;
    }

    cout << "\n===== Все записи =====" << endl;
    cout << "-------------------------------------------------------------\n";
    cout << "| Тип       | Сотрудник          | Детали           | Дата       |\n";
    cout << "-------------------------------------------------------------\n";

    for (int i = 0; i < staffSize; ++i) {
        staffArr[i]->output();
    }

    for (int i = 0; i < worker1Size; ++i) {
        worker1Arr[i]->output();
    }

    cout << "-------------------------------------------------------------\n";
}

int main() {
    setlocale(LC_ALL, "rus");

    Common** staffArr = nullptr;
    Common** worker1Arr = nullptr;
    Common** resultArr = nullptr;
    int staffSize = 0, worker1Size = 0, resultSize = 0;

    while (true) {
        cout << "\n===== Меню =====" << endl;
        cout << "1. Добавить сотрудника\n"
            << "2. Добавить рабочего\n"
            << "3. Поиск\n"
            << "4. Сохранить данные\n"
            << "5. Загрузить данные\n"
            << "6. Показать всех\n"
            << "7. Выход\n"
            << "Выберите действие: ";

        int choice;
        cin >> choice;

        switch (choice) {
        case 1: {
            Staff* staff = new Staff();
            staff->input();
            Common** newStaffArr = new Common * [staffSize + 1];
            for (int i = 0; i < staffSize; ++i) newStaffArr[i] = staffArr[i];
            newStaffArr[staffSize++] = staff;
            delete[] staffArr;
            staffArr = newStaffArr;
            break;
        }
        case 2: {
            Worker1* worker1 = new Worker1();
            worker1->input();
            Common** newWorker1Arr = new Common * [worker1Size + 1];
            for (int i = 0; i < worker1Size; ++i) newWorker1Arr[i] = worker1Arr[i];
            newWorker1Arr[worker1Size++] = worker1;
            delete[] worker1Arr;
            worker1Arr = newWorker1Arr;
            break;
        }
        case 3: {
            Man target;
            cout << "Введите имя и фамилию для поиска: ";
            cin >> target;
            delete[] resultArr;
            resultArr = nullptr;
            resultSize = 0;

            for (int i = 0; i < staffSize; ++i) {
                if (staffArr[i]->compare(target)) {
                    Common** newResult = new Common * [resultSize + 1];
                    for (int j = 0; j < resultSize; ++j) newResult[j] = resultArr[j];
                    newResult[resultSize++] = staffArr[i];
                    delete[] resultArr;
                    resultArr = newResult;
                }
            }

            for (int i = 0; i < worker1Size; ++i) {
                if (worker1Arr[i]->compare(target)) {
                    Common** newResult = new Common * [resultSize + 1];
                    for (int j = 0; j < resultSize; ++j) newResult[j] = resultArr[j];
                    newResult[resultSize++] = worker1Arr[i];
                    delete[] resultArr;
                    resultArr = newResult;
                }
            }

            if (resultSize > 0) {
                cout << "\nНайдено " << resultSize << " записей:\n";
                displayAll(resultArr, resultSize, nullptr, 0);
            }
            else {
                cout << "Совпадений не найдено." << endl;
            }
            break;
        }
        case 4: {
            saveData(staffArr, staffSize, "staff.dat");
            saveData(worker1Arr, worker1Size, "workers.dat");
            break;
        }
        case 5: {
            loadData(staffArr, staffSize, "staff.dat");
            loadData(worker1Arr, worker1Size, "workers.dat");
            break;
        }
        case 6: {
            displayAll(staffArr, staffSize, worker1Arr, worker1Size);
            break;
        }
        case 7: {
            for (int i = 0; i < staffSize; ++i) delete staffArr[i];
            for (int i = 0; i < worker1Size; ++i) delete worker1Arr[i];
            delete[] staffArr;
            delete[] worker1Arr;
            delete[] resultArr;
            return 0;
        }
        default:
            cout << "Неверный выбор!" << endl;
        }
    }
}