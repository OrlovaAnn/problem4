#include <iostream>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <list>

using std::vector;
using std::list;


// структура @MemoryBlock, описывающая блок памяти
// @realIndex - порядковый индекс блока, как он хранится в памяти
// @length - длина блока
// @isFree - индикатор занятости блока памяти
// @indexInHeap - индекс в блока в куче
// @start - начало блока в памяти
struct MemoryBlock
{
    int realIndex;
    int length;
    bool isFree;
    int indexInHeap;
    int start;

    MemoryBlock (int indInM = 0, int len = 0, bool is_free = false,
            int indInH = 0, int rInd = 0):
        start(indInM),
        length(len),
        isFree(is_free),
        indexInHeap(indInH),
        realIndex(rInd);

    MemoryBlock& operator = (const MemoryBlock& right);

    bool operator < (const MemoryBlock &right) const;

    bool operator == (const MemoryBlock &right) const;
    
    bool operator != (const MemoryBlock &right) const;
};

// вспомогательная функция @swapBlocksInHeap меняет местами два элемента в куче
// @block - вектор элементов кучи
// @first - индекс первого элемента
// @second - индекс второго элемента
void swapBlocksInHeap(vector<list<MemoryBlock>::iterator> &block, int first, int second);

// класс @MaxHeap, описывающий структуру MaxHeap и её методы
// @blocks - элементы кучи, являющиеся указателями на блоки памяти в списке
class MaxHeap
{
    vector<list<MemoryBlock>::iterator> blocks;

public:
    // метод @isEmpty возвращает true, если в куче нет элементов
    bool isEmpty() const;

    // метод @getBlocksSize возвращает количество элементов в куче
    int getBlocksSize() const;

    // метод @insertNewBlock вставляет новый элемент в кучу
    // newBlock - указатель на блок в списке блоков памяти 
    void insertNewBlock(list<MemoryBlock>::iterator newBlock);

    // метод @Heapify восстанавливает свойство кучи
    // index - индекс элемента, который нужно упорядочить для
    // восстановления свойств кучи
    void Heapify (int index);

    // метод @removeBlock удаляет элемент из кучи, сохраняя свойства кучи
    // element - указатель на удаляемый элемент
    void removeBlock(list<MemoryBlock>::iterator &element);

    // метод @pop удаляет максимальный (верхний) элемент из кучи, сохраняя свойства кучи
    list<MemoryBlock>::iterator pop();

    // метод @getMaxElement возвращает указатель на
    // максимальный (верхний) элемент из кучи
    list<MemoryBlock>::iterator getMaxElement();
};


// структура @Query, описывающая запрос на выделение/освобождение памяти
// @value - значение запроса
// @response - ответ на запрос (либо -1, либо положительное целое число)
// @pointerToBlock - указатель на блок в памяти, если по этому запросу,
// была выделена память, иначе NULL
struct Query
{
    int value;
    int response;
    list<MemoryBlock>::iterator pointerToBlock;
    explicit Query(int val) : value(val), response(0);
};

// класс @MemoryManager, выполняющий работу по освобождению и выделению памяти
// @memoryHeap - куча, элементами которой являются указатели на блоки памяти в списке @blocksInList
// @blocksInList - двусвязный список с блоками памяти, расположенными в том порядке,
// как они располагаются в памяти
// @queries - вектор запросов 
class MemoryManager
{
    MaxHeap memoryHeap;
    list<MemoryBlock> blocksInList;
    vector<Query> queries;

public:
    // констурктор @MemoryManager создаёт блок памяти, размером @sizeOfMemory
    explicit MemoryManager(int sizeOfMemory);

    // метод @fillQueryVector заполняет вектор запросов из входного потока
    void fillQueryVector();

    // метод @performQuery выполняет все запросы из @queries
    void performQuery();

    // метод @findAndAllocateMemory выполняет запрос @query на выделение 
    // памяти 
    // @query - запрос, в который записывается результат выполнения
    // процедуры веделения памяти
    void findAndAllocateMemory(Query& query);

    // метод @deallocateMemory выполняет освобождение памяти
    // по запросу @query
    // @query - запрос на освобождение памяти
    void deallocateMemory(Query& query);
    
    // метод @printReport выводит результат выполнения запросов
    void printReport();
};

int main()
{
    int memorySize;
    std::cin >> memorySize;

    MemoryManager memoryManager(memorySize);
    // заполнение списка запросов из входного потока
    memoryManager.fillQueryVector();
    // выполнение списка запросов в порядке поступления из входного потока
    memoryManager.performQuery();
    // вывод результатов
    memoryManager.printReport();

    return 0;
}
