# /// script
# requires-python = ">=3.12"
# dependencies = [
#   "colorama",
# ]
# ///

import subprocess
import os
from colorama import init, Fore, Style

# Initialize colorama
init(autoreset=True)

# If your binary supports displaying the file
display_flag=False

# Helper function to run the C program with a given parameter and capture output
def run_c_program(filename, param, display=False):
    if display:
        result = subprocess.run(["./figsearch", "--display", param, filename], capture_output=True, text=True)
    else:
        result = subprocess.run(["./figsearch", param, filename], capture_output=True, text=True)
    return result.returncode, result.stdout, result.stderr

# Function to test files inside the /images directory for all parameters
def test_images_folder():
    # Ensure the images folder exists
    images_folder = './tests'
    if not os.path.exists(images_folder):
        print(f"{Fore.RED}Error: The folder {images_folder} does not exist.{Style.RESET_ALL}")
        return

    # Get all .txt files from the images directory
    files = sorted([f for f in os.listdir(images_folder) if f.endswith(".txt")])

    # Start testing
    test_count = 0
    tests_passed = []
    tests_failed = []

    for file in files:
        file_path = os.path.join(images_folder, file)

        # Extract the expected coordinates for hline, vline, and square from the filename
        parts = file.split('-')
        if len(parts) < 4:
            print(f"{Fore.YELLOW}Skipping invalid file: {file}{Style.RESET_ALL}")
            continue

        # Extract the expected outputs for hline, vline, and square
        expected_hline = parts[1].replace("_", " ")
        expected_vline = parts[2].replace("_", " ")
        expected_square = parts[3].replace(".txt", "").replace("_", " ")

        # List of parameters to run the C program with
        params = ["hline", "vline", "square"]
        expected_values = [expected_hline, expected_vline, expected_square]

        # Initialize output and status lists
        outputs = []
        statuses = []

        # Run the C program for each parameter and store the results
        for i, param in enumerate(params):
            return_code, stdout, stderr = run_c_program(file_path, param)
            output = stdout.strip() + stderr.strip()

            if expected_values[i] == output:
                # Append passing output
                outputs.append(output)
                statuses.append(Fore.GREEN + "✔" + Style.RESET_ALL)
            else:
                # Append failing output or error
                outputs.append(output if output else "(No output)")
                if return_code != 0:
                    statuses.append(Fore.RED + f"✘ (Return {return_code})" + Style.RESET_ALL)
                else:
                    statuses.append(Fore.RED + "✘" + Style.RESET_ALL)

        # Combine results into one printout
        test_count += 1
        test_passed = all(s == Fore.GREEN + "✔" + Style.RESET_ALL for s in statuses)
        header_color = Fore.GREEN + Style.BRIGHT if test_passed else Fore.RED + Style.BRIGHT
        header_status = "PASSED ✔" if test_passed else "FAILED ✘"
        if test_passed:
            tests_passed.append(file.split('-')[0])
        else:
            tests_failed.append(file.split('-')[0])

        # Print the result
        print(f"{Fore.CYAN}{Style.BRIGHT}{'─' * 37}{Style.RESET_ALL}")
        print(f"{header_color}◇ > TEST [{test_count}] === {header_status}{Style.RESET_ALL}")
        print(f"│   {file.split('-')[0]}.txt   ({' '.join(statuses)})")
        print(f"├   {' || '.join(expected_values)}")
        print(f"└   {' || '.join(outputs)}\n")
        if display_flag:
            print(f"{"\n".join(run_c_program(file_path, "hline", True)[1].split('\n')[:-2])}\n")
            print(f"{"\n".join(run_c_program(file_path, "vline", True)[1].split('\n')[:-2])}\n")
            print(f"{"\n".join(run_c_program(file_path, "square", True)[1].split('\n')[:-2])}\n")

    # Print the summary
    header_color = Fore.GREEN + Style.BRIGHT if len(tests_failed) == 0 else Fore.RED + Style.BRIGHT 
    print(f"{Fore.CYAN}{Style.BRIGHT}{'─' * 37}{Style.RESET_ALL}")
    print(f"{header_color}◇ > PASSED [{len(tests_passed)}/{test_count}]{Style.RESET_ALL}")
    for test in tests_failed:
        print(f"├   {Fore.RED}{test}.txt ✘{Style.RESET_ALL}")
    for test in tests_passed:
        print(f"├   {Fore.GREEN}{test}.txt ✔{Style.RESET_ALL}")

# Main function to run all tests
if __name__ == "__main__":
    test_images_folder()
