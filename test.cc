#include <iostream>
#include <strstream>
#include <unistd.h>
#include "threadpool.h"

using namespace std;

int main(void) {
  threadpool::pool pool;

  pool.start(4);

  thread t = thread([&] {
    size_t id = 0;
    while (id < 20) {
      pool.enqueue([=] {
        sleep(1);

        strstream ss;
        ss << "[job " << id << "] is done" << endl;
        cout << ss.str();
      });
      id++;
    }

    sleep(3);

    cout << "shutdown begin" << endl;
    pool.shutdown();
    cout << "shutdown end" << endl;
  });

  t.join();

  return 0;
}
