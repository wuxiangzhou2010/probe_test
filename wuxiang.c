#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/cdev.h>

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
 
 // static struct class_attribute ubi_class_attrs[] = {
	// __ATTR("one", S_IRUGO, ab3100_otp_locked_show, NULL),
	// __ATTR("two", S_IRUGO, ab3100_otp_freq_show, NULL),
	// __ATTR("three", S_IRUGO, ab3100_otp_paf_show, NULL),
	// __ATTR_NULL
// };

static ssize_t ab3100_otp_locked_show1(struct device *dev, struct device_attribute *attr,char *buf){return 0;}
static ssize_t ab3100_otp_locked_show2(struct device *dev, struct device_attribute *attr,char *buf){return 0;}
static ssize_t ab3100_otp_locked_show3(struct device *dev, struct device_attribute *attr,char *buf){return 0;}
 
static struct device_attribute ab3100_otp_attrs[] = {
	__ATTR(one, S_IRUGO, ab3100_otp_locked_show1, NULL),
	__ATTR(two, S_IRUGO, ab3100_otp_locked_show2, NULL),
	__ATTR(three, S_IRUGO, ab3100_otp_locked_show3, NULL),
};

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
	// wuxiang->class_attrs = ubi_class_attrs;
	//check 
	if(IS_ERR(wuxiang)) 
	{ 
		printk("Err: failed in creating class.\n");
		return ;
	}
	// profiler_dev->groups=ab3100_otp_attrs;
	
	/* register your own device in sysfs, and this will cause udevd to create corresponding device node */
	profiler_dev = device_create(wuxiang,NULL, devno,NULL,"profiler"); 
	
	for (i = 0; i < ARRAY_SIZE(ab3100_otp_attrs); i++) {
		err = device_create_file(profiler_dev, &ab3100_otp_attrs[i]);
		if (err)
			//goto err;
		printk(KERN_INFO "ERROR \n");
	}
}
// struct kobject kobj;                     /* Sysfs directory object */

// struct my_attr {
    // struct attribute attr;// has char *name and mode
    // int value;
// };

// static struct my_attr my_first = {
    // .attr.name="stats",
    // .attr.mode = 0644,
    // .value = 1,
// };

// static struct my_attr my_second = {
    // .attr.name="second",
    // .attr.mode = 0644,
    // .value = 2,
// };

// static struct attribute * myattr[] = {
    // &my_first.attr,
    // &my_second.attr,
    // NULL
// };

// static ssize_t default_show(struct kobject *kobj, struct attribute *attr,
        // char *buf)
// {
    // struct my_attr *a = container_of(attr, struct my_attr, attr);
    // return scnprintf(buf, PAGE_SIZE, "%d\n", a->value);
// }

// static ssize_t default_store(struct kobject *kobj, struct attribute *attr,
        // const char *buf, size_t len)
// {
    // struct my_attr *a = container_of(attr, struct my_attr, attr);
    // sscanf(buf, "%d", &a->value);
    // return sizeof(int);
// }

// static struct sysfs_ops myops = { //sysfs_ops has ssize_t(*show) and ssize_t(*show)  
    // .show = default_show,
    // .store = default_store,
// };


// static struct kobj_type mytype = {
    // .sysfs_ops = &myops,
    // .default_attrs = myattr,
// };

// struct kobject *mykobj;
// static struct class *wuxiang_class;
 // struct class *wuxiang = class_create(THIS_MODULE, "wuxiang"); 
 //struct device *device_create(struct class *class, struct device *parent,
			     // dev_t devt, void *drvdata, const char *fmt, ...)


// static struct class_attribute uwb_class_attrs[] = {
// __ATTR(beacon_timeout_ms, S_IWUSR | S_IRUGO,
// beacon_timeout_ms_show, beacon_timeout_ms_store),
// __ATTR_NULL,
// };



static int __init sysfsexample_module_init(void)
{
	int result;
	// int err = -1;
	
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
	dev_t devno = MKDEV (hello_major, hello_minor);
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