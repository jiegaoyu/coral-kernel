#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

#define CREATE_TRACE_POINTS
#include "include/timed_task_trace.h"

static unsigned int delay_s = 5;
module_param(delay_s, uint, 0644);
MODULE_PARM_DESC(delay_s, "定时器触发间隔，单位秒");

static int mode = 1; // 0 = 一次性，1 = 周期性
module_param(mode, int, 0644);
MODULE_PARM_DESC(mode, "定时器模式：0=一次性，1=周期性");

static struct hrtimer my_timer;

static enum hrtimer_restart timer_callback(struct hrtimer *timer)
{
    const char *zh_msg = mode ? "定时器触发：周期性模式" : "定时器触发：一次性模式";

    trace_timed_task_trigger_zh(zh_msg);

    if (mode) {
        ktime_t interval = ktime_set(delay_s, 0);
        hrtimer_forward_now(timer, interval);
        return HRTIMER_RESTART;
    }

    return HRTIMER_NORESTART;
}

static int __init timed_task_init(void)
{
    ktime_t interval = ktime_set(delay_s, 0);

    pr_info("[timed_task] 模块加载完成，%s模式，每 %u 秒触发一次\n",
            mode ? "周期性" : "一次性", delay_s);

    hrtimer_init(&my_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    my_timer.function = timer_callback;
    hrtimer_start(&my_timer, interval, HRTIMER_MODE_REL);

    return 0;
}

static void __exit timed_task_exit(void)
{
    hrtimer_cancel(&my_timer);
    pr_info("[timed_task] 模块已卸载\n");
}

module_init(timed_task_init);
module_exit(timed_task_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("你");
MODULE_DESCRIPTION("带中文 trace_event 的定时内核模块");
