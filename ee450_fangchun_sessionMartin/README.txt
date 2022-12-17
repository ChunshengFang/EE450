A:
Name:Chunsheng Fang


B:
USC ID:5043794779


C:
In this project, there are two clients which are clientA and B and four servers which are serverC, serverS, serverT and serverP. More importantly, I have added a common.cpp which contains the functions related to the tcp and udp connections and the graphs. Furthermore, I have implemented a model that clients can send usernames to the central server and then the central server will send the information to the servers which will deal with the request and send the reply to the central servers. Moreover, the serverS has access to the datebase file named scores.txt which stores scores for users and the serverP has access to the database file named edgelist.txt which stores the connections of each user. To be specific, the serverT handles the requests for the graphs if the central server sends the usernames from the clients.Then it sends back results to the central server. After that, the central system will send them to the serverS which stores the scores for users. If serverS receives the results through the central server, the serverS will send their scores to the central server and then the central server will send the results to the serverP which will calculate the results by the formula.  After the central server receives the results, it will send them to the clients. That is my work in this project.

 
D:
clientA.cpp clientA.h:
create clientA and send and receive data

clientB.cpp clientB.h
create clientA and send and receive data

central.cpp central.h (the main server)
accept links from clientA and clientB, and interacts with serverT, serverP, and serverS

serverT.cpp serverT.h
receive the data sent by central and return after processing

serverS.cpp serverS.h
receive the data sent by central and return after processing

serverP.cpp serverP.h
receive the data sent by central and return after processing

common.cpp common.h
Contain common codes for creating tcp and udp, applicable to all clients and servers. There is also a split function to split the string.
As well as the graphics category, it is a data structure containing dfs, a graphics path and other algorithms, suitable for other categories
(some  codes are from the the internet.https://www.geeksforgeeks.org/graph-and-its-representations/)



E:
clients to central
send to the serverC which are a string of usernames.

central to serverT:
aa string combinations of the clientA and clientB which is spliced by an exclamation mark to split.

serverT to central:
<username>:<adjacent username>，<adjacent username>.....for example, Richvael:Victor,King!King:Rachael. In reality, it converts an adjacency list into a string form. There is another case that if the two usernames are not connected or the username does not exist, it will return wrong. At this time, the server will directly send back the string of usernameA! usernameB to clientA and clientB.

central to serverS:
the data received from the serverT

serverS to central: It is based on the data sent back by serverT, an extra score separated by a colon
<username>:<adjacent username>，<adjacent username>:<score>！

central to serverP(format):
usernameA!usernameB#<username>:<adjacent username>，<adjacentusername>:<score>！

serverP  to central:
It is a list of user names separated by an exclamation mark!,followed by a gap(float number) separated by #. <username>!<username>#<gap>

central to clients
if these servers successfully process, the central directly sends the data sent by serverP back to the client, and then the clientA and clientB.
If it fails, as mentioned above, it will send usernameA!usernameB back to the client




G:
When the input of edgelists file is large, it may fail due to insufficient transmit and receive buffers



H:
The code for creating and linking tcp udp comes from the network(Beej-Tutorial), and I modified the code for clientA and clientB through the Beej_Tutorial. Moreover, the function of the graph comes from the network.（https://www.geeksforgeeks.org/graph-and-its-representations/ ）. And the dfs algorithm is learnt from the class and the code are from the internet(https://www.softwaretestinghelp.com/cpp-dfs-program-to-traverse-graph/).



Note: 
1.When you run the program, you must run the servers at first then run the client. This is the requirement for this program.
2.if you cannot run the program, you can try the command 'make' first.
 
 



