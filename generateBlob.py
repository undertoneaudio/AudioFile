import sys
import os
import random
import string

def generate_random_line(length=80):
    # Generate a random string of a specified length
    return ''.join(random.choices(string.ascii_letters + string.digits + " ", k=length)) + '\n'

def generate_file(filename, target_size_mb):
    target_size_bytes = target_size_mb * 1024 * 1024
    current_size = 0
    
    with open(filename, 'w') as f:
        while current_size < target_size_bytes:
            line = generate_random_line()
            f.write(line)
            current_size += len(line)

if __name__ == "__main__":
    # Check if correct number of arguments are provided
    if len(sys.argv) != 3:
        print("Usage: python generateBlob.py <filename> <size_in_mb>")
        sys.exit(1)

    filename = sys.argv[1]
    try:
        target_size_mb = int(sys.argv[2])
    except ValueError:
        print("Please provide a valid integer for file size in MB.")
        sys.exit(1)

    generate_file(filename, target_size_mb)
    print(f"File '{filename}' generated with target size of {target_size_mb} MB.")
