/* da includere obbligatoriamente nel codice di un modulo */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>         /* module_init */
#include <linux/miscdevice.h>
#include <linux/fs.h>           /* per struct file_operations */
#include <linux/string.h>       /* per snprintf */
#include <linux/slab.h>         /* per kmalloc */

/* licenza */
MODULE_LICENSE("GPL");

#define MIN(x,y) ((x)<(y) ? (x) : (y))

/* operazione di apertura - una sola lettura è permessa */
static int open(struct inode *inode, struct file *file)
{
    int* flag;

    flag = kmalloc(sizeof(int), GFP_USER);
    if (flag == NULL) /* l'allocazione è andata male */
        return -1;

    *flag = 0;
    file->private_data = flag;

    return 0;
}
/* operazione di chiusura */
static int close(struct inode *inode, struct file *file)
{
    kfree(file->private_data);
    return 0;
}

/* operazione di lettura */
static ssize_t read(struct file *file, char __user *buffer, size_t bufLen, loff_t *offset)
{
    /* numero di byte letti */
    size_t to_transfer;
    char test[40] = "panino\n";
    int err;
    void *CR3;

    if (*(int*)file->private_data != 0) /* se ha già letto non fa nulla */
        return 0;
    *(int*)file->private_data = 1; /* marca il file come letto */

    asm ("mov %%cr3, %%rax\n\t"
        "mov %%rax, %0"
        : "=r" (CR3)
        : /* no input */
        : "%rax" /* sporca rax e il compilatore deve deve saperlo */
    );
    /* trasforma in  */
    snprintf(test, 40, "0x%p\n", CR3);

    /* quanti byte trasferire */
    to_transfer = MIN(strlen(test), bufLen);

    err = copy_to_user(buffer, test, to_transfer);
    if (err)
        return -EINVAL;

    return (ssize_t)to_transfer;
}

/* le operazioni sul mio device
 * DOC - https://www.kernel.org/doc/html/latest/filesystems/vfs.html?highlight=file_operations */
static struct file_operations my_devops = {
    .owner =    THIS_MODULE,    /* c'è scritto di fare così */
    .read =     &read,
    .write =    NULL, /* è in sola lettura */
    .open =     &open, /* non serve fare niente */
    .release =  &close /* non serve fare niente */
};

/* descrittore del device */
static struct miscdevice my_dev = {
    MISC_DYNAMIC_MINOR, "test", &my_devops
};


/* entry point - __init specifica che non serve più dopo l'avvio */
static int __init init_cr3_module(void)
{
    printk("Starting CR3 module...\n");

    /* registra il device */
    return misc_register(&my_dev);
}

/* exit point - __exit specifica che serve solo per la rimozione */
static void __exit exit_cr3_module(void)
{
    printk("Terminating CR3 module...\n");

    misc_deregister(&my_dev);
}

/* per specificare le funzioni da chiamare quando si carica
 * e si rimuove il modulo */
module_init(init_cr3_module); /* all'avvio va chiamata questa funzione */
module_exit(exit_cr3_module); /* viene chiamato all'uscita */
