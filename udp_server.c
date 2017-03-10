/*
    Simple udp server

	Open another terminal and connect to this udp server using netcat and then send some data. 
	The same data will be send back by the server. Over here we are using the ncat command from 
	the nmap package.

	$ ncat -vv localhost 8888 -u
	Ncat: Version 5.21 ( http://nmap.org/ncat )
	Ncat: Connected to 127.0.0.1:8888.
	hello
	hello
	world
	world


	The netstat command can be used to check if the udp port is open or not.
	$ netstat -u -a
	Active Internet connections (servers and established)
	Proto Recv-Q Send-Q Local Address           Foreign Address         State      
	udp        0      0 localhost:11211         *:*                                
	udp        0      0 localhost:domain        *:*                                
	udp        0      0 localhost:45286         localhost:8888          ESTABLISHED
	udp        0      0 *:33320                 *:*                                
	udp        0      0 *:ipp                   *:*                                
	udp        0      0 *:8888                  *:*                                
	udp        0      0 *:17500                 *:*                                
	udp        0      0 *:mdns                  *:*                                
	udp        0      0 localhost:54747         localhost:54747         ESTABLISHED
	udp6       0      0 [::]:60439              [::]:*                             
	udp6       0      0 [::]:mdns               [::]:*


*/
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
 
#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data
#define CLIENT1_HEADER 	0x01
#define CLIENT2_HEADER 	0x02
int store_client1_addr_flag = 0;
int store_client2_addr_flag = 0;

 
void die(char *s)
{
    perror(s);
    exit(1);
}
 
int main(void)
{
    struct sockaddr_in si_me, si_other, si_other_store[2];
     
    int s, i, slen = sizeof(si_other) , recv_len;
    char buf[BUFLEN];
     
    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
     
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
     
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }
     
	printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
	
    //keep listening for data
    while(1)
    {
        printf("Waiting for data...");
        fflush(stdout);
         
		memset (buf, 0, BUFLEN);
        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
        {
            die("recvfrom()");
        }
         
        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		
		if (buf[0] == (unsigned char)CLIENT1_HEADER )
		{
			printf (" Message from 1st client = %s \n", &buf[1]);
			if (store_client1_addr_flag != 1) 
			{
				si_other_store[0] = si_other;
				store_client1_addr_flag = 1;
			}
		}
		else if (buf[0] == (unsigned char)CLIENT2_HEADER )
		{
			printf (" Message from 2nd client = %s \n", &buf[1]);
			if (store_client2_addr_flag != 1) 
			{
				si_other_store[1] = si_other;
				store_client2_addr_flag = 1;
			}
		}
		else
		{
			printf ("Wrong client \n");
		}
		
#if 1	
		int client_number;
		printf ("Enter the client number[1:2] to send the data \n");
		scanf("%d", &client_number);

		if ((client_number == 1)  && (store_client1_addr_flag == 1) )
		{
		  	si_other = si_other_store[0];
		}
		else if ((client_number == 2)  && (store_client2_addr_flag == 1) ) 
		{
		  	si_other = si_other_store[1];
		}
		else
		{
			printf (" Wrong client number or client is not available \n");
			memset (&si_other, 0, slen);
			continue;
		}
#endif 			
         
        //now reply the client with the same data
        if (sendto(s, &buf[1], recv_len-1, 0, (struct sockaddr*) &si_other, slen) == -1)
        {
            die("sendto()");
        }
    }
 
    close(s);
    return 0;
}
