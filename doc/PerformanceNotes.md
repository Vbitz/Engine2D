# Performance Notes
All these tests are done on OSX 10.9.1 on a Macbook Air 13 inch with a Haswell i7.

## Engine::Event
Tested using CoreEventTest on version `GIT_0eb0d5e`

```
C++ Event::On Performance Test x 2: 9.05991e-06s
C++ Event::Emit Performance Test x 2: 9.53674e-07s
C++ Event::Emit Performance Test x 200000: 0.0774059s
C++ Event::Emit No Target Performance Test x 200000: 0.0258801s
C++ -> JavaScript Event::Emit Performance Test x 2: 4.05312e-06s
C++ -> JavaScript Event::Emit Performance Test x 100000: 0.145679s
```

New test on version `GIT_79d8e8d`

```
C++ Event::On Performance Test x 2: 5.96046e-06s
C++ Event::Emit Performance Test x 2: 9.53674e-07s
C++ Event::Emit Performance Test x 200000: 0.031553s
C++ Event::Emit No Target Performance Test x 200000: 0.00313091s
C++ -> JavaScript Event::Emit Performance Test x 2: 9.53674e-07s
C++ -> JavaScript Event::Emit Performance Test x 200000: 0.0444219s
```