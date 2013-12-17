#include <iostream>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <list>

using std::vector;
using std::list;


// отказ на выполнение запроса
const int REJECT = -1;


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

    MemoryBlock() : start(0),
        length(0),
        isFree(false),
        indexInHeap(0),
        realIndex(0) {}

    MemoryBlock (int startInMemory, int length, bool is_free,
            int indInHeap, int realIndexInMemory) :
        start(startInMemory),
        length(length),
        isFree(is_free),
        indexInHeap(indInHeap),
        realIndex(realIndexInMemory)
    {}

    MemoryBlock& operator = (const MemoryBlock& right)
    {
        if (this == &right)
        {
            return *this;
        }
        start = right.start;
        length = right.length;
        isFree = right.isFree;
        realIndex = right.realIndex;

        return *this;
    }

    bool operator == (const MemoryBlock &right) const
    {
        if (length == right.length && start == right.start 
            && right.isFree == isFree) 
        {
            return true;
        }
        return false;
    }
    
    bool operator != (const MemoryBlock &right) const
    {
        return !(*this == right);
    }
};

// функтор для сравнения элементов кучи
class MemoryBlockComparator
{
public:
    bool operator()(const MemoryBlock& left, const MemoryBlock& right) const
    {
        if (left.length < right.length)
        {
            return true;
        }
        if (left.length == right.length)
        {
            if (left.start > right.start)
            {
                return true;
            }
        }
        return false;
    }
};

// класс @Heap, описывающий структуру Heap и её методы
// @elements - элементы кучи
// comp - функция сравнения элементов
template <class T, class Comp>
class Heap
{
    vector<T> elements;
    Comp comp;

    // метод @Heapify восстанавливает свойство кучи
    // index - индекс элемента, который нужно упорядочить для
    // восстановления свойств кучи
    void Heapify (int index)
    {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int maximum = index;

        if (left < elements.size() && comp(*elements[left], *elements[index]))
        {
            maximum = left;
        }
        if (right < elements.size() && comp(*elements[index], *elements[right]))
        {
            if (comp(*elements[left], *elements[right]))
            {
                maximum = right;
            }
        }

        if (maximum != index)
        {
            swapElementsInHeap(index, maximum);
            Heapify(maximum);
        }
    }

public:
    bool isEmpty() const
    {
        return elements.empty();
    }

    int getElementsSize() const
    {
        return elements.size();
    }

    void insertNewElement(T newElement)
    {
        int index = elements.size();
        newElement->indexInHeap = index;
        newElement->realIndex = index;
        elements.push_back(newElement);

        while (index > 0 && comp(*elements[(index - 1) / 2], *elements[index]))
        {
            swapElementsInHeap((index - 1) / 2, index);
            index = (index - 1) / 2;
        }
    }

    void swapElementsInHeap(int indI, int indJ)
    {
        int indRealI = elements[indI]->realIndex;
        int indRealJ = elements[indJ]->realIndex;

        std::swap(elements[indI], elements[indJ]);
        std::swap(elements[indI]->indexInHeap, elements[indJ]->indexInHeap);

        elements[indRealI]->indexInHeap = indJ;
        elements[indRealJ]->indexInHeap = indI;
    }

    void removeElement(T &element)
    {
        int newIndex = elements[element->indexInHeap]->realIndex;
        if (elements.empty())
            return;

        if (*element != *elements[elements.size() - 1]) 
        {
            if (elements.size() == 2)
               swapBlocksInHeap(0, 1);
            else
               swapElementsInHeap(elements.size() - 1, newIndex);
        }

        elements[elements[elements.size() - 1]->realIndex]->indexInHeap =
                elements[elements.size() - 1]->indexInHeap;
        elements[elements[elements.size() - 1]->indexInHeap]->realIndex =
                elements[elements.size() - 1]->realIndex;
        elements.pop_back();
        Heapify(newIndex);
    }

    T extractMin()
    {
        T maxElement = elements[0];
        removeElement(maxElement);
        return maxElement;
    }

    T getMaxElement() const
    {
        return elements[0];
    }

    void print()
    {
        for (int i = 0; i < elements.size(); ++i)
        {
            std::cout << i <<") " << elements[i]->length << " "
            << elements[i]->realIndex << " "<< elements[i]->indexInHeap << " ";
        }
        std::cout << std::endl;
    }
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
    explicit Query(int val) : value(val), response(0) {}
};

// класс @MemoryManager, выполняющий работу по освобождению и выделению памяти
// @memoryHeap - куча, элементами которой являются указатели на блоки памяти в списке @blocksInList
// @blocksInList - двусвязный список с блоками памяти, расположенными в том порядке,
// как они располагаются в памяти
class MemoryManager
{
    Heap<list<MemoryBlock>::iterator, MemoryBlockComparator> memoryHeap;
    list<MemoryBlock> blocksInList;

public:
    explicit MemoryManager(int sizeOfMemory)
    {
        MemoryBlock mb(0, sizeOfMemory, true, 0, 0);
        blocksInList.push_back(mb);
        list<MemoryBlock>::iterator pointerToBlockInList = blocksInList.begin();
        memoryHeap.insertNewElement(pointerToBlockInList);
    }

    // метод @findAndAllocateMemory выполняет запрос @query на выделение 
    // памяти 
    // @query - запрос, в который записывается результат выполнения
    // процедуры веделения памяти
    void findAndAllocateMemory(Query& query)
    {
        if (memoryHeap.isEmpty())
        {
            query.response = REJECT;
            return;
        }

        if (memoryHeap.getMaxElement()->length < query.value)
        {
            query.response = REJECT;
            return;
        }

        list<MemoryBlock>::iterator maxBlock = memoryHeap.getMaxElement();

        if ( maxBlock->length == query.value)
        {
            maxBlock->isFree = false;
            memoryHeap.extractMin();
            query.response = maxBlock->start + 1;
            query.pointerToBlock = maxBlock;
        }
        else
        {
            MemoryBlock newFullBlock(maxBlock->start, query.value, false, 0, 0);
            list<MemoryBlock>::iterator pointerToNewFullBlock =
                    blocksInList.insert(maxBlock, newFullBlock);
            MemoryBlock newFreeBlock(maxBlock->start + query.value,
                    maxBlock->length - query.value, true, 0, 0);
            list<MemoryBlock>::iterator pointerToNewFree =
                    blocksInList.insert(maxBlock, newFreeBlock);
            memoryHeap.extractMin();
            blocksInList.erase(maxBlock);

            memoryHeap.insertNewElement(pointerToNewFree);
            query.pointerToBlock = pointerToNewFullBlock;
            query.response = newFullBlock.start + 1;
        }
    }

    // метод @deallocateMemory выполняет освобождение памяти
    // по запросу @query
    // @query - запрос на освобождение памяти
    void deallocateMemory(Query& query)
    {
        if (query.response == REJECT)
            return;

        list<MemoryBlock>::iterator pointerToBlock = query.pointerToBlock;

        if (pointerToBlock == blocksInList.end())
            return;

        pointerToBlock->isFree = true;

        if (pointerToBlock != blocksInList.begin())
        {
            list<MemoryBlock>::iterator prevBlock = pointerToBlock;
            --prevBlock;
            if (prevBlock->isFree)
            {
                pointerToBlock->start = prevBlock->start;
                pointerToBlock->length += prevBlock->length;
                memoryHeap.removeElement(prevBlock);
                blocksInList.erase(prevBlock);
            }
        }

        if (pointerToBlock != (--blocksInList.end()))
        {
            list<MemoryBlock>::iterator nextBlock = pointerToBlock;
            ++nextBlock;
            if (nextBlock->isFree)
            {
                pointerToBlock->length += nextBlock->length;
                memoryHeap.removeElement(nextBlock);
                blocksInList.erase(nextBlock);
            }
        }
        memoryHeap.insertNewElement(pointerToBlock);
    }
};

// метод @readQueryVector заполняет вектор запросов из входного потока
void readQueryVector(vector<Query>& queries)
{
    int numberOfQueries, curValue;
    std::cin >> numberOfQueries;
    for (int i = 0; i < numberOfQueries; ++i)
    {
        std::cin >> curValue;
        queries.push_back(Query(curValue));
    }
}

// метод @performQuery выполняет все запросы из @queries
void performQuery(MemoryManager& memoryManager, vector<Query>& queries)
{
    for (size_t i = 0; i < queries.size(); ++i)
    {
        if (queries[i].value > 0)
            memoryManager.findAndAllocateMemory(queries[i]);
        else
        {
            memoryManager.deallocateMemory(queries[std::abs(queries[i].value) - 1]);
        }
    }
}

void printReport(vector<Query>& queries)
{
    for (int i = 0; i < queries.size(); ++i)
    {
        if (queries[i].value > 0)
            printf("%d\n", queries[i].response);
    }
}

int main()
{
    int memorySize;
    std::cin >> memorySize;

    MemoryManager memoryManager(memorySize);
    vector<Query> queries;

    readQueryVector(queries);
    performQuery(memoryManager, queries);
    printReport(queries);

    system("pause");

    return 0;
}
