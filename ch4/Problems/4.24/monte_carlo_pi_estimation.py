import random
import threading


# Shared global variable to count points inside the circle
points_in_circle = 0


def point_generator(num_points):
    global points_in_circle
    for _ in range(num_points):
        x, y = random.random(), random.random()
        # Check if the point is inside the circle (x^2 + y^2 <= 1)
        if x**2 + y**2 <= 1:
            points_in_circle += 1

if __name__ == "__main__":
    num_threads = 4 # Can be changed later
    total_points = 100000000 # Can be changed later


    threads = []
    points_per_thread = total_points // num_threads

    for _ in range(num_threads):
        thread = threading.Thread(target=point_generator, args=(points_per_thread,))
        thread.start()
        threads.append(thread)

    for thread in threads:
        thread.join()

    
    # Calculate the estimated value of π
    pi_estimation = 4.0 * points_in_circle / total_points

    print("Estimated value of n:", pi_estimation)