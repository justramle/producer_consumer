#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>

#define TABLE_SIZE 2
#define SHARED_MEM_NAME "/shm_table"
#define SEM_FULL_NAME "/sem_full"   // Counting semaphore for full slots
#define SEM_EMPTY_NAME "/sem_empty" // Counting semaphore for empty slots
#define SEM_MUTEX_NAME "/sem_mutex" // Binary semaphore for mutual exclusion


int main() {
    // Open existing shared memory
    int shm_fd = shm_open(SHARED_MEM_NAME, O_RDWR, 0666);
    int* table = static_cast<int*>(mmap(0, TABLE_SIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));

    // Open existing semaphores
    sem_t* sem_full = sem_open(SEM_FULL_NAME, 0);  // Waits on items to consume
    sem_t* sem_empty = sem_open(SEM_EMPTY_NAME, 0); // Waits on available slots
    sem_t* sem_mutex = sem_open(SEM_MUTEX_NAME, 0); // Mutex for critical section

    int consume_index = 0; // Position to consume the next item

    for (int i = 0; i < 10; ++i) { // Consume 10 items
        // Wait for a filled slot
        sem_wait(sem_full);

        // Enter critical section to safely remove item from the buffer
        sem_wait(sem_mutex);
        std::cout << "\nConsumer enters CS: removes item " << table[consume_index] << " from Slot " << consume_index << "\n";
        
        table[consume_index] = 0; // Clear the slot after consuming
        std::cout << "=> Table: Shared Memory (Buffer current state): ";
        std::cout << "[ ";
        for (int i = 0; i < TABLE_SIZE; ++i) {
            std::cout << table[i] << " ";
        }
        std::cout << "]\n";

         // Move to the next slot by swapping position
        consume_index += 1;
        if (consume_index >= TABLE_SIZE) {
            consume_index = 0;
        }

        // Exit critical section
        std::cout << "=> Consumer exits CS\n";
        sem_post(sem_mutex);

        // Signal free slot available
        sem_post(sem_empty);

        sleep(1); 
    }

    // Cleanup
    munmap(table, TABLE_SIZE * sizeof(int));
    close(shm_fd);
    sem_close(sem_full);
    sem_close(sem_empty);
    sem_close(sem_mutex);

    return 0;
}
