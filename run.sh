cd mom
./mom -odebug -i 127.0.0.1 -p 5003
cd ..

cd server 
./server -i 127.0.0.1 -p 5001 -si 127.0.0.1 -sp 5002 -odebug
cd ..


cd scheduler
./scheduler -mode FIFO -i 127.0.0.1 -p 5002
cd ..

