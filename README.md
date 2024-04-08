# Inferno
![Inferno Logo](/assets/images/Inferno.png)

## Note
"Exploring the digital realm through C feels like journeying through the fiery depths of Inferno with nothing but a trusty torch in hand. It's not just coding; it's an adventure, where every line of C unlocks a new door to discovery in the digital underworld."

## Cortex Security (CS) C2
* Client/Server channel over TCP
* Server/Agent channel over HTTP
* Listing HTTP listeners
* Starting HTTP listeners
* Stopping HTTP listeners
* Listing HTTP sessions
* Entering HTTP sessions
* Destroying HTTP sessions
* Switching sessions

## Architecture
![Inferno Architecture](/assets/images/architecture.png)

### Supported commands
| Mode | Command | Example | Description |
| ---  | --- | --- | --- |
| LIST listeners | listeners | / | list actives listeners |
| START listener | http | http 127.0.0.1 80 | Start an HTTP listener |
| STOP listener | stop | stop http 1 | Stop listener HTTP with ID 1 |
| LIST sessions | sessions | / | list actives sessions
| ENTER session | enter | enter 1 | Enter inside session with ID 1 |
| DESTROY session | kill | kill 1 | Destroy session with ID 1 |

### Supported agent commands
| Mode | Command | Description
| --- | --- | --- |
| WHOAMI | whoami | Get user login |
| HOSTNAME | hostname | Get machine hostname |
| LIST | ls | List files |
| DIRECTORY | pwd | Print current directory
> Some commands can be run with arguments 

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
> More devil-lin with different ports 80, 8080, 8000 etc

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
> Be careful to always having server running before start the client

> [!NOTE]
> Readme not updated to latest

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
