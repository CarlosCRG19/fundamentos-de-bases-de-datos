for executable in *; do
    if [ -x "$executable" ]; then
        valgrind --leak-check=full --log-file=valgrind.log ./"$executable"
    fi
done

