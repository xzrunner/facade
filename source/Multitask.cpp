#include "facade/Multitask.h"

#include <multitask/ThreadPool.h>

namespace facade
{

CU_SINGLETON_DEFINITION(Multitask)

Multitask::Multitask()
{
	m_pool = new mt::ThreadPool();

	m_pool->SetMaxQueueSize(1024);

	int count = std::thread::hardware_concurrency() - 1;
	m_pool->Start(count);
}

Multitask::~Multitask()
{
	m_pool->Stop();
	delete m_pool;
}

void Multitask::Run(mt::Task* task)
{
	m_pool->Run(task);
}

}