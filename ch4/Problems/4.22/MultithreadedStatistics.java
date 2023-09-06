import java.util.Arrays;

class StatisticsCalculator implements Runnable{
    private int[] numbers;
    private int average;
    private int minimum;
    private int maximum;

    public StatisticsCalculator(int[] numbers) {
        this.numbers = numbers;
    }

    public int getAverage() {
        return average;
    }

    public int getMinimum() {
        return minimum;
    }

    public int getMaximum() {
        return maximum;
    }

    @Override
    public void run() {
        int sum = 0;
        minimum = numbers[0];
        maximum = numbers[0];

        for (int number : numbers) {
            sum += number;

            if (number < minimum) {
                minimum = number;
            }

            if (number > maximum) {
                maximum = number;
            }
        }

        average = sum / numbers.length;
    }
    
}


public class MultithreadedStatistics {
    public static void main(String[] args) {
        if (args.length == 0) {
            System.out.println("Usage: java MultithreadedStatistics <num1> <num2> ... <numN>");
            return;
        }

        int[] numbers = new int[args.length];

        for (int i = 0; i < args.length; i++) {
            numbers[i] = Integer.parseInt(args[i]);
        }

        StatisticsCalculator calculator = new StatisticsCalculator(numbers);
        Thread thread = new Thread(calculator);
        thread.start();

        try {
            thread.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        System.out.println("The average value is " + calculator.getAverage());
        System.out.println("The minimum value is " + calculator.getMinimum());
        System.out.println("The maximum value is " + calculator.getMaximum());
    }
}