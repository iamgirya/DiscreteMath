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

// функции, выполняющие первые 6 задач
static ofstream fileOut, supFileOut;
vector<char> baseMoh = { 'a','b','c','d','e','f'};
// множество состоит из пар элемента и булевого значения, включён ли символ. Она используется для запрещения выбора элементов
pair<char, vector<pair<char, int>>> madeNextEltMoh(vector<char> &alphabet) 
// коротко: пара из односвязного списка и его первого элемента
// создаёт пару из первого элемента алфавита и массива, в котором для каждого символа данного алфавита
// элемент под индексом этого символа равен паре следующего символа в алфавите и номеру нынешнего символа(начиная с 1)
// то есть для базового алфавита в созданном массиве 'следующих' под индексом 'b' хранится символ 'c' и число 2.
// данный массив необходим для быстрого определения следующей буквы при построении слов, а также для быстрой проверки возможности постоить слово.
{
    vector<pair<char, int>> nextElt(255);
    int number = 1;
    for (int i = 0; i < alphabet.size()-1; i++)
    {
        nextElt[alphabet[i]] = make_pair(alphabet[i+1], number);
        number++;
    }
    nextElt[alphabet[alphabet.size() - 1]] = make_pair(alphabet[0], number); // в конце зацикливаем созданный массив
                                                                              // после последнего элемента идёт первый
    return make_pair(alphabet[0], nextElt);
}
bool takeNextPost(pair<char, vector<pair<char, int>>>& nextElt, vector<char>& word)
// получает на вход слово, первую букву и множество следующих элементов. Изменяет слово на следующее в лексикографическом порядке.
{
    int i = word.size()-1; // проходимся c конца
    char firstElt = nextElt.first; //получаем первую букву
    while (i != -1)
    {
        // меняем нынешнюю букву на следующую
        word[i] = nextElt.second[word[i]].first;
        if (word[i] != firstElt) // если был сделан цикл, то необходимо поменять и следующую букву
            return true;
        i--;
    }
    return false;
}
bool takeNextMoh(pair<char, vector<pair<char, int>>>& nextElt, vector<char>& moh, int coutOfOn)
// получает на вход первый элемент, множество следующих элементов. Изменяет подмножество на следующее в лексикографическом порядке того же размера.
{
    int top = 0;
    int i = moh.size() - 1;
    char firstElt = nextElt.first;
    moh[i] = nextElt.second[moh[i]].first; // меняем последнюю букву
    if (moh[i] != firstElt) // если не было цикла, всё отлично
        return true;
    while (true)
    {
        top++; // иначе начинаем идти по слову в начало
        if (i-top == -1)
            return false; 
        
        if (coutOfOn - nextElt.second[moh[i - top]].second > top)
            break; // необходимо выбрать такую букву, что при изменении её на следующую, буквы справа от неё можно будет расположить в лексикографическом порядке
    }
    moh[i - top] = nextElt.second[moh[i - top]].first; // нашли такую букву
    while(top > 0)
    {
        top--; // меняем весь хвост слова
        moh[i - top] = nextElt.second[moh[i - top-1]].first;
    }
}
bool takeNextPerest(pair<char,vector<pair<char, int>>>& nextElt, vector<char>& moh)
// получает на вход перестановку и выписывает следующую в лексикографическом порядке
{
    int top = 0;
    int i = moh.size() - 1;
    char firstElt = nextElt.first;
    bool noOk = true;
    while (noOk)
    {
        noOk = false;
        moh[i] = nextElt.second[moh[i]].first; // меняем последнюю букву
        if (moh[i] != firstElt)
            for (int j = i - 1; j > -1; j--)
                if (moh[i] == moh[j])
                    noOk = true;
    }
    if (moh[i] != firstElt) // если не было цикла, всё отлично
        return true;
    while (true)
    {
        top++; // иначе начинаем идти по слову в начало
        if (i - top == -1)
            return false;
        noOk = true;
        while (noOk)
        {
            noOk = false;
            moh[i - top] = nextElt.second[moh[i - top]].first;
            if (moh[i - top] != firstElt)
                for (int j = i - top - 1; j > -1; j--)
                    if (moh[i - top] == moh[j])
                        noOk = true;
        }
        if (moh[i - top] != firstElt) // если не было цикла, всё отлично
            break;
    }
    while (top > 0)
    {
        top--; // меняем весь хвост слова
        for (int j = i - top - 1; j > -1; j--)
            if (moh[i - top] == moh[j])
                noOk = true;
        while (noOk)
        {
            noOk = false;
            moh[i - top] = nextElt.second[moh[i - top]].first;
            if (moh[i - top] != firstElt)
                for (int j = i - top - 1; j > -1; j--)
                    if (moh[i - top] == moh[j])
                        noOk = true;
        }
    }
    return true;
}
pair<char, vector<pair<char, int>>> deactivateElt( pair<char, vector<pair<char, int>>>& nextElt, vector<char>& word)
// функция создаёт множество следующих, в котором элементы, находящиеся в word исключены.
// причём создаётся новое множество следующих, чтобы не уничтожать старое
{
    vector<pair<char, int>> newNextElt(255);
    int iter = 0, number = 1;
    char firstChar = nextElt.first;
    char tmp = firstChar, last = tmp;
    char newfirstChar = firstChar;
    do
    {
        if (iter < word.size() && tmp == word[iter] && tmp == newfirstChar)
        {
            newfirstChar = nextElt.second[tmp].first;
            iter++;
            last = nextElt.second[tmp].first;
        }
        else if (iter == word.size() || nextElt.second[tmp].first != word[iter])
        {
            newNextElt[last] = make_pair(nextElt.second[tmp].first,number);
            last = nextElt.second[tmp].first;
            number++;
        }
        else if (iter < word.size() && nextElt.second[tmp].first == word[iter])
        {
            iter++;
        }
        tmp = nextElt.second[tmp].first;
    } while (nextElt.second[tmp].first != firstChar);
    newNextElt[last] = make_pair(newfirstChar, number);
    
    return make_pair(newfirstChar, newNextElt);
}
char takeNnextElt(pair<char, vector<pair<char, int>>>& nextElt, int n)
{
    char c = nextElt.first;
    for (int i = 1; i < n; i++)
    {
        c = nextElt.second[c].first;
    }
    return c;
}
bool takeNextMohPov (pair<char, vector<pair<char, int>>>& nextElt, vector<char>& moh)
// получает на вход первый элемент, множество следующих элементов. Изменяет подмножество с повторениями на следующее в лексикографическом порядке того же размера.
{
    int top = 0;
    int i = moh.size() - 1;
    char firstElt = nextElt.first;
    moh[i] = nextElt.second[moh[i]].first; // меняем последнюю букву
    if (moh[i] != firstElt) // если не было цикла, всё отлично
        return true;
    while (true)
    {
        top++; // иначе начинаем идти по слову в начало
        if (i - top == -1)
            return false;

        if (nextElt.second[moh[i - top]].first != firstElt)
            break; // необходимо выбрать такую букву, что при изменении её на следующую, буквы справа от неё можно будет расположить в лексикографическом порядке
    }
    moh[i - top] = nextElt.second[moh[i - top]].first; // нашли такую букву
    while (top > 0)
    {
        top--; // меняем весь хвост слова
        moh[i - top] = moh[i - top-1];
    }
}


/////////////////////////////////////////////////////////////////////////////////////
int p = 7, q = 10; // количество вершин и рёбер
ll rebrmask2, count1, count2; int maxNum;

void printG(ll mask) // вывод графа, записанного в битовой маске, в виде матрицы смежности
{
    for (int i = 1; i <= p; i++)
    {
        int lvl = ((i - 2) * (i - 1)) / 2;
        for (int j = 1; j < i; j++)
        {
            fileOut << ((mask & (1 << lvl + j - 1)) != 0) << ' ';  // проходимся до главной диагонали
        }
        fileOut << 0 << ' ';  // главная диагональ
        for (int j = i; j < p; j++)
        {

            lvl = ((j - 1) * (j)) / 2;
            fileOut << (0 != (mask & (1 << lvl + i - 1))) << ' '; // проходимся после главной диагонали
        }
        fileOut << endl;
    }
    fileOut << endl;
    count1++;
}
void destroyOneNonOrientir(ll mask, int number, int& maxNum, int numOfReb)
{
    if (mask < 0 || number >= maxNum) // невозможный случай
        return;
    destroyOneNonOrientir(mask, number + 1, maxNum, numOfReb); // вызываем функцию для такого графа, в котором ребро с номером number существует

    ll nMask = mask - (1 << maxNum - number - 1);
    if (nMask >= 0 ) // если граф после удаления ребра number существует и является связным
    {
        if (numOfReb-1 == q || q == -1) 
            printG(nMask);  // записываем его

        return destroyOneNonOrientir(nMask, number + 1, maxNum, numOfReb-1); // вызываем функцию для него, но удаляем следующее ребро
    }
}
///////////////////////////////////////////////////////////////
void printG(vector<vector<bool>> &matr) // вывод графа, записанного в матрице
{
    for (int i = 0; i < p; i++)
    {
        for (int j = 0; j < p; j++)
            fileOut << matr[i][j];
        fileOut << endl;
    }
    fileOut << endl;
    count1++;
}
void destroyOneOrientir(vector<vector<bool>> matr, int i, int j, int numOfReb)
{
    if (i == p) // невозможный случай
        return;
    if (j == p-1)
        destroyOneOrientir(matr, i+1, 0, numOfReb);
    else
        destroyOneOrientir(matr, i, j+1, numOfReb); 

    if (i != j)
    {
        matr[i][j] = false;
        numOfReb--;

        if (numOfReb == q || q == -1)
            printG(matr);  // записываем его

        if (j == p - 1)
            return destroyOneOrientir(matr, i + 1, 0, numOfReb);
        else
            return destroyOneOrientir(matr, i, j + 1, numOfReb);
    }
}
///////////////////////////////////////////////////////////////
int main()
{
    SetConsoleCP(1251); SetConsoleOutputCP(1251);  // Подключаем вывод русских букв
    int numOfAsk;
    vector<vector<bool>>matr;
    vector<char> mohOfNum;
    vector<int> removing;
    vector<bool> supVector;
    vector<char> c1,c2,c3,c4,c5,c6,a1,a2,a3,a4,a5,a6;
    char tmp;
    ifstream fileIn("input.txt");
    vector<char> startMoh = { 'a','a','a' };
    pair<char, vector<pair<char, int>>> nextMoh, numNextMoh, nextMoh1, numNextMoh1, nextMoh2, numNextMoh2, nextMoh3, nextMoh4, numNextMoh3, numNextMoh4;
    cout << "Укажите номер задачи, которую необходимо выполнить, либо введите 0 для выхода: ";
    cin >> numOfAsk;
    switch (numOfAsk)
    {
    case(0):
        return 0;
    case(1):
        fileOut.open(("dz.1.txt"));
        int main_k;
        baseMoh = {};
        tmp='/';
        cout << endl << "Вводите символы множества, последним вводится символ '.' :";
        cin >> tmp;
        while (tmp != '.')
        {
            baseMoh.push_back(tmp);
            cin >> tmp;
        }
        cout << endl << "Введите k: ";
        cin >> main_k;

        nextMoh = madeNextEltMoh(baseMoh);
        a1 = {};
        for (int i = 1; i <= main_k; i++)
            a1.push_back(takeNnextElt(nextMoh, 1));
        do
        {
            for (int i = 1; i <= main_k; i++)
            {
                fileOut << a1[i - 1];
            }
            fileOut << endl;
        } while (takeNextPost(nextMoh, a1));

        fileOut << endl << endl << endl << endl;

        a1 = {};
        for (int i = 1; i <= baseMoh.size(); i++)
            a1.push_back(takeNnextElt(nextMoh, i));
        do
        {
            for (int i = 1; i <= a1.size(); i++)
            {
                fileOut << a1[i - 1];
            }
            fileOut << endl;
        } while (takeNextPerest(nextMoh, a1));

        fileOut << endl << endl << endl << endl;

        if (baseMoh.size() < main_k)
        {
            fileOut << "размещений по данному количеству элементов не существует." << endl;
        }
        else
        {
            a1 = {};
            for (int i = 1; i <= main_k; i++)
                a1.push_back(takeNnextElt(nextMoh, i));
            do
            {
                for (int i = 1; i <= a1.size(); i++)
                {
                    fileOut << a1[i - 1];
                }
                fileOut << endl;
            } while (takeNextPerest(nextMoh, a1));
        }

        fileOut << endl << endl << endl << endl;


        for (int j = 1; j <= baseMoh.size(); j++)
        {
            a1 = {};
            for (int i = 1; i <= j; i++)
                a1.push_back(takeNnextElt(nextMoh, i));
            do
            {
                for (int i = 1; i <= a1.size(); i++)
                {
                    fileOut << a1[i - 1];
                }
                fileOut << endl;
            } while (takeNextMoh(nextMoh, a1, baseMoh.size()));
        }

        fileOut << endl << endl << endl << endl;
        
        a1 = {};
        for (int i = 1; i <= main_k; i++)
            a1.push_back(takeNnextElt(nextMoh, i));
        do
        {
            for (int i = 1; i <= a1.size(); i++)
            {
                fileOut << a1[i - 1];
            }
            fileOut << endl;
        } while (takeNextMoh(nextMoh, a1, baseMoh.size()));
    
        fileOut << endl << endl << endl << endl;

        a1 = {};
        for (int i = 1; i <= main_k; i++)
            a1.push_back(takeNnextElt(nextMoh, 1));
        do
        {
            for (int i = 1; i <= a1.size(); i++)
            {
                fileOut << a1[i - 1];
            }
            fileOut << endl;
        } while (takeNextMohPov(nextMoh, a1));

        fileOut << endl << endl << endl << endl;

        break;
    case(2):
        fileOut.open(("dz.2.txt"));
        nextMoh = madeNextEltMoh(baseMoh);
        nextMoh = deactivateElt(nextMoh, *(new vector<char>{ 'a' }));
        for (int i = 1; i <= 5; i++)
            mohOfNum.push_back(i+48);
        numNextMoh = madeNextEltMoh(mohOfNum);
        c1 = { '1', '2' };
        do// выбираем два места под а из 5
        {
            a1 = { 'b', 'b', 'b' };
            do// выбираем последовательность из оставшихся символов
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
            } while (takeNextPost(nextMoh, a1)); 
        }
        while (takeNextMoh(numNextMoh, c1, 5)); 

        fileOut << endl << endl << endl << endl;

        c1 = { '1', '2' };
        do// выбираем два места под а из 5
        {
            a1 = { 'b', 'c', 'd' };
            do// выбираем 3 символа из оставшихся символов 5
            {
                numNextMoh1 = deactivateElt(numNextMoh, c1);
                c2 = { takeNnextElt(numNextMoh1,1),takeNnextElt(numNextMoh1,2), takeNnextElt(numNextMoh1,3) };
                do// выбираем перестановку из 3-х оставшихся мест
                {
                    int iter1=0, iter2=0;
                    for (int i = 1; i <= 5; i++)
                    {
                        if (iter1 < c1.size() && i == c1[iter1]-48)
                        {
                            fileOut << 'a';
                            iter1++;
                        }
                        else
                        {
                            for (int j =0; j < c2.size(); j++)
                            {
                                if (c2[j]-48 == i)
                                {
                                    fileOut << a1[j];
                                    break;
                                }
                            }
                        }
                    }
                    fileOut << endl;
                } while (takeNextPerest(numNextMoh1, c2));
            } while (takeNextMoh(nextMoh, a1, 5)); 
        } while (takeNextMoh( numNextMoh, c1, 5)); 

        break;
    case(3):
        fileOut.open(("dz.3.txt"));
        nextMoh = madeNextEltMoh(baseMoh);
        for (int i = 1; i <= 5; i++)
            mohOfNum.push_back(i + 48);
        numNextMoh = madeNextEltMoh(mohOfNum);
        a1 = { 'a' };
        do// выбираем одну букву из 6
        {
            nextMoh1 = deactivateElt(nextMoh, a1); // деактивируем выбранную букву, пересобираем множество следующих
            c1 = { '1', '2' };
            do // выбираем два места под повторяющуся букву из 5
            {
                numNextMoh1 = deactivateElt(numNextMoh, c1);
                a2 = { takeNnextElt(nextMoh1,1),takeNnextElt(nextMoh1,2),takeNnextElt(nextMoh1,3) };
                do // выбираем оставшиеся 3 символа из 5
                {
                    c2 = { takeNnextElt(numNextMoh1,1),takeNnextElt(numNextMoh1,2),takeNnextElt(numNextMoh1,3) };
                    do // выбираем перестановку для последних 3-х символов
                    {
                        int iter1 = 0, iter2 = 0;
                        for (int i = 1; i <= 5; i++)
                        {
                            for (int j = 0; j < c2.size(); j++)
                            {
                                if (j < c2.size() && c2[j] - 48 == i)
                                {
                                    fileOut << a2[j];
                                    break;
                                }
                                else if (j < c1.size() && c1[j]-48 == i)
                                {
                                    fileOut << a1[0];
                                    break;
                                }
                            }
                        }
                        fileOut << endl;
                    } while (takeNextPerest(numNextMoh1, c2));
                } while (takeNextMoh(nextMoh1, a2, 5));
            } while (takeNextMoh(numNextMoh, c1, 5)); 
        } while (takeNextMoh(nextMoh, a1, 6)); 

        fileOut << endl << endl << endl << endl;


        for (int i = 6; i <= 6; i++)
            mohOfNum.push_back(i + 48);
        numNextMoh = madeNextEltMoh(mohOfNum);
        a1 = { 'a', 'b'};
        do// выбираем 2 буквы из 6
        {
            nextMoh1 = deactivateElt(nextMoh, a1); // деактивируем буквы, пересобираем множество следующих
            c1 = { '1', '2' };
            do // выбираем два места под 1-ю повторяющуся букву из 6
            {
                numNextMoh1 = deactivateElt(numNextMoh, c1);
                c2 = { takeNnextElt(numNextMoh1,1),takeNnextElt(numNextMoh1,2) };
                do // выбираем два места под 2-ю повторяющуся букву из 4
                {
                    numNextMoh2 = deactivateElt(numNextMoh1, c2);
                    a2 = { takeNnextElt(nextMoh1,1),takeNnextElt(nextMoh1,2) };
                    do // выбираем оставшиеся 2 символа из 5
                    {
                        c3 = { takeNnextElt(numNextMoh2,1),takeNnextElt(numNextMoh2,2) };
                        do // выбираем перестановку для последних 2-х символов
                        {
                            int iter1 = 0, iter2 = 0;
                            for (int i = 1; i <= 6; i++)
                            {
                                for (int j = 0; j < c2.size()+ c1.size()+ c3.size(); j++)
                                {
                                    if (j < c1.size() && c1[j] - 48 == i)
                                    {
                                        fileOut << a1[0];
                                        break;
                                    }
                                    else if (j < c2.size() && c2[j] - 48 == i)
                                    {
                                        fileOut << a1[1];
                                        break;
                                    }
                                    else if (j < c3.size() && c3[j] - 48 == i)
                                    {
                                    fileOut << a2[j];
                                    break;
                                    }
                                }
                            }
                            fileOut << endl;
                        } while (takeNextPerest(numNextMoh2, c3));
                    } while (takeNextMoh(nextMoh1, a2, 4));
                } while (takeNextMoh(numNextMoh1, c2, 4));
            } while (takeNextMoh(numNextMoh, c1, 6));
        } while (takeNextMoh(nextMoh, a1, 6));

        break;
    case(4):
        fileOut.open(("dz.4.txt"));
        nextMoh = madeNextEltMoh(baseMoh);
        for (int i = 1; i <= 7; i++)
            mohOfNum.push_back(i + 48);
        numNextMoh = madeNextEltMoh(mohOfNum);
        a1 = { 'a' };
        do// выбираем 1 букву из 6
        {
            nextMoh1 = deactivateElt(nextMoh, a1); // деактивируем буквы, пересобираем множество следующих
            c1 = { '1', '2' };
            do // выбираем два места под 1-ю повторяющуся букву из 7
            {
                numNextMoh1 = deactivateElt(numNextMoh, c1);
                a2 = { takeNnextElt(nextMoh1,1)};
                do // выбираем 1 символ из 5
                {
                    nextMoh2 = deactivateElt(nextMoh1, a2);
                    c2 = { takeNnextElt(numNextMoh1,1),takeNnextElt(numNextMoh1,2),takeNnextElt(numNextMoh1,3) };
                    do // выбираем 3 места под 2-ю повторяющуся букву из 5
                    {
                        numNextMoh2 = deactivateElt(numNextMoh1, c2);
                        a3 = { takeNnextElt(nextMoh2,1),takeNnextElt(nextMoh2,2) };
                        do // выбираем оставшиеся 2 символа из 4
                        {
                            c3 = { takeNnextElt(numNextMoh2,1),takeNnextElt(numNextMoh2,2) };
                            do // выбираем перестановку для последних 2-х символов
                            {
                                int iter1 = 0, iter2 = 0;
                                for (int i = 1; i <= 7; i++)
                                {
                                    for (int j = 0; j < 7; j++)
                                    {
                                        if (j < c1.size() && c1[j] - 48 == i)
                                        {
                                            fileOut << a1[0];
                                            break;
                                        }
                                        else if (j < c2.size() && c2[j] - 48 == i)
                                        {
                                            fileOut << a2[0];
                                            break;
                                        }
                                        else if (j < c3.size() && c3[j] - 48 == i)
                                        {
                                            fileOut << a3[j];
                                            break;
                                        }
                                    }
                                }
                                fileOut << endl;
                            } while (takeNextPerest(numNextMoh2, c3));
                        } while (takeNextMoh(nextMoh2, a3, 4));
                    } while (takeNextMoh(numNextMoh1, c2, 5));
                } while (takeNextMoh(nextMoh1, a2, 5));
            } while (takeNextMoh(numNextMoh, c1, 7));
        } while (takeNextMoh(nextMoh, a1, 6));

        fileOut << endl << endl << endl << endl;
        
        for (int i = 8; i <= 9; i++)
            mohOfNum.push_back(i + 48);
        numNextMoh = madeNextEltMoh(mohOfNum);
        a1 = { 'a','b' };
        do// выбираем 2 букву из 6
        {
            nextMoh1 = deactivateElt(nextMoh, a1); // деактивируем буквы, пересобираем множество следующих
            c1 = { '1', '2' };
            do // выбираем 2 места под 1-ю повторяющуся букву из 9
            {
                numNextMoh1 = deactivateElt(numNextMoh, c1);
                c2 = { takeNnextElt(numNextMoh1,1),takeNnextElt(numNextMoh1,2) };
                do // выбираем 2 места под 2-ю повторяющуся букву из 7
                {
                    numNextMoh2 = deactivateElt(numNextMoh1, c2);
                    a2 = { takeNnextElt(nextMoh1,1) };
                    do // выбираем 1 символ из 4
                    {
                        nextMoh2 = deactivateElt(nextMoh1, a2);
                        c3 = { takeNnextElt(numNextMoh2,1),takeNnextElt(numNextMoh2,2),takeNnextElt(numNextMoh2,3) };
                        do // выбираем 3 места под 3-ю повторяющуся букву из 5
                        {
                            numNextMoh3 = deactivateElt(numNextMoh2, c3);
                            a3 = { takeNnextElt(nextMoh2,1),takeNnextElt(nextMoh2,2) };
                            do // выбираем оставшиеся 2 символа из 4
                            {
                                c4 = { takeNnextElt(numNextMoh3,1),takeNnextElt(numNextMoh3,2) };
                                do // выбираем перестановку для последних 2-х символов
                                {
                                    int iter1 = 0, iter2 = 0;
                                    for (int i = 1; i <= 9; i++)
                                    {
                                        for (int j = 0; j < 9; j++)
                                        {
                                            if (j < c1.size() && c1[j] - 48 == i)
                                            {
                                                fileOut << a1[0];
                                                break;
                                            }
                                            else if (j < c2.size() && c2[j] - 48 == i)
                                            {
                                                fileOut << a1[1];
                                                break;
                                            }
                                            else if (j < c3.size() && c3[j] - 48 == i)
                                            {
                                                fileOut << a2[0];
                                                break;
                                            }
                                            else if (j < c4.size() && c4[j] - 48 == i)
                                            {
                                                fileOut << a3[j];
                                                break;
                                            }
                                        }
                                    }
                                    fileOut << endl;
                                } while (takeNextPerest(numNextMoh3, c4));
                            } while (takeNextMoh(nextMoh2, a3, 3));
                        } while (takeNextMoh(numNextMoh2, c3, 5));
                    } while (takeNextMoh(nextMoh1, a2, 4));
                } while (takeNextMoh(numNextMoh1, c2, 7));
            } while (takeNextMoh(numNextMoh, c1, 9));
        } while (takeNextMoh(nextMoh, a1, 6));

        break;
    case(5):
        fileOut.open(("dz.5.txt"));
        nextMoh = madeNextEltMoh(baseMoh);
        nextMoh = deactivateElt(nextMoh, *(new vector<char>{ 'a' }));
        for (int i = 1; i <= 4; i++)
            mohOfNum.push_back(i + 48);
        numNextMoh = madeNextEltMoh(mohOfNum);
        for (int count_of_letter = 2; count_of_letter <= 4; count_of_letter++)
        {
            c1 = {};
            for (int i = 1; i <= count_of_letter; i++)
            {
                c1.push_back(takeNnextElt(numNextMoh, i));
            }
            do// выбираем count_of_letter места под а из 5
            {
                a1 = {};
                for (int i = 1; i <= 4-count_of_letter; i++)
                {
                    a1.push_back(takeNnextElt(nextMoh, 1));
                }
                do// выбираем последовательность из оставшихся символов
                {
                    int kc1 = 0;
                    int ka1 = 0;
                    for (int i = 1; i <= 4; i++)
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
                } while (takeNextPost(nextMoh, a1));
            } while (takeNextMoh(numNextMoh, c1, 4));
        }

        fileOut << endl << endl << endl << endl;

        nextMoh = madeNextEltMoh(baseMoh);
        nextMoh = deactivateElt(nextMoh, *(new vector<char>{ 'a' }));
        for (int i = 5; i <= 7; i++)
            mohOfNum.push_back(i + 48);
        numNextMoh = madeNextEltMoh(mohOfNum);
        for (int count_of_letter = 2; count_of_letter <= 7; count_of_letter++)
        {
            c1 = {};
            for (int i = 1; i <= count_of_letter; i++)
            {
                c1.push_back(takeNnextElt(numNextMoh, i));
            }
            do// выбираем count_of_letter места под а из 7
            {
                a1 = {};
                for (int i = 1; i <= 7 - count_of_letter; i++)
                {
                    a1.push_back(takeNnextElt(nextMoh, 1));
                }
                do// выбираем последовательность из оставшихся символов
                {
                    int kc1 = 0;
                    int ka1 = 0;
                    for (int i = 1; i <= 7; i++)
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
                } while (takeNextPost(nextMoh, a1));
            } while (takeNextMoh(numNextMoh, c1, 7));
        }
        break;
    case(6):
        fileOut.open(("dz.6.txt"));
        nextMoh = madeNextEltMoh(baseMoh);
        for (int i = 1; i <= 7; i++)
            mohOfNum.push_back(i + 48);
        numNextMoh = madeNextEltMoh(mohOfNum);
        // Так как имеется лишь 3 варианта расположения
        //1) 4 1 1 1
        
        a1 = { 'a' };
        do// выбираем 1 букву из 6
        {
            nextMoh1 = deactivateElt(nextMoh, a1); // деактивируем буквы, пересобираем множество следующих
            c1 = { '1', '2', '3','4'};
            do // выбираем 4 места под 1-ю повторяющуся букву из 7
            {
                numNextMoh1 = deactivateElt(numNextMoh, c1);
                a2 = { takeNnextElt(nextMoh1,1), takeNnextElt(nextMoh1,2), takeNnextElt(nextMoh1,3) };
                do // выбираем 3 символа из 5
                {
                    nextMoh2 = deactivateElt(nextMoh1, a2);
                    c2 = { takeNnextElt(numNextMoh1,1) };
                    do // выбираем 1 местo под 2-ю повторяющуся букву
                    {
                        numNextMoh2 = deactivateElt(numNextMoh1, c2);
                        c3 = { takeNnextElt(numNextMoh2,1) };
                        do // выбираем 1 местo под 3-ю повторяющуся букву
                        {
                            numNextMoh3 = deactivateElt(numNextMoh2, c3);
                            c4 = { takeNnextElt(numNextMoh3,1) };
                            do // выбираем 1 местo под 4-ю повторяющуся букву
                            {
                                for (int i = 1; i <= 7; i++)
                                {
                                    for (int j = 0; j < 7; j++)
                                    {
                                        if (j < c1.size() && c1[j] - 48 == i)
                                        {
                                            fileOut << a1[0];
                                            break;
                                        }
                                        else if (j < c2.size() && c2[j] - 48 == i)
                                        {
                                            fileOut << a2[0];
                                            break;
                                        }
                                        else if (j < c3.size() && c3[j] - 48 == i)
                                        {
                                            fileOut << a2[1];
                                            break;
                                        }
                                        else if (j < c4.size() && c4[j] - 48 == i)
                                        {
                                            fileOut << a2[2];
                                            break;
                                        }
                                    }
                                }
                                fileOut << endl;
                            } while (takeNextMoh(numNextMoh3, c4, 1));
                        } while (takeNextMoh(numNextMoh2, c3, 2));
                    } while (takeNextMoh(numNextMoh1, c2, 3));
                } while (takeNextMoh(nextMoh1, a2, 5));
            } while (takeNextMoh(numNextMoh, c1, 7));
        } while (takeNextMoh(nextMoh, a1, 6));

        fileOut << endl;
        //2) 3 2 1 1
        
        a1 = { 'a' };
        do// выбираем 1 букву из 6
        {
            nextMoh1 = deactivateElt(nextMoh, a1); // деактивируем буквы, пересобираем множество следующих
            c1 = { '1', '2', '3' };
            do // выбираем 3 места под 1-ю повторяющуся букву из 7
            {
                numNextMoh1 = deactivateElt(numNextMoh, c1);
                a2 = { takeNnextElt(nextMoh1,1) };
                do // выбираем 1 символ из 5
                {
                    nextMoh2 = deactivateElt(nextMoh1, a2);
                    a3 = { takeNnextElt(nextMoh2,1), takeNnextElt(nextMoh2,2)};
                    do // выбираем 2 символа из 5
                    {
                        c2 = { takeNnextElt(numNextMoh1,1),takeNnextElt(numNextMoh1,2) };
                        do // выбираем 2 местo под 2-ю повторяющуся букву
                        {
                            numNextMoh2 = deactivateElt(numNextMoh1, c2);
                            c3 = { takeNnextElt(numNextMoh2,1) };
                            do // выбираем 1 местo под 3-ю повторяющуся букву
                            {
                                numNextMoh3 = deactivateElt(numNextMoh2, c3);
                                c4 = { takeNnextElt(numNextMoh3,1) };
                                do // выбираем 1 местo под 4-ю повторяющуся букву
                                {
                                    for (int i = 1; i <= 7; i++)
                                    {
                                        for (int j = 0; j < 7; j++)
                                        {
                                            if (j < c1.size() && c1[j] - 48 == i)
                                            {
                                                fileOut << a1[0];
                                                break;
                                            }
                                            else if (j < c2.size() && c2[j] - 48 == i)
                                            {
                                                fileOut << a2[0];
                                                break;
                                            }
                                            else if (j < c3.size() && c3[j] - 48 == i)
                                            {
                                                fileOut << a3[0];
                                                break;
                                            }
                                            else if (j < c4.size() && c4[j] - 48 == i)
                                            {
                                                fileOut << a3[1];
                                                break;
                                            }
                                        }
                                    }
                                    fileOut << endl;
                                } while (takeNextMoh(numNextMoh3, c4, 1));
                            } while (takeNextMoh(numNextMoh2, c3, 2));
                        } while (takeNextMoh(numNextMoh1, c2, 4));
                    } while (takeNextMoh(nextMoh2, a3, 4));
                } while (takeNextMoh(nextMoh1, a2, 5));
            } while (takeNextMoh(numNextMoh, c1, 7));
        } while (takeNextMoh(nextMoh, a1, 6));

        fileOut << endl;
        //3) 2 2 2 1

        a1 = { 'a', 'b', 'c'};
        do// выбираем 3 букву из 6
        {
            nextMoh1 = deactivateElt(nextMoh, a1); // деактивируем буквы, пересобираем множество следующих
            c1 = { '1', '2'};
            do // выбираем 2 места под 1-ю повторяющуся букву из 7
            {
                numNextMoh1 = deactivateElt(numNextMoh, c1);
                a2 = { takeNnextElt(nextMoh1,1) };
                do // выбираем 1 символ из 3
                {
                    c2 = { takeNnextElt(numNextMoh1,1),takeNnextElt(numNextMoh1,2) };
                    do // выбираем 2 местo под 2-ю повторяющуся букву
                    {
                        numNextMoh2 = deactivateElt(numNextMoh1, c2);
                        c3 = { takeNnextElt(numNextMoh2,1), takeNnextElt(numNextMoh2,2) };
                        do // выбираем 2 местa под 3-ю повторяющуся букву
                        {
                            numNextMoh3 = deactivateElt(numNextMoh2, c3);
                            c4 = { takeNnextElt(numNextMoh3,1) };
                            do // выбираем 1 местo под 4-ю повторяющуся букву
                            {
                                for (int i = 1; i <= 7; i++)
                                {
                                    for (int j = 0; j < 7; j++)
                                    {
                                        if (j < c1.size() && c1[j] - 48 == i)
                                        {
                                            fileOut << a1[0];
                                            break;
                                        }
                                        else if (j < c2.size() && c2[j] - 48 == i)
                                        {
                                            fileOut << a1[1];
                                            break;
                                        }
                                        else if (j < c3.size() && c3[j] - 48 == i)
                                        {
                                            fileOut << a1[2];
                                            break;
                                        }
                                        else if (j < c4.size() && c4[j] - 48 == i)
                                        {
                                            fileOut << a2[0];
                                            break;
                                        }
                                    }
                                }
                                fileOut << endl;
                            } while (takeNextMoh(numNextMoh3, c4, 1));
                        } while (takeNextMoh(numNextMoh2, c3, 3));
                    } while (takeNextMoh(numNextMoh1, c2, 5));
                } while (takeNextMoh(nextMoh1, a2, 3));
            } while (takeNextMoh(numNextMoh, c1, 7));
        } while (takeNextMoh(nextMoh, a1, 6));

        break;
    case(7):
        fileOut.open(("dz.7.txt"));
        cout << endl << "Введите порядок графа: ";
        cin >> p;
        cout << endl << "Введите количество рёбер: ";
        cin >> q;

        count1 = 0;
        maxNum = ((p - 1) * (p)) / 2;
        rebrmask2 = (1 << maxNum) - 1;
        if (q == maxNum)
            printG(rebrmask2);
        destroyOneNonOrientir(rebrmask2, 0, maxNum, maxNum);
        fileOut << endl << endl << endl << endl << endl << endl << endl << endl;
        cout << endl << "Количество графов данного порядка с данными рёбрами: " << count1;
        q = -1; count1 = 0;

        printG(rebrmask2);
        destroyOneNonOrientir(rebrmask2, 0, maxNum, maxNum);
        cout << endl << "Количество графов данного порядка: " << count1;
        break;
    case(8):
        fileOut.open(("dz.8.txt"));
        fileIn >> p;
        cout << "Граф и перестановка введёны из файла input.txt" << endl;

        for (int i = 0; i < p; i++)
        {
            matr.push_back(*(new vector<bool>));
            for (int j = 0; j < p; j++)
            {
                int tmp; fileIn >> tmp;
                if (tmp == 1)
                    matr[i].push_back(true);
                else
                    matr[i].push_back(false);
            }
        }


        for (int i = 0; i < p; i++)
        {
            int tmp; fileIn >> tmp;
            removing.push_back(tmp-1);
            supVector.push_back(true);
        }
        for (int i = 0; i < p; i++)
        {
            if (removing[i] < p && removing[i] >= 0 && supVector[removing[i]])
                supVector[removing[i]] = false;
            else
            {
                fileOut.close();
                cout << "Неправильная перестановка!"; return 1;
            }
        }

        for (int i = 0; i < p; i++)
        {

            for (int j = 0; j < p; j++)
            {
                if (!(matr[i][j] == matr[removing[i]][removing[j]]))
                {
                    cout << "Данная перестановка не автоморфизм!";
                    fileOut << "Нет";
                    fileOut.close();
                    return 1;
                }
            }
        }
        cout << "Данная перестановка автоморфизм!";
        fileOut << "Да";
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
        cout << endl << "Введите порядок графа: ";
        cin >> p;
        cout << endl << "Введите количество рёбер: ";
        cin >> q;

        count1 = 0;

        for (int i = 0; i < p; i++)
        {
            matr.push_back(*(new vector<bool>));
            for (int j = 0; j < p; j++)
                if (i != j)
                    matr[i].push_back(true);
                else
                    matr[i].push_back(false);
        }

        if (q == p * p - p)
            printG(rebrmask2);
        destroyOneOrientir(matr, 0, 0, p * p - p);
        cout << endl << "Количество графов данного порядка с данными рёбрами: " << count1;

        break;
    case(12):
        fileOut.open(("dz.12.txt"));
        cout << endl << "Введите порядок графа: ";
        cin >> p;
        q = -1;

        count1 = 0;

        for (int i = 0; i < p; i++)
        {
            matr.push_back(*(new vector<bool>));
            for (int j = 0; j < p; j++)
                if (i != j)
                    matr[i].push_back(true);
                else
                    matr[i].push_back(false);
        }

        printG(rebrmask2);
        destroyOneOrientir(matr, 0, 0, p * p - p);
        cout << endl << "Количество графов данного порядка: " << count1;
        break;
    default:
        cout << "Задачи с таким номером не существует." << endl;
        break;
    }
    fileOut.close();
}