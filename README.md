# Socket-Programming-with-Server-Client-Interaction


A different approach in the Eshop Project. We have the server.c code which has the role of the eshop, the client.c in the role of the customer
and one script running the clients.

The script runs the server and client code. In the server code we use a port as a parameter and the character & to run in the background. Then we run the code 5 times
depending on the number of the clients we have.

In the server code we open a socket , and we aim to create the catalog with the products.

When the server and the client establish connection we use the fork() so as to deal with the orders

At the end, when the requests from the customers about the products finish, we provide data about the orders.

We suggest the following commands to run the codes.

1.	sudo netstat -tnlp | grep :22222
2.	gcc server.c -o server
3.	gcc client.c -o client
4.	chmod +x script.sh
5.	./script.sh

