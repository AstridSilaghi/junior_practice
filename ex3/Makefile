build: client.c server.c
	gcc -g client.c -o client
	gcc -g server.c -o server

run_server: server
	./server $(PORT)

run_client: client
	./client $(PORT)

clean: client server
	rm client server 
	rm -r client_*_files
