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
