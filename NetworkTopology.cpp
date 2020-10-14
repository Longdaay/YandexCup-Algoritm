﻿/*Задача Е. ТОПОЛОГИЯ СЕТИ.
*    Ограничение времени    3 секунды
*    Ограничение памяти	    512Mb
*    Ввод	                стандартный ввод или input.txt
*    Вывод	                стандартный вывод или output.txt
* 
* Условие задачи:
* Распределённая сеть Александра состоит из n вычислительных узлов, соединённых с помощью помощью n − 1 кабелей.
* Каждый кабель соединяет ровно два различных узла, при этом любые два узла соединены кабелем напрямую, либо через цепочку промежуточных узлов.
* Александр очень переживает за сохранность данных в системе, поэтому хочет установить дополнительные жесткие диски на два компьютера-хранилища.
* Расстоянием между двумя узлами Александр называет минимальное количество соединений на цепочке от одного узла к другому. После выбора узлов для
* установки дополнительных хранилищ, для каждого узла сети Александр определяет ближайшее к нему хранилище. 
* Ненадёжностью сети он называет максимальное значение этой величины по всем узлам.
* Помогите Александру, сообщите, на какие различные компьютеры необходимо установить дополнительные жесткие диски, чтобы ненадёжность сети была минимальна.
* 
* Формат ввода:
* В первой строке входных данных записано одно целое число n(2 ≤ n ≤ 200000) — количество компьютеров в системе Александра.
* Далее в n − 1 строках записаны по два целых числа xy (1 ≤ x, y ≤ n, x ≠ y) — номера компьютеров, соединенных кабелем
* Формат вывода:
* В единственной строке выведите номера двух различных выбранных компьютеров. Если существует несколько решений, удовлетворяющих условию задачи, то выведите любое из них.
*/

/*
* Логика решения построена на наблюдениях, в которых потенциальные вершины для хранилищ имеют
* глубину(максимальное расстояние до крайних точек) больше чем у центра графа на 2
* Однако такое решение не верно
* Это практически классическая минимаксная задача размещения, только в интернете решают её для 1 центра, а мы решаем
* для двух центров(часто приводят в пример задачу как разместить центры МЧС или Скорой помощи в городе, чтобы сократить
* максимальное расстояние до каждого жителя)
* Для решения задачи ключевым является алгоритм Дейкстры для поиска самого короткого пути от целевой вершины графа к остальным
*/
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <set>
#include <map>

#define MAXWEIGHTVALUE 10000

using namespace std;
int max_depth = 0;
vector<int> other_centres_indexes_list;

class Topology {
public:
    Topology();
    void setComputersConnectionsMatrix();
    void checkAllComputers();
    vector<int> Dijkstra(int begin_index, int trigger);
    void checkAllpairStorage(vector<int>& potentionalStorage);
    void findPotentionalComputerStorage(vector<vector<int>>& lits_computerDepth);
    int get_total() { return total_computers; }
private:
    int total_computers;
    vector <vector<int>> computersConnectionsMatrix;
};

Topology::Topology() { // конструктор
    setComputersConnectionsMatrix();
}
void Topology::setComputersConnectionsMatrix() {
    /*
    * Запись элементов из текстового файла в программу, для создания матрицы смежности
    * Сначала считывается количество компьютеров, а дальше записываются отдельные связи двух компьютеров.
    * Количество связей определяется количеством компьютеров - 1. Не допускается зацикливание связей (т.е. 1 1 и пр.) 
    * Значения элементов должны быть в диапазоне от 1 до количества компьютеров.
    * Примерный ввод элементов выглядит так:
    * 3
    * 1 2
    * 2 3
    */
    ifstream in; // открываем поток для считывания данных
    vector<int> ComputersConnection; // вектор для одного соединения
    int tempComputer, iterator = 0;
    char t;
    
    in.open("input.txt"); // открываем файл
    in >> total_computers; // получаем количество узлов

    computersConnectionsMatrix.resize(total_computers);// расширяем матрицу смежности узлов
    for (int i = 0; i < total_computers; i++)//
        computersConnectionsMatrix[i].resize(total_computers);//

    /*
    * запись из файла происходит следующим образом: считывается первое значение (количество компьютеров)
    * дальше считывается следующий символ строки. Затем организовывается цикл. Пока следующий символ не будет равен переносу строки, продолжаем считывать из файла соединение.
    * В цикле первоочередно проверяется достигли ли конца файла. Если достигли, то присваиваем конец строки и выходим из цикла. Затем в переменные записываются значения компьютеров - 1.
    * После этого заполняется матрица смежности. 1 - обозначет, что связь между двумя компьютерами есть. Условием не оговорено, что номера компьютеров могут больше количества компьютеров.
    * Поэтому логика жестко связана на этих номерах.
    */
    while (iterator < total_computers - 1) { // пока не дошли до конечного соединения
        in >> tempComputer; // считываем первый узел
        ComputersConnection.push_back(tempComputer); //  в массив
        in.get(t); // считываем символ
        while (t != '\n') { // пока символ не конец строки
            if (!in.eof()) { // если не достигли конца файла
                in >> tempComputer;  // считываем второй узел
                ComputersConnection.push_back(tempComputer); // в массив
                in.get(t); // считываем символ
            }
            else // иначе
                t = '\n'; // присваиваем конец строки
        }
        int first = ComputersConnection[0] - 1; // первая позиция
        int second = ComputersConnection[1] - 1; // вторая позиция
        computersConnectionsMatrix[first][second] = 1; // заполнили матрицу смежности
        computersConnectionsMatrix[second][first] = 1; //

        iterator++; // добавили к итератору
        ComputersConnection.clear();
    }
    in.close();
}


vector<int> Topology::Dijkstra(int begin_index, int trigger) {
    /*
    * Алгоритм Дейкстры. Используется лишь часть алгоритма, а именно расчет всех путей от заданной точки ко всем точкам в графе. Алгоритм работает со взвешенными графами,
    * значения у которых неотрицательны. Дополнительное условие состоит в установке, так называемого, триггера - узла, через который пройти нельзя.
    * Алгоритм заключается в следующем: 
    *   Инициализация всех расстояний компьютера максимальным значением, кроме искомого (у искомого расстояние = 0)
    *   Поиск текущего минимального непосещенного веса компьютера и присваивание его номера и расстоянния
    *   Поиск связей этого компьютера с другими и сравнение полученного сложение путей к связному компьютеру и исходное расстояние связного компьютера
    *   Помечаем этот компьютер как посещенный и возвращаемся в цикл снова.
    *   Сортировка полученных расстояний
    *   Поиск максимального расстояния.
    */   

    vector <int> countSteps_computerToAll;// список, размерностью количества компьютеров, который хранит пути от заданной точке ко всем.
    countSteps_computerToAll.resize(computersConnectionsMatrix.size());
    vector <int> visited_computers_list; //Есть список посещенных компьютеров, который необходим для обработки всех компьютеров(1 - компьютер не был посещен).
    visited_computers_list.resize(computersConnectionsMatrix.size());
    int temp, minValueIndex, minWeightToComputer;
    int i = 1;



    // Первоначально необходимо расстояние до каждого элемента указать как максимально большое значение (в идеале бесконечность);
    // постепенно эти значения будут заменятся на корректные значения расстояний.
    for (int i = 0; i < computersConnectionsMatrix.size(); i++) {
        countSteps_computerToAll[i] = MAXWEIGHTVALUE;
        visited_computers_list[i] = 1;
    }
    countSteps_computerToAll[begin_index] = 0; // После этого необходимо указать расстояние у искомой точки равное 0 (т.е. мы уже в ней находимся, расстояние 0)
    visited_computers_list[trigger] = 0;

    /*
    * Дальше задается цикл с постусловием, в котором первом делом инициализируем максимальными значениями номер компьютера с минимальным значением, а потом и расстояние этого
    * компьютера. Далее запускаем цикл по поиску непосещенной минимальной вершины (в 1 раз это будет начальная точка - дальше зависит от весов компьютеров)
    * Если компьютер не был еще посещен и его значение расстояния меньше, чем номинальное, то мы присваиваем его номер и его расстояние соответсвующим переменным.
    * затем мы снова задаем цикл, в котором производим поиск связей этого компьютера с другими и производим сравнение полученного сложение путей к связному компьютеру и исходное расстояние связного компьютера
    * Проходим этим циклом по всем связей такого компьютера и после помечаем этот компьютер как посещенный и возвращаемся в цикл снова
    */
    do {
        minValueIndex = MAXWEIGHTVALUE; // задаем максимально возможное значение индексу
        minWeightToComputer = MAXWEIGHTVALUE; // задаем максимально возможное значение расстояние
        for (int i = 0; i < computersConnectionsMatrix.size(); i++) { // проходим циклом по всем компьютерам
            if ((visited_computers_list[i] == 1) && (countSteps_computerToAll[i] < minWeightToComputer)) { // если нашли непосещенный компьютер с минимальным весом, то присваиваем его номер и его расстояние соответсвующим переменным
                minWeightToComputer = countSteps_computerToAll[i]; // минимальное расстояние такого компьютера
                minValueIndex = i; // индекс такого компьютера
            }
        }

        // цикл поиска связей этого компьютера с другими
        if (minValueIndex != MAXWEIGHTVALUE) { // проверка на конец просчета всех путей (исходя из предыдущего цикла - если все вершины будут посещены, то присваиваемое значение максимально возможное не изменится)
            for (int i = 0; i < computersConnectionsMatrix.size(); i++) {
                if (trigger == i)
                    continue;
                if (computersConnectionsMatrix[minValueIndex][i] > 0) { // если есть связь с другим компьютером
                    temp = minWeightToComputer + computersConnectionsMatrix[minValueIndex][i]; // Добавляем найденный минимальный вес к текущему весу вершины
                    if (temp < countSteps_computerToAll[i]) //и сравниваем с текущим минимальным весом вершины
                        countSteps_computerToAll[i] = temp; // если меньше, то присваиваем
                }
            }
            visited_computers_list[minValueIndex] = 0; // помечаем компьютер как посещенный
        }
    } while (minValueIndex < MAXWEIGHTVALUE); // пока не прошли все компьютеры

    //* Сортировка полученных расстояний
    //sort(countSteps_computerToAll.begin(), countSteps_computerToAll.begin() + countSteps_computerToAll.size());

    //* Поиск максимального расстояния. Загвоздка заключается в следующем. По причине наличия триггера расстояния до компьютеров, где триггер был элементом в цепочке,
    //* равны максимально возможному значению. Поэтому необходимо организовать цикл в котором следует проверят текущий элемент с максимально возможным значением.
    //* если он равен, итератору добавляем шаг + 1. Как только мы находим значение не равное максимально возможному значению, выходим из цикла и выводим максимальный элемент.
    return countSteps_computerToAll;
}

void Matrixrecursion(int currentComp, vector<vector<int>>& MainMatrix, vector<int>& computers_visited, int depth) {
    /*
    * Рекурсивная функция поиска глубины графа из начальной точки. Функция обходит каждый элемент в текущей строке матрицы смежности
    * и проверяет есть ли связь с следующим компьютером. Если такая связь есть проверяется был ли этот компьютер до этого посещен.
    * Если нет, то помечаем этот компьютер как посещенным и запускаем рекурсивную функцию заново с новой строкой и глубина увеличивается на 1.
    * Иначе цикл движется дальше по элементам.
    * После того как цикл прошел все элементы в строке, сравнивается максимально полученная глубина с текущей. Если текущая больше, то максимальной глубине
    * присваивается текущая.
    */
    for (int j = 0; j < MainMatrix.size(); j++) { // идем циклом по всем элементам в строке
        if (MainMatrix[currentComp][j] == 1) { // проверка наличия свзяи со другим компьютером
            if (!computers_visited[j]) { // проверка был ли это компьютер посещен
                computers_visited[j] = 1; // помечаем компьютер посещенным
                Matrixrecursion(j, MainMatrix, computers_visited, depth + 1); // запускаем рекурсию с новой строкой и с увеличенным значением глубины
            }
        }
    }
    max_depth < depth ? max_depth = depth : max_depth; // проверка максимальной и текущей
}

void other_centres_finder_recursion(int center_index, vector<vector<int>>& MainMatrix, vector<int>& computers_visited, int distance_to_other_centres, int depth) {

    if (depth == distance_to_other_centres) {
        other_centres_indexes_list.push_back(center_index);
        return;
    }

    for (int j = 0; j < MainMatrix.size(); j++) { // идем циклом по всем элементам в строке
        if (MainMatrix[center_index][j] == 1) { // проверка наличия свзяи со другим компьютером
            if (!computers_visited[j]) { // проверка был ли это компьютер посещен
                computers_visited[j] = 1; // помечаем компьютер посещенным
                other_centres_finder_recursion(j, MainMatrix, computers_visited, distance_to_other_centres, depth + 1); // запускаем рекурсию с новой строкой и с увеличенным значением глубины
            }
        }
    }
}

void printMatrix(vector<vector<int>>& matrix) {
    for (auto& token : matrix) {
        for (auto& element : token) {
            cout << element << " \t";
        }
        cout << endl;
    }
}

void Topology::findPotentionalComputerStorage(vector<vector<int>>& lits_computerDepth) {
    /*
    * поиск потенциальных компьютеров-хранилиш. Для этого создается список, который будет хранить глубины и номера таких компьютеров.
    * Также необходимо значение (триггер), которое будет определять сколько неповторяющих значений глубины компьютеров потенциально подходят как хранилища.
    * К примеру, если значения глубин 4, 5, 5, 6, 7, 8, а триггер равен 2, то будут выбраны компьютеры с глубиной 4, 5, 5.
    * Если триггер будет равен 3, то будут выбраны компьютеры 4, 5, 5, 6.
    * После получения списка потенциальных хранилищ отправляемся в функцию проверки пар этих хранилищ для поиска минимальной ненадежности сети
    */
    vector<int> centres_list;
    int DEPTH = 1;
    int distance_to_other_centres;
    vector<int> computersVisited_list(computersConnectionsMatrix.size(), 0);
    vector<int> other_centres_for_indexation;

    //cout << "Potential Storage:\n";
    //printMatrix(lits_computerDepth);
    if (lits_computerDepth[0][1] != lits_computerDepth[1][1])
        centres_list.push_back(lits_computerDepth[0][1]);
    else {
        centres_list.push_back(lits_computerDepth[0][1]);
        centres_list.push_back(lits_computerDepth[1][1]);
    }

    distance_to_other_centres = lits_computerDepth[0][0] / 2;
    
    for (int i = 0; i < centres_list.size(); i++) {
        computersVisited_list.clear(); // очистка списка посещенных компьютеров
        computersVisited_list.resize(computersConnectionsMatrix.size()); // расширение списка до количества компьютеров
        computersVisited_list[centres_list[i]] = true; // помечаем текущий компьютер посещенным
        other_centres_finder_recursion(centres_list[i], computersConnectionsMatrix, computersVisited_list, distance_to_other_centres, 0);
    }
    other_centres_for_indexation = other_centres_indexes_list;
    for (int i = 0; i < other_centres_for_indexation.size(); i++) {
        computersVisited_list.clear(); // очистка списка посещенных компьютеров
        computersVisited_list.resize(computersConnectionsMatrix.size()); // расширение списка до количества компьютеров
        computersVisited_list[other_centres_for_indexation[i]] = true; // помечаем текущий компьютер посещенным
        other_centres_finder_recursion(other_centres_for_indexation[i], computersConnectionsMatrix, computersVisited_list, DEPTH, 0);
    }
    checkAllpairStorage(other_centres_indexes_list);
}

void Topology::checkAllComputers() {
    /*
    * проверка все компьютеров графа на их глубину. Для этого создается массив, который будет хранить глубины и номера этих компьютеров,
    * а также одномерный массив, размерностью количества компьютеров, в который будет после каждой итерации записываться посещенный компьютер.
    * Организовывается цикл по количеству компьютеров, в котором очищается массив посещенных компьютеров, запись текущего компьютера как посещенного.
    * Затем запускается рекурсивная функция поиска глубины от этого компьютера и текущем значением глубины 0. После в массив глубина - компьютер записывается
    * полученное значение и номер компьютера.
    * После прохода всех компьютеров массив сортируется по возрастанию глубины и отправляется в функию нахождения потенциальных компьютеров-хранилищ
    */
    vector<vector<int>> depthValueComputer_list; // массив, который будет хранить глубины и номера этих компьютеров
    vector<int> computersVisited_list(computersConnectionsMatrix.size(), 0); // массив посещенных компьютеров

    for (int i = 0; i < total_computers; i++) { // цикл по всем компьютерам
        computersVisited_list.clear(); // очистка списка посещенных компьютеров
        computersVisited_list.resize(computersConnectionsMatrix.size()); // расширение списка до количества компьютеров
        computersVisited_list[i] = true; // помечаем текущий компьютер посещенным
        Matrixrecursion(i, computersConnectionsMatrix, computersVisited_list, 0); // рекурсивная функция поиска глубины от текущего компьютера
        depthValueComputer_list.push_back({ max_depth, i }); // записываем значение глубины и номер компьютера
        max_depth = 0; // обнуляем значение глубины для дальнейших просчетов
    }
    sort(depthValueComputer_list.begin(), depthValueComputer_list.begin() + depthValueComputer_list.size()); // сортируем в порядке возрастания элементы списка
    findPotentionalComputerStorage(depthValueComputer_list); // функция нахождения потенциальных компьютеров-хранилищ
}

bool checkPairCompsInList(const vector<vector<int>>& pairList, vector<int> potentionalPair) {
    /*
    * проверка пары компьютеров на уникальность в списке пар. Прогоняется массив по первому элементу пары, а дальше поиск по второму элементу пары
    * если найдена - true, если не найдена пара - false
    */
    for (int i = 0; i < pairList.size(); i++) {
        if (pairList[i][0] == potentionalPair[0])
            if (pairList[i][1] == potentionalPair[1])
                return true;
    }
    return false;
}

vector <int> clear_list_from_nonequial_numbers(vector<int>& potentialStorage) {
    set <int> list_equial_numbers;
    vector <int> list_equial_numbers_vec;
    int previous_size;
    int last_value_list_storage;

    while (potentialStorage.size()) {
        previous_size = list_equial_numbers.size();
        last_value_list_storage = potentialStorage[potentialStorage.size() - 1];
        potentialStorage.pop_back();
        list_equial_numbers.insert(last_value_list_storage);
        if (previous_size != list_equial_numbers.size())
            list_equial_numbers_vec.push_back(last_value_list_storage);
    }
    return list_equial_numbers_vec;
}

void Topology::checkAllpairStorage(vector<int>& potentialStorage) {
    potentialStorage = clear_list_from_nonequial_numbers(potentialStorage);
    /*
    * проверка всех пар потенциальных хранилищ на минимальную ненадежность сети. Логика заключается в следующем: перебор всех неповторяющихся пар хранилищ и получение их максимальной ненадежности.
    * запись этих значений и пар хранилищ в словарь и выбор минимального ключа словаря. Это и будет пара компьютеры, где наиболее выгодно можно расположить хранилища.  
    * Для этого необходимо создать список, в котором будут записываться пары компьютеров. Организовывается цикл и вложенный цикл для перебора пар элементов. Если элементы совпали по значениям, то цикл переходит
    * на новую итерацию. В переменные записываются номера компьютеров и проверяется пара на уникальность в списке пар. Проверка реализована в другой функции - результат ее работы: true - пара такая есть, false - пары такой нет.
    * Если текущей пары еще нет в списке пар, то добавляем ее в двух комбинациях : (1элем, 2элем) и (2элем, 1элем). После расчитываем ненадежность для каждого из компьютеров с помощью Алгоритма Дейкстры с триггером на партнера.
    * Дальше из этих двух полученных ненадежностей выбирается максимальная и записывается как словарь как: ненадежность : пара хранилищ. Далее выбирается следующая пара храниилищ.
    * 
    * После прохода по всем парам хранилищ и запись их в словарь, выбирается минимальмальный ключ и выводится соответствующая ему пара компьютеров.
    */
    vector<vector<int>> pairList; // список пар компьютеров - хранилищ
    map<int, pair <int, int>> map_reliability_to_pair_computers; // словарь ненадежности пар компьютеров (ненадежность : пара хранилищ)
    map <int, pair<int, int>> ::iterator it; // итератор по словарю
    vector<int> reliabilityValueFirstComputer, reliabilityValueSecondComputer; // переменные для записи ненадежности 
    int firstPotentialStorage, secondPotentialStorage; // переменные для записи номеров компьютеров
    vector<int> best_reliability_list;

    for (int i = 0; i < potentialStorage.size(); i++) {
        for (int j = 0; j < potentialStorage.size(); j++) {
            if (i == j) // если одинаковые - переходим на следующую итерацию
                continue;
            firstPotentialStorage = potentialStorage[i]; // записываем первый компьютер
            secondPotentialStorage = potentialStorage[j]; // записываем второй компьютер
            if (!checkPairCompsInList(pairList, { firstPotentialStorage, secondPotentialStorage })) { // если такой пары нет, то записываем ее в список пар
                pairList.push_back({ firstPotentialStorage , secondPotentialStorage }); // записываем пару 1,2
                pairList.push_back({ secondPotentialStorage , firstPotentialStorage }); // записываем пару 2,1
                reliabilityValueFirstComputer = Dijkstra(firstPotentialStorage, secondPotentialStorage); // ненадежность первого компьютера
                reliabilityValueSecondComputer = Dijkstra(secondPotentialStorage, firstPotentialStorage); // ненадежность второго компьютера 
                best_reliability_list.clear();
                for (int i = 0; i < reliabilityValueFirstComputer.size(); i++) {
                    if (reliabilityValueFirstComputer[i] < reliabilityValueSecondComputer[i])
                        best_reliability_list.push_back(reliabilityValueFirstComputer[i]);
                    else
                        best_reliability_list.push_back(reliabilityValueSecondComputer[i]);
                }
                sort(best_reliability_list.begin(), best_reliability_list.begin() + best_reliability_list.size());
                map_reliability_to_pair_computers[best_reliability_list[best_reliability_list.size() - 1]] = { firstPotentialStorage , secondPotentialStorage }; // записываем в словарь максимальное из значений ненадежности как ключ и его пару хранилищ
            }

        }
    }
    it = map_reliability_to_pair_computers.begin(); // задаем итератору минимальный элемент словаря
    cout << (*it).second.first + 1 << " " << (*it).second.second + 1; // выводим пару компьютеров 
}

int main() {
    setlocale(0, "");
    Topology tp;
    if (tp.get_total() == 2)
        cout << 1 << " " << 2;
    else
        tp.checkAllComputers();
    return 0;
}