/*
 * kretprobe_example.c
 *
 * Here's a sample kernel module showing the use of return probes to
 * report the return value and total time taken for probed function
 * to run.
 *
 * usage: insmod kretprobe_example.ko func=<func_name>
 *
 * If no func_name is specified, _do_fork is instrumented
 *
 * For more information on theory of operation of kretprobes, see
 * Documentation/kprobes.txt
 *
 * Build and insert the kernel module as done in the kprobe example.
 * You will see the trace data in /var/log/messages and on the console
 * whenever the probed function returns. (Some messages may be suppressed
 * if syslogd is configured to eliminate duplicate messages.)
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/ktime.h>
#include <linux/limits.h>
#include <linux/sched.h>

static char func_name[NAME_MAX] = "sys_clone,sys_getpid,sys_getppid";

// static long long int called_average = 0;// for the average duration 
// static long long int called_count = 0;//for all the func that is called 
static int count=1; //cout of probe
static char list[20][255];
int i=0,j=0;
struct s_list{
	char *fun;
	 unsigned long called_average;
	 unsigned long long called_count;
};
struct s_list listall[20];

static struct kretprobe my_kretprobe[10];
static struct kretprobe *p_kretprobe;
module_param_string(func, func_name, NAME_MAX, S_IRUGO);
MODULE_PARM_DESC(func, "Function to kretprobe; this module will report the"
			" function's execution time");
int get_the_probe_num(char  *arg)
{
char *ar = arg;
int i =0;
int j=0;

	while(*ar!='\0')
	{
        list[i][j] = *ar;
		//listall[i].fun[j]=*ar;
		if(*ar ==',')
		{
			count++;
            list[i][j] = '\0';//one func is end 
          //  listall[i].fun[j]= '\0';//one func is end 
            i++;
            j=0;
            ar++;
		}
        else
        {
            ar++;
            j++;
        }
	}
	ar = NULL;
    list[i][j]='\0';
   // listall[i].fun[j]= '\0';//one func is end 
	
	printk(KERN_INFO "number of probe is %d \n", count);

    for(i=0;i<count;i++)
    {

        printk(KERN_INFO "list: = %s\n",list[i]);
		listall[i].called_average=0;
		listall[i].called_count = 0;
		listall[i].fun= list[i];
        printk(KERN_INFO "listall[%d]: = %s\n",i,listall[i].fun);

     }       
     return 0;
}

int strcmp__( const  char *a,  char *b)
{
	/* This implementation is buggy: it never returns -1. */
	while (*a || *b) {
		if (*a != *b)
			return 1;
		if (*a == 0 || *b == 0)
			return 1;
		a++;
		b++;
	}

	return 0;
}
			
/* per-instance private data */
struct my_data {
	ktime_t entry_stamp;
};

/* Here we use the entry_hanlder to timestamp function entry */
static int entry_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{
	struct my_data *data;

	if (!current->mm)
		return 1;	/* Skip kernel threads */

	data = (struct my_data *)ri->data;
	data->entry_stamp = ktime_get();
	return 0;
}

/*
 * Return-probe handler: Log the return value and duration. Duration may turn
 * out to be zero consistently, depending upon the granularity of time
 * accounting on the platform.
 */
static int ret_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{
	struct my_data *data = (struct my_data *)ri->data;
	s64 delta;
	ktime_t now;

	now = ktime_get();
	delta = ktime_to_ns(ktime_sub(now, data->entry_stamp));
	//printk(KERN_INFO "%s,%d\n",__FUNCTION__,__LINE__);

	for(i=0;i<count;i++)
	{
		if(!strcmp__(ri->rp->kp.symbol_name,listall[i].fun))
		{
			unsigned long long a, b;
			
			listall[i].called_count++;//called one more time 
			// listall[i].called_average = (unsigned long)(((delta - listall[i].called_average)/listall[i].called_count)+ listall[i].called_average);
			a=(delta + listall[i].called_average*(listall[i].called_count-1));
			b=listall[i].called_count;
			do_div(a,b);//32bit arc is different http://www.crifan.com/resolved_undefined_reference_to___udivdi3/
			listall[i].called_average =a;
			// listall[i].called_average = ((delta + listall[i].called_average*(listall[i].called_count-1))/listall[i].called_count);
			printk(KERN_INFO "%s:%lu:%llu\n",
					ri->rp->kp.symbol_name, listall[i].called_average, listall[i].called_count);
			break;
		}
		
	}
	//printk(KERN_INFO "%s:%lld\n",ri->rp->kp.symbol_name,delta);
	return 0;
}

static int __init kretprobe_init(void)
{
	int ret;
	get_the_probe_num(func_name);
	printk(KERN_INFO "%s,%d\n",__FUNCTION__,__LINE__);
	for(i=0;i<count;i++)// construct the  kretprobe array 
	{
		my_kretprobe[i].handler= ret_handler;
		my_kretprobe[i].entry_handler= entry_handler;
		my_kretprobe[i].data_size= sizeof(struct my_data);
		my_kretprobe[i].maxactive= 20;
		my_kretprobe[i].kp.symbol_name = list[i];
		
		printk(KERN_INFO "%s,%d\n",__FUNCTION__,__LINE__);
	}
	for(i = 0;i<count;i++)
	{
		printk(KERN_INFO "my_kretprobe[%d]:symbol_name:%s\n",i,my_kretprobe[i].kp.symbol_name);
	}
	p_kretprobe = my_kretprobe;

	
	for(i = 0;i<count;i++)
	{
		ret = register_kretprobe(&p_kretprobe[i]);
		
		if (ret < 0) {
		printk(KERN_INFO "register_kretprobe failed, returned %d\n",
				ret);
		return -1;
	}
		printk(KERN_INFO "[%d]:symbol_name:%s\n"
				,i,p_kretprobe[i].kp.symbol_name);
		printk(KERN_INFO "%s,%d\n",__FUNCTION__,__LINE__);
	}
	
	
	i =count;
	j=0;
	while(i--)
	{
	printk(KERN_INFO "Planted return probe at %s: %p\n",
			my_kretprobe[j].kp.symbol_name, my_kretprobe[j].kp.addr);
	j++;
	}
	return 0;
}

static void __exit kretprobe_exit(void)
{

for(i=0;i<count;i++)
	{
		unregister_kretprobe(&p_kretprobe[i]);
		printk(KERN_INFO "kretprobe at %p unregistered\n", 
				p_kretprobe[i].kp.addr);
		/* nmissed > 0 suggests that maxactive was set too low. */
		printk(KERN_INFO "Missed probing %d instances of %s\n",
			p_kretprobe[i].nmissed, p_kretprobe[i].kp.symbol_name);
	}
}

module_init(kretprobe_init)
module_exit(kretprobe_exit)
MODULE_LICENSE("GPL");
