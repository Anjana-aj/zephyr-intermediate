#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(l1_task1, LOG_LEVEL_INF);

#define STACK_SIZE 512

K_THREAD_STACK_DEFINE(t_low_stack,  STACK_SIZE);
K_THREAD_STACK_DEFINE(t_med_stack,  STACK_SIZE);
K_THREAD_STACK_DEFINE(t_high_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(t_coop_stack, STACK_SIZE);

static struct k_thread t_low;
static struct k_thread t_med;
static struct k_thread t_high;
static struct k_thread t_coop;

/* Priority 7 – runs least often; 300 ms sleep */
static void t_low_fn(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);
    while (1) {
        LOG_INF("T_LOW  running  (prio 7)");
        k_sleep(K_MSEC(300));
    }
}

/* Priority 5 – middle ground; 200 ms sleep */
static void t_med_fn(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);
    while (1) {
        LOG_INF("T_MED  running  (prio 5)");
        k_sleep(K_MSEC(200));
    }
}

/* Priority 3 – highest preemptive; runs most often; 100 ms sleep */
static void t_high_fn(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);
    while (1) {
        LOG_INF("T_HIGH running  (prio 3)");
        k_sleep(K_MSEC(100));
    }
}

static void t_coop_fn(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

    for (int iter = 0; iter < 5; iter++) {
        /* Simulate busy work – spin for a short count */
        volatile uint32_t busy = 0;
        for (uint32_t i = 0; i < 50000U; i++) {
            busy++;
        }
        LOG_INF("T_COOP busy-work iteration %d/5  (prio -1, busy=%u)",
                iter + 1, (unsigned)busy);

        /* Yield after each iteration so preemptive threads get a turn */
        k_yield();
    }

    LOG_INF("T_COOP done – thread exits");
}

int main(void)
{
    LOG_INF("=== L1-Task1: Preemptive vs Cooperative scheduling ===");

    /* Spawn preemptive threads (positive priorities) */
    k_thread_create(&t_low,  t_low_stack,  STACK_SIZE,
                    t_low_fn,  NULL, NULL, NULL,
                    7, 0, K_NO_WAIT);
    k_thread_name_set(&t_low,  "t_low");

    k_thread_create(&t_med,  t_med_stack,  STACK_SIZE,
                    t_med_fn,  NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);
    k_thread_name_set(&t_med,  "t_med");

    k_thread_create(&t_high, t_high_stack, STACK_SIZE,
                    t_high_fn, NULL, NULL, NULL,
                    3, 0, K_NO_WAIT);
    k_thread_name_set(&t_high, "t_high");

    /* Spawn cooperative thread (negative priority = cooperative) */
    k_thread_create(&t_coop, t_coop_stack, STACK_SIZE,
                    t_coop_fn, NULL, NULL, NULL,
                    -1, 0, K_NO_WAIT);
    k_thread_name_set(&t_coop, "t_coop");

    return 0;
}