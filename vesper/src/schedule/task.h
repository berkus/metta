//
// Copyright 2007 - 2009, Stanislav Karchebnyy <berkus+metta@madfire.net>
//
// Distributed under the Boost Software License, Version 1.0.
// (See file LICENSE_1_0.txt or a copy at http://www.boost.org/LICENSE_1_0.txt)
//
/** TODO **
* As per the architecture docs: task is a protection domain with associated memory and portals.
* The schedulable property of the thread should be factored out into a Thread Scheduling Record,
* and this task class disbanded into more fine-grained ones.
**/
#pragma once

#include "types.h"
#include "object.h"

namespace metta {
namespace kernel {

class page_directory;

// This structure defines 'task' - a protection domain.
class task : public object
{
	public:
		static task *self();

		static void init();// Initialises the kernel task.
//		static void yield();// Called by the timer hook, this changes the running process.

		// Forks the current process, spawning a new one with a different
		// memory space.
		int fork();
        // RVM concept means that every newly created PD will receive a Parent interface to its parent nester. A fork is therefore merely setting nesting relationship and makes current task control the child task via Parent interface interposition.

		// Returns the pid of the current process.
		int getpid();

	private:
		int id;                        // Process ID.
//		uint32_t esp, ebp;             // Stack and base pointers.
//		uint32_t eip;                  // Instruction pointer.
		page_directory* page_dir;      // Page directory.
		task *next;                    // The next task in a linked list.
};

}
}

// kate: indent-width 4; replace-tabs on;
// vim: set et sw=4 ts=4 sts=4 cino=(4 :
