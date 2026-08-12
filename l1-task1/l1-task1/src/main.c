#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(l1_task1, LOG_LEVEL_INF);

/* Stack sizes */
#define STACK_SIZE 1024

/* Thread stacks */
K_THREAD_STACK_DEFINE(t_low_stack,  STACK_SIZE);
K_THREAD_STACK_DEFINE(t_med_stack,  STACK_SIZE);
K_THREAD_STACK_DEFINE(t_high_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(t_coop_stack, STACK_SIZE);

/* Thread control blocks */
static struct k_thread t_low_data;
static struct k_thread t_med_data;
static struct k_thread t_high_data;
static struct k_thread t_coop_data;

void t_low_fn(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);
    while (1) {
        LOG_INF("T_LOW  running (prio 7)");
        k_sleep(K_MSEC(300));
    }
}

void t_med_fn(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);
    while (1) {
        LOG_INF("T_MED  running (prio 5)");
        k_sleep(K_MSEC(200));
    }
}

void t_high_fn(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);
    while (1) {
        LOG_INF("T_HIGH running (prio 3)");
        k_sleep(K_MSEC(100));
    }
}

void t_coop_fn(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);
    while (1) {
        for (int i = 0; i < 5; i++) {
            LOG_INF("T_COOP busy-work iteration %d/5 (prio -1)", i + 1);
            /* Simulate some work without blocking */
            volatile uint32_t dummy = 0;
            for (uint32_t j = 0; j < 50000U; j++) {
                dummy++;
            }
            k_yield(); /* hand off to any thread of equal or higher priority */
        }
        LOG_INF("T_COOP finished 5 iterations, sleeping 500ms");
        k_sleep(K_MSEC(500)); /* become unready so preemptive threads can run */
    }
}

int main(void)
{
    LOG_INF("=== L1-Task1: Ready vs Waiting / k_yield vs k_sleep ===");

    k_thread_create(&t_low_data, t_low_stack, STACK_SIZE,
                    t_low_fn, NULL, NULL, NULL,
                    7, 0, K_NO_WAIT);
    k_thread_name_set(&t_low_data, "t_low");

    k_thread_create(&t_med_data, t_med_stack, STACK_SIZE,
                    t_med_fn, NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);
    k_thread_name_set(&t_med_data, "t_med");

    k_thread_create(&t_high_data, t_high_stack, STACK_SIZE,
                    t_high_fn, NULL, NULL, NULL,
                    3, 0, K_NO_WAIT);
    k_thread_name_set(&t_high_data, "t_high");

    k_thread_create(&t_coop_data, t_coop_stack, STACK_SIZE,
                    t_coop_fn, NULL, NULL, NULL,
                    -1, 0, K_NO_WAIT);
    k_thread_name_set(&t_coop_data, "t_coop");

    return 0;
}