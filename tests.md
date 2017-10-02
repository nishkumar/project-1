# TESTS
Tests for simple Web server.

## Http request
Example htttp request as seen by a server running on port 8080. Run dumper.py and type in the url 0.0.0.0:8080 in your browser to see incoming requests.

### Test request using curl

Run the following command to check the http response and compare agianst our liso response :  ``` curl -I http://amazon.com ```
```
curl -I http://amazon.com
HTTP/1.1 301 Moved Permanently
Server: Server
Date: Mon, 02 Oct 2017 01:27:38 GMT
Content-Type: text/html
Content-Length: 179
Connection: keep-alive
Location: https://amazon.com/

```

Start the server on port 8080. Now run the following command to send request to liso server : ```  curl -I 127.0.0.1:8080/index.html or curl --head 127.0.0.1:8080/index.html ``` 
```
HTTP/1.1 200 OK
Connection: close
Content-Length: 796
Content-Type: text/html
Date: Mon, 02 Oct 2017 01:32:23 GMT
Last-Modified: Sun, 24 Sep 2017 20:20:01 GMT
Server: Liso/1.0
```

Similarly curl can be sued to test GET and POST requests.

```
GET : curl 127.0.0.1:8080/index.html
POST: curl -i  --data  "name=nish" 127.0.0.1:8080 
HEAD: curl --head 127.0.0.1:8080/index.html

```



## Memory Leak Check

We used the memcheck tool from Valgrind to detect if there's any memory leaks in our program.
>From Valgrind.org :   For Ubuntu linux ``` sudo apt-get install valgrind ```

>Valgrind is an instrumentation framework for building dynamic analysis tools. It comes with a set of tools each of which performs some kind of debugging, profiling, or similar task that helps you improve your programs. Valgrind’s architecture is modular, so new tools can be created easily and without disturbing the existing structure.

One of the useful tools provided by Valgring is Memcheck, which is a memory error detector. It helps you make your programs, particularly those written in C and C++, more correct.

To test our server code, run:
>valgrind --leak-check=full -v ./lisod <portnumber>

Here's the result we got after running the memory check:
![memCheck](./images/memLeak.png)
We can see that there appears to be no leaks in our server code. Hooray!
