import sys
import threading

class StatisticsCalculator(threading.Thread):
    def __init__(self, numbers):
        super(StatisticsCalculator, self).__init__()
        self.numbers = numbers
        self.average = 0
        self.minimum = self.maximum = numbers[0]
    
    def run(self):
        sum_numbers = 0
        for number in self.numbers:
            sum_numbers += number
            if number < self.minimum:
                self.minimum = number
            if number > self.maximum:
                self.maximum = number
        self.average = sum_numbers / len(self.numbers)

def main():
    if len(sys.argv) < 2:
        print("Usage: python program_name.py <num1> <num2> ... <numN>")
        return
    
    numbers = [int(arg) for arg in sys.argv[1:]]
    calculator = StatisticsCalculator(numbers)
    calculator.start()
    calculator.join()

    print("The average value is", calculator.average)
    print("The minimum value is", calculator.minimum)
    print("The maximum value is", calculator.maximum)


if __name__ == "__main__":
    main()