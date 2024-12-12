
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>

long task_period[3]={100000,200000,300000};
int task_id[3]={1,2,3};

//struct to assign function attributs
typedef struct {
    int id;
    long period;
} function_arg;

//function to convert time from us to struct time 
void timespec_add_us(struct timespec *t, long us) {
    t->tv_nsec += us * 1000;
    if (t->tv_nsec >= 1000000000) {
        t->tv_nsec -= 1000000000;
        t->tv_sec += 1;
    }
}

//function to compare two times
int timespec_cmp(struct timespec *a, struct timespec *b) {
    if (a->tv_sec > b->tv_sec)
        return 1;
    else if (a->tv_sec < b->tv_sec)
        return -1;
    else
        return (a->tv_nsec > b->tv_nsec) - (a->tv_nsec < b->tv_nsec);
}


//task to consume some cpu time
void cpu_time() {
     int a;
    for (int i = 0; i < 1000; i++) {
        for (int j = 0; j < 1000; j++) {
            a = j / 2;
        }
    }
}  
//periodic tasks 
void* make_task_periodic(function_arg* task_arg) {   
    struct timespec now,next;
    clock_gettime(CLOCK_MONOTONIC, &next); 
    while (1) {
		
		timespec_add_us(&next, task_arg-> period );  
   
       clock_gettime(CLOCK_MONOTONIC, &now);
        printf("Task %d starts execution at %ld sec/ %03ld msec\n", task_arg-> id, now.tv_sec, now.tv_nsec/1000000); // print the start time of the execution of the task

        cpu_time(); // call the function to be executed
        
        
        printf("Task %d finishes execution at %ld sec / %03ld msec\n", task_arg-> id , now.tv_sec, now.tv_nsec/1000000); // print which task end in
        
        
       clock_gettime(CLOCK_MONOTONIC, &now); // time of end exectution
        
        if (timespec_cmp(&now, &next) > 0) { //compare deadline with time of end exectution
            printf("Task %d missed its deadline\n", task_arg-> id);
        }
       
       
        
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL); // suspend the task untill it's next period
    }

    return NULL;
}	

int main(int argc, char **argv)
{

    pthread_t thread1,thread2,thread3;
    pthread_attr_t myattr1,myattr2,myattr3;
    cpu_set_t cpu1,cpu2,cpu3;
    struct sched_param myparam;

	
    pthread_attr_init(&myattr1);
    pthread_attr_setinheritsched(&myattr1, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&myattr1, SCHED_FIFO);
    CPU_ZERO(&cpu1);CPU_SET(1,&cpu1);
    pthread_attr_setaffinity_np(&myattr1,sizeof(cpu_set_t),&cpu1);
    myparam.sched_priority = 3;
    pthread_attr_setschedparam(&myattr1, &myparam);
    	
    pthread_attr_init(&myattr2);
    pthread_attr_setinheritsched(&myattr2, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&myattr2, SCHED_FIFO);
    CPU_ZERO(&cpu1);CPU_SET(1,&cpu2);
    pthread_attr_setaffinity_np(&myattr2,sizeof(cpu_set_t),&cpu2);
    myparam.sched_priority = 2;
    pthread_attr_setschedparam(&myattr2, &myparam);
	
	
    pthread_attr_init(&myattr3);
    pthread_attr_setinheritsched(&myattr3, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&myattr3, SCHED_FIFO);
    CPU_ZERO(&cpu1);CPU_SET(1,&cpu3);
    pthread_attr_setaffinity_np(&myattr3,sizeof(cpu_set_t),&cpu3);
    myparam.sched_priority = 1;
    pthread_attr_setschedparam(&myattr3, &myparam);
    
    function_arg	tasks[3] ;
    
    for(int i =0;i<=2;i++){
		tasks[i].id=task_id[i];
		tasks[i].period=task_period[i];
	}
    	
    pthread_create (&thread1,&myattr1,&make_task_periodic,&tasks[0]);
    pthread_create (&thread2,&myattr2,&make_task_periodic,&tasks[1]);
    pthread_create (&thread3,&myattr3,&make_task_periodic,&tasks[2]);
	
    pthread_attr_destroy(&myattr1);
    pthread_attr_destroy(&myattr2);
    pthread_attr_destroy(&myattr3);	
	
    pthread_join(thread1,NULL);
    pthread_join(thread2,NULL);
    pthread_join(thread3,NULL);
	
    return 0;
}

