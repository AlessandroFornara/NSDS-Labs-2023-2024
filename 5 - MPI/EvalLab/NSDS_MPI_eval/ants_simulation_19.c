#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <math.h>
#include <string.h>

/*
 * Group number: 19
 *
 * Group members
 *  - Matteo Figini
 *  - Fabrizio Monti
 *  - Alessandro Fornara
 */

const float min = 0;
const float max = 1000;
const float len = max - min;
const int num_ants = 8 * 1000 * 1000;
const int num_food_sources = 10;
const int num_iterations = 500;

float random_position() {
  return (float) rand() / (float)(RAND_MAX/(max-min)) + min;
}

/*
 * Process 0 invokes this function to initialize food sources.
 */
void init_food_sources(float* food_sources) {
  for (int i=0; i<num_food_sources; i++) {
    food_sources[i] = random_position();
  }
}

/*
 * Process 0 invokes this function to initialize the position of ants.
 */
void init_ants(float* ants) {
  for (int i=0; i<num_ants; i++) {
    ants[i] = random_position();
  }
}

// Compute the average value for the positions array
float compute_average (float* positions, int size) {
  float average = 0.0f;
  for (int i = 0; i < size; i++) {
    average += positions[i];
  }
  average = average / size;
  return average;
}

// Return the index of the nearest source of food for the ant_pos.
int index_of_nearest_source (float ant_pos, float *food_sources, int num_food_sources) {
  int index_nearest_source = 0;
  float nearest_source = fabs(ant_pos - food_sources[0]);
  for (int i = 1; i < num_food_sources; i++) {
    float current_source = fabs(ant_pos - food_sources[i]);
    if (current_source < nearest_source) {
      nearest_source = current_source;
      index_nearest_source = i;
    }
  }
  return index_nearest_source;
}

// Update the position of the ant 
float update_position (float ant_pos, float center, float nearest_source) {
  float f1 = 0.01 * (nearest_source - ant_pos);
  float f2 = 0.012 * (center - ant_pos);
  float new_pos = ant_pos + f1 + f2;
  return new_pos;
}

int main() {
  MPI_Init(NULL, NULL);
    
  int rank;
  int num_procs;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

  srand(rank);
  const int ants_per_procs = num_ants / num_procs;

  float *global_ants = NULL;

  // Allocate space in each process for food sources and ants
  float *local_ants = (float *) malloc (sizeof(float) * ants_per_procs);
  memset((void *) local_ants, 0.0f, ants_per_procs * sizeof(float));

  float *food_sources = (float *) malloc(sizeof(float) * num_food_sources);
  memset((void *) food_sources, 0.0f, num_food_sources * sizeof(float));
  
  // Process 0 initializes food sources and ants
  if (rank == 0) {
    init_food_sources(food_sources);

    global_ants = (float *) malloc(sizeof(float) * num_ants);
    init_ants(global_ants);
  }

  // Process 0 distributed food sources and ants
  // For food sources, we broadcast the array
  MPI_Bcast(food_sources, num_food_sources, MPI_FLOAT, 0, MPI_COMM_WORLD);

  // For ants, we scatter the global_ants
  MPI_Scatter(global_ants, ants_per_procs, MPI_FLOAT, local_ants, ants_per_procs, MPI_FLOAT, 0, MPI_COMM_WORLD);
  
  // Iterative simulation
  float center = 0;

  float local_center = compute_average(local_ants, ants_per_procs);
  MPI_Reduce(&local_center, &center, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
  if (rank == 0) {
    center = center / num_procs;
  }
  MPI_Bcast(&center, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

  for (int iter = 0; iter < num_iterations; iter++) {
    for (int i = 0; i < ants_per_procs; i++) {
      int index = index_of_nearest_source(local_ants[i], food_sources, num_food_sources);
      local_ants[i] = update_position(local_ants[i], center, food_sources[index]);
    }

    local_center = compute_average(local_ants, ants_per_procs);
    MPI_Reduce(&local_center, &center, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank == 0) {
      center = center / num_procs;
    }
    MPI_Bcast(&center, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
      printf("Iteration: %d - Average position: %f\n", iter, center);
    }
  }

  // Free memory
  free(local_ants);
  free(food_sources);
  if (rank == 0) {
    free(global_ants);
  }
  
  MPI_Finalize();
  return 0;
}
