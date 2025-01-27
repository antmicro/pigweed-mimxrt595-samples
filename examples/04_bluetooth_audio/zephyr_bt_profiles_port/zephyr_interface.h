#ifndef _ZEPHYR_INTERFACE_H
#define _ZEPHYR_INTERFACE_H

#include "pw_log/log.h"
#include "pw_bluetooth_sapphire/internal/host/gap/adapter.h"
#include "pw_sync/counting_semaphore.h"
#include "pw_sync/binary_semaphore.h"

#define BT_BUF_ACL_RX_COUNT 6 

#ifndef CONTAINER_OF
#define CONTAINER_OF(ptr, type, field) \
  ((type*)(((char*)(ptr)) - offsetof(type, field)))
#endif

#define sys_cpu_to_le16(x) (x)
#define sys_le16_to_cpu(x) (x)

#define CONFIG_BT_MAX_CONN 1

#define LOG_DBG PW_LOG_DEBUG
#define LOG_WRN PW_LOG_WARN
#define LOG_ERR PW_LOG_ERROR
#define MIN std::min
#define __fallthrough [[fallthrough]]
#define ARRAY_SIZE PW_ARRAY_SIZE
#define CONFIG_BT_RFCOMM_L2CAP_MTU 1016
#define BT_BUF_RESERVE 1

#ifndef __aligned
#define __aligned(x)   __attribute__((__aligned__(x)))
#endif
#ifndef __must_check
#define __must_check __attribute__((warn_unused_result))
#endif
#undef __deprecated
#define __deprecated
#define BIT(n)  (1UL << (n))

#define BT_ASSERT PW_ASSERT


typedef void (*bt_conn_tx_cb_t)(struct bt_conn *conn, void *user_data, int err);


struct _snode {
	struct _snode *next;
};

typedef struct _snode sys_snode_t;

#define CONFIG_TIMEOUT_64BIT
#ifdef CONFIG_TIMEOUT_64BIT
typedef int64_t k_ticks_t;
#else
typedef uint32_t k_ticks_t;
#endif

typedef struct {
	k_ticks_t ticks;
} k_timeout_t;

#ifdef CONFIG_POLL
#define Z_POLL_EVENT_OBJ_INIT(obj) \
	.poll_events = SYS_DLIST_STATIC_INIT(&obj.poll_events),
#define Z_DECL_POLL_EVENT sys_dlist_t poll_events;
#else
#define Z_POLL_EVENT_OBJ_INIT(obj)
#define Z_DECL_POLL_EVENT
#endif

struct k_spinlock {
#ifdef CONFIG_SMP
#ifdef CONFIG_TICKET_SPINLOCKS
	/*
	 * Ticket spinlocks are conceptually two atomic variables,
	 * one indicating the current FIFO head (spinlock owner),
	 * and the other indicating the current FIFO tail.
	 * Spinlock is acquired in the following manner:
	 * - current FIFO tail value is atomically incremented while it's
	 *   original value is saved as a "ticket"
	 * - we spin until the FIFO head becomes equal to the ticket value
	 *
	 * Spinlock is released by atomic increment of the FIFO head
	 */
	atomic_t owner;
	atomic_t tail;
#else
	atomic_t locked;
#endif /* CONFIG_TICKET_SPINLOCKS */
#endif /* CONFIG_SMP */

#ifdef CONFIG_SPIN_VALIDATE
	/* Stores the thread that holds the lock with the locking CPU
	 * ID in the bottom two bits.
	 */
	uintptr_t thread_cpu;
#ifdef CONFIG_SPIN_LOCK_TIME_LIMIT
	/* Stores the time (in cycles) when a lock was taken
	 */
	uint32_t lock_time;
#endif /* CONFIG_SPIN_LOCK_TIME_LIMIT */
#endif /* CONFIG_SPIN_VALIDATE */

#if defined(CONFIG_CPP) && !defined(CONFIG_SMP) && \
	!defined(CONFIG_SPIN_VALIDATE)
	/* If CONFIG_SMP and CONFIG_SPIN_VALIDATE are both not defined
	 * the k_spinlock struct will have no members. The result
	 * is that in C sizeof(k_spinlock) is 0 and in C++ it is 1.
	 *
	 * This size difference causes problems when the k_spinlock
	 * is embedded into another struct like k_msgq, because C and
	 * C++ will have different ideas on the offsets of the members
	 * that come after the k_spinlock member.
	 *
	 * To prevent this we add a 1 byte dummy member to k_spinlock
	 * when the user selects C++ support and k_spinlock would
	 * otherwise be empty.
	 */
	char dummy;
#endif
};

struct _sfnode {
	uintptr_t next_and_flags;
};

typedef struct _sfnode sys_sfnode_t;

struct _sflist {
	sys_sfnode_t *head;
	sys_sfnode_t *tail;
};

typedef struct _sflist sys_sflist_t;

struct _dnode {
	union {
		struct _dnode *head;
		struct _dnode *next;
	};
	union {
		struct _dnode *tail;
		struct _dnode *prev;
	};
};

typedef struct _dnode sys_dlist_t;

typedef struct {
	sys_dlist_t waitq;
} _wait_q_t;

struct k_queue {
	sys_sflist_t data_q;
	struct k_spinlock lock;
	_wait_q_t wait_q;

	Z_DECL_POLL_EVENT

	//SYS_PORT_TRACING_TRACKING_FIELD(k_queue)
};

struct k_lifo {
	struct k_queue _queue;
#ifdef CONFIG_OBJ_CORE_LIFO
	struct k_obj_core  obj_core;
#endif
};

struct _slist {
	sys_snode_t *head;
	sys_snode_t *tail;
};

typedef struct _slist sys_slist_t;

#define k_lifo_put(lifo, data) \
	({ \
	void *_data = data; \
	})
	//k_queue_prepend(&(lifo)->_queue, _data); \

struct k_sem {
    pw::sync::CountingSemaphore counting_semaphore;
    pw::sync::BinarySemaphore binary_semaphore;
    bool is_initialized;
    bool is_binary;
};

#define K_SEM_MAX_LIMIT UINT_MAX

int k_sem_init(struct k_sem *sem, unsigned int initial_count,
			  unsigned int limit);

void k_sem_give(struct k_sem *sem);

#endif
