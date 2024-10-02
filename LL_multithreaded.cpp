#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <chrono>
#include <ctime>
#include <cstdint> 

// Constantes
const int MAX_KEY = 100000000;

// Estructura para los nodos de la lista
struct list_node_s {
    int data;
    pthread_mutex_t mutex;
    struct list_node_s* next;
};

// Variables compartidas
struct list_node_s* head = NULL;
pthread_mutex_t head_mutex;
int thread_count;
int total_ops;
double insert_percent, search_percent, delete_percent;
pthread_mutex_t count_mutex;
int member_total = 0, insert_total = 0, delete_total = 0;

// Prototipos
void* Thread_work(void* rank);
int Insert(int value);
int Member(int value);
int Delete(int value);
void Free_list();

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " <thread_count>\n";
        exit(0);
    }

    // Obtener inputs
    thread_count = atoi(argv[1]);
    int inserts_in_main;
    std::cout << "How many keys to insert initially?\n";
    std::cin >> inserts_in_main;
    std::cout << "Total ops for threads?\n";
    std::cin >> total_ops;
    std::cout << "Percent of search ops? (0-1)\n";
    std::cin >> search_percent;
    std::cout << "Percent of insert ops? (0-1)\n";
    std::cin >> insert_percent;
    delete_percent = 1.0 - (search_percent + insert_percent);

    // Insertar elementos iniciales
    srand(time(0));
    for (int i = 0; i < inserts_in_main; i++) {
        Insert(rand() % MAX_KEY);
    }

    // Crear y ejecutar threads
    pthread_t* thread_handles = (pthread_t*) malloc(thread_count * sizeof(pthread_t));
    for (long i = 0; i < thread_count; i++) {
        pthread_create(&thread_handles[i], NULL, Thread_work, (void*) i);
    }
    for (long i = 0; i < thread_count; i++) {
        pthread_join(thread_handles[i], NULL);
    }

    // Limpiar
    Free_list();
    free(thread_handles);
    pthread_mutex_destroy(&head_mutex);
    pthread_mutex_destroy(&count_mutex);

    return 0;
}

// Funciones

void* Thread_work(void* rank) {
    long my_rank = (intptr_t) rank;
    int ops_per_thread = total_ops / thread_count;
    unsigned seed = time(NULL) + (intptr_t) rank;
    srand(seed);

    for (int i = 0; i < ops_per_thread; i++) {
        int val = rand() % MAX_KEY;
        double op_type = (double) rand() / RAND_MAX;

        if (op_type < search_percent) Member(val);
        else if (op_type < search_percent + insert_percent) Insert(val);
        else Delete(val);
    }
    return NULL;
}

int Insert(int value) {
    pthread_mutex_lock(&head_mutex);
    list_node_s* new_node = (list_node_s*) malloc(sizeof(list_node_s));
    new_node->data = value;
    pthread_mutex_init(&(new_node->mutex), NULL);

    // Insertar al inicio o en su lugar correcto
    list_node_s* curr = head;
    list_node_s* prev = NULL;
    while (curr != NULL && curr->data < value) {
        prev = curr;
        curr = curr->next;
    }

    new_node->next = curr;
    if (prev == NULL) head = new_node;
    else prev->next = new_node;

    pthread_mutex_unlock(&head_mutex);
    return 1;
}

int Member(int value) {
    pthread_mutex_lock(&head_mutex);
    list_node_s* curr = head;
    while (curr != NULL && curr->data < value) {
        curr = curr->next;
    }
    pthread_mutex_unlock(&head_mutex);
    return (curr != NULL && curr->data == value);
}

int Delete(int value) {
    pthread_mutex_lock(&head_mutex);
    list_node_s* curr = head;
    list_node_s* prev = NULL;

    while (curr != NULL && curr->data < value) {
        prev = curr;
        curr = curr->next;
    }

    if (curr != NULL && curr->data == value) {
        if (prev == NULL) head = curr->next;
        else prev->next = curr->next;

        pthread_mutex_destroy(&(curr->mutex));
        free(curr);
        pthread_mutex_unlock(&head_mutex);
        return 1;
    }
    pthread_mutex_unlock(&head_mutex);
    return 0;
}

void Free_list() {
    while (head != NULL) {
        list_node_s* temp = head;
        head = head->next;
        pthread_mutex_destroy(&(temp->mutex));
        free(temp);
    }
}
