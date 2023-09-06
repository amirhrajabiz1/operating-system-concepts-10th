import threading
import math
import sys

class PrimeNumberGenerator(threading.Thread):
    def __init__(self, limit):
        super(PrimeNumberGenerator, self).__init__()
        self.limit = limit


    def run(self):
        print(f"Prime numbers up to {self.limit}:")
        for number in range(2, self.limit + 1):
            if self.is_prime(number):
                print(number, end=" ")
        print()


    def is_prime(self, num):
        if num <= 1:
            return False
        
        for i in range(2, int(math.sqrt(num)) + 1):
            if num % i == 0:
                return False
            
        return True
    

def main():
    if len(sys.argv) != 2:
        print("Usage: python prime_number_generator.py <number>")
        return
    
    try:
        number = int(sys.argv[1])
    except ValueError:
        print("Invalid input. Please enter a valid number.")
        return
    
    prime_thread = PrimeNumberGenerator(number)
    prime_thread.start()
    prime_thread.join()

    print("Main thread exits.")

if __name__ == "__main__":
    main()
