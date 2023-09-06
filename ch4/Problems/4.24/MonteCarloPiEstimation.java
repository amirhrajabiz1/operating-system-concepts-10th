import java.util.Random;

public class MonteCarloPiEstimation {

    // Shared global variable to count points inside the circle
    private static long pointsInCircle = 0;

    public static void main(String[] args) {
        int numThreads = 1; // You can change the number of threads here
        int totalPoints = 1000000000; // You can change the number of random points generated here

        Thread[] threads = new Thread[numThreads];

        for (int i = 0; i < numThreads; i++) {
            threads[i] = new Thread(new PointGenerator(totalPoints / numThreads));
            threads[i].start();
        }

        for (int i = 0; i < numThreads; i++) {
            try {
                threads[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        // Calculate the estimated value of n
        double piEstimation = 4.0 * pointsInCircle / totalPoints;

        System.out.println("Estimated value of n: " + piEstimation);
    }

    // Thread class to generate random points and count points inside the circle
    private static class PointGenerator implements Runnable {
        private int numPoints;

        public PointGenerator(int numPoints) {
            this.numPoints = numPoints;
        }

        @Override
        public void run() {
            Random random = new Random();
            for (int i = 0; i < numPoints; i++) {
                double x = random.nextDouble();
                double y = random.nextDouble();
                // Check if the point is inside the circle (x^2 + y^2 <= 1)
                if (x * x + y * y <= 1) {
                    synchronized(MonteCarloPiEstimation.class) {
                        pointsInCircle++;
                    }
                }
            }
        }
    }
}