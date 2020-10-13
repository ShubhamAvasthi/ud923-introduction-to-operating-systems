#include <array>
#include <chrono>
#include <cstdio>
#include <random>
#include <thread>
#include <pthread.h>

const int num_readers = 5;
const int num_writers = 5;
const int num_reads = 5;
const int num_writes = 5;
const int min_sleep_millis = 0;
const int max_sleep_millis = 200;
const int min_shared_val = 111111111;
const int max_shared_val = 999999999;

int shared_val = 0;
int num_active_readers = 0;
int num_waiting_readers = 0;
int num_readers_minus_num_writers = 0;

pthread_mutex_t shared_val_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t read_phase = PTHREAD_COND_INITIALIZER;
pthread_cond_t write_phase = PTHREAD_COND_INITIALIZER;

void *reader_main(void *args)
{
	const int id = *((int*) args);
	// Using a Mersenne Twister Engine, because default_random_engine (a Linear Congruential Engine)
	// produces unacceptably less randomized values on MinGW-w64
	std::mt19937 generator(id);
	std::uniform_int_distribution<> sleep_millis_dist(min_sleep_millis, max_sleep_millis);
	for(int i = 0; i < num_reads; i++)
	{
		const int random_sleep_millis = sleep_millis_dist(generator);
		std::this_thread::sleep_for(std::chrono::milliseconds(random_sleep_millis));

		// Enter critical section
		pthread_mutex_lock(&shared_val_mutex);
		num_waiting_readers++;
		while(num_readers_minus_num_writers == -1)
		{
			pthread_cond_wait(&read_phase, &shared_val_mutex);
		}
		num_waiting_readers--;
		num_readers_minus_num_writers++;
		num_active_readers++;
		pthread_mutex_unlock(&shared_val_mutex);

		// Write shared_val
		printf(">>>>> Reader %2d : shared_val = %d, num_active_readers = %2d, random_sleep_millis = %3d\n", id, shared_val, num_active_readers, random_sleep_millis);
		
		// Exit critical section
		pthread_mutex_lock(&shared_val_mutex);
		num_readers_minus_num_writers--;
		num_active_readers--;
		if(num_readers_minus_num_writers == 0)
		{
			pthread_cond_signal(&write_phase);
		}
		pthread_mutex_unlock(&shared_val_mutex);
	}
}

void *writer_main(void *args)
{
	const int id = *((int*) args);
	// Using a Mersenne Twister Engine, because default_random_engine (a Linear Congruential Engine)
	// produces unacceptably less randomized values on MinGW-w64
	std::mt19937 generator(num_readers + id);
	std::uniform_int_distribution<> sleep_millis_dist(min_sleep_millis, max_sleep_millis);
	std::uniform_int_distribution<> shared_val_dist(min_shared_val, max_shared_val);
	for(int i = 0; i < num_reads; i++)
	{
		const int random_sleep_millis = sleep_millis_dist(generator);
		std::this_thread::sleep_for(std::chrono::milliseconds(random_sleep_millis));

		// Enter critical section
		pthread_mutex_lock(&shared_val_mutex);
		while(num_readers_minus_num_writers != 0)
		{
			pthread_cond_wait(&write_phase, &shared_val_mutex);
		}
		num_readers_minus_num_writers = -1;
		pthread_mutex_unlock(&shared_val_mutex);

		// Write and read shared_val
		shared_val = shared_val_dist(generator);
		printf("@@@@@ Writer %2d : shared_val = %d, num_active_readers = %2d, random_sleep_millis = %3d\n", id, shared_val, num_active_readers, random_sleep_millis);
		
		// Exit critical section
		pthread_mutex_lock(&shared_val_mutex);
		num_readers_minus_num_writers = 0;
		if(num_waiting_readers > 0)
		{
			pthread_cond_broadcast(&read_phase);
		}
		else
		{
			pthread_cond_signal(&write_phase);
		}
		pthread_mutex_unlock(&shared_val_mutex);
	}
}

int main()
{
	std::array<pthread_t, num_readers> reader_threads;
	std::array<int, num_readers> reader_num;
	for(int i = 0; i < num_readers; i++)
	{
		reader_num[i] = i;
		pthread_create(&reader_threads[i], nullptr, reader_main, &reader_num[i]);
	}

	std::array<pthread_t, num_writers> writer_threads;
	std::array<int, num_writers> writer_num;
	for(int i = 0; i < num_writers; i++)
	{
		writer_num[i] = i;
		pthread_create(&writer_threads[i], nullptr, writer_main, &writer_num[i]);
	}

	for(int i = 0; i < num_readers; i++)
	{
		pthread_join(reader_threads[i], nullptr);
	}

	for(int i = 0; i < num_writers; i++)
	{
		pthread_join(writer_threads[i], nullptr);
	}
}
