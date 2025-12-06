#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include "gpio_driver.h"
#include <linux/kthread.h>
#include <linux/delay.h>

// Etiqueta para el autor del modulo
#define AUTHOR	"TP5_Multithreading"

// Puntero para primer hilo
static struct task_struct *thread1;
// Puntero para segundo hilo
static struct task_struct *thread2;

uint8_t gpio_led = 16;

static int thread_led_on(void *params) {
    uint8_t gpio = *(uint8_t *)params;      //Recibo el dato al inicio del thread
    while(!kthread_should_stop()) {
        printk(KERN_INFO "%s: Led %d encendido\n", AUTHOR,gpio);
        gpio_set(gpio);
        msleep(2000);
    }
    return 0;
}

static int thread_led_off(void *params) {
    uint8_t gpio = *(uint8_t *)params;      //Recibo el dato al inicio del thread
    msleep(1000);
    while(!kthread_should_stop()) {
        printk(KERN_INFO "%s: Led %d apagado\n", AUTHOR,gpio);
        gpio_clr(gpio);
        msleep(2000);
    }
    return 0;
}


/**
 * @brief Se llama cuando el modulo se carga en el kernel
*/
static int __init kernel_module_init(void) {
	// TODO
    printk(KERN_INFO "%s: Iniciando programa...\n",AUTHOR);

    void __iomem* map_addr = gpio_map();
    if(!map_addr){
        printk(KERN_ERR "%s: No se pudo mapear la memoria\n",AUTHOR);
        return -1;
    }
    printk(KERN_INFO "%s: Memoria mapeada en direccion %p\n", AUTHOR, map_addr);
    gpio_set_dir_output(gpio_led);
	thread1 = kthread_run(
        thread_led_on,  // Callback
        &gpio_led,       // Paso dato del gpio
        "thread_led_on"   // Nombre del hilo
    );
    if (IS_ERR(thread1)) {
        printk(KERN_ERR "%s: Error al crear thread 1\n", AUTHOR);
        return -1;
    }
    thread2 = kthread_run(
        thread_led_off,  // Callback
        &gpio_led,       // Paso dato del gpio
        "thread_led_off"   // Nombre del hilo
    );
    if (IS_ERR(thread2)) {
        printk(KERN_ERR "%s: Error al crear thread 2\n", AUTHOR);
        // Elimino el hilo anterior
        kthread_stop(thread1);
        return -1;
    }
	printk(KERN_INFO"%s: Se inicializo tp5 ejemplo multithreading\n", AUTHOR);
	return 0;
}

/**
 * @brief Se llama cuando el modulo se quita del kernel
 */
static void __exit kernel_module_exit(void) {
	pr_info(KERN_INFO"%s: Removiendo el modulo de kernel\n", AUTHOR);
    // Si se habia podido crear el hilo
	if (thread1) {
        // Detengo el hilo
        kthread_stop(thread1);
    }
    pr_info(KERN_INFO"%s: Detenido hilo 1\n", AUTHOR);
    // Si se habia podido crear el hilo
    if (thread2) {
        // Detengo el hilo
        kthread_stop(thread2);
    }
    pr_info(KERN_INFO"%s: Detenido hilo 2\n", AUTHOR);
    gpio_clr(gpio_led);
    gpio_unmap();
	printk(KERN_INFO"%s: Modulo removido\n",AUTHOR);	
}

// Registro la funcion de inicializacion y salida
module_init(kernel_module_init);
module_exit(kernel_module_exit);

// Informacion del modulo
MODULE_LICENSE("GPL");
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION("UTN FRA Tecnicas Digitales III - TP5: GPOS");
