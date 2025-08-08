#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

#include <linux/kobject.h>
#include <linux/kernfs.h>
#include <linux/slab.h>

#define CREATE_TRACE_POINTS
#include "include/timed_task_trace.h"

static struct kset *timed_task_kset;
static struct kobject *timed_task_kobj;

static unsigned int delay_s = 5;
module_param(delay_s, uint, 0644);
MODULE_PARM_DESC(delay_s, "定时器触发间隔，单位秒");

static int mode = 0; // 0 = 一次性，1 = 周期性
module_param(mode, int, 0644);
MODULE_PARM_DESC(mode, "定时器模式：0=一次性，1=周期性");

static struct hrtimer my_timer;

static void send_user_event(void)
{
    char *envp[] = {
        "ACTION=timed_task_triggered",
        "TYPE=periodic",
        "SUBSYSTEM=timed_task",
        "CUSTOM=jiegaoyu",
        NULL
    };
    int ret;	
    pr_info("[timed_task] sending uevent to userspace\n");
    ret = kobject_uevent_env(timed_task_kobj, KOBJ_CHANGE, envp);
    if (ret)
        pr_err("[timed_task] kobject_uevent_env failed: %d\n", ret);
    else
        pr_info("[timed_task] KOBJ_CHANGE uevent sent\n");
}

static enum hrtimer_restart timer_callback(struct hrtimer *timer)
{
    const char *zh_msg = mode ? "定时器触发：周期性模式" : "定时器触发：一次性模式";
    trace_timed_task_trigger_zh(zh_msg);
    send_user_event();

    if (mode) {
        ktime_t interval = ktime_set(delay_s, 0);
        hrtimer_forward_now(timer, interval);
        return HRTIMER_RESTART;
    }
    return HRTIMER_NORESTART;
}

static void timed_task_release(struct kobject *kobj){
	pr_info("[timed_task] kobject released\n");
}
static int __init timed_task_init(void)
{
    int ret;
    ktime_t interval = ktime_set(delay_s, 0);
    static struct kobj_type timed_task_ktype = {
        .release=timed_task_release,
    };

    pr_info("[timed_task] 模块加载完成，%s模式，每 %u 秒触发一次\n",
            mode ? "周期性" : "一次性", delay_s);

    // 创建 kset: /sys/kernel/timed_task
    timed_task_kset = kset_create_and_add("timed_task", NULL, kernel_kobj);
    if (!timed_task_kset) {
        pr_err("[timed_task] 创建 timed_task_kset 失败\n");
        return -ENOMEM;
    }

    // 创建 kobject: /sys/kernel/timed_task/node0
    timed_task_kobj = kzalloc(sizeof(*timed_task_kobj), GFP_KERNEL);
    if (!timed_task_kobj) {
        kset_unregister(timed_task_kset);
        return -ENOMEM;
    }

    kobject_init(timed_task_kobj, &timed_task_ktype);
    timed_task_kobj->kset = timed_task_kset;
    ret = kobject_add(timed_task_kobj, NULL, "node0");

    if (ret) {
        pr_err("[timed_task] kobject_add 失败: %d\n", ret);
        kobject_put(timed_task_kobj);
        kset_unregister(timed_task_kset);
        return ret;
    }

    // 发送 KOBJ_ADD 事件
    {
        char *envp[] = {
            "ACTION=timed_task_add",
            "TYPE=init",
            NULL
        };
        ret = kobject_uevent_env(timed_task_kobj, KOBJ_ADD, envp);
        if (ret)
            pr_err("[timed_task] KOBJ_ADD uevent 发送失败: %d\n", ret);
        else
            pr_info("[timed_task] 已发送 KOBJ_ADD uevent\n");
    }

    hrtimer_init(&my_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    my_timer.function = timer_callback;
    hrtimer_start(&my_timer, interval, HRTIMER_MODE_REL);

    return 0;
}

static void __exit timed_task_exit(void)
{
    hrtimer_cancel(&my_timer);

    if (timed_task_kobj) {
        kobject_put(timed_task_kobj);
        timed_task_kobj = NULL;
    }

    if (timed_task_kset) {
        kset_unregister(timed_task_kset);
        timed_task_kset = NULL;
    }

    pr_info("[timed_task] 模块已卸载\n");
}

module_init(timed_task_init);
module_exit(timed_task_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("你");
MODULE_DESCRIPTION("带中文 trace_event 的定时内核模块");
