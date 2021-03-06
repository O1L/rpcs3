#pragma once

#include "sys_sync.h"

struct lv2_mutex;

struct sys_cond_attribute_t
{
	be_t<u32> pshared;
	be_t<s32> flags;
	be_t<u64> ipc_key;

	union
	{
		char name[8];
		u64 name_u64;
	};
};

struct lv2_cond final : lv2_obj
{
	static const u32 id_base = 0x86000000;

	const u32 shared;
	const s32 flags;
	const u64 key;
	const u64 name;

	std::shared_ptr<lv2_mutex> mutex; // Associated Mutex
	atomic_t<u32> waiters{0};
	std::deque<cpu_thread*> sq;

	lv2_cond(u64 name, std::shared_ptr<lv2_mutex> mutex)
		: shared(0)
		, key(0)
		, flags(0)
		, name(name)
		, mutex(std::move(mutex))
	{
		this->mutex->cond_count++;
	}
};

class ppu_thread;

// Syscalls

error_code sys_cond_create(vm::ps3::ptr<u32> cond_id, u32 mutex_id, vm::ps3::ptr<sys_cond_attribute_t> attr);
error_code sys_cond_destroy(u32 cond_id);
error_code sys_cond_wait(ppu_thread& ppu, u32 cond_id, u64 timeout);
error_code sys_cond_signal(u32 cond_id);
error_code sys_cond_signal_all(u32 cond_id);
error_code sys_cond_signal_to(u32 cond_id, u32 thread_id);
