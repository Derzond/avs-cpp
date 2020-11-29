#include <iostream>
#include <thread>
#include <random>
#include <omp.h>
#include <vector>
#include <string>

// Вариант: 26
// Выполнил: Федченко Всеволод Александрович БПИ199

// Вторая задача об Острове Сокровищ.
// Шайка пиратов подпредводительством Джона Сильвера высадилась на берег Острова Сокровищ.
// Несмотря на добытую карту старого Флинта, местоположение сокровищ по - прежнему остается загадкой,
// поэтому искать клад приходится практически на ощупь. Так как Сильвер ходит на деревянной ноге,
// то самому бродить по джунглям ему не с руки. Джон Сильвер поделил остров на участки, а пиратов на небольшие группы.
// Каждой группе поручается искать клад на нескольких участках, а сам Сильвер ждет на берегу.
// Группа пиратов, обшарив одну часть острова, переходит к другой, еще не обследованной части.
// Закончив поиски, пираты возвращаются к Сильверу и докладывают о результатах.
// Требуется создать многопоточное приложение с управляющим потоком, моделирующее действия Сильвера и пиратов.
// При решении использовать парадигму портфеля задач.


// Программа компилируется командой "c++ avs_hw_4.cpp -fopenmp -std=c++11" (проверено только на Windows 10).
// Команда для запуска: a.exe <кол-во секторов> <кол-во потоков> 


using namespace std;

//Кол-во участков.
int numOfSectors;
// Кол-во потоков.
int numOfThreads;

int treasureIndex = -1;

vector<bool> islandSectors;
volatile bool globalFlag = false;

void searchForTreasure(int numOfThreads)
{
	bool localFlag = false;

#pragma omp parallel private(localFlag) shared(globalFlag) num_threads(numOfThreads)
	{
#pragma omp critical
		{
			cout << "Team " << omp_get_thread_num() << " started searching " << endl;
		}

#pragma omp for 
		for (int i = 0; i < numOfSectors; i++)
		{
			if (!localFlag)
			{
#pragma omp critical
				{
					if (globalFlag)
					{
						cout << "Team " << omp_get_thread_num() << " stops searching for treasure because other team has already found it" << endl;
						localFlag = true;
					}

					if (islandSectors[i] && !globalFlag)
					{
						globalFlag = true;
						cout << "Team " << omp_get_thread_num() << " has found treasure in sector " << i << " and stopped searching" << endl;
						localFlag = true;
					}
				}
			}
		}
	}
}

int main(int argc, char* argv[])
{
	numOfSectors = stoi(argv[1]);
	numOfThreads = stoi(argv[2]);

	// Обрабтка некорректного ввода.
	if (numOfSectors < 1)
	{
		cout << "incorrect input. Number of sectors should be more than 0";
		return -1;
	}
	if (numOfThreads < 1 || numOfThreads > numOfSectors)
	{
		cout << "incorrect input. Number of threads should be more than 0 and not more than number of sectors";
		return -1;
	}

	islandSectors.reserve(numOfSectors);

	for (int i = 0; i < numOfSectors; i++)
	{
		islandSectors.push_back(0);
	}

	//Создание рандомайзера.
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> uni(0, numOfSectors);

	auto random_integer = uni(rng);

	// Рандомно задать сектор с сокровищем.
	treasureIndex = random_integer;
	islandSectors[treasureIndex] = true;
	cout << "number of sectors = " << numOfSectors << endl;
	cout << "number of teams = " << numOfThreads << endl;
	cout << "treasure index = " << treasureIndex << endl;


	searchForTreasure(numOfThreads);
}
