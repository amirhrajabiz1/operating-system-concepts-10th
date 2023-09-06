import threading

def generate_fibonacci(n, fib_sequence):
    fib_sequence[0] = 0
    fib_sequence[1] = 1
    for i in range(2, n):
        fib_sequence[i] = fib_sequence[i - 1] + fib_sequence[i - 2]

def main():
    num_fibonacci = int(input("Enter the number of Fibonacci numbers to generate: "))
    fib_sequence = [0] * num_fibonacci

    fibonacci_thread = threading.Thread(target=generate_fibonacci, args=(num_fibonacci, fib_sequence))
    fibonacci_thread.start()
    fibonacci_thread.join()

    print("Generated Fibonacci sequence: ", fib_sequence)


if __name__ == "__main__":
    main()