/* da includere obbligatoriamente nel codice di un modulo */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h> /* module_init */


/* licenza */
MODULE_LICENSE("GPL");

/* entry point - __init specifica che non serve più dopo l'avvio */
static int __init init_cr3_module(void)
{
    printk("Starting CR3 module...\n");

    return 0;
}

/* exit point - __exit specifica che serve solo per la rimozione */
static void __exit exit_cr3_module(void)
{
    printk("Terminating CR3 module...\n");
}

/* per specificare le funzioni da chiamare quando si carica
 * e si rimuove il modulo */
module_init(init_cr3_module); /* all'avvio va chiamata questa funzione */
module_exit(exit_cr3_module); /* viene chiamato all'uscita */
