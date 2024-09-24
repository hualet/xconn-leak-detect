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
- [ ] capture necessary signals
- [ ] work with X11 sessions

## License

MIT
