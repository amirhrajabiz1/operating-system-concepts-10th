public class Client
{
    public static void main(String[] args) {
        ThreadPool pool = new ThreadPool(2);

        pool.add(new Task(1,0));
        pool.add(new Task(2,0));
        pool.add(new Task(3,0));
        pool.add(new Task(4,0));
        pool.add(new Task(5,0));
        pool.add(new Task(6,0));
        pool.shutdown();
    }
}
