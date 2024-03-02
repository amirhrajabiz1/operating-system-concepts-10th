import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class DiningPhilosophers {
  public static void main(String[] args) {
    final int numPhilosophers = 5;
    DiningServer diningServer = new DiningServerImpl(numPhilosophers);
    Thread[] philosophers = new Thread[numPhilosophers];

    for (int i = 0; i < numPhilosophers; i++) {
      final int philosopherNumber = i;
      philosophers[i] = new Thread(() -> {
	try {
	  while (true) {
	    System.out.println("Philosopher " + philosopherNumber + " is thinking.");
	    Thread.sleep((long) (Math.random() * 3000) + 1000);
	    System.out.println("Philosopher " + philosopherNumber + " wants to eat.");
	    diningServer.takeForks(philosopherNumber);
	    System.out.println("Philosopher " + philosopherNumber + " is eating.");
	    Thread.sleep((long) (Math.random() * 3000) + 1000);
	    diningServer.returnForks(philosopherNumber);
	  }
	} catch (InterruptedException e) {
	  e.printStackTrace();
	}
      });
      philosophers[i].start();
    }
  }
}

interface DiningServer {
  void takeForks(int philosopherNumber) throws InterruptedException;
  void returnForks(int philosopherNumber) throws InterruptedException;
}

class DiningServerImpl implements DiningServer {
  private enum State { THINKING, HUNGRY, EATING };

  private int numPhilosophers;
  private State[] philosopherStates;
  private Lock lock;
  private Condition[] self;

  public DiningServerImpl(int numPhilosophers) {
    this.numPhilosophers = numPhilosophers;
    philosopherStates = new State[numPhilosophers];
    lock = new ReentrantLock();
    self = new Condition[numPhilosophers];
    for (int i = 0; i < numPhilosophers; i++) {
      philosopherStates[i] = State.THINKING;
      self[i] = lock.newCondition();
    }
  }

  public void takeForks(int philosopherNumber) throws InterruptedException {
    lock.lock();
    try {
      philosopherStates[philosopherNumber] = State.HUNGRY;
      System.out.println("Philosopher " + philosopherNumber + " is trying to pick up forks.");
      test(philosopherNumber);
      if (philosopherStates[philosopherNumber] != State.EATING)
	self[philosopherNumber].await();
    } finally {
      lock.unlock();
    }
  }

  public void returnForks(int philosopherNumber) throws InterruptedException {
    lock.lock();
    try {
      philosopherStates[philosopherNumber] = State.THINKING;
      System.out.println("Philosopher " + philosopherNumber + " has returned the forks.");
      test((philosopherNumber + numPhilosophers - 1) % numPhilosophers);
      test((philosopherNumber + 1) % numPhilosophers);
    } finally {
      lock.unlock();
    }
  }

  private void test(int philosopherNumber) {
      if (philosopherStates[philosopherNumber] == State.HUNGRY &&
	  philosopherStates[(philosopherNumber + numPhilosophers - 1) % numPhilosophers] != State.EATING &&
	  philosopherStates[(philosopherNumber + 1) % numPhilosophers] != State.EATING) {
	    philosopherStates[philosopherNumber] = State.EATING;
	    self[philosopherNumber].signal();
	    System.out.println("Philosopher " + philosopherNumber + " has picked up forks and started eating.");
	  }
  }
}
