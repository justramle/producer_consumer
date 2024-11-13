#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>

// identify semaphores
#define TABLE_SIZE 2
#define SHARED_MEM_NAME "/shm_table"
#define SEM_FULL_NAME "/sem_full"   // Count semaphore for full slots
#define SEM_EMPTY_NAME "/sem_empty" // Count semaphore for empty slots
#define SEM_MUTEX_NAME "/sem_mutex" // Binary semaphore for mutual exclusion


int main() {
    // Shared memory setup
    int shm_fd = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, TABLE_SIZE * sizeof(int));
    int* table = static_cast<int*>(mmap(0, TABLE_SIZE * sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0));

    // Semaphore setup
    sem_t* sem_full = sem_open(SEM_FULL_NAME, O_CREAT, 0666, 0);            // No items initially
    sem_t* sem_empty = sem_open(SEM_EMPTY_NAME, O_CREAT, 0666, TABLE_SIZE); // All slots are empty 
    sem_t* sem_mutex = sem_open(SEM_MUTEX_NAME, O_CREAT, 0666, 1);          // Mutex initialized to unlocked

    int produce_index = 0; // Position to add the next item

    for (int i = 0; i < 10; ++i) { // Produce 10 items
        // Wait for an empty slot
        sem_wait(sem_empty);

        // Enter critical section to safely add item to the buffer
        sem_wait(sem_mutex);
        std::cout << "\nProducer enters CS: places item " << i + 1 << " in Slot " << produce_index << "\n";
        
        table[produce_index] = i + 1; // Produce an item
        std::cout << "=> Table: Shared Memory (Buffer current state): ";
        std::cout << "[ ";
        for (int i = 0; i < TABLE_SIZE; ++i) {
            std::cout << table[i] << " ";
        }
        std::cout << "]\n";

        // Move to the next slot by swapping position
        produce_index += 1;
        if (produce_index >= TABLE_SIZE) {
            produce_index = 0;
        }

        std::cout << "=> Producer exits CS\n";
        sem_post(sem_mutex); // Exit CS

        // Signal that there's a new item
        sem_post(sem_full);
        sleep(1); // delay
    }

    // Cleanup mem
    munmap(table, TABLE_SIZE * sizeof(int));
    close(shm_fd);
    sem_close(sem_full);
    sem_close(sem_empty);
    sem_close(sem_mutex);

    return 0;
}
