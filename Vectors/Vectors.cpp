#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <execution>
#include <stdio.h>
#include "omp.h"


using namespace std;


// Функция, возвращающая вектор заполненный рандомными числами
vector<int> getVector(int size) {

    vector<int> new_vector;
    int value = 0;
    for (int i = 0; i < size; i++) {
        value = 1 + rand() % 100;
        new_vector.push_back(value);
    }
    return new_vector;
}

// Фукнция, возврающая определенный диапозон вектора
vector<int> share_vector(int begin, int end, vector<int> input_vector) {
    vector<int> out_vector;

#pragma omp parralel for
    for (int i = begin; i <= end; i++) {
        out_vector.push_back(input_vector[i]);
    }

    return out_vector;
}

// Не распараллелинная функция проверки доминантности векторов
void vector_dominance(vector<int> first_vector, vector<int> second_vector)
{
    bool check = true;

    auto start_time = chrono::steady_clock::now();

    first_vector[0] > second_vector[0] ? check = true : check = false;

    if (check) {
        for (int i = 0; i < first_vector.size(); i++) {
            first_vector[i] > second_vector[i] ? 0 : check = false;
        }
        check == true ? cout << "vector 1 dominaite" << endl : cout << "invalid value";
    }
    else {
        for (int i = 0; i < first_vector.size(); i++) {
            second_vector[i] > first_vector[i] ? 0 : check = true;
        }
        check == false ? cout << "vector 2 dominaite" : cout << "invalid value";
    }
}


// Распараллелинная функция проверки доминантности векторов
static void par_vector_dominaite(vector<int>& first_vector, vector<int>& second_vector) {

    bool check = true;
#pragma omp parralel
#pragma omp parralel for
    omp_set_num_threads(omp_get_max_threads());

    auto start_time = chrono::steady_clock::now();

    for (int i = 0; i < first_vector.size(); i++) {
        first_vector[i] > second_vector[i] ? 0 : check = false;
    }
    check == true ? cout << "1" << endl : cout << "2" << endl;

    auto end_time = chrono::steady_clock::now();

    cout << endl << "parralel for" << endl;
    cout << chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count() << "ms" << endl;

#pragma omp for ordered schedule(dynamic)
    omp_set_num_threads(omp_get_max_threads());

    check = true;
    start_time = chrono::steady_clock::now();

    for (int i = 0; i < first_vector.size(); i++) {
        first_vector[i] > second_vector[i] ? 0 : check = false;
    }
    check == true ? cout << "1" << endl : cout << "2" << endl;

    end_time = chrono::steady_clock::now();

    cout << endl << "parralel for ordered" << endl;
    cout << chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count() << " ms" << endl;
}

//распараллеливание с помощью потоков
static void paralel_wh_threads(vector<int> &first_vector, vector<int> &second_vector) {
    int i = first_vector.size();


    // немного не знания мною языка cpp, осуждаю подобное, но суть задачи далеко не в этом
    vector<int> f1_shared_vector = share_vector(0, i / 4, first_vector);
    vector<int> s1_shared_vector = share_vector(0, i / 4, second_vector);
    vector<int> f2_shared_vector = share_vector((i/4) + 1, i / 2, first_vector);
    vector<int> s2_shared_vector = share_vector((i/4) + 1, i / 2, second_vector);
    vector<int> f3_shared_vector = share_vector((i / 2) + 1, (i / 2) + (i / 4), first_vector);
    vector<int> s3_shared_vector = share_vector((i / 2) + 1, (i / 2) + (i / 4), second_vector);
    vector<int> f4_shared_vector = share_vector((i / 2) + (i / 4) + 1, i-1, first_vector);
    vector<int> s4_shared_vector = share_vector((i / 2) + (i / 4) + 1, i-1, second_vector);
    
    auto start_time = chrono::steady_clock::now();

    thread th1(vector_dominance, f1_shared_vector, s1_shared_vector);
    th1.detach();
    thread th2(vector_dominance, f2_shared_vector, s2_shared_vector);
    th2.detach();
    thread th3(vector_dominance, f3_shared_vector, s3_shared_vector);
    th3.detach();
    thread th4(vector_dominance, f4_shared_vector, s4_shared_vector);
    th4.detach();

    auto end_time = chrono::steady_clock::now();

    cout << endl << "parralel with thread" << endl;
    cout << chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count() << "ms" << endl;
}



int main(int argc, char* argv[])
{
    srand(time(nullptr));
    setlocale(LC_ALL, "ru");

    vector<int> vector1 = getVector(100 * 1000 * 1000);
    vector<int> vector2 = getVector(100 * 1000 * 1000);


    auto start_time = chrono::steady_clock::now();
    vector_dominance(vector1, vector2);
    auto end_time = chrono::steady_clock::now();

    cout << endl << "no parralel" << endl;
    cout << chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count() << "ms" << endl;

    par_vector_dominaite(vector1, vector2);
 
    paralel_wh_threads(vector1, vector2);
 

    return 0;
}


