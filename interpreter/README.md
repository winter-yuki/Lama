# Lama iterative bytecode interpreter

## Analysis mode

Prints parametrized bytecodes frequences in `.bc` file.

```bash
$ cd interpreter
$ make
$ ./driver -a something.bc
```

## Interpreter mode

Interprets `.bc` file.

Testing:
```bash
$ cd interpreter
$ make runTest
```

Evaluating:
```bash
$ cd interpreter
$ make
$ ./driver /path/to/file.bc
```
