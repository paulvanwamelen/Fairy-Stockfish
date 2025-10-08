
make -j4 build ARCH=x86-64 largeboards=yes all=yes debug=yes

echo -e "setoption name UCI_Variant value urbino\nposition startpos moves A@e5 A@d4 a1a1pe5\nd\nquit" | ./stockfish 2>&1


setoption name UCI_Variant value urbino
position startpos moves A@e5 A@d4 a1a1pe4 d4f3hf4 e5d3he3
go perft 1
f3d5tc4

echo -e "setoption name UCI_Variant value urbino\nposition startpos moves A@e5 A@d4 a1a1pe4 d4f3hf4 e5d3he3\nd\ngo perft 1\nquit" | ./stockfish 2>&1 > dump.txt
echo -e "setoption name UCI_Variant value urbino\nposition startpos moves A@e5 A@d4 a1a1pe4 d4f3hf4 e5d3he3 d3g5tg4\nd\n" | ./stockfish
f1d3h
d3a1he4

echo -e "setoption name UCI_Variant value urbino\nposition startpos moves A@a3 A@g6 a3a3 a1a1hd6 a3i2pe6 g6f3hf5 f3i6hg4 i2d9hg6 d9h9hh6 i6f6hh8 h9e9he7 e9h7hg7 f6i7pi8 h7e1hi5 i7i1hg3 i1d1he2 e1b2hd2 b2a1hc1 d1b8hb1 b8a5ha2 a1f2hc5 a5d1pe1 d1a4tf4 f2a6tb5 a6e3pa3 e3a5pb4 a4c8ha6 a5c7td7 c8d9h d9b6pb8 a1a1ha7 b6c9he9 c7d8td9 d8a9tb9\nd\n" | ./stockfish
echo -e "setoption name UCI_Variant value urbino\nposition startpos moves A@a3 A@g6 a3a3 a1a1hd6 a3i2pe6 g6f3hf5 f3i6hg4 i2d9hg6 d9h9hh6 i6f6hh8 h9e9he7 e9h7hg7 f6i7pi8 h7e1hi5 i7i1hg3 i1d1he2 e1b2hd2 b2a1hc1 d1b8hb1 b8a5ha2 a1f2hc5 a5d1pe1 d1a4tf4 f2a6tb5 a6e3pa3 e3a5pb4 a4c8ha6 a5c7td7\nd\n" | ./stockfish


urbino_update_blocks in position.cpp and 162 to 170 in movegen.cpp


cat >/tmp/sf.in <<'EOF'
<your big long input goes here>
EOF


setoption name UCI_Variant value urbino\nposition startpos moves A@a3 A@g6 a3a3 a1a1hd6 a3i2pe6 g6f3hf5 f3i6hg4 i2d9hg6 d9h9hh6 i6f6hh8 h9e9he7 e9h7hg7 f6i7pi8 h7e1hi5 i7i1hg3 i1d1he2 e1b2hd2 b2a1hc1 d1b8hb1 b8a5ha2 a1f2hc5 a5d1pe1 d1a4tf4 f2a6tb5 a6e3pa3 e3a5pb4 a4c8ha6 a5c7td7 c8d9h d9b6pb8\nd\ngo perft 2\nquit\n

echo -e "setoption name UCI_Variant value urbino\nposition startpos moves A@a3 A@g6 a3a3 a1a1hd6 a3i2pe6 g6f3hf5 f3i6hg4 i2d9hg6 d9h9hh6 i6f6hh8 h9e9he7 e9h7hg7 f6i7pi8 h7e1hi5 i7i1hg3 i1d1he2 e1b2hd2 b2a1hc1 d1b8hb1 b8a5ha2 a1f2hc5 a5d1pe1 d1a4tf4 f2a6tb5 a6e3pa3 e3a5pb4 a4c8ha6\nd\ngo perft 3\nquit\n" | ./stockfish
Nodes searched: 33290423

ulimit -c unlimited
echo 'core.%e.%p.%t' | sudo tee /proc/sys/kernel/core_pattern

(echo -e "setoption name UCI_Variant value urbino\nposition startpos\ngo movetime 2000"; sleep 3; echo "quit") | ./stockfish 2>&1

(echo -e "setoption name UCI_Variant value urbino\nposition startpos moves A@a3 A@g6 a3a3 a1a1hd6 a3i2pe6 g6f3hf5 f3i6hg4 i2d9hg6 d9h9hh6 i6f6hh8 h9e9he7 e9h7hg7 f6i7pi8 h7e1hi5 i7i1hg3 i1d1he2 e1b2hd2 b2a1hc1 d1b8hb1 b8a5ha2 a1f2hc5 a5d1pe1 d1a4tf4 f2a6tb5 a6e3pa3 e3a5pb4 a4c8ha6 a5c7td7\nd\ngo movetime 1000"; sleep 2; echo "quit") | ./stockfish 2>&1

go depth 6

make clean && make -j build ARCH=x86-64 largeboards=yes all=yes debug=no COMP=mingw EXE=urbino.exe EXTRACXXFLAGS='-static -static-libgcc -static-libstdc++' LDFLAGS='-static -static-libgcc -static-libstdc++'

TODO
TEST that we can do and undo over a pass.
move double pass end of hgame from is_optional_game_end to is_game_end(sp?)
Looks like movepick looks at certain moves first... Can we make a move that changes the score a CAPTURE, and others QUIET?
