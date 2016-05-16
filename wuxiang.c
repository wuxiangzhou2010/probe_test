#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/cdev.h>

#include <linux/kprobes.h>
#include <linux/ktime.h>
#include <linux/limits.h>
#include <linux/sched.h>

struct class *wuxiang;
static dev_t dev_number;          /* Allotted Device Number */

int hello_major = 100;
int hello_minor = 0;
int number_of_devices = 1;
struct cdev cdev;
dev_t dev = 0;
struct device *profiler_dev;

struct file_operations hello_fops = {
		.owner = THIS_MODULE,
 };
 
 static char func_name[NAME_MAX] = "sys_clone,sys_getpid,sys_getppid";
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
 

static ssize_t ab3100_otp_locked_show1(struct device *dev, struct device_attribute *attr,char *buf)
{
	// int i =10;
	char test[1000]={0,};
	// char *s;
	ssize_t len = 0;
	// snprintf(buf, PAGE_SIZE, "3w-sas Driver version: %s\n"
		
			   //int snprintf(char *buf, size_t size, const char *fmt, ...)
	// snprintf(buf, PAGE_SIZE, "3w-sas Driver version: %s\n"
	// len = snprintf(buf, PAGE_SIZE,
				// "======%4d"
				// "======%s",
				// i,
				// s);
		       // "Current commands posted:   %4d\n"
	for(i=0;i<count;i++)
	{
		snprintf(buf, PAGE_SIZE, "%s:%lu:%llu\n",
				listall[i].fun, listall[i].called_average, listall[i].called_count);
		strcat(test,buf);
		//break;
	}		
		
	len=  snprintf(buf, PAGE_SIZE, "%s",
		test);
		   
	return len;
}
static ssize_t ab3100_otp_locked_show2(struct device *dev, struct device_attribute *attr,char *buf){return 0;}
static ssize_t ab3100_otp_locked_show3(struct device *dev, struct device_attribute *attr,char *buf){return 0;}
 
static struct device_attribute ab3100_otp_attrs[] = {
	__ATTR(one, S_IRUGO, ab3100_otp_locked_show1, NULL),
	__ATTR(two, S_IRUGO, ab3100_otp_locked_show2, NULL),
	__ATTR(three, S_IRUGO, ab3100_otp_locked_show3, NULL),
};

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

static void char_reg_setup_cdev (void)
{
	int error, i, devno = MKDEV (hello_major, hello_minor);
	int err = 0;
	cdev_init (&cdev, &hello_fops);
	cdev.owner = THIS_MODULE;
	cdev.ops = &hello_fops;
	error = cdev_add (&cdev, devno , 1);
	if (error)
	{
		printk (KERN_NOTICE "Error %d adding char_reg_setup_cdev", error);
	}

	/* creating your own class */
	wuxiang = class_create(THIS_MODULE, "wuxiang");
	//check 
	if(IS_ERR(wuxiang)) 
	{ 
		printk("Err: failed in creating class.\n");
		return ;
	}
	
	/* register your own device in sysfs, and this will cause udevd to create corresponding device node */
	profiler_dev = device_create(wuxiang,NULL, devno,NULL,"profiler"); 
	
	for (i = 0; i < ARRAY_SIZE(ab3100_otp_attrs); i++) {
		err = device_create_file(profiler_dev, &ab3100_otp_attrs[i]);
		if (err)
			//goto err;
		printk(KERN_INFO "ERROR \n");
	}
}

static int __init sysfsexample_module_init(void)
{
	int result;
	int ret;
	// int err = -1;
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
	//return 0;
	dev = MKDEV (hello_major, hello_minor);
	result = register_chrdev_region (dev, number_of_devices, "test");
	if (result<0) {
			printk (KERN_WARNING "hello: can't get major number %d\n", hello_major);
			return result;
	}
	char_reg_setup_cdev ();
	printk (KERN_INFO "char device registered\n");
	//return 0;

   // int err = -1;
	if(alloc_chrdev_region(&dev_number, 0,1, "profiler") <0)
	{
		printk(KERN_DEBUG "can't register device \n");
		return -1;
	}
	// if (IS_ERR(wuxiang)) printk("Bad class create\n");
	// profiler_dev = device_create(wuxiang,NULL, dev_number,NULL,"profiler"); 
	// if(wuxiang==NULL)
	// {
		// printk("<0> create class failed!\n");
		// return -1;
	// }
		
       // kobj.parent = &profiler_dev->kobj;

    // mykobj = kzalloc(sizeof(*mykobj), GFP_KERNEL);
    // if (mykobj) {
        // kobject_init(mykobj, &mytype);
        // if (kobject_add(mykobj, wuxiang->dev_kobj, "%s", "sysfs_sample")) {
             // err = -1;
             // printk("Sysfs creation failed\n");
             // kobject_put(mykobj);
             // mykobj = NULL;
        // }
        // err = 0;
    // }
    // return err;
    return 0;
}

static void   __exit sysfsexample_module_exit(void)
{
	int i, err = 0;
	dev_t devno = MKDEV (hello_major, hello_minor);// for ungister the wuxiang device 
	
	for(i=0;i<count;i++)
	{
		unregister_kretprobe(&p_kretprobe[i]);
		printk(KERN_INFO "kretprobe at %p unregistered\n", 
				p_kretprobe[i].kp.addr);
		/* nmissed > 0 suggests that maxactive was set too low. */
		printk(KERN_INFO "Missed probing %d instances of %s\n",
			p_kretprobe[i].nmissed, p_kretprobe[i].kp.symbol_name);
	}
	for (i = 0; i < ARRAY_SIZE(ab3100_otp_attrs); i++) {
		device_remove_file(profiler_dev,&ab3100_otp_attrs[i]);
		if (err)
			//goto err;
		printk(KERN_INFO "ERROR \n");
	}
	
	
	cdev_del (&cdev);
	unregister_chrdev_region (devno, number_of_devices);
	device_destroy(wuxiang, devno);
	
	//unregister wuxiang
	if (wuxiang != NULL) 
	{
		class_destroy(wuxiang);
		wuxiang = NULL;
	}
	
	printk(KERN_INFO " %d__exit sysfsexample_module_exit\n",__LINE__);
}

module_init(sysfsexample_module_init);
module_exit(sysfsexample_module_exit);
MODULE_LICENSE("GPL v2");