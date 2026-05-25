#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <iomanip>
#include <sstream>
#include <limits>

using namespace std;

struct Country {
    string name;
    string capital;
    unsigned int population;
    double area;
    string industry;
    string religion;
};

class Database {
private:
    vector<Country> countries;

    void calculateWidths(int& wName, int& wCapital, int& wPop, int& wArea, int& wIndustry, int& wReligion) const {
        wName = 15;   // минимальная ширина
        wCapital = 10;
        wPop = 10;
        wArea = 10;
        wIndustry = 15;
        wReligion = 12;

        for (const auto& c : countries) {
            wName = max(wName, (int)c.name.length());
            wCapital = max(wCapital, (int)c.capital.length());
            wPop = max(wPop, (int)to_string(c.population).length());
            wArea = max(wArea, (int)to_string(c.area).length());
            wIndustry = max(wIndustry, (int)c.industry.length());
            wReligion = max(wReligion, (int)c.religion.length());
        }
        // Добавим отступы
        wName += 2; wCapital += 2; wPop += 2; wArea += 2; wIndustry += 2; wReligion += 2;
    }

public:
    // Загрузка из текстового файла (разделитель '|')
    bool loadFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Ошибка: не удалось открыть файл " << filename << endl;
            return false;
        }
        countries.clear();
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string token;
            Country c;
            try {
                getline(ss, token, '|'); c.name = token;
                getline(ss, token, '|'); c.capital = token;
                getline(ss, token, '|'); c.population = stoul(token);
                getline(ss, token, '|'); c.area = stod(token);
                getline(ss, token, '|'); c.industry = token;
                getline(ss, token, '|'); c.religion = token;
                countries.push_back(c);
            } catch (...) {
                cerr << "Ошибка формата в строке: " << line << endl;
            }
        }
        file.close();
        cout << "Загружено " << countries.size() << " записей.\n";
        return true;
    }

    // Сохранение в файл
    bool saveToFile(const string& filename) const {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Ошибка: не удалось создать файл " << filename << endl;
            return false;
        }
        for (const auto& c : countries) {
            file << c.name << "|" << c.capital << "|" << c.population << "|"
                 << c.area << "|" << c.industry << "|" << c.religion << "\n";
        }
        file.close();
        cout << "Сохранено " << countries.size() << " записей в файл " << filename << endl;
        return true;
    }

    // Просмотр всей базы в виде таблицы
    void display() const {
        if (countries.empty()) {
            cout << "База данных пуста.\n";
            return;
        }

        int wName, wCapital, wPop, wArea, wIndustry, wReligion;
        calculateWidths(wName, wCapital, wPop, wArea, wIndustry, wReligion);

        // Заголовки
        cout << left << setw(4) << "№"
             << setw(wName) << "Название"
             << setw(wCapital) << "Столица"
             << setw(wPop) << "Население"
             << setw(wArea) << "Площадь"
             << setw(wIndustry) << "Промышленность"
             << setw(wReligion) << "Религия" << endl;
        cout << string(4 + wName + wCapital + wPop + wArea + wIndustry + wReligion, '-') << endl;

        // Данные
        for (size_t i = 0; i < countries.size(); ++i) {
            const auto& c = countries[i];
            cout << left << setw(4) << i+1
                 << setw(wName) << c.name
                 << setw(wCapital) << c.capital
                 << setw(wPop) << c.population
                 << setw(wArea) << c.area
                 << setw(wIndustry) << c.industry
                 << setw(wReligion) << c.religion << endl;
        }
    }

    // Добавление новой записи (ручной ввод)
    void addRecord() {
        Country c;
        cout << "Введите название: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, c.name);
        cout << "Столица: ";
        getline(cin, c.capital);
        cout << "Население (целое): ";
        cin >> c.population;
        cout << "Площадь (км²): ";
        cin >> c.area;
        cin.ignore();
        cout << "Промышленность: ";
        getline(cin, c.industry);
        cout << "Религия: ";
        getline(cin, c.religion);
        countries.push_back(c);
        cout << "Запись добавлена.\n";
    }

    // Удаление записи по номеру (1..size)
    bool removeRecord(int index) {
        if (index < 1 || index > static_cast<int>(countries.size())) {
            cerr << "Неверный номер записи.\n";
            return false;
        }
        countries.erase(countries.begin() + (index - 1));
        cout << "Запись удалена.\n";
        return true;
    }

    // Сортировка по названию (ключевое поле)
    void sortByName() {
        sort(countries.begin(), countries.end(),
             [](const Country& a, const Country& b) { return a.name < b.name; });
        cout << "База отсортирована по названию.\n";
    }

    // Поиск записи по названию (ключевое поле)
    void searchByName(const string& name) const {
        bool found = false;
        int count = 0;
        for (const auto& c : countries) {
            if (c.name == name) {
                if (!found) {
                    cout << "Найденные записи:\n";
                    found = true;
                }
                cout << ++count << ". " << c.name << ", " << c.capital
                     << ", население: " << c.population << ", площадь: " << c.area
                     << ", промышленность: " << c.industry << ", религия: " << c.religion << endl;
            }
        }
        if (!found) cout << "Страна с названием \"" << name << "\" не найдена.\n";
    }

    // Выборка по диапазону населения (поле **)
    void filterByPopulationRange(unsigned int minPop, unsigned int maxPop) const {
        vector<Country> result;
        for (const auto& c : countries) {
            if (c.population >= minPop && c.population <= maxPop)
                result.push_back(c);
        }
        if (result.empty()) {
            cout << "Нет стран с населением в диапазоне [" << minPop << ", " << maxPop << "].\n";
            return;
        }
        cout << "Страны с населением от " << minPop << " до " << maxPop << ":\n";
        for (const auto& c : result) {
            cout << " - " << c.name << " (" << c.capital << ") — " << c.population << " чел.\n";
        }
    }

    // группировка по промышленности, внутри группы по названию
    void groupByIndustry() const {
        if (countries.empty()) {
            cout << "Нет данных для группировки.\n";
            return;
        }
        map<string, vector<Country>> groups;
        for (const auto& c : countries) {
            groups[c.industry].push_back(c);
        }
        for (auto& pair : groups) {
            // сортировуа страны в группе по названию
            sort(pair.second.begin(), pair.second.end(),
                 [](const Country& a, const Country& b) { return a.name < b.name; });
        }
        cout << "\nГруппировка по видам промышленности:\n";
        for (const auto& pair : groups) {
            cout << "\n=== " << pair.first << " ===\n";
            for (const auto& c : pair.second) {
                cout << "  " << c.name << " (столица: " << c.capital
                     << ", население: " << c.population << ")\n";
            }
        }
    }

    // создание новой БД со странами, плотность населения 
    Database filterByDensity(double densityThreshold) const {
        Database newDB;
        for (const auto& c : countries) {
            if (c.area > 0) {
                double density = c.population / c.area;
                if (density > densityThreshold)
                    newDB.countries.push_back(c);
            }
        }
        cout << "Отобрано " << newDB.countries.size() << " стран с плотностью населения > "
             << densityThreshold << " чел/км².\n";
        return newDB;
    }

    bool isEmpty() const { return countries.empty(); }
    size_t size() const { return countries.size(); }
};



    int choice;
    do {
        cout << "\n========== МЕНЮ ==========\n"
             << "1. Загрузить БД из файла\n"
             << "2. Просмотреть БД (таблица)\n"
             << "3. Добавить новую запись\n"
             << "4. Удалить запись (по номеру)\n"
             << "5. Сохранить БД в файл\n"
             << "6. Сортировка по названию\n"
             << "7. Поиск по названию\n"
             << "8. Выборка по диапазону населения\n"
             << "9. [Задача 1] Группировка по промышленности\n"
             << "10. [Задача 2] Новая БД по плотности населения\n"
             << "0. Выход\n"
             << "Ваш выбор: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                string fname;
                cout << "Введите имя файла для загрузки: ";
                cin >> fname;
                db.loadFromFile(fname);
                break;
            }
            case 2:
                db.display();
                break;
            case 3:
                db.addRecord();
                break;
            case 4: {
                int idx;
                cout << "Введите номер записи для удаления: ";
                cin >> idx;
                db.removeRecord(idx);
                break;
            }
            case 5: {
                string fname;
                cout << "Введите имя файла для сохранения: ";
                cin >> fname;
                db.saveToFile(fname);
                break;
            }
            case 6:
                db.sortByName();
                break;
            case 7: {
                string name;
                cout << "Введите название страны: ";
                cin.ignore();
                getline(cin, name);
                db.searchByName(name);
                break;
            }
            case 8: {
                unsigned int minPop, maxPop;
                cout << "Минимальное население: ";
                cin >> minPop;
                cout << "Максимальное население: ";
                cin >> maxPop;
                db.filterByPopulationRange(minPop, maxPop);
                break;
            }
            case 9:
                db.groupByIndustry();
                break;
            case 10: {
                double p;
                cout << "Введите порог плотности населения (чел/км²): ";
                cin >> p;
                Database newDB = db.filterByDensity(p);
                if (!newDB.isEmpty()) {
                    cout << "Новая база данных (плотность > " << p << "):\n";
                    newDB.display();
                    char saveChoice;
                    cout << "Сохранить новую БД в файл? (y/n): ";
                    cin >> saveChoice;
                    if (saveChoice == 'y' || saveChoice == 'Y') {
                        string outFile;
                        cout << "Имя файла для сохранения: ";
                        cin >> outFile;
                        newDB.saveToFile(outFile);
                    }
                }
                break;
            }
            case 0:
                cout << "Выход из программы.\n";
                break;
            default:
                cout << "Неверный выбор.\n";
        }
    } while (choice != 0);

    return 0;
}