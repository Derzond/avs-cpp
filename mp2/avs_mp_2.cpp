#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <pthread.h>
#include <cstring>


const int numOfPhilosophers = 5; // кол-во фиолсофов
pthread_mutex_t forks[numOfPhilosophers]; // мутекс для взятия i-ой вилки.
pthread_mutex_t generalLockWrite; // мутекс для вывода текста в консоль
unsigned int seed = 1; // инициализатор генератора случайных чисел
int iterations = 3;

void* Execute(void* param)
{
    int philNum = *((int*)param);

    bool isHungry;

    // назначение философу номера его левой и правой вилок.
    int leftFork = philNum;
    int rightFork = philNum + 1;

    // чтобы избежать дедлока, философу с максимальным номером назначается 
    // левой вилкой вилка справа от него, и наоборот.
    if (philNum == numOfPhilosophers - 1)
    {
        rightFork = philNum;
        leftFork = 0;
    }

    int exit = 0;
    while (exit < iterations)
    {
        ++exit;
        isHungry = rand() % 2 == 0 ? false : true;

        if (isHungry)
        {
            // Вывод информации.
            pthread_mutex_lock(&generalLockWrite); 
            std::cout << "philosopher " << philNum << " wants to eat." << std::endl;
            pthread_mutex_unlock(&generalLockWrite);

            // Попытка взять сначала левую вилку, затем правую.
            // Если какая-то из вилок уже занята - подождать, пока она не освободится
            pthread_mutex_lock(&forks[leftFork]);
            pthread_mutex_lock(&forks[rightFork]);
            
            // Вывод информации.
            pthread_mutex_lock(&generalLockWrite); 
            std::cout << "philosopher " << philNum << " started eating" << std::endl;
            pthread_mutex_unlock(&generalLockWrite); 

            // Философ тратит секунду, чтобы поесть.
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            
            // Филосов кладет вилки в обратном порядке:
            // сначала правую, потом левую.
            pthread_mutex_unlock(&forks[rightFork]);
            pthread_mutex_unlock(&forks[leftFork]);
            
            // Вывод информации.
            pthread_mutex_lock(&generalLockWrite); 
            std::cout << "philosopher " << philNum << " finished eating" << std::endl;
            pthread_mutex_unlock(&generalLockWrite); 
        }
        else
        {
            // Если филосов непроголодался, то он продолжает размышлять.
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
            
            // Вывод информации.
            pthread_mutex_lock(&generalLockWrite); 
            std::cout << "philosopher " << philNum << " is thinking" << std::endl;
            pthread_mutex_unlock(&generalLockWrite); 
        }
    }
    
    std::cout << "Philosopher " << philNum <<" is tired and will go sleep" << std::endl;

    return nullptr;
}

bool isNum(char* number)
{
    int i = 0;

    if (number[0] == '-')
    {
        i = 1;
    }

    for (i; i < strlen(number); ++i)
    {
        if (!isdigit(number[i]))
        {
            return false;
        }
    }

    return true;
}


int main(int argc, char *argv[])
{
    if(argv[1] != nullptr && isNum(argv[1]))
    {
        iterations = std::stoi(argv[1]);
        
        if (iterations < 1)
        {
            std::cout << "the given argument was incorrect. Arguement should be greater than 0." << std::endl;
            return 1;
        }
    }
    else
    {
        if (argv[1] != nullptr)
        {
            std::cout << "the given argument was incorrect. Arguement should be an integer greater than 0." << std::endl;
            return 2;
        }
    }
    
    std::cout << "Philosophers will be tired after " << iterations <<" cycles of thinking or eating" << std::endl;
    
    // инициализация рандомайзера
    srand(seed);
    
    // Выделение памяти под семафоры
    pthread_t philosophers[numOfPhilosophers];
    
    // инициализация семафоров
    for (int i = 0; i < numOfPhilosophers; i++)
    {
        pthread_mutex_init(&forks[i], nullptr);
    }

    // Запуск философов
    int philosophersInt[numOfPhilosophers];
    for (int i = 0; i < numOfPhilosophers; i++)
    {
        philosophersInt[i] = i;
        pthread_create(&philosophers[i], nullptr, Execute, (void*)(philosophersInt + i));
    }

    // Ожидание завершения всех потоков.
    for (int i = 0; i < numOfPhilosophers; ++i) 
    {
        pthread_join(philosophers[i], NULL);
    }
}
