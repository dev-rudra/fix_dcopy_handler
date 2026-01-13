# Build binary with CMakeLists.txt
```bash
mkdir -p build
cmake -S . -B build
cmake --build build
```

# Build binary with Makefile
```bash
make build
```

# Run
```bash
./buld/fix_dropcopy

# Keep running after logout/SSH disconnect.
nohup .build/fix_dropcopy > dropcopy.log 2>&1 &
```

# Stop
```bash
ps -ef | grep fix_dropcopy
kill <pid>
```

## Normal Live mode
- Catch live messages
```bash
./fix_dropcopy
```

## Recover mode (replay old messages within the server window)
Forces `reset_on_logon=false` (read seq files and request resend on gaps).
```
./fix_dropcopy --recover
```

## Start replay from a specific point (historical recovery)
Edit incoming expected seq before running:
```sh
echo <expected_seqnum> tokens/<sender_comp_id>_<target_comp_id>.in.seq 

# e.g., expected_seqnum = 2

./fix_dropcopy --recover
```
What happens:
* If first live message is `34=123` and expected is `2`, the client sends
  `ResendRequest(7=2, 16=0)` and prints replayed messages.

* If the server replay window is limited, you may see GapFill `(35=4,123=Y,36=..)` and the client will continue catching up until live.

## Fresh restart but keep continuity
Leave seq files alone and just run:
```bash
./fix_dropcopy --recover
```
Then client will continue from:
* Last saved outgoing seq `(*.out.seq)`
* Last saved expected incoming seq `(*.in.seq)`

## Reset everything and start fresh

