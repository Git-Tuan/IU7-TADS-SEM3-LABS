#include "simulation.h"
#include "runner.h"
#include "input.h"
#include <stdio.h>

int main(void)
{
    sim_params_t p;
    set_default_params(&p);

    int show_addr = 0;
    int choice = -1;

    while (choice != 0)
    {
        print_params(&p);
        printf("Вывод адресов: %s\n", show_addr ? "включён" : "выключен");
        printf("1 - моделирование на массиве\n"
               "2 - моделирование на списке\n"
               "3 - изменить входные данные\n"
               "4 - вкл/выкл вывод адресов при добавлении/удалении\n"
               "5 - сравнить массив и список (время, память)\n"
               "0 - выход\n> ");

        if (scanf("%d", &choice) != 1)
        {
            clear_input();
            choice = -1;
        }

        if (choice == 1)
        {
            run_static(&p, show_addr);
        }
        else if (choice == 2)
        {
            run_list(&p, show_addr);
        }
        else if (choice == 3)
        {
            rc_t rc = change_params(&p);
            print_change_result(rc, &p);
        }
        else if (choice == 4)
        {
            show_addr = !show_addr;
        }
        else if (choice == 5)
        {
            compare_impl(&p);
        }
        else if (choice == 0)
        {
            /* выход */
        }
        else
        {
            printf("Неверный пункт меню.\n");
        }
    }

    return 0;
}
