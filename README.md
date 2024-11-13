## Producer-consumer Problem solution

This program is to solve producer and consumer problems. It involves two types of processes: 

1. **Producers** to generate data
2. **Consumers** to process that data

Both process share common buffer present as shared memory. The goal is to ensure that the producer doesn’t add data to a full buffer and the consumer doesn’t remove data from an empty buffer. Only one process can accesses the buffer at any given time to prevent conflict

To solve this, I use **semaphore** (counting and binary) to synchronize producer and consumer.  I also use **mutual exclusion** to lock the buffer when it being accessed by either process

## Outline:
1. Shared buffer setup:
    - Create a shared memory buffer (table) that has two slots (each slot can hold 1 item). The buffer is shared to allow processes to communicate

2. Semaphore setup:
    - Counting semaphore to keep track empty/filled slots in buffer: `sem_empty`, `sem_full`
    - Binary semaphore (acting as a mutex) to do mutual exclusion, so that only 1 process  accesses the buffer at a time: `sem_mutex`
        * `sem_full`: Initialized to 0, so there are no items in the buffer at the beginning
        * `sem_empty`: Initialized to TABLE_SIZE (2), so the buffer has two empty slots initially
        * `sem_mutex`: Initialized to 1, as a binary semaphore for mutual exclusion

3. Shared Memory setup: 
    - `shm_open` to opens or creates a shared memory object, allowing both processes to access the same buffer
    - `ftruncate` to sets the size of the shared memory to TABLE_SIZE * sizeof(int) (space for 2 integer slots - buffer size)
    - `mmap` to maps the shared memory into the process’s address space so that it can be accessed as a regular array (table)

## Execution step:

### Producer process:
1. Wait for empty slot: The producer first waits on `sem_empty` to ensure there is at least one empty slot in the buffer, decrements empty by 1 because 1 slot will be filled. If `sem_empty` is 0 (buffer is full), the producer is blocked until a slot becomes available

2. Enter Critical Section: The producer waits on `sem_mutex` to enter the critical section, ensuring that no other process (consumer or another producer) is accessing the buffer concurrently

3. Produce item and place in buffer: Inside the CS, producer places an item in the buffer at the current `produce_index` position in table. Using if condition to check and moves to the next slot in table(after 1, it wraps back to 0)

4. Exit Critical Section:
    - The producer release the `sem_mutex` so other processes to enter CS
    - Then call to `sem_full` to signal
     that there is an item available in the buffer for consumer

### Consumer process:
1. Wait for filled slot: The consumer first waits on `sem_full` to ensure there is at least one item in the buffer to consume, decrements full by 1 because 1 slot will be available. If sem_full is 0 (buffer is empty), the consumer is blocked until an item becomes available.

2. Enter CS: consumer waits on `sem_mutex` to enter CS, ensuring that no other process (producer or another consumer) is accessing the buffer concurrently.

3. Consume item from buffer: Inside the CS, consumer places an item in the buffer at the current `consume_index` position in table. Using if condition to check and moves to the next slot in table(after 1, it wraps back to 0)

4. Exit Critical Section:
    - The consumerr release the `sem_mutex` so other processes to enter CS
    - Then call to `sem_empty` to signal that there is an item available in the buffer for consumer

### Cleanup: 
- The producer and consumer processes release their shared resources by unmapped the process's address space and closed. Also closed and unlinked semaphores to free resources

## Reference:  

[Producer-Consumer Problem using Semaphores][producer-consumer-problem-using-semaphores]

[producer-consumer-problem-using-semaphores]: https://www.geeksforgeeks.org/producer-consumer-problem-using-semaphores-set-1/

## Instruction to run on your local machine
`g++ producer.cpp -pthread -o producer`

`g++ consumer.cpp -pthread -o consume`

`./producer & ./consumer &`

## Example:
<div align=center>
  <image src="1.png" alt="cs">
  <image src="2.png" alt="cs">
</div>




