# Inferno
![Inferno Logo](/assets/images/Inferno.png)

## Note
"In honor of Dennis Ritchie's legacy, may Inferno serve as a beacon of ingenuity and collaboration, embodying the spirit of exploration and advancement in the realm of technology."

## Cortex Security (CS) C2
* Client/Server channel over TCP
* Server/Agent channel over HTTP
* Starting HTTP listener
* Entering session

## Architecture
![Inferno Architecture](/assets/images/architecture.png)

### Supported commands
| Mode | Command | Description
| ---  | --- | --- |
| HTTP listener | http | Start an HTTP listener |
| ENTER | enter | Enter inside session |

### Supported agent commands
| Mode | Command | Description
| --- | --- | --- |
| WHOAMI | whoami | Get user login |
| HOSTNAME | hostname | Get machine hostname |

## Manual

### Compiling instructions
Navigate to the project directory and run:
```bash
gcc client.c -o client
```
```bash
gcc server.c -o server
```
```bash
gcc devil-lin.c -o devil-lin
```

### Utilisation instructions
First you will need to start the server
```bash
┌──(asx㉿asx)-[~/Inferno]
└─$ ./server
Initializing socket...
Binding socket to c2 address...
Listening to 127.0.0.1:33333
Accepting...
```
> Starting the server

Once the server is started it will listen on port 33333. Now you can start the client
```bash
┌──(asx㉿asx)-[~/Inferno]
└─$ ./client
[>] Initializing socket...

  _     <-. (`-')_            (`-')  _   (`-') <-. (`-')_                
 (_)       \( OO) )  <-.      ( OO).-/<-.(OO )    \( OO) )     .->       
 ,-(`-'),--./ ,--/(`-')-----.(,------.,------,),--./ ,--/ (`-')----.     
 | ( OO)|   \ |  |(OO|(_\---' |  .---'|   /`. '|   \ |  | ( OO).-.  '    
 |  |  )|  . '|  |)/ |  '--. (|  '--. |  |_.' ||  . '|  |)( _) | |  |    
(|  |_/ |  |\    | \_)  .--'  |  .--' |  .   .'|  |\    |  \|  |)|  |    
 |  |'->|  | \   |  `|  |_)   |  `---.|  |\  \ |  | \   |   '  '-'  '    
 `--'   `--'  `--'   `--'     `------'`--' '--'`--'  `--'    `-----'     

Info: X joined session
[>] Connecting...
[>] Connected

[ASX]@[INFERNO]> 
```
> Starting the client

You will see the connection is accepted. Now you can start an HTTP listener on port 80
```bash
┌──(asx㉿asx)-[~/Inferno]
└─$ ./client
[>] Initializing socket...

  _     <-. (`-')_            (`-')  _   (`-') <-. (`-')_                
 (_)       \( OO) )  <-.      ( OO).-/<-.(OO )    \( OO) )     .->       
 ,-(`-'),--./ ,--/(`-')-----.(,------.,------,),--./ ,--/ (`-')----.     
 | ( OO)|   \ |  |(OO|(_\---' |  .---'|   /`. '|   \ |  | ( OO).-.  '    
 |  |  )|  . '|  |)/ |  '--. (|  '--. |  |_.' ||  . '|  |)( _) | |  |    
(|  |_/ |  |\    | \_)  .--'  |  .--' |  .   .'|  |\    |  \|  |)|  |    
 |  |'->|  | \   |  `|  |_)   |  `---.|  |\  \ |  | \   |   '  '-'  '    
 `--'   `--'  `--'   `--'     `------'`--' '--'`--'  `--'    `-----'     

Info: X joined session
[>] Connecting...
[>] Connected

[ASX]@[INFERNO]> http

[>] Starting listener...
[>] Listening
==============================
 IP: 127.0.0.1                  
==============================
 PORT: 80                
==============================

[>] Waiting for response...
```
> Starting an HTTP listener on port 80

Now the client will wait that an agent is launched. You can launch an agent by the following command:
```bash
┌──(asx㉿asx)-[~/Inferno]
└─$ ./devil 
Initializing socket...
Connecting...
Connected
```
> Starting an agent

Now you can enter session to interact with agent
```bash
[ASX]@[INFERNO]> enter
Entering session...
Sent: ok

[ASX]@[SESSION]> 
```
> Entering the session

Now its time to send commands
```bash
[ASX]@[SESSION]> whoami
[>] Sending payload to server: whoami
[>] Waiting for response...
[>] Result: asx
```
> Task the agent to perform whoami command

You can now exit properly
```bash
[ASX]@[SESSION]> exit
[>] 
Closing c2 and exiting...
```
> Exiting the C2

## Drawbacks
> [!CAUTION]
> If you receive a segfault while launching client its cause a bug can make the server not started the port 80

> [!CAUTION]
> If one of the three composant of the C2 e.g. client/server/agent loose connection, the C2 will no more work and you need close all program and restart C2

> [!CAUTION]
> Be careful to always having server running before start the client

> [!CAUTION]
> Be careful to always having listener ready before starting the agent

## Infos
> [!TIP]
> You can analyze the content of the payload sent and received
```bash
==========START PAYLOAD==========
Receiving from client:
POST /endpoint HTTP/1.0
Host: inferno.com
Content-Type: application/json
Content-Length: 20
Connection: close

{'payload':'whoami'}
Sending to agent:

POST /endpoint HTTP/1.0
Host: inferno.com
Content-Type: application/json
Content-Length: 20
Connection: close

{'payload':'whoami'}
Received from agent: asx
Sending to client: asx
===========END PAYLOAD===========
```
> Analyzing payload in server message

## End of transmission
