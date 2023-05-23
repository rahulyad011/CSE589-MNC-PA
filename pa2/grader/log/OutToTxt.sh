for file in *.out; do
    # Check if the file exists
    if [ -e "$file" ]; then
        # Rename the file by replacing the .out extension with .txt
        mv "$file" "${file%.out}.txt"
        echo "Renamed $file to ${file%.out}.txt"
    fi
done