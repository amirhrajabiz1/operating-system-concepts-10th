
public class PrimeNumberGenerator extends Thread {
    private int limit;

    public PrimeNumberGenerator(int limit) {
        this.limit = limit;
    }

    @Override
    public void run() {
        System.out.println("Prime numbers up to " + limit + ":");
        for (int number = 2; number <= limit; number++) {
            if (isPrime(number)) {
                System.out.print(number + "  ");
            }
        }
        System.out.println();
    }

    private boolean isPrime(int num) {
        if (num <= 1) {
            return false;
        }

        for (int i = 2; i <= Math.sqrt(num); i++) {
            if (num % i == 0) {
                return false;
            }
        }
        return true;
    }

    public static void main(String[] args) {
        if (args.length != 1) {
            System.out.println("Usage: java PrimeNumberGenerator <number>");
            return;
        }

        int number = Integer.parseInt(args[0]);

        PrimeNumberGenerator primeThread = new PrimeNumberGenerator(number);
        primeThread.start();

        try {
            primeThread.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        System.out.println("Main thread exits.");
    }
}