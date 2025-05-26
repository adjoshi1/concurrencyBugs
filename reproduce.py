
import threading

import time



def worker(num):

    print(f'Worker {num} started')

    time.sleep(2)

    print(f'Worker {num} finished')



threads = []

for i in range(10):

    t = threading.Thread(target=worker, args=(i,))

    threads.append(t)

    t.start()



for t in threads:

    t.join()

print('Main thread finished')