#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <cstdint> 
const int MAX_KEY = 100000000;

using namespace std;
// Estructura para los nodos de la lista
struct list_node_s {
    int data;
    list_node_s* next;
};

// Variables compartidas
list_node_s* head = NULL;
pthread_mutex_t mutex;
int total_ops, member_total = 0, insert_total = 0, delete_total = 0;
double insert_percent, search_percent, delete_percent;

// Prototipos de funciones
void* Thread_work(void* rank);
int Insert(int value);
int Member(int value);
int Delete(int value);
void Free_list();
void Get_input(int* inserts_in_main_p);
int nt=0;
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " <thread_count>\n";
        return 1;
    }
    
    int thread_count = std::stoi(argv[1]);
    nt=thread_count;
    int inserts_in_main;
    Get_input(&inserts_in_main);
    
    // Inicializar el mutex
    pthread_mutex_init(&mutex, NULL);
    
    // Insertar claves iniciales
    for (int i = 0; i < inserts_in_main; i++) {
        Insert(rand() % MAX_KEY);
    }
    
    // Crear hilos
    pthread_t* thread_handles = (pthread_t*) malloc(thread_count * sizeof(pthread_t));
    for (long i = 0; i < thread_count; i++) {
        pthread_create(&thread_handles[i], NULL, Thread_work, (void*) i);
    }
    
    for (long i = 0; i < thread_count; i++) {
        pthread_join(thread_handles[i], NULL);
    }

    // Liberar la lista y destruir el mutex
    Free_list();
    pthread_mutex_destroy(&mutex);
    free(thread_handles);

    return 0;
}

void Get_input(int* inserts_in_main_p) {
    std::cout << "claves a insertar?\n";
    std::cin >> *inserts_in_main_p;
    std::cout << " total de operaciones a ejecutar:\n";
    std::cin >> total_ops;
    std::cout << "Porcentaje de operaciones que deben ser buscadas (0-1):\n";
    std::cin >> search_percent;
    std::cout << "Porcentaje de operaciones que deben ser inserciones (0-1):\n";
    std::cin >> insert_percent;
    delete_percent = 1.0 - (search_percent + insert_percent);
}

void* Thread_work(void* rank) {
    long my_rank = (intptr_t) rank;
    int my_member = 0, my_insert = 0, my_delete = 0;
    int ops_per_thread = total_ops / nt;
    
    for (int i = 0; i < ops_per_thread; i++) {
        double op_type = (double) rand() / RAND_MAX;
        int value = rand() % MAX_KEY;

        pthread_mutex_lock(&mutex);
        if (op_type < search_percent) {
            my_member += Member(value);
        } else if (op_type < search_percent + insert_percent) {
            my_insert += Insert(value);
        } else {
            my_delete += Delete(value);
        }
        pthread_mutex_unlock(&mutex);
    }

    // Acumular totales
    pthread_mutex_lock(&mutex);
    member_total += my_member;
    insert_total += my_insert;
    delete_total += my_delete;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int Insert(int value) {
    list_node_s* new_node = new list_node_s{value, NULL};
    list_node_s* curr = head;
    list_node_s* prev = NULL;

    while (curr != NULL && curr->data < value) {
        prev = curr;
        curr = curr->next;
    }

    if (prev == NULL) {
        new_node->next = head;
        head = new_node;
    } else {
        new_node->next = curr;
        prev->next = new_node;
    }
    
    return 1;
}

int Member(int value) {
    list_node_s* curr = head;
    while (curr != NULL) {
        if (curr->data == value) return 1;
        curr = curr->next;
    }
    return 0;
}

int Delete(int value) {
    list_node_s* curr = head;
    list_node_s* prev = NULL;

    while (curr != NULL && curr->data < value) {
        prev = curr;
        curr = curr->next;
    }

    if (curr != NULL && curr->data == value) {
        if (prev == NULL) head = curr->next;
        else prev->next = curr->next;
        delete curr;
        return 1;
    }
    return 0;
}

void Free_list() {
    while (head != NULL) {
        list_node_s* temp = head;
        head = head->next;
        delete temp;
    }
}
