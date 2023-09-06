public class FibonacciMultithreaded {
    private static int[] fibonacciSequence;

    public static void main(String[] args) {
        if (args.length != 1) {
            System.out.println("Usage: java FibonacciMultithreaded <number_of_fibonacci_numbers>");
            return;
        }

        int n = Integer.parseInt(args[0]);
        fibonacciSequence = new int[n];

        Runnable fibonacciGenerator = () -> {
            for (int i = 0; i < n; i++) {
                if (i <= 1) {
                    fibonacciSequence[i] = i;
                } else {
                    fibonacciSequence[i] = fibonacciSequence[i - 1] + fibonacciSequence[i - 2];
                }
            }
        };

        Thread fibonacciThread = new Thread(fibonacciGenerator);
        fibonacciThread.start();

        try {
            fibonacciThread.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        System.out.println("Fibonacci Sequence:");
        for (int num : fibonacciSequence) {
            System.out.print(num + "\t");
        }
    }
}