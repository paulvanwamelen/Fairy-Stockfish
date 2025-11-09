
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
(echo -e "setoption name UCI_Variant value urbino\nposition startpos moves A@a3 A@g6 a3a3 a1a1hd6 a3i2pe6 g6f3hf5 f3i6hg4 i2d9hg6 d9h9hh6 i6f6hh8 h9e9he7 e9h7hg7 f6i7pi8 h7e1hi5 i7i1hg3 i1d1he2 e1b2hd2 b2a1hc1 d1b8hb1 b8a5ha2 a1f2hc5 a5d1pe1 d1a4tf4 f2a6tb5 a6e3pa3 e3a5pb4 a4c8ha6 a5c7td7\nd\ngo depth 6"; sleep 2; echo "quit") | ./stockfish 2>&1



make clean && make -j build ARCH=x86-64 largeboards=yes all=yes debug=no COMP=mingw EXE=urbino.exe EXTRACXXFLAGS='-static -static-libgcc -static-libstdc++' LDFLAGS='-static -static-libgcc -static-libstdc++'

TODO
TEST that we can do and undo over a pass.
move double pass end of hgame from is_optional_game_end to is_game_end(sp?)a1d6bb4 d6b7rb5
Looks like movepick looks at certain moves first... Can we make a move that changes the score a CAPTURE, and others QUIET?


echo -e "xboard\nprotover 2\nvariant urbino\nsetboard 9/9/9/9/9/9/9/9/9[QPPPPPPPPPPPPPPPPPPBBBBBBRRRqppppppppppppppppppbbbbbbrrr] w - - 0 1\nd\nquit" | 

Q@c4 Q@i1 c4c4pf4 i1e9pe4 c4c4pe6 e9d2pd4 d2b4pc3 b4d9rd5 d9d6pc6 c4d7pc7 d6e8pd8 e8i7be7 d7g4pg5 g4d9pf7 d9i8ph7 i7g9pf8 g9i6pi7 i8e5pf6 e5h5bh6 h5i2pe2 i2f5rg4 f5i2pi5 i6h5bi4 i2d3ph3 h5f1rf3 f1d1rc2 d3f2be1 f2a1bc1 d1c4pa2


(echo -e "setoption name UCI_Variant value urbino\nposition startpos moves Q@c4 Q@i1 a1a1pf4 i1e9pe4 a1a1pe6 e9d2pd4 d2b4pc3 b4d9rd5 d9d6pc6 c4d7pc7 d6e8pd8 e8i7be7 d7g4pg5 g4d9pf7 d9i8ph7 i7g9pf8 g9i6pi7 i8e5pf6 e5h5bh6 h5i2pe2 i2f5rg4 f5i2pi5 i6h5bi4 i2d3ph3 h5f1rf3 f1d1rc2 d3f2be1 f2a1bc1 d1c4pa2\nd\ngo depth 20"; sleep 200; echo "quit") | ./stockfish 2>&1

(echo -e "setoption name UCI_Variant value urbino\nposition fen 9/3a5/9/A8/9/9/9/9/9[TTTPPPPPPHHHHHHHHHHHHHHHHHHtttpppppphhhhhhhhhhhhhhhhhh] b - - 1 2\nd\ngo movetime 1000\nd\nquit"; sleep 2; echo "quit") | ./stockfish 2>&1

python3 variantfishtest.py ../Fairy-StockfishMine/src/stockfishV1 ../Fairy-StockfishMine/src/stockfish -v urbino --alias 1:Old --alias 2:New --book ../Fairy-StockfishMine/urbino_openings.epd -t 2000 --sprt --elo0 0 --elo1 10 -n 1000 --verbosity 1 -T 16 -l test.log


python3 variantfishtest.py ../Fairy-StockfishMine/src/stockfishV1 ../Fairy-StockfishMine/src/stockfish -v urbino --alias 1:Old --alias 2:New --book ../Fairy-StockfishMine/urbino_openings.epd -t 1000 -i 200 -n 500 --verbosity 1 -T 16

echo -e "setoption name VariantPath value variants.ini\nsetoption name UCI_Variant value monuments\nposition fen 9/H6hP/4H1h2/3hP1hH1/1tH2h2h/1p3TH2/P3paH2/h2Hh4/1Hh1p1A2[TTPPPHHHHHHHHHHttppphhhhhhhhh] b ABCDEFGHIabcdefghi - 26 14 moves a5g1pe3\nd" | ./stockfish 2>&1

setoption name VariantPath value variants.ini
setoption name UCI_Variant value monuments
setoption name Threads value 16
setoption name MultiPV value 5
position startpos moves A@c3 A@i5 a1a1hg7 i5h2he5 c3i8hh7 i8c1th6 h2f6hg5 f6h2hf4 h2a5pc5 a5d4tc4 d4d3pc2 c1g4pf5 g4d4he3

echo -e "setoption name VariantPath value variants.ini\nsetoption name UCI_Variant value monuments\nposition startpos moves A@c3 A@i5 a1a1hg7 i5h2he5 c3i8hh7 i8c1th6 h2f6hg5 f6h2hf4 h2a5pc5 a5d4tc4 d4d3pc2 c1g4pf5 g4d4he3\nd" | ./stockfish 2>&1

(echo -e "setoption name VariantPath value variants.ini\nsetoption name UCI_Variant value monuments\nposition startpos moves A@c3 A@i5 a1a1hg7 i5h2he5 c3i8hh7 i8c1th6 h2f6hg5 f6h2hf4 h2a5pc5 a5d4tc4 d4d3pc2 c1g4pf5 g4d4he3 d4g4pe2 d3h5hh3 h5f1tg2 g4i2ph1\nd\ngo depth 6"; sleep 2; echo "quit") | ./stockfish 2>&1