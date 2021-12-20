#include <algorithm> 
#include <cmath> 
#include <cstdio>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <Windows.h>
using namespace std;
typedef long long ll;

static ofstream fileOut;
vector<pair<char,bool>> baseMoh = { make_pair('a',true), make_pair('b',true),make_pair('c',true),make_pair('d',true),make_pair('e',true),make_pair('f',true) };
// множество состоит из пар элемента и булевого значения, включён ли символ. Она используется для запрещения выбора элементов
vector<pair<char, int>> madeNextEltMoh(vector<pair<char, bool>> &alphabet) // создаёт массив для алфавита, в котором для каждого символа
// элемент под индексом этого символа равен паре следующего символа в множестве и номеру нынешнего символа(начиная с 1)
// то есть для базового алфавита в созданном массиве под индексом 'b' хранится символ 'c' и число 2.
// данный массив необходим для быстрого определения следующей буквы при построении слов, а также для быстрой проверки возможности постоить слово.
{
    vector<pair<char, int>> nextElt(255);
    int last, number=1, first;
    for (int i = 0; i < alphabet.size() ; i++)
        if (alphabet[i].second)
        {
            first = i;
            last = i; // находим первый включённый элемент
            break;
        }
    for (int i = last+1; i < alphabet.size(); i++)
        if (alphabet[i].second)
        { // если следующий найденный элемент тоже включён, то предыдущему ставим в соответствие данный элемент
            nextElt[alphabet[last].first - 0] = make_pair(alphabet[i].first, number);
            number++;
            last = i;
        }
    
    nextElt[alphabet[last].first] = make_pair(alphabet[first].first, number); // в конце зацикливаем созданный массив
                                                                              // после последнего элемента идёт первый
    return nextElt;
}
bool takeNextPost(vector<pair<char, bool>>& alphabet, vector<pair<char, int>>& nextElt, vector<char>& word)
// получает на вход слово, алфавит и множество следующих элементов. Изменяет слово на следующее в лексикографическом порядке.
{
    int i = word.size()-1; // проходимся c конца

    char firstElt;
    for (int ii = 0; ii < alphabet.size(); ii++)
    {
        firstElt = alphabet[ii].first;
        if (alphabet[ii].second)
            break;
    }

    while (i != -1)
    {
        // меняем нынешнюю букву на следующую
        word[i] = nextElt[word[i]].first;
        if (word[i] != firstElt) // если был сделан цикл, то необходимо поменять и следующую букву
            return true;
        i--;
    }
    return false;
}
bool takeNextMoh(vector<pair<char, bool>>& alphabet, vector<pair<char, int>>& nextElt, vector<char>& moh, int coutOfOn)
// получает на вход подмножество, множество(алфавит) и множество следующих элементов. Изменяет подмножество на следующее в лексикографическом порядке того же размера.
{
    int top = 0;
    int i = moh.size() - 1;
    char firstElt;
    for (int ii = 0; ii < alphabet.size(); ii++)
    {
        firstElt = alphabet[ii].first;
        if (alphabet[ii].second)
            break;
    }
    moh[i] = nextElt[moh[i]].first; // меняем последнюю букву
    if (moh[i] != firstElt) // если не было цикла, всё отлично
        return true;
    while (true)
    {
        top++; // иначе начинаем идти по слову в начало
        if (i-top == -1)
            return false; 
        
        if (coutOfOn - nextElt[moh[i - top]].second > top)
            break; // необходимо выбрать такую букву, что при изменении её на следующую, буквы справа от неё можно будет расположить в лексикографическом порядке
    }
    moh[i - top] = nextElt[moh[i - top]].first; // нашил такую букву
    while(top > 0)
    {
        top--; // меняем весь хвост слова
        moh[i - top] = nextElt[moh[i - top-1]].first;
    }
}

int main()
{
    SetConsoleCP(1251); SetConsoleOutputCP(1251);  // Подключаем вывод русских букв
    int numOfAsk;
    vector<pair<char, bool>> mohOfNum;
    vector<char> c1,c2,c3,c4,c5,c6,a1,a2,a3,a4,a5,a6;
    vector<char> startMoh = { 'a','a','a' };
    vector<pair<char, int>> nextMoh, numNextMoh;
    cout << "Укажите номер задачи, которую необходимо выполнить, либо введите 0 для выхода: ";
    cin >> numOfAsk;
    switch (numOfAsk)
    {
    case(0):
        return 0;
    case(1):
        fileOut.open(("dz.1.txt"));
        cout << "Данная задача ещё не готова." << endl;
        break;
    case(2):
        fileOut.open(("dz.2.txt"));
        baseMoh[0].second = false;
        nextMoh = madeNextEltMoh(baseMoh);
        for (int i = 1; i <= 5; i++)
            mohOfNum.push_back(make_pair(i+48,true));
        numNextMoh = madeNextEltMoh(mohOfNum);
        c1 = { '1', '2' };
        do
        {
            a1 = { 'b', 'b', 'b' };
            do
            {
                int kc1=0;
                int ka1= 0;
                for (int i = 1; i <= 5; i++)
                {
                    if (kc1 < c1.size() && i+48 == c1[kc1])
                    {
                        fileOut << 'a';
                        kc1++;
                    }
                    else
                    {
                        fileOut << a1[ka1];
                        ka1++;
                    }
                }
                fileOut << endl;
            } while (takeNextPost(baseMoh, nextMoh, a1)); // выбираем последовательность из оставшихся символов
        }
        while (takeNextMoh(mohOfNum, numNextMoh, c1, 5)); // выбираем два места под а из 5

        fileOut << endl << endl << endl << endl;

        c1 = { '1', '2' };
        
        do
        {
            a1 = { 'b', 'c', 'd' };
            do
            {
                int kc1 = 0;
                int ka1 = 0;
                for (int i = 1; i <= 5; i++)
                {
                    if (kc1 < c1.size() && i + 48 == c1[kc1])
                    {
                        fileOut << 'a';
                        kc1++;
                    }
                    else
                    {
                        fileOut << a1[ka1];
                        ka1++;
                    }
                }
                fileOut << endl;
            } while (takeNextMoh(baseMoh, nextMoh, a1, 5)); // выбираем подмножество из оставшихся символов
        } while (takeNextMoh(mohOfNum, numNextMoh, c1, 5)); // выбираем два места под а из 5

        break;
    case(3):
        fileOut.open(("dz.3.txt"));
        cout << "Данная задача ещё не готова." << endl;
        break;
    case(4):
        fileOut.open(("dz.4.txt"));
        cout << "Данная задача ещё не готова." << endl;
        break;
    case(5):
        fileOut.open(("dz.5.txt"));
        cout << "Данная задача ещё не готова." << endl;
        break;
    case(6):
        fileOut.open(("dz.6.txt"));
        cout << "Данная задача ещё не готова." << endl;
        break;
    case(7):
        fileOut.open(("dz.7.txt"));
        cout << "Данная задача ещё не готова." << endl;
        break;
    case(8):
        fileOut.open(("dz.8.txt"));
        cout << "Данная задача ещё не готова." << endl;
        break;
    case(9):
        fileOut.open(("dz.9.txt"));
        cout << "Данная задача ещё не готова." << endl;
        break;
    case(10):
        fileOut.open(("dz.10.txt"));
        cout << "Данная задача ещё не готова." << endl;
        break;
    case(11):
        fileOut.open(("dz.11.txt"));
        cout << "Данная задача ещё не готова." << endl;
        break;
    case(12):
        fileOut.open(("dz.12.txt"));
        cout << "Данная задача ещё не готова." << endl;
        break;
    default:
        cout << "Задачи с таким номером не существует." << endl;
        break;
    }
    fileOut.close();
}