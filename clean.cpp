#include <iostream>
#include <sys/mman.h>
#include <semaphore.h>

int main() {
    // Unlink shared memory
    if (shm_unlink("/shm_table") == 0)
        std::cout << "Shared memory /shm_table unlinked successfully.\n";
    else
        std::cerr << "Error: Could not unlink shared memory /shm_table or it does not exist.\n";

    // Unlink semaphores
    if (sem_unlink("/sem_full") == 0)
        std::cout << "Semaphore /sem_full unlinked successfully.\n";
    else
        std::cerr << "Error: Could not unlink semaphore /sem_full or it does not exist.\n";

    if (sem_unlink("/sem_empty") == 0)
        std::cout << "Semaphore /sem_empty unlinked successfully.\n";
    else
        std::cerr << "Error: Could not unlink semaphore /sem_empty or it does not exist.\n";

    if (sem_unlink("/sem_mutex") == 0)
        std::cout << "Semaphore /sem_mutex unlinked successfully.\n";
    else
        std::cerr << "Error: Could not unlink semaphore /sem_mutex or it does not exist.\n";

    return 0;
}
