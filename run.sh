cd server -odebug
./server
cd ..

cd mom
./mom -odebug
cd ..

cd scheduler
./scheduler -odebug -mode FIFO
cd ..
