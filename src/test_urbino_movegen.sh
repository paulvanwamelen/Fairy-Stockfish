#!/bin/bash
# Test script to verify Urbino move generation against a reference game
# This plays through a known game and captures legal moves at each position

MOVES=(
    "A@a3" "A@g6" "a3a3" "a1a1hd6" "a3i2pe6" "g6f3hf5" "f3i6hg4" "i2d9hg6"
    "d9h9hh6" "i6f6hh8" "h9e9he7" "e9h7hg7" "f6i7pi8" "h7e1hi5" "i7i1hg3"
    "i1d1he2" "e1b2hd2" "b2a1hc1" "d1b8hb1" "b8a5ha2" "a1f2hc5" "a5d1pe1"
    "d1a4tf4" "f2a6tb5" "a6e3pa3" "e3a5pb4" "a4c8ha6" "a5c7td7" "c8d9h"
    "d9b6pb8" "a1a1ha7" "b6c9he9" "c7d8td9" "d8a9tb9"
)

STOCKFISH="./stockfish"

if [ ! -f "$STOCKFISH" ]; then
    echo "Error: stockfish executable not found at $STOCKFISH"
    exit 1
fi

echo "Testing Urbino move generation..."
echo "Playing through ${#MOVES[@]} moves"
echo ""

POSITION="startpos"
MOVE_LIST=""

for i in "${!MOVES[@]}"; do
    MOVE="${MOVES[$i]}"
    MOVE_NUM=$((i + 1))

    echo "Move $MOVE_NUM: $MOVE"

    # Build position command
    if [ -z "$MOVE_LIST" ]; then
        POS_CMD="position $POSITION moves $MOVE"
        MOVE_LIST="$MOVE"
    else
        POS_CMD="position $POSITION moves $MOVE_LIST $MOVE"
        MOVE_LIST="$MOVE_LIST $MOVE"
    fi

    # Get legal moves at this position
    LEGAL_MOVES=$(echo -e "setoption name UCI_Variant value urbino\n$POS_CMD\ngo perft 1\nquit" | $STOCKFISH 2>/dev/null | grep -A10000 "^[a-zA-Z@]" | grep -B10000 "^Nodes searched" | grep -E "^([a-i][0-9]|A@)" | awk '{print $1}' | sort)

    MOVE_COUNT=$(echo "$LEGAL_MOVES" | wc -l)

    echo "  Legal moves at this position: $MOVE_COUNT"

    # Optional: save moves to file for comparison
    echo "$LEGAL_MOVES" > "/tmp/urbino_movegen_${MOVE_NUM}.txt"

done

echo ""
echo "Move generation test complete!"
echo "Legal move lists saved to /tmp/urbino_movegen_*.txt"
echo ""
echo "To compare with a reference implementation:"
echo "  1. Run this script and save output: ./test_urbino_movegen.sh > current_movegen.txt"
echo "  2. Make your changes"
echo "  3. Run again and compare: ./test_urbino_movegen.sh > new_movegen.txt && diff current_movegen.txt new_movegen.txt"
