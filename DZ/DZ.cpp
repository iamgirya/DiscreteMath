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
vector<char> baseMoh = { 'a','b','c','d','e','f'};
// множество состоит из пар элемента и булевого значения, включён ли символ. Она используется для запрещения выбора элементов
pair<char, vector<pair<char, int>>> madeNextEltMoh(vector<char> &alphabet) 
// создаёт пару из первого элемента алфавита и массива для алфавита, в котором для каждого символа
// элемент под индексом этого символа равен паре следующего символа в множестве и номеру нынешнего символа(начиная с 1)
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


int main()
{
    SetConsoleCP(1251); SetConsoleOutputCP(1251);  // Подключаем вывод русских букв
    int numOfAsk;
    vector<char> mohOfNum;
    vector<char> c1,c2,c3,c4,c5,c6,a1,a2,a3,a4,a5,a6;
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
        cout << "Данная задача ещё не готова." << endl;
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