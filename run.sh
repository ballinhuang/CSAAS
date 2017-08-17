cd server 
./server -odebug -i 127.0.0.1 -p 5001 -si 127.0.0.1 -sp 5002
cd ..

cd mom
./mom -odebug
cd ..

cd scheduler
./scheduler -odebug -mode FIFO
cd ..

