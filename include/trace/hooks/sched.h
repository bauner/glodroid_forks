/* SPDX-License-Identifier: GPL-2.0 */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM sched
#define TRACE_INCLUDE_PATH trace/hooks
#if !defined(_TRACE_HOOK_SCHED_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_HOOK_SCHED_H
#include <linux/tracepoint.h>
#include <trace/hooks/vendor_hooks.h>
/*
 * Following tracepoints are not exported in tracefs and provide a
 * mechanism for vendor modules to hook and extend functionality
 */
#if defined(CONFIG_TRACEPOINTS) && defined(CONFIG_ANDROID_VENDOR_HOOKS)
struct task_struct;
DECLARE_RESTRICTED_HOOK(android_rvh_select_task_rq_fair,
	TP_PROTO(struct task_struct *p, int prev_cpu, int sd_flag, int wake_flags, int *new_cpu),
	TP_ARGS(p, prev_cpu, sd_flag, wake_flags, new_cpu), 1);

DECLARE_RESTRICTED_HOOK(android_rvh_select_task_rq_rt,
	TP_PROTO(struct task_struct *p, int prev_cpu, int sd_flag, int wake_flags, int *new_cpu),
	TP_ARGS(p, prev_cpu, sd_flag, wake_flags, new_cpu), 1);

DECLARE_RESTRICTED_HOOK(android_rvh_select_fallback_rq,
	TP_PROTO(int cpu, struct task_struct *p, int *new_cpu),
	TP_ARGS(cpu, p, new_cpu), 1);

struct rq;
DECLARE_RESTRICTED_HOOK(android_rvh_scheduler_tick,
	TP_PROTO(struct rq *rq),
	TP_ARGS(rq), 1);

DECLARE_RESTRICTED_HOOK(android_rvh_enqueue_task,
	TP_PROTO(struct rq *rq, struct task_struct *p),
	TP_ARGS(rq, p), 1);

DECLARE_RESTRICTED_HOOK(android_rvh_dequeue_task,
	TP_PROTO(struct rq *rq, struct task_struct *p),
	TP_ARGS(rq, p), 1);

DECLARE_RESTRICTED_HOOK(android_rvh_can_migrate_task,
	TP_PROTO(struct task_struct *p, int dst_cpu, int *can_migrate),
	TP_ARGS(p, dst_cpu, can_migrate), 1);

DECLARE_RESTRICTED_HOOK(android_rvh_find_lowest_rq,
	TP_PROTO(struct task_struct *p, struct cpumask *local_cpu_mask,
			int *lowest_cpu),
	TP_ARGS(p, local_cpu_mask, lowest_cpu), 1);
#else
#define trace_android_rvh_select_task_rq_fair(p, prev_cpu, sd_flag, wake_flags, new_cpu)
#define trace_android_rvh_select_task_rq_rt(p, prev_cpu, sd_flag, wake_flags, new_cpu)
#define trace_android_rvh_select_fallback_rq(cpu, p, dest_cpu)
#define trace_android_rvh_scheduler_tick(rq)
#define trace_android_rvh_enqueue_task(rq, p)
#define trace_android_rvh_dequeue_task(rq, p)
#define trace_android_rvh_can_migrate_task(p, dst_cpu, can_migrate)
#define trace_android_rvh_find_lowest_rq(p, local_cpu_mask, lowest_cpu)
#endif
#endif /* _TRACE_HOOK_SCHED_H */
/* This part must be outside protection */
#include <trace/define_trace.h>
