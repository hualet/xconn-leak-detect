# xconn-leak-detect

X11 connection leak detection tool.

Sometimes we encounter errors like: `Maximum number of clients reached`.

This is usually due to a connection leak.

To find which process is suspicious, you can use `sudo lsof -f -U /tmp/.X11-unix/X0` to list all X11 connections and find out which process has the most connections.

And then you can use this tool to analyze where the leaks happen in your application.

## Build

```
make -j`nproc`
```

## Usage

```
LD_PRELOAD=./xconn-leak-detect.so your_application
```

output should look like this:

```
leaks detected:
    open connections:
        connection: 0x55904e1f9460
        calltrace:
            ./libxconn_leak_detect.so(xcb_connect+0x87) [0x7f8c2ea627d5]
            /lib/x86_64-linux-gnu/libX11.so.6(_XConnectXCB+0x1ba) [0x7f8c2e8bed0a]
            /lib/x86_64-linux-gnu/libX11.so.6(XOpenDisplay+0xd7) [0x7f8c2e8af0c7]
            glxgears(+0x30f2) [0x55901e7c00f2]
            /lib/x86_64-linux-gnu/libc.so.6(+0x27b8a) [0x7f8c2e5dfb8a]
            /lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0x85) [0x7f8c2e5dfc45]
            glxgears(+0x3161) [0x55901e7c0161]

```


TODO:

- [x] count per frame 
- [ ] add logging
- [x] capture necessary signals
- [ ] work with X11 sessions
- [x] call location is not really readable, add calltrace
- [ ] optimize memory usage
- [x] hook xcb instead of x11


## License

MIT
