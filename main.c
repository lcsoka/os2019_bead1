#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include "passenger.h"
#include "linkedlist.h"
#include "place.h"

#define ADD_PASSENGER 1
#define SHOW_PASSENGERS 2
#define TEST 3
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
void force_expedition();
void add_passenger();
void ask_for_string_value(char[100], char *);
void ask_for_int_value(char[100], int *);
void ask_value_from_array(char[100], const char *[], int, int *);
void ask_for_place_id(char[100], const PLACE[], int, int *);
void write_data();
void read_data();
void list_passengers();
void modify_passenger(int);
void delete_passenger(int);
void display(List *list);
void filter_by_place_id(int place_id, List *list);
int get_user_ids_for_place(int place_id, List *list, int *);
int get_user_count_for_place(int place_id, List *list);
PLACE get_place(int);
int can_start_expedition(int);
const char *travel_types[3] = {"Repülő", "Hajó", "Autóbusz"};
const PLACE places[] = {
    {0, "Bali", 2},
    {1, "Mali", 3},
    {2, "Cook szigetek", 3},
    {3, "Bahamák", 3},
    {3, "Izland", 5},
};

struct success_message
{
    long mtype;
    int data[2]; // First int = place_id, second int = passenger count
};

int send_msg(int, int, int);
int receive_msg(int);
void start_expedition(int);

int passenger_manifest_pipe[2];

void request_passenger_manifest(int place_id)
{
    // Get passengers, send them in a pipe
    printf("Utaslista elküldése csővezetéken keresztül ide: %s\n", get_place(place_id).name);
    int users[count(list)];                                          // This array's length is equal to the list's length
    int actual_size = get_user_ids_for_place(place_id, list, users); // Actual passengers on this place (should be equal for place's threshold)
    int actual_users[actual_size];                                   // An array for the passengers

    close(passenger_manifest_pipe[0]); // Close read end

    // Create correct size array for user ids
    for (int i = 0; i < actual_size; i++)
    {
        actual_users[i] = users[i];
    }

    // Send data
    write(passenger_manifest_pipe[1], actual_users, sizeof(actual_users));
    close(passenger_manifest_pipe[1]); // Close write end
}

static void handler(int sig, siginfo_t *si, void *ucontext)
{
    if (sig == SIGUSR1)
    {
        request_passenger_manifest(si->si_value.sival_int);
    }
    else if (sig == SIGUSR2)
    {
        printf("Az expedíció hazatért. Összegzés lekérése üzenetsoron:\n");
    }
    else
    {
        printf("Other signal captured %d\n", sig);
    }
}

int main()
{
    // Initialize the list
    list = initList();
    // Load data from file, if possible.
    read_data();

    // Create menu
    int choice = menu();

    // Register event handlers
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = handler;
    sa.sa_flags = SA_SIGINFO;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

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
        case TEST:
            force_expedition();
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
    printf("3.\tExpedíció indítása\n");
    printf("0.\tKilépés\n\n");
    printf("Válasszon: ");

    while ((scanf(" %i", &option) != 1) || (option < 0) || (option > 3))
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
    ask_for_place_id("Adja meg a helyszínt!\n", places, sizeof(places) / sizeof(places[0]), &place_id);

    PASSENGER *p = malloc(sizeof(PASSENGER));
    strcpy(p->name, name);
    strcpy(p->phone, phone);
    p->travel_type_id = travel_type_id;
    p->place_id = place_id;
    add(p, list);

    // Save it
    write_data();
}

void force_expedition()
{
    int place_id;
    ask_for_place_id("Adja meg a ahonnan mentőexpedíciót szeretne indítani!\n", places, sizeof(places) / sizeof(places[0]), &place_id);
    start_expedition(place_id);
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

// This was necessary since I refactored the places to use struct instead of 'string' array
// That's why this is almost the same method then the 'ask_value_from_array'
void ask_for_place_id(char question[100], const PLACE list[], int size, int *var)
{
    printf("%s", question);

    for (int i = 0; i < size; i++)
    {
        printf("%d. %s\n", i + 1, list[i].name);
    }

    while (scanf(" %d", var) != 1 || *var < 1 || *var > size)
    {
        fflush(stdin);
        fseek(stdin, 0, SEEK_END);
        printf("Hibás paraméter! Válasszon újra!\n\n");
        printf("%s", question);
        for (int i = 0; i < size; i++)
        {
            printf("%d. %s\n", i + 1, list[i].name);
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
            ask_for_place_id("Adja meg a helyszín nevét amire szűrni szeretne: \n", places, sizeof(places) / sizeof(places[0]), &place_id);
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
        printf("(%s)\n", places[p->place_id].name);
        ask_for_place_id("Adja meg a helyszínt!\n", places, sizeof(places) / sizeof(places[0]), &place_id);

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
               places[current->data->place_id].name);
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
                   places[current->data->place_id].name);
        }
    }

    if (i == 0)
    {
        printf("Nem találhatóak utasok ezen a helyen!\n");
    }
}

int get_user_ids_for_place(int place_id, List *list, int *users)
{
    Node *current = list->head;
    int i = 0;
    int passenger_id = 0;

    if (list->head == NULL)
    {
        printf("Nincsenek felvitt utasok!\n");
        return 0;
    }
    for (; current != NULL; current = current->next)
    {

        if (place_id == current->data->place_id)
        {
            users[i] = current->data->id;
            i++;
        }
        passenger_id++;
    }
    return i;
}

// Code duplication again... But it's 23:22 in the evening and i don't really care. Sorry
int get_user_count_for_place(int place_id, List *list)
{
    Node *current = list->head;
    int i = 0;
    int passenger_id = 0;

    if (list->head == NULL)
    {
        printf("Nincsenek felvitt utasok!\n");
        return 0;
    }
    for (; current != NULL; current = current->next)
    {

        if (place_id == current->data->place_id)
        {
            i++;
        }
        passenger_id++;
    }
    return i;
}

PLACE get_place(int place_id)
{
    for (int i = 0; i < sizeof(places) / sizeof(PLACE); i++)
    {
        if (places[i].id == place_id)
        {
            return places[i];
        }
    }
    return places[0];
}

int can_start_expedition(int place_id)
{
    int users[count(list)];
    int actual_size = get_user_ids_for_place(place_id, list, users);
    if (actual_size < get_place(place_id).threshold)
    {
        return -1;
    }
    return 0;
}

void start_expedition(int place_id)
{
    // Check if expedition can be started
    if (can_start_expedition(place_id) < 0)
    {
        printf("Még nincs elég utas a mentőexpedícióhoz! (Min. %d)\n", get_place(place_id).threshold);
        return;
    }

    printf("Indul a mentő expedíció!\n");
    // Letrehozzuk a pipe-ot
    if (pipe(passenger_manifest_pipe) == -1)
    {
        perror("Hiba a pipe nyitaskor!");
        exit(EXIT_FAILURE);
    }

    int message_queue, status;
    char *path = "/tmp";
    key_t key;
    key = ftok(path, 1);
    message_queue = msgget(key, 0600 | IPC_CREAT);
    if (message_queue < 0)
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    pid_t child = fork();
    if (child > 0)
    {
        // Parent
        sigset_t sigset;
        sigfillset(&sigset);
        sigdelset(&sigset, SIGUSR1);
        sigdelset(&sigset, SIGUSR2);
        // Wait for SIGUSR1
        sigsuspend(&sigset); //pause
        // There was a signal, from the child which means the handler is now sending the passenger list in a pipe,
        // So we sleep a little
        sleep(1);
        // Wait for SIGUSR2
        sigsuspend(&sigset); //pause

        // Receive message through message queue
        receive_msg(message_queue);

        // Wait for child to end
        int status;
        wait(&status);
        printf("Mentőexpedíció vége!\n\n");
    }
    else
    {
        // Child
        sleep(1);

        // Send a signal to the parent with the place id
        union sigval sv;
        sv.sival_int = place_id;
        sigqueue(getppid(), SIGUSR1, sv);

        // Wait for parent handler sending the passenger list
        sleep(1);

        close(passenger_manifest_pipe[1]); // Close write end
        int user_count = get_user_count_for_place(place_id, list);
        int user_ids[user_count];

        // Read from pipe
        read(passenger_manifest_pipe[0], user_ids, sizeof(user_ids));
        printf("Utaslista megérkezett:\n");
        for (int i = 0; i < user_count; i++)
        {
            printf("%d. %s\n", (i + 1), getPassengerWithId(user_ids[i], list)->name);
        }
        close(passenger_manifest_pipe[0]);

        // All passengers are here, now send the info in a message queue to parent
        // But wait before it
        sleep(1); // Must wait a little because it can send the kill before the parent is actually waiting for it
        send_msg(message_queue, place_id, user_count);
        kill(getppid(), SIGUSR2);

        sleep(1);
        // Remove the message queue
        // After terminating child process, the message queue is deleted.
        status = msgctl(message_queue, IPC_RMID, NULL);
        if (status < 0)
        {
            perror("msgctl");
        }
        // printf("child end\n");
        return;
    }
}

int send_msg(int message_queue, int place_id, int user_count)
{
    const struct success_message msg = {1, {place_id, user_count}};
    int status = msgsnd(message_queue, &msg, sizeof(msg.data), IPC_NOWAIT);
    if (status < 0)
    {
        perror("msgsnd");
    }
    return 0;
}

int receive_msg(int message_queue)
{
    struct success_message msg;
    int status;
    status = msgrcv(message_queue, &msg, sizeof(msg.data), 1, 0);
    if (status < 0)
    {
        perror("msgrcv");
    }
    else
    {
        printf("%s-rol hazahozott emberek száma: %d\n", get_place(msg.data[0]).name, msg.data[1]);
    }
    return 0;
}
