## Details of producer-consumer problems

This program is to solve producer and consumer problems. It involves two types of processes: producers to generate data, and consumers to process that data and Both share same buffer. The problem is to ensure that the producer doesn’t add data to a full buffer and the consumer doesn’t remove data from an empty buffer, avoiding conflicts when they access the buffer. 

To solve this I use semaphore (counting and binary) to synchronize producer and consumer.  I also use mutual exclusion to lock the other process when the other process try to get in. I use Shared memory is used for the “table” to keep track of buffer state

## Step:
Step: 
* Initial share met buffer (table) which has 2 slot 
* Set up 3 semaphore
    * for counting semaphore to keep track empty/filled slots in buffer: sem_empty, sem_full 
    *  another  binary semaphore (acting as a mutex) to do mutual exclusion, so that only 1 process  accesses the buffer at any certain time: sem_mutex
* Create and Set Up Shared Memory: 
    * shm_open to opens a shared memory object named SHARED_MEM_NAME
    * ftruncate to sets the size of the shared memory to TABLE_SIZE * sizeof(int) (space for two integer slots).
    * mmap to maps the shared memory into the process’s address space so that it can be accessed as a regular array (table)
* Initialize Semaphores
    * sem_full: Initialized to 0, so there are no items in the buffer at the beginning
    * sem_empty: Initialized to TABLE_SIZE (2), so the buffer has two empty slots initially
    * sem_mutex: Initialized to 1, as a binary semaphore for mutual exclusion
* Producer:
    * //produce an item
    * sem_wait(empty): Waits until there is an empty slot in the buffer (decrements empty by 1 because 1 slot will be filled. If empty is 0 which mean buffer is full
    * sem_wait(mutex): The value of mutex is also reduced to prevent consumer to access the buffer. Producer enters the critical section. Mutex blocking the consumer(Only one process can enter the critical section at a time)
* Producer Inside the Critical Section:
    * table[produce_index] = i + 1: produces an item (ex: 1 for the 1st item) then places it at the produce index position in the table.
    * Using if condition to check and Moves to the next slot in table(after 1, it wraps back to 0).
* Exiting the Critical Section:
    * sem_post(sem_mutex): Releases the mutex, allowing the consumer or producer to enter the critical section.
    * sem_post(sem_full): Signals that a new item is available in the buffer (increments sem_full), unblocking the consumer if it’s waiting.
* Clean and releases shared memory and closes the semaphores to free system resources.
* Consumer: Do similar when consumer enter and exist the CS except the consumer is removing an item from buffer, the value of “full” is reduced by 1 , if sem_full is 0 (buffer is empty). The value is mutex is also reduced so that the producer can’t access the buffer at this moment. Then after consumer has consumed the item, it increases the value of “empty” by 1. The value of mutex is also increased as it turn on the signal so that producer can access the buffer 
    * //consume an item
    * sem_wait(full);
    * sem_wait(sem_mutex);
    * sem_post(sem_mutex); 
    * sem_post(sem_empty)

## Instruction to run on your local machine
`g++ producer.cpp -pthread -o producer`
`g++ consumer.cpp -pthread -o consume`
`./producer & ./consumer &`

## Example:
<div align=center>
  <image src="1.png" alt="cs">
  <image src="2.png" alt="cs">
</div>




