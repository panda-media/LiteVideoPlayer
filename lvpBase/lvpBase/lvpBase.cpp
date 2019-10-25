// lvpBase.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"

extern "C"
{

#include <lvp_mutex.h>
#include <lvp_stack.h>

}

#include <vector>
#include <assert.h>

void TestMutex()
{
    long int li = 0;
    int lisize = sizeof(li);
    LVPMutex mutex = lvp_mutex_create();
    LVP_BOOL lockRet = lvp_mutex_try_lock(mutex);
    LVP_BOOL lockRet2 = lvp_mutex_try_lock(mutex);
    LVP_BOOL unlockRet = lvp_mutex_unlock(mutex);
    LVP_BOOL closeRet = lvp_mutex_free(mutex);
    assert(!lockRet2);
    assert(lockRet&&unlockRet&&closeRet);
}

void TestStack()
{
    {
        LVPStack* stack = lvp_stack_create();

        std::vector<int> ivalues;
        for (int i = 0; i < 10000; i++)
        {
            ivalues.push_back(i);
        }

        for (auto &i : ivalues)
        {
            lvp_stack_push(stack, &i, nullptr, nullptr, LVP_FALSE);
        }

        {
            int *outValuePtr;
            lvp_stack_top(stack, (void**)&outValuePtr, LVP_LP_NULL);
            auto orgValue = ivalues[ivalues.size() - 1];
            assert(orgValue == *outValuePtr);
        }

        for (size_t i = 0; i < ivalues.size(); i++)
        {
            int *outValuePtr;
            lvp_stack_pop(stack, (void**)&outValuePtr, LVP_LP_NULL);

            auto orgValue = ivalues[ivalues.size() - 1 - i];
            assert(*outValuePtr == orgValue);

        }

        lvp_stack_free(stack);
    }
    
    {
        LVPStack* stack = lvp_stack_create();
        std::vector<int*> ivalues;
        const char* usrDataPtr = "I am ha ha";
        auto freeFunc = [](void *data, void *usrData)
        {
            delete (int*)data;
        };

        for (int i = 0; i < 10000; i++)
        {
            ivalues.push_back(new int(i));
        }

        for (auto &i : ivalues)
        {
            lvp_stack_push(stack, i, &usrDataPtr, freeFunc, LVP_TRUE);
        }

        {
            int *outValuePtr;
            lvp_stack_top(stack, (void**)&outValuePtr, LVP_LP_NULL);
            auto orgValue = ivalues[ivalues.size() - 1];
            assert(*orgValue == *outValuePtr);
        }

        for (size_t i = 0; i < ivalues.size()-1; i++)
        {
            int *outValuePtr;
            lvp_stack_pop(stack, (void**)&outValuePtr, LVP_LP_NULL);

            auto orgValue = ivalues[ivalues.size() - 1 - i];
            assert(*outValuePtr == *orgValue);

        }

        lvp_stack_free(stack);

        for (size_t i = 1; i < ivalues.size(); i++)
        {
            delete ivalues[i];
        }
    }
}


int main()
{
    while (true)
    {
        TestMutex();
        TestStack();
    }
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
