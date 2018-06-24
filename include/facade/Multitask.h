#pragma once

#include <cu/cu_macro.h>

namespace mt { class Task; class ThreadPool; }

namespace facade
{

class Multitask
{
public:
	void Run(mt::Task* task);

private:
	mt::ThreadPool* m_pool;

	CU_SINGLETON_DECLARATION(Multitask)

}; // Multitask

}