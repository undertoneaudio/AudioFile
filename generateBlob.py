import sys
import os
import random
import string

def generate_random_line(length=80):
    # Generate a random string of a specified length
    return ''.join(random.choices(string.ascii_letters + string.digits + " ", k=length)) + '\n'

def generate_file(filename, target_size_mb, tolerance_bytes=1024 * 1024):
    target_size_bytes = target_size_mb * 1024 * 1024
    
    # Check if the file already exists and is close enough to the target size
    if os.path.isfile(filename):
        existing_file_size = os.path.getsize(filename)
        if abs(existing_file_size - target_size_bytes) <= tolerance_bytes:
            print(f"File '{filename}' already exists and is within the target size tolerance.")
            return
        else:
            print(f"File '{filename}' exists but does not match the target size. Regenerating the file.")
    
    # Start generating the file
    current_size = 0
    with open(filename, 'w') as f:
        while current_size < target_size_bytes:
            line = generate_random_line()
            f.write(line)
            current_size += len(line)
    print(f"File '{filename}' generated with target size of {target_size_mb} MB.")

if __name__ == "__main__":
    # Check if correct number of arguments are provided
    if len(sys.argv) != 3:
        print("Usage: python generate_random_text.py <filename> <size_in_mb>")
        sys.exit(1)

    filename = sys.argv[1]
    try:
        target_size_mb = int(sys.argv[2])
    except ValueError:
        print("Please provide a valid integer for file size in MB.")
        sys.exit(1)

    # Call generate_file with a tolerance of 1 KB to account for slight size discrepancies
    generate_file(filename, target_size_mb, tolerance_bytes=1024 * 1024)
