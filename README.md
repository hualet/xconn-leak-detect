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
- [ ] call location is not really readable
```
leaks detected:
  call locations: 0x7f7e94cf9d93(_ZN19QXcbBasicConnectionC1EPKc)
    open displays:
      - display: 0x560a33940e50
  call locations: 0x560a07c3840f(Unknown)
    open displays:
      - display: 0x7f7dcc0012e0
  call locations: 0x560a07c38c05(Unknown)
    open displays:
      - display: 0x7f7dcc0132b0

```

## License

MIT
