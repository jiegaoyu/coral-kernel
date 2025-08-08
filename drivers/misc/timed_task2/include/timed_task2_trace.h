#undef TRACE_SYSTEM
#define TRACE_SYSTEM timed_task2

#if !defined(_TIMED_TASK2_TRACE_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TIMED_TASK2_TRACE_H

#include <linux/tracepoint.h>

TRACE_EVENT(timed_task2_trigger_zh,

    TP_PROTO(const char *msg),

    TP_ARGS(msg),

    TP_STRUCT__entry(
        __string(message, msg)
    ),

    TP_fast_assign(
        __assign_str(message, msg);
    ),

    TP_printk("%s", __get_str(message))
);

#endif /* _TIMED_TASK2_TRACE_H */

#define TRACE_INCLUDE_PATH .
#define TRACE_INCLUDE_FILE timed_task2_trace
/* 必须 */
#include <trace/define_trace.h>
