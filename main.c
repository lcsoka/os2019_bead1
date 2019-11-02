#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include "passenger.h"
#include "linkedlist.h"

#define ADD_PASSENGER 1
#define SHOW_PASSENGERS 2
#define DELETE_PASSENGER 1
#define MODIFY_PASSENGER 2
#define LIST_PASSENGERS 3
#define FILTER_BY_PLACE 4
#define QUIT 0
#define BACK 0

#define NUMBER_OF_STRING 10
#define MAX_STRING_SIZE 40

List *list;
int menu();
int show_list_menu();
void add_passenger();
void ask_for_string_value(char[100], char *);
void ask_for_int_value(char[100], int *);
void ask_value_from_array(char[100], const char *[], int, int *);
void write_data();
void read_data();
void list_passengers();
void modify_passenger(int);
void delete_passenger(int);
void display(List * list);
void filter_by_place_id(int place_id, List * list);
const char *travel_types[3] = {"Repülő", "Hajó", "Autóbusz"};
const char *places[5] = {"Bali", "Mali", "Cook szigetek", "Bahamák", "Izland"};

int main()
{
    // Initialize the list
    list = initList();
    // Load data from file, if possible.
    read_data();

    // Create menu
    int choice = menu();

    while (choice != QUIT)
    {
        switch (choice)
        {
        case ADD_PASSENGER:
            add_passenger();
            break;
        case SHOW_PASSENGERS:
            list_passengers();
            break;
        }
        choice = menu();
    }

    return 0;
}

int menu(void)
{
    int option;

    printf("-== Amazing CRUD for unfortunate passengers ==-\n\n");
    printf("1.\tÚj utas felvitele\n");
    printf("2.\tUtasok listázása\n");
    printf("0.\tKilépés\n\n");
    printf("Válasszon: ");

    while ((scanf(" %i", &option) != 1) || (option < 0) || (option > 2))
    {
        fflush(stdin);
        fseek(stdin, 0, SEEK_END);
        printf("Hibás parancs! Válasszon újra!.\n\n");
        printf("Válasszon: ");
    }
    return option;
}

void add_passenger()
{

    char name[40];
    char phone[15];
    int travel_type_id;
    int place_id;

    ask_for_string_value("Adja meg a nevet: ", &name[0]);
    ask_for_string_value("Adja meg a telefonszámot: ", &phone[0]);
    ask_value_from_array("Adja meg az utazás módját!\n", travel_types, sizeof(travel_types) / sizeof(travel_types[0]), &travel_type_id);
    ask_value_from_array("Adja meg a helyszínt!\n", places, sizeof(places) / sizeof(places[0]), &place_id);

    PASSENGER *p = malloc(sizeof(PASSENGER));
    strcpy(p->name, name);
    strcpy(p->phone, phone);
    p->travel_type_id = travel_type_id;
    p->place_id = place_id;
    add(p, list);

    // Save it
    write_data();
}

void ask_for_string_value(char question[100], char *var)
{
    printf("%s", question);
    while (scanf(" %[^\n]s", var) != 1)
    {
        fflush(stdin);
        fseek(stdin, 0, SEEK_END);
        printf("Hibás paraméter! Válasszon újra!\n\n");
        printf("%s", question);
    }
}

void ask_for_int_value(char question[100], int *var)
{
    printf("%s", question);
    while (scanf(" %d", var) != 1)
    {
        fflush(stdin);
        fseek(stdin, 0, SEEK_END);
        printf("Hibás paraméter! Válasszon újra!\n\n");
        printf("%s", question);
    }
}

void ask_value_from_array(char question[100], const char *list[], int size, int *var)
{
    printf("%s", question);

    for (int i = 0; i < size; i++)
    {
        printf("%d. %s\n", i + 1, list[i]);
    }

    while (scanf(" %d", var) != 1 || *var < 1 || *var > size)
    {
        fflush(stdin);
        fseek(stdin, 0, SEEK_END);
        printf("Hibás paraméter! Válasszon újra!\n\n");
        printf("%s", question);
        for (int i = 0; i < size; i++)
        {
            printf("%d. %s\n", i + 1, list[i]);
        }
    }
    // Subtract 1 because we want to save the index of the selected element
    *var = *var - 1;
}

void read_data()
{
    FILE *infile;
    PASSENGER tmp;

    infile = fopen("test.dat", "r");
    if (infile == NULL)
    {
        // Error opening the file. Probably doesn't exist
        return;
    }

    while (fread(&tmp, sizeof(PASSENGER), 1, infile))
    {
        // load passenger from tmp into a new pointer
        PASSENGER *p = malloc(sizeof(PASSENGER));
        strcpy(p->name, tmp.name);
        strcpy(p->phone, tmp.phone);
        p->place_id = tmp.place_id;
        p->travel_type_id = tmp.travel_type_id;
        add(p, list);
    }
    // close file
    fclose(infile);
}

void write_data()
{
    FILE *outfile;

    // open file for writing
    outfile = fopen("test.dat", "w");
    if (outfile == NULL)
    {
        fprintf(stderr, "\nHiba a file megnyitasa kozben!\n");
        exit(1);
    }
    Node *current = list->head;

    for (; current != NULL; current = current->next)
    {
        // printf("WRITE: %i\n",current->data->id);
        fwrite(current->data, sizeof(PASSENGER), 1, outfile);
    }

    printf("Sikeres mentés !\n");

    // close file
    fclose(outfile);
}

void list_passengers()
{
    printf("Utasok:\n");
    display(list);
    int choice = show_list_menu();

    while (choice != BACK)
    {
        switch (choice)
        {
        case DELETE_PASSENGER:
        {
            int index = 0;
            ask_for_int_value("Adja meg a törölni szánt utas sorszámát: ", &index);
            delete_passenger(index);
        }
        break;
        case MODIFY_PASSENGER:
        {
            int index = 0;
            ask_for_int_value("Adja meg a módosítandó utas sorszámát: ", &index);
            printf("\n");
            modify_passenger(index);
        }
        break;
        case LIST_PASSENGERS:
        {
            display(list);
        }
        break;
        case FILTER_BY_PLACE:
        {
            int place_id;
            ask_value_from_array("Adja meg a helyszín nevét amire szűrni szeretne: \n", places,sizeof(places)/sizeof(places[0]),&place_id);
            printf("\n");
            filter_by_place_id(place_id, list);
        }
        break;
        case BACK:
            return;
        }
        choice = show_list_menu();
    }
}

int show_list_menu()
{
    int option;

    printf("\nMűveletek:\n");
    printf("1.\tTörlés\n");
    printf("2.\tMódosítás\n");
    printf("3.\tÚjra listázás\n");
    printf("4.\tSzűrés helyszín szerint\n");
    printf("0.\tVissza\n\n");
    printf("Válasszon: ");

    while ((scanf(" %i", &option) != 1) || (option < 0) || (option > 5))
    {
        fflush(stdin);
        fseek(stdin, 0, SEEK_END);
        printf("Hibás parancs! Válasszon újra!.\n\n");
        printf("Válasszon: ");
    }
    return option;
}

void modify_passenger(int i)
{
    printf("\nUtas (%d) módosítása\n", i);
    PASSENGER *p = getNthPassenger(i, list);
    if (p == NULL)
    {
        printf("Az utas nem található!\n");
        return;
    }
    else
    {
        char name[40];
        char phone[15];
        int travel_type_id;
        int place_id;

        printf("(%s)\n", p->name);
        ask_for_string_value("Adja meg a nevet: ", &name[0]);
        printf("(%s)\n", p->phone);
        ask_for_string_value("Adja meg a telefonszámot: ", &phone[0]);
        printf("(%s)\n", travel_types[p->travel_type_id]);
        ask_value_from_array("Adja meg az utazás módját!\n", travel_types, sizeof(travel_types) / sizeof(travel_types[0]), &travel_type_id);
        printf("(%s)\n", places[p->place_id]);
        ask_value_from_array("Adja meg a helyszínt!\n", places, sizeof(places) / sizeof(places[0]), &place_id);

        strcpy(p->name, name);
        strcpy(p->phone, phone);
        p->travel_type_id = travel_type_id;
        p->place_id = place_id;
        write_data();
    }
}

void delete_passenger(int i)
{
    printf("\nUtas (%d) törlése\n", i);
    PASSENGER *p = getNthPassenger(i, list);
    if (p == NULL)
    {
        printf("Az utas nem található!\n");
        return;
    }
    else
    {
        deleteFromList(p, list);
        write_data();
    }
}

void display(List *list)
{
    Node *current = list->head;
    int i = 0;
    if (list->head == NULL)
    {
        printf("Nincsenek felvitt utasok!\n");
        return;
    }

    for (; current != NULL; current = current->next)
    {
        i++;
        printf("%i. %s %s %s %s\n", i, current->data->name,
               current->data->phone,
               travel_types[current->data->travel_type_id],
               places[current->data->place_id]);
    }
}

void filter_by_place_id(int place_id, List *list)
{
    Node *current = list->head;
    int i = 0;
    if (list->head == NULL)
    {
        printf("Nincsenek felvitt utasok!\n");
        return;
    }
    for (; current != NULL; current = current->next)
    {
        // if(strcmp(&name[0],current->data->name)==0)//compare both the strings
        if (place_id == current->data->place_id)
        {
            i++;
            printf("%i. %s %s %s %s\n", i, current->data->name,
                   current->data->phone,
                   travel_types[current->data->travel_type_id],
                   places[current->data->place_id]);
        }
    }

    if(i == 0) {
        printf("Nem találhatóak utasok ezen a helyen!\n");
    }
}
