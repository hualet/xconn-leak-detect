# xconn-leak-detect
X11 connection leak detection tool.

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


## License

MIT
