#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/kmod.h>
#include <linux/workqueue.h>
#include <linux/string.h>

#define CREATE_TRACE_POINTS
#include "include/timed_task2_trace.h"

static struct hrtimer my_timer;
static struct workqueue_struct *timed_task2_wq;
static struct work_struct am_work;

/* 参数定义 */
static unsigned int delay_s = 5; // 定时器间隔
static int mode = 0; // 0 = 一次性，1 = 周期性
static char script_path[256] = "/data/data/com.termux/files/home/.shortcuts/tasks/sayhi";

/* 参数修改回调：delay_s */
static int set_delay_s(const char *val, const struct kernel_param *kp)
{
    int ret = param_set_uint(val, kp);
    if (ret == 0) {
        pr_info("[timed_task2] delay_s 修改为 %u，重启定时器\n", delay_s);
        hrtimer_cancel(&my_timer);
        hrtimer_start(&my_timer, ktime_set(delay_s, 0), HRTIMER_MODE_REL);
    }
    return ret;
}
static const struct kernel_param_ops delay_s_ops = {
    .set = set_delay_s,
    .get = param_get_uint,
};
module_param_cb(delay_s, &delay_s_ops, &delay_s, 0644);
MODULE_PARM_DESC(delay_s, "定时器触发间隔，单位秒（动态生效）");

/* 参数修改回调：mode */
static int set_mode(const char *val, const struct kernel_param *kp)
{
    int ret = param_set_int(val, kp);
    if (ret == 0) {
        pr_info("[timed_task2] mode 修改为 %d，重启定时器\n", mode);
        hrtimer_cancel(&my_timer);
        hrtimer_start(&my_timer, ktime_set(delay_s, 0), HRTIMER_MODE_REL);
    }
    return ret;
}
static const struct kernel_param_ops mode_ops = {
    .set = set_mode,
    .get = param_get_int,
};
module_param_cb(mode, &mode_ops, &mode, 0644);
MODULE_PARM_DESC(mode, "定时器模式：0=一次性，1=周期性（动态生效）");

/* 参数修改回调：script_path（不重启定时器） */
static int set_script_path(const char *val, const struct kernel_param *kp)
{
    return param_set_copystring(val, kp); // 直接改变量，下次定时器自然用新值
}
static const struct kernel_param_ops script_ops = {
    .set = set_script_path,
    .get = param_get_string,
};
module_param_cb(script_path, &script_ops, &script_path, 0644);
MODULE_PARM_DESC(script_path, "Termux 脚本路径（下次执行时生效）");

/* 执行 am 命令的 workqueue 任务 */
static void am_work_func(struct work_struct *work)
{
    static char *argv[20];
    static char *envp[] = {
        "HOME=/",
        "PATH=/sbin:/system/sbin:/system/bin:/system/xbin",
        NULL
    };

    argv[0] = "/system/bin/am";
    argv[1] = "startservice";
    argv[2] = "-n";
    argv[3] = "com.termux/.app.RunCommandService";
    argv[4] = "-a";
    argv[5] = "com.termux.RUN_COMMAND";
    argv[6] = "--es";
    argv[7] = "com.termux.RUN_COMMAND_PATH";
    argv[8] = script_path;
    argv[9] = "--ez";
    argv[10] = "com.termux.RUN_COMMAND_BACKGROUND";
    argv[11] = "true";
    argv[12] = "--es";
    argv[13] = "com.termux.RUN_COMMAND_SESSION_ACTION";
    argv[14] = "0";
    argv[15] = NULL;

    pr_info("[timed_task2] 执行脚本: %s\n", script_path);
    call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);
}

/* 定时器回调 */
static enum hrtimer_restart timer_callback(struct hrtimer *timer)
{
    trace_timed_task2_trigger_zh(mode ? "定时器触发：周期性模式" : "定时器触发：一次性模式");
    queue_work(timed_task2_wq, &am_work);

    if (mode) {
        hrtimer_forward_now(timer, ktime_set(delay_s, 0));
        return HRTIMER_RESTART;
    }
    return HRTIMER_NORESTART;
}

/* 模块加载 */
static int __init timed_task2_init(void)
{
    pr_info("[timed_task2] 加载完成，模式=%s，间隔=%u秒，脚本=%s\n",
            mode ? "周期性" : "一次性", delay_s, script_path);

    timed_task2_wq = alloc_workqueue("timed_task2_wq", WQ_UNBOUND | WQ_MEM_RECLAIM, 1);
    if (!timed_task2_wq)
        return -ENOMEM;

    INIT_WORK(&am_work, am_work_func);

    hrtimer_init(&my_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    my_timer.function = timer_callback;
    hrtimer_start(&my_timer, ktime_set(delay_s, 0), HRTIMER_MODE_REL);

    return 0;
}

/* 模块卸载 */
static void __exit timed_task2_exit(void)
{
    hrtimer_cancel(&my_timer);
    flush_workqueue(timed_task2_wq);
    destroy_workqueue(timed_task2_wq);
    pr_info("[timed_task2] 卸载完成\n");
}

module_init(timed_task2_init);
module_exit(timed_task2_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("你");
MODULE_DESCRIPTION("支持动态修改 delay_s / mode / script_path 的定时任务模块");
