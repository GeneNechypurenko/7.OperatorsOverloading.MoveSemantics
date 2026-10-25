#include <iostream>
#include <cstring>
#include <utility>
#include <windows.h>

using namespace std;

/*
==========================================================================================================================================
                                        ДОМАШНЄ ЗАВДАННЯ 27. Перевантаження операторів. Клас MyString
==========================================================================================================================================
*/

class MyString
{
private:
    char* data;
    int length;

    void assign(const char* text)
    {
        int newLength = (int)strlen(text);
        char* newData = new char[newLength + 1];
        strcpy_s(newData, newLength + 1, text);
        delete[] data;
        data = newData;
        length = newLength;
    }

public:
    //   1. У модулі 16 радили робити конструктор з одним параметром explicit. Чому MyString(const char*) — без explicit?
    //   Які рядки вашого main перестануть компілюватися, якщо його додати ?
    MyString(const char* text) : data(nullptr), length(0)
    {
        if (text == nullptr)
        {
            text = "";
        }
        assign(text);
    }

    MyString() : MyString("") {}

    MyString(const MyString& other) : data(nullptr), length(0)
    {
        assign(other.data);
    }

    //  2. Що станеться при b = a, якщо не написати власний operator=? Коли саме програма «зламається»?
    MyString& operator=(const MyString& other)
    {
        if (this == &other)
        {
            return *this;
        }
        assign(other.data);
        return *this;
    }

    ~MyString()
    {
        delete[] data;
    }

    // ===== Рівень 2: перенесення (правило п'яти) =====
    MyString(MyString&& other) : data(other.data), length(other.length)
    {
        other.data = nullptr;
        other.length = 0;
    }

    MyString& operator=(MyString&& other)
    {
        if (this == &other)
        {
            return *this;
        }
        delete[] data;
        data = other.data;
        length = other.length;
        other.data = nullptr;
        other.length = 0;
        return *this;
    }

    int getLength() const
    {
        return length;
    }

    // 3. Навіщо дві версії []? Що буде, якщо залишити лише неконстантну?
    char& operator[](int index)
    {
        return data[index];
    }

    char operator[](int index) const
    {
        return data[index];
    }

    MyString& operator+=(const MyString& other)
    {
        int newLength = length + other.length;
        char* newData = new char[newLength + 1];
        strcpy_s(newData, newLength + 1, data);
        strcat_s(newData, newLength + 1, other.data);
        delete[] data;
        data = newData;
        length = newLength;
        return *this;
    }

    // замінено дружньою функцією
    // MyString operator+(const MyString& other) const
    // {
    //     MyString result = *this;
    //     result += other;
    //     return result;
    // }

    bool operator==(const MyString& other) const
    {
        return strcmp(data, other.data) == 0;
    }

    bool operator!=(const MyString& other) const
    {
        return !(*this == other);
    }

    bool operator<(const MyString& other) const
    {
        return strcmp(data, other.data) < 0;
    }

    explicit operator bool() const
    {
        return length > 0;
    }

    friend ostream& operator<<(ostream& out, const MyString& s);
    friend istream& operator>>(istream& in, MyString& s);
    friend MyString operator+(const MyString& left, const MyString& right);
};

ostream& operator<<(ostream& out, const MyString& s)
{
    if (s.data != nullptr)
    {
        out << s.data;
    }
    return out;
}

istream& operator>>(istream& in, MyString& s)
{
    char buffer[256];
    in >> buffer;
    s = buffer;
    return in;
}

// 6. (Рівень 3) Чому "Hello, " + name не компілювалося, коли + був методом, і компілюється з дружньою функцією?
MyString operator+(const MyString& left, const MyString& right)
{
    MyString result = left;
    result += right;
    return result;
}

int main()
{
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    // ---------------- РІВЕНЬ 1 ----------------
    cout << "1. Копія незалежна від оригіналу" << endl;
    MyString greeting("Hello");
    MyString copy = greeting;
    copy[0] = 'J';
    cout << "  " << greeting << " / " << copy << endl;                     // Hello / Jello

    cout << "2. Порожній рядок" << endl;
    MyString empty;
    cout << "  [" << empty << "], довжина " << empty.getLength() << endl;  // [], 0

    cout << "3. Склеювання" << endl;
    MyString name("World");
    MyString message = greeting + ", " + name + "!";
    cout << "  " << message << ", довжина " << message.getLength() << endl; // Hello, World!, 13
    name += "!!!";
    cout << "  " << name << endl;                                           // World!!!

    cout << "4. Порівняння з неявним перетворенням" << endl;
    if (greeting == "Hello")
    {
        cout << "  greeting == \"Hello\"" << endl;
    }
    if (copy != greeting)
    {
        cout << "  copy != greeting" << endl;
    }

    cout << "5. Присвоєння" << endl;
    MyString a("one");
    MyString b("two");
    MyString c("three");
    a = b = c;
    c = c;
    cout << "  " << a << " " << b << " " << c << endl;

    cout << "6. Константний рядок" << endl;
    const MyString title("OOP");
    cout << "  title[0] = " << title[0] << endl;

    cout << "7. Довжина кирилиці" << endl;      // 4. Чому довжина "Привіт" не 6?
    MyString word("Привіт");
    cout << "  \"" << word << "\" має довжину " << word.getLength() << endl;

    // ---------------- РІВЕНЬ 2 ----------------
    // 5. (Рівень 2) Чому обмін рядків у сортуванні зроблено через std::move? Що відбувалося б при кожному обміні,
    // якби в класі не було методів перенесення ?
    cout << "8. Рівень 2: сортування з перенесенням" << endl;
    MyString names[5] = { "Олена", "Марко", "Анна", "Тарас", "Богдан" };
    for (int i = 0; i < 5 - 1; i++)
    {
        for (int j = 0; j < 5 - 1 - i; j++)
        {
            if (names[j + 1] < names[j])
            {
                MyString temp = std::move(names[j]);
                names[j] = std::move(names[j + 1]);
                names[j + 1] = std::move(temp);
            }
        }
    }
    cout << "  ";
    for (int i = 0; i < 5; i++)
    {
        cout << names[i] << " ";
    }
    cout << endl;

    // ---------------- РІВЕНЬ 3 ----------------
    cout << "9. Рівень 3: рядок ліворуч" << endl;
    MyString hello = "Hello, " + name;
    cout << "  " << hello << endl;

    cout << "10. Рівень 3: operator bool і >>" << endl;
    if (!empty)
    {
        cout << "  empty порожній" << endl;
    }
    MyString input;
    cout << "  Введіть слово: ";
    cin >> input;
    if (input)
    {
        cout << "  Ви ввели: " << input << " (довжина " << input.getLength() << ")" << endl;
    }

    return 0;
}

/*
  1. У модулі 16 радили робити конструктор з одним параметром explicit. Чому MyString(const char*) — без explicit?
     Які рядки вашого main перестануть компілюватися, якщо його додати?
  2. Що станеться при b = a, якщо не написати власний operator=? Коли саме програма «зламається»?
  3. Навіщо дві версії []? Що буде, якщо залишити лише неконстантну?
  4. Чому довжина "Привіт" не 6?
  5. (Рівень 2) Чому обмін рядків у сортуванні зроблено через std::move? Що відбувалося б при кожному обміні,
     якби в класі не було методів перенесення?
  6. (Рівень 3) Чому "Hello, " + name не компілювалося, коли + був методом, і компілюється з дружньою функцією?
 
  ВІДПОВІДІ НА ЧАСТИНУ «ПОЯСНИ»

  1. 

  2. 

  3. 

  4. 

  5. 

  6. 
*/
